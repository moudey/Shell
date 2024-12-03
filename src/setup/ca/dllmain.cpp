#define VC_EXTRALEAN
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <msi.h>
#include <msiquery.h>
#include <string>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "msi.lib")

template<typename ... Args>
static void log(const char *format, Args ... args)
{
	/*
	using ___snprintfFT = size_t(__stdcall *)(char *, size_t, const char *, Args... args);
	if(auto hm = ::GetModuleHandleW(L"msvcrt.dll"); hm)
	{
		auto ___snprintf = (___snprintfFT)::GetProcAddress(hm, "_snprintf");
		if(___snprintf)
		{
			auto size = ___snprintf(nullptr, 0, format, args ...);
			if(size > 0)
			{
				std::string buf(size, '\0');
				___snprintf(buf.data(), size, format, args ...);
				::OutputDebugStringA(buf.c_str());
			}
		}
	}
	*/
	auto size = ::_snprintf(nullptr, 0, format, args ...);
	if(size > 0)
	{
		std::string buf(size, '\0');
		::_snprintf(buf.data(), size, format, args ...);
		::OutputDebugStringA(buf.c_str());
	}
}

struct MSI
{
	MSIHANDLE hInstall = 0;

	MSI(MSIHANDLE hInstall = 0)
		: hInstall{ hInstall }
	{
	}

	bool get(const wchar_t *name, std::wstring &value) const
	{
		if(hInstall && name != nullptr)
		{
			DWORD length = 0;
			auto hr = ::MsiGetPropertyW(hInstall, name, nullptr, &length);
			if(length > 0)
			{
				value.resize(length++);
				hr = ::MsiGetPropertyW(hInstall, name, value.data(), &length);
				return true;
			}
		}
		return false;
	}


	std::wstring InstallFolder()
	{
		std::wstring result;
		get(L"INSTALLFOLDER", result);
		return std::move(result);
	}

	std::wstring ProductCode()
	{
		std::wstring result;
		get(L"ProductCode", result);
		return std::move(result);
	}

	bool set(const wchar_t *name, const wchar_t *value) const
	{
		if(hInstall && name != nullptr)
		{
			return ERROR_SUCCESS == ::MsiSetPropertyW(hInstall, name, value);
		}
		return false;
	}
};

bool streq(const std::wstring &s1, const wchar_t *s2)
{
	return ::memicmp(s1.c_str(), s2, s1.size() * sizeof(wchar_t)) == 0;
}

#undef ShellExecute

constexpr auto FILEEXE = L"shell.exe";
constexpr auto FILEDLL = L"shell.dll";
constexpr auto FILEOLD = L"shell.old";

constexpr auto upgrading = L"UPGRADINGPRODUCTCODE";

BOOL ShellExec(const wchar_t *file, const wchar_t *parameters, const wchar_t *directory, bool run_as_admin = false, int nshow = SW_NORMAL, bool wait = false)
{
	SHELLEXECUTEINFOW sei = { };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_FLAG_NO_UI | (wait ? SEE_MASK_NOCLOSEPROCESS : 0);
	sei.lpFile = file;
	sei.lpVerb = run_as_admin ? L"runas" : nullptr;
	sei.lpParameters = parameters;
	sei.lpDirectory = directory;
	sei.nShow = nshow;

	auto res = ::ShellExecuteExW(&sei) || (wait && !sei.hProcess);
	if(!res)
		return res;

	if(!wait)
		return res;

	// Wait until child process exits.
	MSG msg;
	DWORD dw;
	while(wait)
	{
		dw = ::MsgWaitForMultipleObjects(1, &sei.hProcess, FALSE, INFINITE, QS_ALLINPUT);
		if(dw != WAIT_OBJECT_0 + 1)
		{
			// unexpected failure
			wait = false;
			break;
		}
		else while(::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))  // we have a message - peek and dispatch it
		{
			if(msg.message == WM_QUIT)
			{
				wait = false;
				break;
			}
			::DispatchMessageW(&msg);
		}
	}
	::CloseHandle(sei.hProcess);
	return res;
}

