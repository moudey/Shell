#pragma once

namespace Nilesoft
{
	class DLL
	{
		HMODULE m_handle = nullptr;
		const wchar_t* m_dllPath = nullptr;
		bool loadLibrary = false;

	public:
		DLL() noexcept {}
		DLL(HMODULE handel) :m_handle(handel) {}
		DLL(const wchar_t* dllPath, bool load_as_data = false) 
			: m_dllPath(dllPath), m_handle(nullptr) 
		{
			load(load_as_data);
		}
		~DLL()
		{
			if(loadLibrary && m_handle)
				::FreeLibrary(m_handle);
		}

		HMODULE handel() { return m_handle; }
		operator HMODULE() { return m_handle; }

		explicit operator bool() const { return m_handle != nullptr; }

		bool load(bool load_as_data = false)
		{
			if(m_handle)
				return true;

			if(m_dllPath)
			{
				m_handle = ::GetModuleHandleW(m_dllPath);
				if(!m_handle)
				{
					if(load_as_data)
						m_handle = ::LoadLibraryExW(m_dllPath, nullptr, LOAD_LIBRARY_AS_DATAFILE);
					else
						m_handle = ::LoadLibraryW(m_dllPath);

					loadLibrary = true;
				}
				return m_handle != nullptr;
			}
			return false;
		}

		void unload()
		{
			if(m_handle && loadLibrary)
			{
				loadLibrary = false;
				::FreeLibrary(m_handle);
			}
			m_handle = nullptr;
		}

		bool is_func(const char* lpProcName)
		{
			if(m_handle)
				return ::GetProcAddress(m_handle, lpProcName) != nullptr;
			return false;
		}

		template<typename RET = int, typename... Args>
		RET invoke(const char *lpProcName, Args... arguments)
		{
			RET ret{};
			if(m_handle && lpProcName)
			{
				if(auto lpFunc = ::GetProcAddress(m_handle, lpProcName); lpFunc)
				{
					using func_call = RET(__stdcall *)(Args... args);
					ret = ((func_call)lpFunc)(arguments...);
				}
			}
			return ret;
		}

		static bool Load(const wchar_t* dllPath, HMODULE &hModule)
		{
			hModule = ::LoadLibraryW(dllPath);
			return (hModule != nullptr);
		}

		static bool Unload(HMODULE hModule)
		{
			if(hModule)
				return !!::FreeLibrary(hModule);
			return false;
		}

		template<typename T>
		T Get(const char* lpProcName)
		{
			if(lpProcName && m_handle)
				return (T)::GetProcAddress(m_handle, lpProcName);
			return nullptr;
		}

		template<typename T>
		T Get(int ordinal)
		{
			if(m_handle)
				return (T)::GetProcAddress(m_handle, MAKEINTRESOURCEA(ordinal));
			return nullptr;
		}

		template<typename T>
		bool Get(const char* lpProcName, T* lpFunc)
		{
			if(lpFunc)
			{
				*lpFunc = Get<T>(lpProcName);
				return *lpFunc != nullptr;
			}
			return false;
		}

		template<typename T>
		static T Get(const wchar_t* dllPath, const char* lpProcName)
		{
			T lpFunc = nullptr;
			if(dllPath && lpProcName)
			{
				auto hModule = ::GetModuleHandleW(dllPath);
				if(hModule)
					lpFunc = (T)::GetProcAddress(hModule, lpProcName);
				else
				{
					hModule = ::LoadLibraryW(dllPath);
					if(hModule)
					{
						lpFunc = (T)::GetProcAddress(hModule, lpProcName);
						::FreeLibrary(hModule);
					}
				}
			}
			return lpFunc;
		}

		template<typename T>
		static bool Get(const wchar_t* dllPath, const char* lpProcName, T* lpFunc)
		{
			if(lpFunc)
			{
				*lpFunc = Get<T>(dllPath, lpProcName);
				return *lpFunc != nullptr;
			}
			return false;
		}

		template<typename T>
		static T Get(HMODULE hModule, const char* lpProcName)
		{
			if(hModule && lpProcName)
				return reinterpret_cast<T>(::GetProcAddress(hModule, lpProcName));
			return nullptr;
		}

		template<typename T>
		static bool Get(HMODULE hModule, const char* lpProcName, T* lpFunc)
		{
			if(hModule && lpProcName && lpFunc)
			{
				*lpFunc = (T)::GetProcAddress(hModule, lpProcName);
				return *lpFunc != nullptr;
			}
			return false;
		}

