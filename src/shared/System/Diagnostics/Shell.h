#pragma once

namespace Nilesoft
{
	namespace Diagnostics
	{
		using namespace Text;
	/*
	verb
	"open"			- Opens a file or a application
	"openas"		- Opens dialog when no program is associated to the extension
	"opennew"		- see MSDN
	"runas"			- In Windows 7 and Vista, opens the UAC dialog and in others, open the Run as... Dialog
	"null"			- Specifies that the operation is the default for the selected file type.
	"edit"			- Opens the default text editor for the file.
	"explore"		- Opens the Windows Explorer in the folder specified in lpDirectory.
	"properties"	- Opens the properties window of the file.
	"copy"			- see MSDN
	"cut"			- see MSDN
	"paste"			- see MSDN
	"pastelink"		- pastes a shortcut
	"delete"		- see MSDN
	"print"			- Start printing the file with the default application.
	"printto"		- see MSDN
	"find"			- Start a search
	*/
		class ShellExec
		{
			static constexpr auto m_explorer = L"explorer";
			static constexpr auto m_open = L"open";
			static constexpr auto m_open_as = L"openas";
			static constexpr auto m_run_as = L"runas";

		public:

			ShellExec() {}

			uint32_t operator()(const wchar_t *file, const wchar_t *parameters,
								const wchar_t *directory, const wchar_t *verb,
								bool wait = false, int showCmd = SW_SHOWNORMAL)
			{
				return Run(file, parameters, directory, verb, wait, showCmd);
			}

			static BOOL Explorer(const wchar_t* path)
			{
				return Run(m_explorer, path, nullptr, m_open, SW_SHOWNORMAL);
			}

			static BOOL ExplorerSelect(const wchar_t* path)
			{
				string arg;
				arg.format(L"/select,\"%s\"", path);
				return Run(m_explorer, arg, nullptr, m_open, SW_SHOWNORMAL);
			}

			static BOOL OpenAsAdmin(const wchar_t* path, const wchar_t* parameters, const wchar_t* directory = nullptr, int showCmd = SW_SHOWNORMAL)
			{
				return Run(path, parameters, directory, m_run_as, showCmd);
			}

			static BOOL Open(SHELLEXECUTEINFOW * sei)
			{
				return ::ShellExecuteExW(sei);
			}

			static BOOL Open(const wchar_t* path, const wchar_t* directory = nullptr,
							 const wchar_t* verb = m_open, int showCmd = SW_SHOWNORMAL)
			{
				return Run(path, nullptr, directory, verb, false, showCmd);
			}

			static string getpath(const wchar_t *file)
			{
				wchar_t szfile[MAX_PATH]{};
				auto len = ::SearchPathW(nullptr, file, nullptr, MAX_PATH, szfile, nullptr);
				if(len == 0)
				{
					if(!string::FindLast(file, L'.', false))
					{
						len = ::SearchPathW(nullptr, file, L".exe", MAX_PATH, szfile, nullptr);
						if(len == 0)
							len = ::SearchPathW(nullptr, file, L".cmd", MAX_PATH, szfile, nullptr);
						if(len == 0)
							len = ::SearchPathW(nullptr, file, L".bat", MAX_PATH, szfile, nullptr);
					}
				}
				return len == 0 ? file : szfile;
			}