/*
static BOOL ProcessStart(const wchar_t *application, const wchar_t *parameters, const wchar_t *directory = nullptr, bool wait = false, int showCmd = SW_SHOWNORMAL)
{
	PROCESS_INFORMATION processInfo = { };
	STARTUPINFOW startupInfo = { };
	startupInfo.cb = sizeof(STARTUPINFOW);
	startupInfo.wShowWindow = (WORD)showCmd;
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	std::wstring commandLine= parameters;

	auto result = ::CreateProcessW(application,
							commandLine.data(), nullptr, nullptr, FALSE,
							NORMAL_PRIORITY_CLASS,
							nullptr, directory,
							&startupInfo, &processInfo);
	if(!result)
		return FALSE;

	// Wait until child process exits.
	if(wait && processInfo.hProcess)
		::WaitForSingleObject(processInfo.hProcess, INFINITE);

	if(processInfo.hThread)
		::CloseHandle(processInfo.hThread);
	if(processInfo.hProcess)
		::CloseHandle(processInfo.hProcess);

	return result;
}

static bool IsDirectoryExists(const std::wstring_view &path)
{
	auto attr = ::GetFileAttributesW(path.data());
	return (attr != UINT32_MAX && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

// Checks if a file exists (accessible)
static bool IsFileExists(const std::wstring_view &path)
{
	auto attr = ::GetFileAttributesW(path.data());
	//return (attr != INVALID_FILE_ATTRIBUTES && (!(attr & FILE_ATTRIBUTE_DIRECTORY)));
	if(attr == INVALID_FILE_ATTRIBUTES)
	{
		auto errval = ::GetLastError();
		return (errval != ERROR_FILE_NOT_FOUND)
			&& (errval != ERROR_PATH_NOT_FOUND)
			&& (errval != ERROR_INVALID_NAME)
			&& (errval != ERROR_INVALID_DRIVE)
			&& (errval != ERROR_NOT_READY)
			&& (errval != ERROR_INVALID_PARAMETER)
			&& (errval != ERROR_BAD_PATHNAME)
			&& (errval != ERROR_BAD_NETPATH);
	}
	return true;
}
*/

static std::wstring JoinPath(const std::wstring &path1, const std::wstring &path2)
{
	std::wstring path = path1;
	if(path.size() > 0 && path.back() != L'\\')
	{
		if(path2.size() > 0 && path2.front() != L'\\')
			path += L'\\';
	}
	return std::move(path1 + path2);
}

static bool InstallFolder(MSIHANDLE hInstall, std::wstring& install_folder, bool find_by_reg)
{
	MSI msi(hInstall);
	DWORD length = MAX_PATH;
	install_folder = msi.InstallFolder();
	if(install_folder.size() == 0)
	{
		if(find_by_reg)
		{
			install_folder.resize(MAX_PATH);
			auto rc = ::RegGetValueW(HKEY_CLASSES_ROOT,
									 L"CLSID\\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF1}\\InprocServer32",
									 nullptr, RRF_RT_REG_SZ | KEY_WOW64_64KEY, nullptr, static_cast<void *>(install_folder.data()), &length);
			if(rc == ERROR_SUCCESS)
			{
				install_folder.resize(length / sizeof(wchar_t));
				auto p = install_folder.find_last_of(L"\\");
				if(p != install_folder.npos)
				{
					install_folder = std::move(install_folder.substr(0, p + 1));
				}
			}
		}
	}
	return install_folder.size() > 0;
}
/*
bool start(MSIHANDLE hInstall, const wchar_t *parameters, bool run_as_admin, bool wait, bool find_dir_by_reg)
{
	auto result = false;
	MSI msi(hInstall);
	std::wstring install_folder;
	if(InstallFolder(hInstall, install_folder, find_dir_by_reg))
	{
		std::wstring sh = std::move(JoinPath(install_folder, FILEEXE));
		result = ShellExec(JoinPath(install_folder, FILEEXE).c_str(), 
							parameters, install_folder.c_str(), true, SW_HIDE, true);
		std::wstring old = std::move(JoinPath(install_folder, FILEOLD));
		if(::PathFileExistsW(old.c_str()))
			::DeleteFileW(old.c_str());
	}
	return result;
}
*/
//MsiGetProperty (hMSI, "CustomActionData", strOurVersion, numSize);
//::MsiSetPropertyW(hInstall, L"MSIRESTARTMANAGERCONTROL", L"Disable");

//constexpr auto UninstallKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";