		static bool IsFunc(HMODULE hModule, const char *lpProcName)
		{
			if(hModule)
				return ::GetProcAddress(hModule, lpProcName) != nullptr;
			return false;
		}

		static bool IsFunc(HMODULE hModule, uint32_t ordinal)
		{
			if(hModule)
				return ::GetProcAddress(hModule, MAKEINTRESOURCEA(ordinal)) != nullptr;
			return false;
		}

		template<typename RET=int, typename... Args>
		static RET Invoke(HMODULE hModule, const char *lpProcName, Args... arguments)
		{
			RET ret{};
			if(hModule && lpProcName)
			{
				if(auto lpFunc = ::GetProcAddress(hModule, lpProcName); lpFunc)
				{
					using func_call = RET(__stdcall *)(Args... args);
					ret = ((func_call)lpFunc)(arguments...);
				}
			}
			return ret;
		}

		template<typename RET = int, typename... Args>
		static RET Invoke(HMODULE hModule, uint32_t ordinal, Args... arguments)
		{
			RET ret{};
			try {
				ret = Invoke<RET>(hModule, MAKEINTRESOURCEA(ordinal), arguments...);
			}
			catch(...) {
			}
			return ret;
		}

		/*template<typename... Args>
		static void Invoke(HMODULE hModule, const char *lpProcName, Args... arguments)
		{
			if(hModule && lpProcName)
			{
				if(auto lpFunc = ::GetProcAddress(hModule, lpProcName))
				{
					using func_call = RET(__stdcall *)(Args... args);
					((func_call)lpFunc)(arguments...);
				}
			}
		}*/

		template<typename... Args>
		static void Invoke(const wchar_t*name, const char* lpProcName, Args... arguments)
		{
			if(name && lpProcName)
			{
				bool freeLibrary = false;
				auto hModule = ::GetModuleHandleW(name);
				if(!hModule)
				{
					hModule = ::LoadLibraryW(name);
					freeLibrary = true;
				}

				if(hModule)
				{
					if(auto lpFunc = ::GetProcAddress(hModule, lpProcName); lpFunc)
					{
						using func_call = void(__stdcall*)(Args... args);
						(func_call(lpFunc))(arguments...);
					}
					if(freeLibrary)
						::FreeLibrary(hModule);
				}
			}
		}

		template<typename Result, typename... Args>
		static Result Invoke(const wchar_t* name, const char* lpProcName, Args... arguments)
		{
			Result res{};
			if(name && lpProcName)
			{
				bool freeLibrary = false;
				auto hModule = ::GetModuleHandleW(name);
				if(!hModule)
				{
					hModule = ::LoadLibraryW(name);
					freeLibrary = true;
				}

				if(hModule)
				{
					using func_call = Result(__stdcall*)(Args... args);
					if(auto lpFunc = (func_call)::GetProcAddress(hModule, lpProcName); lpFunc)
					{
						res = lpFunc(arguments...);
					}
					if(freeLibrary)
						::FreeLibrary(hModule);
				}
			}
			return res;
		}

		
		static constexpr auto kernel32 = L"kernel32.dll";
		static constexpr auto user32 = L"user32.dll";

		template<typename Result, typename... Args>
		static Result Kernel32(const char *lpProcName, Args... arguments)
		{
			Result res {};
			if(lpProcName)
			{
				bool freeLibrary = false;
				auto hModule = ::GetModuleHandleW(kernel32);
				if(!hModule)
				{
					hModule = ::LoadLibraryW(kernel32);
					freeLibrary = true;
				}

				if(hModule)
				{
					using func_call = Result(__stdcall *)(Args... args);
					if(auto lpFunc = (func_call)::GetProcAddress(hModule, lpProcName); lpFunc)
					{
						res = lpFunc(arguments...);
					}

					if(freeLibrary)
						::FreeLibrary(hModule);
				}
			}
			return res;
		}

		template<typename Result, typename... Args>
		static Result User32(const char *lpProcName, Args... arguments)
		{
			Result res {};
			if(lpProcName)
			{
				bool freeLibrary = false;
				auto hModule = ::GetModuleHandleW(user32);
				if(!hModule)
				{
					hModule = ::LoadLibraryW(user32);
					freeLibrary = true;
				}

				if(hModule)
				{
					using func_call = Result(__stdcall *)(Args... args);
					if(auto lpFunc = (func_call)::GetProcAddress(hModule, lpProcName); lpFunc)
					{
						res = lpFunc(arguments...);
					}

					if(freeLibrary)
						::FreeLibrary(hModule);
				}
			}
			return res;
		}
	};
}