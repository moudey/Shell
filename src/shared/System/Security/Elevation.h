#pragma once

#ifndef ELEVATION_H
#define ELEVATION_H

namespace Nilesoft
{
	namespace Security
	{
		class Elevation
		{
		public: // API functions (used in exports)

			// Tests whether the current user is a member of the Administrator's group.
			bool IsUserAnAdmin()
			{
				return ::IsUserAnAdmin();
			}

			// Whether the current process is run as administrator and it is elevated. 
			//IsUserAdmin
			static bool IsElevated()
			{
				struct ADMIN {
					BOOL isAdmin = FALSE;
					PSID pAdminSID = nullptr; // AdministratorsGroup
					~ADMIN() { if(pAdminSID) ::FreeSid(pAdminSID); }
				} admin;
			
				// Allocate and initialize a SID of the administrators group.
				SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
				if(::AllocateAndInitializeSid(&NtAuthority, 2,
											  SECURITY_BUILTIN_DOMAIN_RID,
											  DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, 
											  &admin.pAdminSID))
				{
					// Determine whether the SID of administrators group is enabled 
					// in the primary access token of the process.
					if(admin.pAdminSID)
						::CheckTokenMembership(nullptr, admin.pAdminSID, &admin.isAdmin);
				}
				return admin.isAdmin;
			}

			static bool IsUserInAdminGroup()
			{
				struct Token
				{
					HANDLE hToken = nullptr;
					HANDLE hCheck = nullptr;

					~Token()
					{
						if(hToken)
							::CloseHandle(hToken);
						if(hCheck)
							::CloseHandle(hCheck);
					}
				} token{};

				BOOL   isAdmin = FALSE;
				DWORD  dwSize = 0;

				auto winver = &Windows::Version::Instance();
				// Open the primary access token of the process for query and duplicate.
				if(!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &token.hToken))
					return false;

				// Determine whether system is running Windows Vista or later operating systems

				if(winver->IsWindowsVistaOrGreater())
				{
					// Running Windows Vista or later (major version >= 6). 
					// Determine token type: limited, elevated, or default. 
					TOKEN_ELEVATION_TYPE eType{};
					if(!::GetTokenInformation(token.hToken, TokenElevationType, &eType, sizeof(eType), &dwSize))
						return false;

					// If limited, get the linked elevated token for further check.
					if(TokenElevationTypeLimited == eType)
					{
						if(!::GetTokenInformation(token.hToken, TokenLinkedToken, &token.hCheck, sizeof(token.hCheck), &dwSize))
							return false;
					}
				}

				// CheckTokenMembership requires an impersonation token. If we just got a 
				// linked token, it already is an impersonation token.  If we did not get 
				// a linked token, duplicate the original into an impersonation token for 
				// CheckTokenMembership.
				if(!token.hCheck)
				{
					if(!::DuplicateToken(token.hToken, SecurityIdentification, &token.hCheck))
						return false;
				}

				// Create the SID corresponding to the Administrators group.
				BYTE adminSID[SECURITY_MAX_SID_SIZE] = { };
				dwSize = sizeof(adminSID);
				if(::CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize))
				{
					// Check if the token to be checked contains admin SID.
					// http://msdn.microsoft.com/en-us/library/aa379596(VS.85).aspx:
					// To determine whether a SID is enabled in a token, that is, whether it 
					// has the SE_GROUP_ENABLED attribute, call CheckTokenMembership.
					::CheckTokenMembership(token.hCheck, &adminSID, &isAdmin);
				}

				return isAdmin ? true : false;
			}

			// Whether the current process is run as administrator and it is elevated.
			// for Windows vista or later
			static bool IsProcessElevated()
			{
				HANDLE hToken = nullptr;
				DWORD  dwSize = 0;
				TOKEN_ELEVATION elevation = {};
				// Open the primary access token of the process with TOKEN_QUERY.
				if(::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hToken) && hToken)
				{
					// Retrieve token elevation information.
					if(::GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(TOKEN_ELEVATION), &dwSize))
					{
						// When the process is run on operating systems prior to Windows 
						// Vista, GetTokenInformation returns FALSE with the 
						// ERROR_INVALID_PARAMETER error code because TokenElevation is 
						// not supported on those operating systems.
					}
					::CloseHandle(hToken);
					return elevation.TokenIsElevated != 0;
				}
				return false;
			}

			static DWORD SelfElevate(HWND hWnd, int nShow, const wchar_t* szExecutable, const wchar_t* szParameters)
			{
				// Elevate the process if it is not run as administrator.
				if(!Elevation::IsElevated())
				{
					// Launch itself as administrator.
					SHELLEXECUTEINFOW sInfo = { };
					sInfo.cbSize = sizeof(sInfo);
					sInfo.lpVerb = L"runas";
					sInfo.lpFile = szExecutable;
					sInfo.lpParameters = szParameters;
					sInfo.hwnd = hWnd;
					sInfo.nShow = nShow;//SW_NORMAL;

					if(!::ShellExecuteExW(&sInfo))
					{
						auto dwError = ::GetLastError();
						if(dwError == ERROR_CANCELLED)
							// The user refused to allow privileges elevation.
							return ERROR_CANCELLED;
						else
							return dwError;
					}
					else
						// REMARK: Process termination is task of the caller!
						return ERROR_SUCCESS;
				}
				else
					return ERROR_SUCCESS;
			}
		};
	}
}
#endif