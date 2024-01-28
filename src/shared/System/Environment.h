#pragma once

namespace Nilesoft
{
	using namespace Text;

	class Environment
	{
	public:
		//GetEnvironmentVariable
		static const wchar_t* GetVariables()
		{
			return nullptr;
		}

		static BOOL Variable(const wchar_t* variable, const wchar_t* value)
		{
			return ::SetEnvironmentVariableW(variable, value);
		}

		static string Variable(const wchar_t* variable)
		{
			string value;
			auto size = ::GetEnvironmentVariableW(variable, value.buffer(MAX_PATH), MAX_PATH);
			if(size >= MAX_PATH)
				size = ::GetEnvironmentVariableW(variable, value.buffer(size + 1), size);
			return value.release(size).move();
		}

		static string Expand(const wchar_t *value, bool user = false)
		{
			string expands = value;
			if(user)
			{
				//auto x = DLL::Invoke<int>(L"Userenv.dll", "ExpandEnvironmentStringsForUserW", 0, 0);
				/*struct Token
				{
					HANDLE handle = nullptr;
					~Token()
					{
						if(handle)
							::CloseHandle(handle);
					}
				} token{};

				if(::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token.handle) && token.handle)
				{
					::ExpandEnvironmentStringsForUserW(token.handle, L"%USERPROFILE%", expands.buffer(MAX_PATH), MAX_PATH);
					return expands.release().move();
				}
				*/
			}
			
			if(!expands.empty())
			{
				if(expands.starts_with(L"@{", false) && expands.back(L'}', false))
				{
					if(S_OK == ::SHLoadIndirectString(value, expands.buffer(MAX_PATH), MAX_PATH, nullptr))
						return expands.release().move();
				}

				auto size = ::ExpandEnvironmentStringsW(value, expands.buffer(MAX_PATH), MAX_PATH);
				if(size >= MAX_PATH)
					size = ::ExpandEnvironmentStringsW(value, expands.buffer(size), size);
				return expands.release(size - 1).move();
			}
			return nullptr;
		}

		static int32_t RandomNumber(int32_t lowEnd, int32_t highEnd)
		{
			if(highEnd < lowEnd)
			{
				auto tempForSwap = highEnd;
				highEnd = lowEnd;
				lowEnd = tempForSwap;
			}

			//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			//std::default_random_engine generator(seed);
			//std::normal_distribution<double> distribution(0.0, 2.5);

			std::random_device rd;   // non-deterministic generator
			std::mt19937 generator(rd());  // to seed mersenne twister.
			std::uniform_int_distribution<int32_t> distribution(lowEnd, highEnd);
			return distribution(generator);
		}

/*
		static string ExtractString(uint32_t id)
		{
			return ExtractString(CurrentModule, id).move();
		}

		static string ExtractString(HMODULE module,  uint32_t id)
		{
			string str(string::MAX);
			auto length = ::LoadStringW(module, id, str.buffer(), str.capacity<int>());
			if(length > 0)
			{
				if(length >= str.capacity<int>())
				{
					str.capacity<int>(length);
					length = ::LoadStringW(module, id, str.buffer(), str.capacity<int>());
				}
				str.release(length);
			}
			return str.move();
		}

		static string ExtractString(const wchar_t *path, uint32_t id)
		{
			auto module = ::GetModuleHandleW(path);
			bool unload = false;
			if(!module)
			{
				module = ::LoadLibraryExW(path, nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
				unload = module != nullptr;
			}
			
			if(module == nullptr)
				return nullptr;

			string str = ExtractString(module, id).move();

			if(unload)
				::FreeLibrary(module);

			return str.move();
		}

		static string ExtractString(const wchar_t *path)
		{
			string p = path;
			auto id = ParseMUILocation(p);
			if(id < 0)
				id *= -1;
			return ExtractString(p.c_str(), (uint32_t)id);
		}

		static int ParseMUILocation(string &path)
		{
			int result = -1;
			if(path.length() > 3)
			{
				if(path[0] != '@')
					path.remove(0, 1);

				intptr_t p = path.last_index_of(',', false);
				if(p > 0)
				{
					result = (int)string::ToInt(&path.c_str()[p + 1], 0LL);
					path = path.substr(0, p);
				}
			}
			return result;
		}*/
	};


	/*class Time
	{
	public:
		static int TickCount()
		{
			// Something like GetTickCount but portable
			// It rolls over every ~ 12.1 days (0x100000/24/60/60)
			// Use GetMilliSpan to correct for rollover
			timeb tb;
			ftime(&tb);
			int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
			return nCount;
		}

		static int TickCount(int timeStart)
		{
			int nSpan = TickCount() - timeStart;
			if(nSpan < 0)
				nSpan += 0x100000 * 1000;
			return nSpan;
		}
	};*/
}
