#pragma once

namespace Nilesoft
{
	struct REGOP
	{
		bool REGISTER;
		bool UNREGISTER;
		bool TREAT;
		bool CONTEXTMENU;
		bool FOLDEREXTENSIONS;
		bool ICONOVERLAY;
		bool RESTART;
		bool SILENT;
	};

	class RegistryConfig
	{
#define	APP_SIG						L"\u0020@nilesoft.shell"
#define	APP_COMP_NAME				APP_COMPANY L"." APP_NAME
#define APP_KEY						L"SOFTWARE\\" APP_COMPANY L"\\" APP_NAME

//L"SOFTWARE\\Classes\\Drive\\shellex\\FolderExtensions\\"
#define HKLM_DRIVE_FolderExtensions	L"SOFTWARE\\Classes\\Drive\\shellex\\FolderExtensions\\" CLS_FolderExtensions
#define HKCR_CONTEXTMENUHANDLERS	L"\\shellex\\ContextMenuHandlers\\" APP_SIG

#define HKLM_CURRENTVERSION			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
#define HKLM_APPROVED				HKLM_CURRENTVERSION L"\\Shell Extensions\\Approved"
#define HKLM_ICONOVERLAY			HKLM_CURRENTVERSION L"\\Explorer\\ShellIconOverlayIdentifiers\\" APP_SIG

		//Computer\HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}
		//reg.exe add "HKCU\Software\Classes\CLSID\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}\InprocServer32" /f /ve

		public:
			static const wchar_t *GetKeyPath()
			{/*
#ifndef _WIN64
				//if(Is64BitWindows())
				return L"Software\\Classes\\Wow6432Node";
#endif
*/
				return L"Software\\Classes";
			}

			static bool IsRegistered()
			{
				return IsContextMenu() || IsIconOverlay() || IsFolderExtensions();
			}

			static bool IsHKCR(const string &key)
			{
				return Registry::Exists(HKCR, L"CLSID\\" + key);
			}

			static bool IsContextMenu()
			{
				return IsHKCR(CLS_ContextMenu);
			}

			static bool IsIconOverlay()
			{
				return IsHKCR(CLS_IconOverlay);
			}

			static bool IsFolderExtensions()
			{
				return IsHKCR(CLS_FolderExtensions);
			}

			// register COM-object Add HKCR\CLSID\{<CLSID>} key.
			static bool RegisterInprocServer(const wchar_t *module, const wchar_t *clsid, const wchar_t *value)
			{
				bool ret = false;
				auto keyCLSID = Registry::ClassesRoot.OpenSubKey(L"CLSID", false, true);
				if(keyCLSID)
				{
					auto keyGuid = keyCLSID.CreateSubKey(clsid);
					if(keyGuid)
					{
						if(keyGuid.SetString(nullptr, value))
						{
							auto keyInprocServer32 = keyGuid.CreateSubKey(L"InprocServer32");
							if(keyInprocServer32)
							{
								if(keyInprocServer32.SetString(nullptr, module))
								{
									ret = keyInprocServer32.SetString(L"ThreadingModel", L"Apartment");
								}
								keyInprocServer32.Close();
							}
						}

						keyGuid.Close();

						if(ret == false)
							keyCLSID.DeleteSubKey(clsid);
					}
					keyCLSID.Close();
				}
				return ret;
			}

			// Require admininstrator's rights!
			static bool Register(const wchar_t *dllPath, REGOP reg)
			{
				if(!dllPath || !dllPath[0])
					return false;
				
				if(!::IsWindows7OrGreater())
					return false;

				int ret = 0;

				if(reg.FOLDEREXTENSIONS)
				{
					if(RegisterInprocServer(dllPath, CLS_FolderExtensions, APP_COMP_NAME))
					{
						ret++;
						auto key = Registry::LocalMachine.CreateSubKey(HKLM_DRIVE_FolderExtensions, false);
						if(key)
						{
							ret += key.SetString(nullptr, APP_COMP_NAME);
							ret += key.SetInt(L"DriveMask", 0xff);
							key.Close();
						}
						else
						{
							Registry::DeleteSubKey(HKCR, L"CLSID\\" CLS_FolderExtensions);
							ret = 0;
						}
					}
				}
				
				if(reg.ICONOVERLAY)
				{
					// register COM-object for overlay icon handler
					if(RegisterInprocServer(dllPath, CLS_IconOverlay, APP_COMP_NAME))
					{
						// register overlay icon handler
						ret += Registry::LocalMachine.SetString(HKLM_ICONOVERLAY, nullptr, CLS_IconOverlay, false);
					}
				}

				if(reg.CONTEXTMENU)
				{
					// register COM-object for shortcut menu handler
					if(RegisterInprocServer(dllPath, CLS_ContextMenu, APP_COMP_NAME))
					{
						if(Registry::LocalMachine.SetString(HKLM_APPROVED, CLS_ContextMenu, APP_SIG, false))
						{
							//register shortcut menu handler
							ret += RegisterContextMenuHandler(true);
						}
					}
				}

				if(ret > 0)
				{
					auto keyNSS = Registry::ClassesRoot.CreateSubKey(L".nss", false);
					if(keyNSS)
					{
						keyNSS.SetString(L"Content Type", L"text/plain");
						auto key_CMD = keyNSS.CreateSubKey(L"shell\\open\\command");
						if(key_CMD)
						{
							key_CMD.SetString(nullptr, L"notepad \"%1\"");
							key_CMD.Close();
						}
						keyNSS.Close();
					}
					return true;
				}
				return false;
			}

