#pragma once
#include <psapi.h>
#include <tlhelp32.h>

namespace Nilesoft
{
	namespace Diagnostics
	{
		using namespace Text;

		/*BOOL StartProcess(ATL::CStringW &Path, BOOL Wait)
		{
			return StartProcess(const_cast<LPWSTR>(Path.GetString()), Wait);;
		}

		BOOL StartProcess(LPWSTR lpPath, BOOL Wait)
		{
			PROCESS_INFORMATION pi;
			STARTUPINFOW si;
			DWORD dwRet;
			MSG msg;

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOW;

			if(!CreateProcessW(NULL, lpPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				return FALSE;
			}

			CloseHandle(pi.hThread);

			if(Wait)
			{
				EnableWindow(hMainWnd, FALSE);
			}

			while(Wait)
			{
				dwRet = MsgWaitForMultipleObjects(1, &pi.hProcess, FALSE, INFINITE, QS_ALLEVENTS);
				if(dwRet == WAIT_OBJECT_0 + 1)
				{
					while(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessageW(&msg);
					}
				}
				else
				{
					if(dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED)
						break;
				}
			}

			CloseHandle(pi.hProcess);

			if(Wait)
			{
				EnableWindow(hMainWnd, TRUE);
				SetForegroundWindow(hMainWnd);
				SetFocus(hMainWnd);
			}

			return TRUE;
		}
		*/
		class Process
		{
			//typedef HINSTANCE(WINAPI *ShellExecuteProc)(HWND hwnd, const wchar_t* lpOperation, const wchar_t* lpFile, const wchar_t* lpParameters,
			//											const wchar_t* lpDirectory, INT nShowCmd);
		public:
			static void OpenCommandPrompt(const wchar_t* directory)
			{
				STARTUPINFOW si = { };
				si.cb = sizeof(si);
				PROCESS_INFORMATION pi = {};
				if(::CreateProcessW(0, _wcsdup(L"cmd.exe"), 0, 0,
				   FALSE, CREATE_NEW_CONSOLE, 0,
				   directory, &si, &pi))
				{
					::CloseHandle(pi.hProcess);
					::CloseHandle(pi.hThread);
				}
			}

