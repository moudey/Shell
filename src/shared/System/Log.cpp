
//#include <time.h>
#include <system.h>
//#include <Globals.h>
#include <Resource.h>
//#include <mutex>

namespace Nilesoft
{
	using namespace Text;

	// A mutex to protect the display resource.
	// Initialization of the display protection mutex.
	std::mutex _mutex; // mutex for critical section

	string Logger::GetPath()
	{
		string path = IO::Path::Module(CurrentModule).move();
		auto len = path.length();
		if(len > 4)
			path = (path.substr(0, len - 3) + L"log").move();
		return path.move();
	}

	string Logger::Now()
	{
		string date_time(128, '\0');
		auto now = time(nullptr);
		tm ts{};
		if(0 == ::localtime_s(&ts, &now))
		{
			auto l = (size_t)::wcsftime(&date_time[0], date_time.capacity(), L"%Y-%m-%d %X", &ts);
			date_time.release(l);
		}
		return date_time.move();
	}

	void Logger::close()
	{
		if(hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	bool Logger::open(const wchar_t *path)
	{
		_path = path;
		return open();
	}

	bool Logger::reset()
	{
		close();
		return open(CREATE_NEW);
	}

	bool close_if_not_new = false;
	bool Logger::open(uint32_t creation)
	{
		if(hFile != INVALID_HANDLE_VALUE)
			return true;

		hFile = ::CreateFileW(_path.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE,
							  nullptr, creation,
							  FILE_ATTRIBUTE_NORMAL, nullptr);

		if(hFile == INVALID_HANDLE_VALUE)
		{
			auto error = ::GetLastError();
			if(error == ERROR_ALREADY_EXISTS ||
			   error == ERROR_FILE_EXISTS)
			{
				return open(OPEN_EXISTING);
			}

			hFile = INVALID_HANDLE_VALUE;

			return false;
		}
		else if(creation == CREATE_NEW)
		{
			Security::Permission::SetFile(_path.c_str());

			uint8_t smarker[2] = {0xFF, 0xFE};
			::WriteFile(hFile, smarker, 2, nullptr, nullptr);

			string msg;
			msg.append(APP_FULLNAME);
			msg.append_format(L"\r\nversion %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			if constexpr(VERSION_REV > 0)
				msg.append_format(L".%d", VERSION_REV);
			msg.append_format(L" %s\r\n", APP_PROCESS);
			msg.append_format(L"%s\r\n", APP_EMAIL);
			msg.append_format(L"%s\r\n", APP_WEBSITELINK);
			//msg.append_format(L"Create on %s\r\n", Now().c_str());
			auto ver = &Windows::Version::Instance();
			msg.append_format(L"\r\n%s, version %s\r\n", ver->Name.c_str(), ver->BuildNumber.c_str());
			msg.append_format(L"%s\r\n", string(80, '-').c_str());
			close_if_not_new = false;
			_write(LogLevel::Write, msg.c_str());
		}
		close_if_not_new = true;
		::SetFilePointer(hFile, 0, nullptr, FILE_END);
		return true;
	}

	std::mutex mutex;

	bool Logger::_write(LogLevel logLevel, const wchar_t *message, ...)
	{
		auto result = false;
		if(!message || !open()) return result;

		// Waiting for output access.
		std::lock_guard<std::mutex> lock(mutex);
		string msg;

		va_list argList;
		va_start(argList, message);
		msg.format(argList, message);
		va_end(argList);

		if(!msg.empty())
		{
			string str;
			if(logLevel == LogLevel::Write)
				str = msg.move();
			else if(logLevel == LogLevel::None)
				str.format(L"\r\n%s %s", Now().c_str(), msg.c_str());
			else if(logLevel != LogLevel::Debug)
				str.format(L"\r\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg.c_str());
#ifdef _DEBUG
			else
				str.format(L"\r\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg.c_str());

			if(logLevel == LogLevel::Error)// || logLevel == LogLevel::Debug)
				::MessageBeep(MB_ICONERROR);
			else if(logLevel == LogLevel::Warning)
				::MessageBeep(MB_ICONWARNING);
#endif
			result =::WriteFile(hFile, str.c_str(),
							   uint32_t(str.length() * sizeof(wchar_t)), nullptr, nullptr);
		}

		if(close_if_not_new)
			this->close();

		return result;
	}

	/*	bool Logger::_write(LogLevel logLevel, const wchar_t *msg)
		{
			// Waiting for output access.
			//std::lock_guard<std::mutex> lock(mutex);
			if(msg)
			{
				if(!open()) return false;

				std::wstring str;
				if(logLevel == LogLevel::Write)
					str = msg;
				else if(logLevel == LogLevel::None)
					string::Format(str, L"\n%s %s", Now().c_str(), msg);
				else if(logLevel != LogLevel::Debug)
					string::Format(str, L"\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg);
	#ifdef _DEBUG
				else
					string::Format(str, L"\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg);
	#endif

	#ifdef _DEBUG
				if(logLevel == LogLevel::Error)// || logLevel == LogLevel::Debug)
					::MessageBeep(MB_ICONERROR);
				else if(logLevel == LogLevel::Warning)
					::MessageBeep(MB_ICONWARNING);
	#endif
				return ::WriteFile(hFile, str.c_str(),
								   uint32_t(str.length() * sizeof(wchar_t)), nullptr, nullptr);
			}
			return false;
		}*/

		/*
		template<class... Types>
		struct count_args
		{
			static const auto value = (uint32_t)sizeof...(Types);
		};
				template<typename ...Args>
				bool write(LogLevel logLevel, const wchar_t *message_format, const Args& ... args)
				{
					if(message_format == nullptr) return false;

					// Waiting for output access.
					//std::lock_guard<std::mutex> lock(mutex);

					string msg;
					//constexpr auto size = Debug<sizeof(Args)...>::value;
					//const std::size_t n = sizeof...(T); //you may use `constexpr` instead of `const`
					//auto n = sizeof...(args);
					//constexpr auto n = (sizeof(Args) + ..);//c++17
					auto n = sizeof...(Args);
					if(n > 0)
					{
						const auto z = 260;
						msg.capacity(z);

						auto size = string::StringPrintF(msg.data(), z, message_format, args ...);

						if(size > z)
						{
							msg.capacity(size);
							size = string::StringPrintF(msg.data(), z + 1, message_format, args ...);
						}

						if(size == 0) return false;
						msg.release(size);
					}
					else
					{
						msg = message_format;
					}

					if(msg.empty())
					{
						if(!open()) return false;

						string str;

						if(logLevel == LogLevel::Write)
							str = msg;
						else if(logLevel == LogLevel::None)
							string::Format(str, L"\n%s %s", Now().c_str(), msg.c_str());
						else if(logLevel != LogLevel::Debug)
							string::Format(str, L"\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg.c_str());
		#ifdef _DEBUG
						else
							string::Format(str, L"\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg.c_str());
		#endif

		#ifdef _DEBUG
						if(logLevel == LogLevel::Error)// || logLevel == LogLevel::Debug)
							::MessageBeep(MB_ICONERROR);
						else if(logLevel == LogLevel::Warning)
							::MessageBeep(MB_ICONWARNING);
		#endif
						return ::WriteFile(hFile, str.c_str(),
										   uint32_t(str.length() * sizeof(wchar_t)), nullptr, nullptr);
						return true;
					}
					return false;
				}
		*/

		/*
		bool write(LogLevel logLevel, const wchar_t *message_format, ...)
				{
					if(message_format == nullptr) return false;

					// Waiting for output access.
					//std::lock_guard<std::mutex> lock(mutex);
					string msg;

					va_list argList;
					va_start(argList, message_format);
					msg.format(argList, message_format);
					va_end(argList);

					if(msg.empty())
					{
						if(!open()) return false;

						string str;

						if(logLevel == LogLevel::Write)
							str = msg;
						else if(logLevel == LogLevel::None)
							str.format(L"\n%s %s", Now().c_str(), msg.c_str());
						else if(logLevel != LogLevel::Debug)
							str.format(L"\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg.c_str());
		#ifdef _DEBUG
						else
							str.format(L"\n%s [%s] %s", Now().c_str(), sLogLevel[(int)logLevel], msg.c_str());
		#endif

		#ifdef _DEBUG
						if(logLevel == LogLevel::Error)// || logLevel == LogLevel::Debug)
							::MessageBeep(MB_ICONERROR);
						else if(logLevel == LogLevel::Warning)
							::MessageBeep(MB_ICONWARNING);
		#endif
						return ::WriteFile(hFile, str.c_str(),
										   uint32_t(str.length() * sizeof(wchar_t)), nullptr, nullptr);
						return true;
					}
					return false;
				}
		*/
}