UINT __stdcall Install(MSIHANDLE hInstall)
{
	std::wstring install_folder;
	if(InstallFolder(hInstall, install_folder, false))
	{
		ShellExec(JoinPath(install_folder, FILEEXE).c_str(), 
				  L"-r -s -t -restart", install_folder.c_str(), true, SW_HIDE, true);

		std::wstring old = std::move(JoinPath(install_folder, FILEOLD));
		if(::PathFileExistsW(old.c_str()))
			::DeleteFileW(old.c_str());
	}
	return ERROR_SUCCESS;
	//return ERROR_INSTALL_FAILURE;
}

UINT __stdcall Uninstall(MSIHANDLE hInstall)
{
	std::wstring install_folder;
	if(InstallFolder(hInstall, install_folder, true))
	{
		ShellExec(JoinPath(install_folder, FILEEXE).c_str(), 
				  L"-u -s -t -restart", install_folder.c_str(), true, SW_HIDE, true);
	}
	return ERROR_SUCCESS;
	//return res ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
}

UINT __stdcall Update(MSIHANDLE hInstall)
{
	std::wstring install_folder;
	if(InstallFolder(hInstall, install_folder, true))
	{
		std::wstring dll = std::move(JoinPath(install_folder, FILEDLL));
		std::wstring old = std::move(JoinPath(install_folder, FILEOLD));

		if(::PathFileExistsW(dll.c_str()))
			::MoveFileW(dll.c_str(), old.c_str());

		if(::PathFileExistsW(old.c_str()))
			::DeleteFileW(old.c_str());
	}
	return ERROR_SUCCESS;
}

UINT __stdcall ValidatePath(MSIHANDLE hInstall)
{
	HRESULT hr = S_OK;

	BOOL fInstallPathIsRemote = TRUE;
	BOOL fInstallPathIsRemoveable = TRUE;

	MSI msi(hInstall);
	std::wstring pwszWixUIDir, pwszInstallPath;
	if(!msi.get(L"WIXUI_INSTALLDIR", pwszWixUIDir))
	{
		log("failed to get WixUI Installation Directory");
		return ERROR_INSTALL_FAILURE;
	}

	if(!msi.get(pwszWixUIDir.c_str(), pwszInstallPath))
	{
		log("failed to get Installation Directory");
		return ERROR_INSTALL_FAILURE;
	}

	std::wstring pStrippedTargetFolder = pwszInstallPath;

	// Terminate the path at the root
	if(!::PathStripToRootW(pStrippedTargetFolder.data()))
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE);
		if(FAILED(hr))
		{
			log("failed to parse target folder");
			return hr;
		}
	}

	auto drive_type = ::GetDriveTypeW(pStrippedTargetFolder.c_str());

	fInstallPathIsRemote = (DRIVE_REMOTE == drive_type);
	fInstallPathIsRemoveable = ((DRIVE_CDROM == drive_type) || (DRIVE_REMOVABLE == drive_type) || (DRIVE_RAMDISK == drive_type) || (DRIVE_UNKNOWN == drive_type));

	// If the path does not point to a network drive, mapped drive, or removable drive,
	// then set WIXUI_INSTALLDIR_VALID to "1" otherwise set it to 0
	BOOL fInstallPathIsUnc = ::PathIsUNCW(pwszInstallPath.c_str());
	if(!fInstallPathIsUnc && !fInstallPathIsRemote && !fInstallPathIsRemoveable)
	{
		// path is valid
		if(!msi.set(L"WIXUI_INSTALLDIR_VALID", L"1"))
		{
			log("failed to set WIXUI_INSTALLDIR_VALID");
			return ERROR_INSTALL_FAILURE;
		}
	}
	else
	{
		// path is invalid; we can't log it because we're being called from a DoAction control event
		// but we can at least call WcaLog to get it to write to the debugger from a debug build
		log("Installation path %ls is invalid: it is %s UNC path, %s remote path, or %s path on a removable drive, and must be none of these.",
			pwszInstallPath.c_str(), fInstallPathIsUnc ? "a" : "not a", fInstallPathIsRemote ? "a" : "not a", fInstallPathIsRemoveable ? "a" : "not a");

		if(!msi.set(L"WIXUI_INSTALLDIR_VALID", L"0"))
		{
			log("failed to set WIXUI_INSTALLDIR_VALID");
			return ERROR_INSTALL_FAILURE;
		}
	}
	return ERROR_SUCCESS;
}

extern "C" BOOL WINAPI DllMain(IN HINSTANCE hInst, IN ULONG ulReason, IN LPVOID)
{
	switch(ulReason)
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}


