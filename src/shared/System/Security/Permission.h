#pragma once

namespace Nilesoft 
{
	namespace Security
	{
		class Permission
		{
		public:
			static bool SetRegistry(HKEY hkey)
			{
				if(hkey == nullptr) return false;

				struct dest
				{
					PSID sid = nullptr;
					PACL dacl = nullptr;
					PSECURITY_DESCRIPTOR sd = nullptr;

					~dest()
					{
						if(sid) ::FreeSid(sid);
						if(dacl) ::LocalFree(dacl);
						if(sd) ::LocalFree(sd);
					}
				}d;

				DWORD dwRes;
				PACL pOldDACL = nullptr;
				EXPLICIT_ACCESSW ea = { 0 };
				SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

				dwRes = ::GetSecurityInfo(hkey, SE_REGISTRY_KEY,
										  DACL_SECURITY_INFORMATION, nullptr, nullptr, &pOldDACL, nullptr, &d.sd);
				if(dwRes != ERROR_SUCCESS)
					return false;

				// Create a well-known SID for the Everyone group.
				if(!::AllocateAndInitializeSid(&SIDAuthNT, 2,
											   SECURITY_BUILTIN_DOMAIN_RID,
											   DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &d.sid))
				{
					return false;
				}

				// Initialize an EXPLICIT_ACCESS structure for an ACE.
				// The ACE will allow Users all access to the key.
				ea.grfAccessPermissions = KEY_ALL_ACCESS;
				ea.grfAccessMode = SET_ACCESS;
				ea.grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;;
				ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
				ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
				ea.Trustee.ptstrName = (wchar_t*)d.sid;

				// Create a new ACL that contains the new ACEs.
				if(ERROR_SUCCESS != ::SetEntriesInAclW(1, &ea, pOldDACL, &d.dacl))
					return false;

				if(d.dacl)
				{
					return ERROR_SUCCESS == ::SetSecurityInfo(
						hkey, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, nullptr, nullptr, d.dacl, nullptr);
				}
				return false;
			}

			static bool SetRegistry(HKEY hkeyRoot, const wchar_t* subkey)
			{
				bool fSuccess = false;
				HKEY hkey = nullptr;

				LONG lRes = ::RegOpenKeyExW(hkeyRoot, subkey, REG_OPTION_NON_VOLATILE, KEY_READ, &hkey);

				if(lRes != ERROR_SUCCESS)
					return fSuccess;

				fSuccess = SetRegistry(hkey);

				if(hkey)
					::RegCloseKey(hkey);

				return fSuccess;
			}

			static bool SetFile(const wchar_t* path)
			{
				auto_handle hFile = ::CreateFileW(path, READ_CONTROL | WRITE_DAC,
												  0, nullptr, OPEN_EXISTING,
												  FILE_ATTRIBUTE_NORMAL, nullptr);
				if(hFile)
				{
					return SetFile(hFile);
				}
				return false;
			}

			static bool SetFile(HANDLE hFile)
			{

				if(hFile == INVALID_HANDLE_VALUE)
					return false;

				struct dest
				{
					PSID sid = nullptr;
					PACL dacl = nullptr;
					PSECURITY_DESCRIPTOR sd = nullptr;

					~dest()
					{
						if(sid) ::FreeSid(sid);
						if(dacl) ::LocalFree(dacl);
						if(sd) ::LocalFree(sd);
					}
				}d;

				DWORD dwRes;
				PACL pOldDACL = nullptr;
				EXPLICIT_ACCESSW ea = { 0 };
				SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

				dwRes = ::GetSecurityInfo(hFile, SE_FILE_OBJECT,
										  DACL_SECURITY_INFORMATION, nullptr, nullptr, &pOldDACL, nullptr, &d.sd);
				if(dwRes != ERROR_SUCCESS)
					return false;

				// Create a well-known SID for the Everyone group.
				if(!::AllocateAndInitializeSid(&SIDAuthNT, 2,
											   SECURITY_BUILTIN_DOMAIN_RID,
											   DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &d.sid))
				{
					return false;
				}

				// Initialize an EXPLICIT_ACCESS structure for an ACE.
				// The ACE will allow Users all access to the key.
				ea.grfAccessPermissions = GENERIC_ALL;
				ea.grfAccessMode = SET_ACCESS;
				ea.grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
				ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
				ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
				ea.Trustee.ptstrName = (wchar_t*)d.sid;

				// Create a new ACL that contains the new ACEs.
				if(ERROR_SUCCESS != ::SetEntriesInAclW(1, &ea, pOldDACL, &d.dacl))
					return false;

				if(d.dacl)
				{
					return ERROR_SUCCESS == ::SetSecurityInfo(
						hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, d.dacl, nullptr);
				}

				return false;
			}
		};
	}
}