			static uint32_t Run(const wchar_t* file, const wchar_t* parameters,
							 const wchar_t* directory, const wchar_t* verb, 
							 bool wait = false, int showCmd = SW_SHOWNORMAL)
			{
				bool openas = false;

				string f = getpath(file).move();

				if(f.is_null_whitespace())
					return FALSE;

				SHELLEXECUTEINFOW sei = { };
				sei.cbSize = sizeof(SHELLEXECUTEINFOW);
				sei.fMask = SEE_MASK_ASYNCOK;

				if(!string::IsNullOrWhiteSpace(verb))
				{
					sei.lpVerb = verb;
					if(string::Equals(verb, m_open_as))
					{
						sei.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_INVOKEIDLIST;
						openas = true;
					}
				}

				if(!openas)
					sei.fMask = SEE_MASK_NOCLOSEPROCESS;

				sei.fMask |= SEE_MASK_FLAG_NO_UI;

				sei.lpFile = f;
				if(!string::IsNullOrWhiteSpace(parameters))
					sei.lpParameters = parameters;

				if(!string::IsNullOrWhiteSpace(directory))
					sei.lpDirectory = directory;

				sei.nShow = showCmd;

				//Open the file using its associated program.
				auto ret = ::ShellExecuteExW(&sei);
				if(ret == FALSE)
				{
					auto dwError = ::GetLastError();
					if(dwError == ERROR_CANCELLED && string::Equals(verb, L"runas"))
					{
						// The operation was canceled by the user.
						// The user refused to allow privileges elevation.
						Logger::Debug(L"User did not allow elevation '%s'.", file);
					}
					//if(sei.fMask & SEE_MASK_NOCLOSEPROCESS)
					// The function failed, so report the error.
					// Err.LastDllError could also be used instead, if you wish.
					else switch(uintptr_t(sei.hInstApp))
					{
						//case ERROR_FILE_NOT_FOUND:
						case SE_ERR_FNF:
							Logger::Debug(L"Command File not found '%s'.", file);
							break;
							//case ERROR_PATH_NOT_FOUND:
						case SE_ERR_PNF:
							Logger::Debug(L"Command Path not found '%s'.", file);
							break;
						case SE_ERR_ACCESSDENIED:
							Logger::Debug(L"Command Access denied.");
							break;
						case SE_ERR_OOM:
							Logger::Debug(L"Command Out of memory.");
							break;
						case SE_ERR_DLLNOTFOUND:
							Logger::Debug(L"Command Dynamic-link library not found.");
							break;
						case SE_ERR_NOASSOC:
							Logger::Debug(L"Command No program is associated file.");
							break;
						default:
							Logger::Debug(L"Command An unexpected error occured.");
							break;
					}

					if(sei.hProcess)
						::CloseHandle(sei.hProcess);
					return FALSE;
				}

				ret = FALSE;

				if(sei.hProcess)
				{
					if(openas || wait)
					{
						DWORD dwWaitResult = 0;
						// Wait until child process exits.
						//WaitMsgQueue(sei.hProcess);
						// no time-out interval
						dwWaitResult = ::WaitForSingleObject(sei.hProcess, INFINITE);
						//or
						// Wait until child process exits.
						// Wait for the opened process to close before continuing.  Instead
						//of waiting once for a time of INFINITE, this example repeatedly checks to see if the
						//is still open.  This allows the DoEvents VB function to be called, preventing
						//our program from appearing to lock up while it waits.
						/*do //DoEvents
						{
							dwWaitResult = ::WaitForSingleObject(sei.hProcess, 0);
						} while(dwWaitResult == WAIT_TIMEOUT);
						
						// do threaded work
						while(::WaitForSingleObject(sei.hProcess, 1) == WAIT_TIMEOUT)
						{
							// do thread busy work
						}*/

						if(dwWaitResult == WAIT_OBJECT_0)
						{
							// child process end normal
							//Logger::Debug(L"child process end normal");
						}
					}

					DWORD exit_code = 0; //child process exit code.
					if(::GetExitCodeProcess(sei.hProcess, &exit_code))
					{
						if(exit_code != STILL_ACTIVE)
						{
							Logger::Debug(L"exit code = %d", exit_code);
							ret = exit_code;
						}
					}
					::CloseHandle(sei.hProcess);
				}

				return ret;
			}

			static BOOL OpenAndWait(const wchar_t* file, const wchar_t* parameters,
									const wchar_t* directory, const wchar_t* verb, int showCmd = SW_SHOWNORMAL)
			{
				SHELLEXECUTEINFOW sei = { };
				sei.cbSize = sizeof(SHELLEXECUTEINFO);

				sei.fMask = SEE_MASK_NOCLOSEPROCESS;
				sei.lpFile = getpath(file);
				sei.lpVerb = verb;
				sei.lpParameters = parameters;
				sei.lpDirectory = directory;
				sei.nShow = showCmd;
				if(::ShellExecuteExW(&sei))
				{
					if(sei.hProcess)
					{	// Wait until child process exits.
						WaitMsgQueue(sei.hProcess);
						::CloseHandle(sei.hProcess);
					}
					return TRUE;
				}
				return FALSE;
			}

