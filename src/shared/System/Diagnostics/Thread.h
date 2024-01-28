#pragma once

namespace Nilesoft
{
	namespace Diagnostics
	{
		/*class MyClass
		{
			static DWORD WINAPI StaticThreadStart(void *Param)
			{
				MyClass *This = (MyClass *)Param;
				return This->ThreadStart();
			}

			DWORD ThreadStart(void)
			{
				// Do stuff
			}

			void startMyThread()
			{
				DWORD ThreadID;
				CreateThread(NULL, 0, StaticThreadStart, (void *)this, 0, &ThreadID);
			}
		};
		*/
		/*
		bool thread_done = false;
		void *thread_function(void *arg)
		{
			void *res = nullptr;

			try
			{
				res = real_thread_function(arg);
			}
			catch (...)
			{
			}

			thread_done = true;

			return res;
		}


			void threadFunc() {
				// your function here
				onThreadExit(std::this_thread::get_id());
			}
			void onThreadExit(std::thread::id id) {
				std::lock_guard<std::mutex> lk(some_global_mutex);//you will have concurency here, so protect it
				// enter your exit code here
			}
		*/
		class thread
		{
			HANDLE handle{ nullptr };

		public:
			thread()
			{
			}

			template<typename F, typename T>
			thread(F func, T args = nullptr)
			{
				create<F, T>(func, args);
			}

			~thread()
			{
				detach();
			}

			operator HANDLE() const { return handle; }
			operator bool() const { return handle != nullptr; }

			template<typename F, typename T>
			thread &operator()(F func, T args = nullptr)
			{
				return create<F, T>(func, args);
			}

			template<typename F, typename T>
			thread& create(F func, T args = nullptr)
			{
				handle = reinterpret_cast<HANDLE>(::_beginthreadex(
					nullptr, 0, reinterpret_cast<_beginthreadex_proc_type>(func), reinterpret_cast<void *>(args), 0, nullptr));
				return *this;
			}

			bool join() 
			{
				if(!handle) return false;

				unsigned long res;
				if(::WaitForSingleObjectEx(handle, INFINITE, FALSE) == WAIT_FAILED || ::GetExitCodeThread(handle, &res) == 0)
					return false;
				return detach();
			}

			bool detach()
			{
				if(handle && ::CloseHandle(handle))
				{
					handle = nullptr;
					return true;
				}
				return false;
			}

			uint32_t id() const 
			{
				return ::GetCurrentThreadId();
			}

			template<typename F, typename T>
			static bool start(F func, T args = nullptr)
			{
				if(auto hThread = ::CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(func), reinterpret_cast<void *>(args), 0, nullptr))
				{	// Destroy the thread object.
					::CloseHandle(hThread);
					return true;
				}
				return false;
			}

			static void exit(uint32_t exit_code = 0)
			{
				::ExitThread(exit_code);
			}

			template<typename F, typename T>
			static bool begin(F func, T args = nullptr)
			{
				if(auto handle = ::_beginthreadex(nullptr, 0, reinterpret_cast<_beginthreadex_proc_type>(func), reinterpret_cast<void *>(args), 0, nullptr))
				{	// Destroy the thread object.
					::CloseHandle(reinterpret_cast<HANDLE>(handle));
					return true;
				}
				return false;
			}

			static void end(uint32_t exit_code = 0)
			{
				::_endthreadex(exit_code);
			}
		};
	}
}