			static BOOL Start(const wchar_t *commandLine, const wchar_t *directory = nullptr, bool wait = false, int showCmd = SW_SHOWNORMAL)
			{
				PROCESS_INFORMATION processInfo = { };
				auto result = Start(nullptr, commandLine, directory, showCmd, &processInfo);
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

			static BOOL Start(const wchar_t* application, const wchar_t* parameters,
							  const wchar_t* directory, bool wait, int showCmd)
			{
				PROCESS_INFORMATION processInfo = { };
				if(!Start(application, parameters, directory, showCmd, &processInfo))
					return FALSE;
				// Wait until child process exits.
				if(wait && processInfo.hProcess)
					::WaitForSingleObject(processInfo.hProcess, INFINITE);

				// Close process and thread handles. 
				if(processInfo.hThread)
					::CloseHandle(processInfo.hThread);
				if(processInfo.hProcess)
					::CloseHandle(processInfo.hProcess);

				return TRUE;
			}

			static BOOL Start(const wchar_t *application, const wchar_t *parameters,
							  const wchar_t *directory, int showCmd,
							  LPPROCESS_INFORMATION processInfo)
			{

				STARTUPINFOW startupInfo = { };
				startupInfo.cb = sizeof(STARTUPINFOW);
				startupInfo.wShowWindow = (WORD)showCmd;
				startupInfo.dwFlags = STARTF_USESHOWWINDOW;
				string commandLine = parameters;
				/*
				if(application && parameters)
					commandLine.format(L"\"%s\" %s", application, parameters);
				*/
				return ::CreateProcessW(application,
										commandLine, nullptr, nullptr, FALSE,
										NORMAL_PRIORITY_CLASS,
										nullptr, directory,
										&startupInfo, processInfo);
			}

			static uint32_t EnumId(std::vector<DWORD> *lphProcesses)
			{
				uint32_t count = 0;
				DWORD processes[1024], cbNeeded{};
				if(::EnumProcesses(processes, sizeof(processes), &cbNeeded))
				{
					count = cbNeeded / sizeof(DWORD);
					if(lphProcesses) for(auto i = 0u; i < count; i++)
					{
						lphProcesses->push_back(processes[i]);
					}
				}
				return count;
			}

			//Get ProcessList
			static std::vector<PROCESSENTRY32W> EnumInfo()
			{
				std::vector<PROCESSENTRY32W> list;
				PROCESSENTRY32W processInfo{ sizeof(processInfo) };
				auto snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if(snapshot != INVALID_HANDLE_VALUE)
				{
					if(::Process32FirstW(snapshot, &processInfo)) do {
						list.push_back(std::move(processInfo));
						processInfo = { sizeof(processInfo) };
					} while(::Process32NextW(snapshot, &processInfo));
					::CloseHandle(snapshot);
				}
				return list;
			}

			static std::vector<HMODULE> Modules(HANDLE hProcess, DWORD dwFilterFlag = LIST_MODULES_DEFAULT)
			{
				std::vector<HMODULE> hModules;
				uint32_t count{};
				DWORD cbNeeded{};
				if(EnumProcessModulesEx(hProcess, nullptr, 0, &cbNeeded, dwFilterFlag))
				{
					count = cbNeeded / sizeof(HMODULE);
					hModules.resize(count);
					if(!EnumProcessModulesEx(hProcess, hModules.data(), cbNeeded, &cbNeeded, dwFilterFlag))
					{
						hModules.clear();
						count = 0;
					}
				}
				return hModules;
			}

			static std::vector<MODULEENTRY32W> Modules(DWORD dwProcessId)
			{
				std::vector<MODULEENTRY32W> hModules;
				auto snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
				if(snapshot != INVALID_HANDLE_VALUE)
				{
					MODULEENTRY32W me32 = { sizeof(MODULEENTRY32W) };
					if(::Module32FirstW(snapshot, &me32)) do {
						hModules.push_back(std::move(me32));
						me32 = { sizeof(MODULEENTRY32W) };
					} while(::Module32NextW(snapshot, &me32));
					::CloseHandle(snapshot);
				}
				return hModules;
			}

			/*
			static uint32_t Modules(HANDLE hProcess, std::vector<HMODULE> *lphModules = {}, DWORD dwFilterFlag = LIST_MODULES_DEFAULT)
			{
				uint32_t count{};
				DWORD cbNeeded{};
				if(EnumProcessModulesEx(hProcess, nullptr, 0, &cbNeeded, dwFilterFlag))
				{
					count = cbNeeded / sizeof(HMODULE);
					if(lphModules)
					{
						lphModules->resize(count);
						if(!EnumProcessModulesEx(hProcess, lphModules->data(), cbNeeded, &cbNeeded, dwFilterFlag))
						{
							lphModules->clear();
							count = 0;
						}
					}
				}
				return count;
			}*/

			static string ModuleBaseName(HANDLE hProcess, HMODULE hModule = {})
			{
				string name(MAX_PATH + 1);
				return name.release(::GetModuleBaseNameW(hProcess, hModule, name.data(), MAX_PATH)).move();
			}

			static string ModuleFileName(HANDLE hProcess, HMODULE hModule = {})
			{
				string name(MAX_PATH + 1);
				return name.release(::GetModuleFileNameExW(hProcess, hModule, name.data(), MAX_PATH)).move();
			}
		};

		class ProcessReference : public IUnknown
		{
		public:
			HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
			{
				if(riid == IID_IUnknown)
				{
					*ppv = static_cast<IUnknown*>(this);
					AddRef();
					return S_OK;
				}

				*ppv = NULL;
				return E_NOINTERFACE;
			}

			ULONG __stdcall AddRef()
			{
				return ::InterlockedIncrement(&m_cRef);
			}

			ULONG __stdcall Release()
			{
				LONG lRef = ::InterlockedDecrement(&m_cRef);
				if(lRef == 0)
					::PostThreadMessageW(m_dwThread, WM_NULL, 0, 0);
				return lRef;
			}

			ProcessReference()
				: m_cRef(1), m_dwThread(GetCurrentThreadId())
			{
				::SHSetInstanceExplorer(this);
			}

			~ProcessReference()
			{
				::SHSetInstanceExplorer(nullptr);
				Release();

				MSG msg;
				while(m_cRef && ::GetMessageW(&msg, nullptr, 0, 0))
				{
					::TranslateMessage(&msg);
					::DispatchMessageW(&msg);
				}
			}

		private:
			LONG m_cRef;
			DWORD m_dwThread;
		};
	}
}