			static void WaitMsgQueue(HANDLE handle)
			{
				MSG msg;
				DWORD dw;
				while(true)
				{
					dw = ::MsgWaitForMultipleObjects(1, &handle, FALSE, INFINITE, QS_ALLINPUT);
					switch(dw)
					{
						case WAIT_OBJECT_0 + 1:
							// we have a message - peek and dispatch it
							while(::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
							{
								if(msg.message == WM_QUIT)
									return;
								::DispatchMessageW(&msg);
							}
							break;
						default:
							return; // unexpected failure
					}
				}
			}

			// Wait while processing messages 
			static DWORD WaitMsgQueue(const HANDLE * pHandles, DWORD nCount, bool fWaitAll = true)
			{
				DWORD dw;
				MSG msg;
				DWORD i;
				HANDLE myHandles[MAXIMUM_WAIT_OBJECTS]{};
				unsigned int myCount;

				// Initialize handle array
				for(i = 0; i < nCount; i++)
				{
					myHandles[i] = pHandles[i];
				}

				myCount = nCount;

				while(true)
				{
					dw = ::MsgWaitForMultipleObjects(myCount, myHandles, false, INFINITE, QS_ALLINPUT);
					// Object got signaled
					if(dw < (WAIT_OBJECT_0 + myCount))
					{
						if(!fWaitAll)
							return dw;
						// Remove object
						i = dw - WAIT_OBJECT_0;
						myCount--;
						for(unsigned int j = i; j < myCount - 1 && myCount > 0; j++)
							myHandles[j] = myHandles[j + 1];
						// Exit if all objects have been signaled
						if(myCount == 0)
							return WAIT_OBJECT_0;
					}
					else if(dw == (WAIT_OBJECT_0 + nCount))
					{
						while(::PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
						{
							if(msg.message == WM_QUIT)
								return dw;
							::DispatchMessageW(&msg);
						}
					}
					else
						return dw;
				}
				return dw;
			}
		};

		/*static int run_program(const std::wstring &command)
		{
			STARTUPINFOW si{};
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi{};

			// Allegedly CreateProcess overwrites the command line. Ugh.
			std::wstring mutable_command(command);
			if(::CreateProcessW(nullptr, &mutable_command[0],
							 nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
			{
				::WaitForSingleObject(pi.hProcess, INFINITE);
				DWORD exit_code;
				bool got_exit_code = !!::GetExitCodeProcess(pi.hProcess, &exit_code);
				::CloseHandle(pi.hProcess);
				::CloseHandle(pi.hThread);
				// Not 100% sure about this still active thing is correct,
				// but I'm going with it because I *think* WaitForSingleObject might
				// return in some cases without INFINITE-ly waiting.
				// But why/wouldn't GetExitCodeProcess return false in that case?
				if(got_exit_code && exit_code != STILL_ACTIVE)
					return static_cast<int>(exit_code);
			}
			return EXIT_FAILURE;
		}*/
	}
}

/*
SHELLEXECUTEINFO rSEI = { 0 };
	rSEI.cbSize = sizeof(rSEI);
	//rSEI.lpVerb = "runas";
	rSEI.lpVerb = "open";
	rSEI.lpFile = "python.Exe";
	rSEI.lpParameters = LPCSTR(path.c_str());
	rSEI.nShow = SW_NORMAL;
	rSEI.fMask = SEE_MASK_NOCLOSEPROCESS;

	if (ShellExecuteEx(&rSEI))   // you should check for an error here
			;
		else
			errorMessageID = GetLastError();        //MessageBox("Error", "Status", 0);

		WORD nStatus;
		MSG msg;     // else process some messages while waiting...
		while (TRUE)
		{
			nStatus = MsgWaitForMultipleObjects(1, &rSEI.hProcess, FALSE, INFINITE, QS_ALLINPUT);   // drop through on user activity

			if (nStatus == WAIT_OBJECT_0)
			{  // done: the program has ended
				break;
			}

			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&msg);
				//MessageBox("Wait...", "Status", 0);
			}

		}  // launched process has exited

		DWORD dwCode=0;
		if (!GetExitCodeProcess(rSEI.hProcess, &dwCode)) //errorvalue
		{
			DWORD lastError = GetLastError();
		}
*/
/*
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Shellapi.h>



//Note:
//	The exitCode for a "Cmd Process" is not the exitCode
//	for a sub process launched from it!  That can be retrieved
//	via the errorlevel variable in the command line like so:
//	set errorlevel=&[launch command]&echo.&echo exitCode=%errorlevel%&echo.
//	The stdOut vector will then contain the exitCode on a seperate line

			BOOL executeCommandLine(const CStringW &command,
									DWORD &exitCode,
									const BOOL asCmdProcess = FALSE,
									std::vector<CStringW> *stdOutLines = NULL)
			{
				// Init return values
				BOOL bSuccess = FALSE;
				exitCode = 0;
				if(stdOutLines) stdOutLines->clear();

				// Optionally prepend cmd.exe to command line to execute
				CStringW cmdLine((asCmdProcess ? L"cmd.exe /C " : L"") +
								 command);

				// Create a pipe for the redirection of the STDOUT
				// of a child process.
				HANDLE g_hChildStd_OUT_Rd = NULL;
				HANDLE g_hChildStd_OUT_Wr = NULL;
				SECURITY_ATTRIBUTES saAttr;
				saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
				saAttr.bInheritHandle = TRUE;
				saAttr.lpSecurityDescriptor = NULL;
				bSuccess = CreatePipe(&g_hChildStd_OUT_Rd,
									  &g_hChildStd_OUT_Wr, &saAttr, 0);
				if(!bSuccess) return bSuccess;
				bSuccess = SetHandleInformation(g_hChildStd_OUT_Rd,
												HANDLE_FLAG_INHERIT, 0);
				if(!bSuccess) return bSuccess;

				// Setup the child process to use the STDOUT redirection
				PROCESS_INFORMATION piProcInfo;
				STARTUPINFO siStartInfo;
				ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
				ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
				siStartInfo.cb = sizeof(STARTUPINFO);
				siStartInfo.hStdError = g_hChildStd_OUT_Wr;
				siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
				siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

				// Execute a synchronous child process & get exit code
				bSuccess = CreateProcess(NULL,
										 cmdLine.GetBuffer(),  // command line
										 NULL,                 // process security attributes
										 NULL,                 // primary thread security attributes
										 TRUE,                 // handles are inherited
										 0,                    // creation flags
										 NULL,                 // use parent's environment
										 NULL,                 // use parent's current directory
										 &siStartInfo,         // STARTUPINFO pointer
										 &piProcInfo);        // receives PROCESS_INFORMATION
				if(!bSuccess) return bSuccess;
				WaitForSingleObject(piProcInfo.hProcess, (DWORD)(-1L));
				GetExitCodeProcess(piProcInfo.hProcess, &exitCode);
				CloseHandle(piProcInfo.hProcess);
				CloseHandle(piProcInfo.hThread);

				// Return if the caller is not requesting the stdout results
				if(!stdOutLines) return TRUE;

				// Read the data written to the pipe
				DWORD bytesInPipe = 0;
				while(bytesInPipe == 0)
				{
					bSuccess = PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL,
											 &bytesInPipe, NULL);
					if(!bSuccess) return bSuccess;
				}
				if(bytesInPipe == 0) return TRUE;
				DWORD dwRead;
				CHAR *pipeContents = new CHAR[bytesInPipe];
				bSuccess = ReadFile(g_hChildStd_OUT_Rd, pipeContents,
									bytesInPipe, &dwRead, NULL);
				if(!bSuccess || dwRead == 0) return FALSE;

				// Split the data into lines and add them to the return vector
				std::stringstream stream(pipeContents);
				std::string str;
				while(getline(stream, str))
					stdOutLines->push_back(CStringW(str.c_str()));

				return TRUE;
			}
*/
/*
PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter
STARTUPINFO StartupInfo; //This is an [in] parameter
ZeroMemory(&StartupInfo, sizeof(StartupInfo));
StartupInfo.cb = sizeof StartupInfo ; //Only compulsory field
if(CreateProcess("c:\\winnt\\notepad.exe", NULL,
	NULL,NULL,FALSE,0,NULL,
	NULL,&StartupInfo,&ProcessInfo))
{
	WaitForSingleObject(ProcessInfo.hProcess,INFINITE);
	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);
}
else
{
	MessageBox("The process could not be started...");
}
			* */