			// Require administrator's rights!
			static bool Unregister()
			{
				int ret = 0;

				ret += Registry::DeleteSubKey(HKCR, L"CLSID\\" CLS_FolderExtensions);
				//ret += Registry::DeleteSubKey(HKLM, HKLM_DRIVE_FolderExtensions);

				ret += Registry::DeleteSubKey(HKLM, HKLM_ICONOVERLAY);
				// unregister COM-object Delete the HKCR\CLSID\{<CLSID>} key.
				ret += Registry::DeleteSubKey(HKCR, L"CLSID\\" CLS_IconOverlay);

				ret += Registry::DeleteKeyValue(HKLM, HKLM_APPROVED, CLS_ContextMenu);
				// unregister COM-object Delete the HKCR\CLSID\{<CLSID>} key.
				ret += Registry::DeleteSubKey(HKCR, L"CLSID\\" CLS_ContextMenu);
				ret += RegisterContextMenuHandler(false);

				/*if(Windows::Version::Instance().IsWindows11OrGreater())
				{
					string key_treatas;
					key_treatas.format(L"CLSID\\%s\\TreatAs", string::ToString(IID_FileExplorerContextMenu, 2).c_str());
					Registry::DeleteSubKey(HKCR, key_treatas);
				}
				*/
				Registry::DeleteSubKey(HKCR, L".nss");

				return ret > 0;// == 5;
			}

			// NOTE: The function add or removes the {{<CLSID>}} key under
			// HKCU\Software\Classes\<type>\shellex\ContextMenuHandlers in the registry.
			static bool RegisterContextMenuHandler(const wchar_t *fileType, bool bRegister)
			{
				if(!fileType || !fileType[0])
					return false;

				wchar_t default_value[MAX_PATH] { 0 };
				if(*fileType == L'.')
				{
					auto key = Registry::ClassesRoot.OpenSubKey(fileType);
					// If the key exists and its default value is not empty,
					// use the ProgID as the file type.
					if(key)
					{
						if(key.GetString(nullptr, default_value, MAX_PATH))
							fileType = default_value;
						key.Close();
					}
				}

				string subKey = fileType;
				subKey += HKCR_CONTEXTMENUHANDLERS;
				// key HKCR\<Types>\shellex\ContextMenuHandlers\{{<CLSID>}}
				// Create
				if(bRegister)
					return Registry::SetKeyValue(HKCR, subKey, nullptr, CLS_ContextMenu);
				// Remove 
				return Registry::DeleteSubKey(HKCR, subKey);
			}

			// Update Registry
			// true	 = Register the context menu handler. 
			// false = Unregister the context menu handler.
			static bool RegisterContextMenuHandler(bool _register)
			{
				int res = 0;
				// The context menu handler is associated with the classes.
				res += RegisterContextMenuHandler(L"*", _register);
				res += RegisterContextMenuHandler(L"Directory", _register);
				//res += RegisterContextMenuHandler(L"AllFileSystemObjects", _register);
				res += RegisterContextMenuHandler(L"Drive", _register);
				res += RegisterContextMenuHandler(L"Folder", _register);
				res += RegisterContextMenuHandler(L"Directory\\Background", _register);
				res += RegisterContextMenuHandler(L"DesktopBackground", _register);
				res += RegisterContextMenuHandler(L"LibraryFolder", _register);
				res += RegisterContextMenuHandler(L"LibraryFolder\\Background", _register);
				return res > 0;
			}

			static bool get(const wchar_t* subkey, const wchar_t *name, string &value)
			{
				string _subkey = APP_KEY;
				if(subkey)
				{
					if(subkey[0] != L'\\')
						_subkey += L"\\";
					_subkey += subkey;
				}

				if(auto key = Registry::CurrentUser.OpenSubKey(_subkey, true, false); key)
					return key.ReadString(name, value);
				return false;
			}

			static bool get(const wchar_t *subkey, const wchar_t *name, int &value)
			{
				string _subkey = APP_KEY;
				if(subkey)
				{
					if(subkey[0] != L'\\')
						_subkey += L"\\";
					_subkey += subkey;
				}
				
				if(auto key = Registry::CurrentUser.OpenSubKey(_subkey, true, false); key)
				{
					value = key.GetInt(name, value);
					return true;
				}
				return false;
			}
		};
	}