			/*if(0 == CreateProcess(argv[2], params, NULL, NULL, false, 0, NULL, NULL, &si, &pi))
			{
				//runas word is a hack to require UAC elevation
				ShellExecute(NULL, "runas", argv[2], params, NULL, SW_SHOWNORMAL);
			}*/

			/*
SHELLEXECUTEINFO lpExecInfo={};
lpExecInfo.cbSize  = sizeof(SHELLEXECUTEINFO);
lpExecInfo.lpFile = AdobeAcrobat // name of file that you want to execute/ print/ or open/ in your case Adobe Acrobat.
lpExecInfo.fMask=SEE_MASK_DOENVSUBST|SEE_MASK_NOCLOSEPROCESS ;
lpExecInfo.hwnd = NULL;
lpExecInfo.lpVerb = "open"; // to open  program
lpExecInfo.lpParameters = fileName; //  file name as an argument
lpExecInfo.lpDirectory = NULL;
lpExecInfo.nShow = SW_SHOW ;  // show command prompt with normal window size
lpExecInfo.hInstApp = (HINSTANCE) SE_ERR_DDEFAIL ;   //WINSHELLAPI BOOL WINAPI result;
ShellExecuteEx(&lpExecInfo);


  //wait until a file is finished printing
  if(lpExecInfo.hProcess !=NULL)
  {
	::WaitForSingleObject(lpExecInfo.hProcess, INFINITE);
	::CloseHandle(lpExecInfo.hProcess);
  }
			*/