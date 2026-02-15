#pragma once

//#define TRACE

namespace Nilesoft
{

/*
std::runtime_error
std::overflow_error

	double f(double d)
	{
		return d > 1e7 ? throw std::overflow_error("too big") : d;
	}
	int main()
	{
		try {
			std::cout << f(1e10) << '\n';
		} catch (const std::overflow_error& e) {
			std::cout << e.what() << '\n';
		}
	}
*/

/*
//Replacing Virtual Methods with Templates
//http://groups.di.unipi.it/~nids/docs/templates_vs_inheritance.html
class logger {
public:
  void warn(const std::string& msg){ write("WARN", msg); }
  void error(const std::string& msg){ write("ERROR", msg); }
  void info(const std::string& msg){ write("INFO", msg); }

  virtual ~logger() { }

protected:
  virtual void write( const std::string& kind, const std::string& msg ) = 0;
};

class file_logger: public logger {

public:
	file_logger(const std::string &file) { }
	~file_logger() {  }

protected:
	virtual void write(const std::string &kind, const std::string &msg)
	{
	}
};

class network_logger : public logger
{

public:
	network_logger(const std::string &address, unsigned short port) {  }
	~network_logger() {  }

protected:
	virtual void write(const std::string &kind, const std::string &msg)
	{
		
	}
};
*/

/*
template<typename writer>
class logger: public writer {
public:
  void warn(const std::string& msg){ this->write("WARN", msg); }
  void error(const std::string& msg){ this->write("ERROR", msg); }
  void info(const std::string& msg){ this->write("INFO", msg); }
};

class file_writer {
public:
	void init(const std::string &file) {  }

protected:
	void write(const std::string &kind, const std::string &msg)
	{
	}
};

class network_writer
{
public:
	void init(const std::string &address, unsigned short port) {  }

protected:
	void write(const std::string &kind, const std::string &msg)
	{
	}
};

typedef logger<file_writer> file_logger;
typedef logger<network_writer> network_logger;

logger *l = new file_logger( "foo.log" );
logger *m = new network_logger( "10.0.0.32", 8888 );

std::vector v;
v.push_back( new file_logger( "foo.log" ) );
v.push_back( new network_logger( "10.0.0.32", 8888 ) );

for( std::vector::iterator i = v.begin(); i != v.end(); ++i ) {
	(*i)->warn("something")
}

template<typename writer>
class logger {
public:
  void warn(const std::string& msg) {
	   static_cast(this)->write("WARN", msg);
  }
  //...error(), info()
};

class file_logger : public logger<file_logger>
{
public:
	void write(const std::string &kind, const std::string &msg)
	{
	}
};

class network_logger : public logger<network_logger>
{
public:
	void write(const std::string &kind, const std::string &msg)
	{
	}
};
*/
	// The different logger levels.
	enum class LogLevel : unsigned char { Error, Warning, Info, Debug, None, Write };
	
	using namespace Text;

	class Logger final
	{
		static constexpr const wchar_t *sLogLevel[6] = { L"error", L"warning", L"info", L"debug", L"", L"" };

	private:
		string _path;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		//Logger() = default;

	public:
		Logger() = default;
		Logger(const Text::string &path) : _path{ path } {};
		~Logger() { close(); }

		Logger(Logger &&) noexcept = default;
		Logger &operator=(Logger &&) noexcept = default;

		Logger(const Logger &) = delete;
		void operator=(const Logger &) = delete;

		static Logger &Instance(const Text::string &path = nullptr)
		{
			static Logger *plogger = nullptr;
			if(!plogger)
			{
				// The logger variable is initialized once with a move assignment of a logger
				// object build with the private constructor.
				static auto &&logger = Logger(path.empty() ? GetPath() : path);
				plogger = &logger;
			}
			return (*plogger);
		}

		static string Now();
		static string GetPath();

		const string &path() const { return _path; }

		void close();
		bool open(const wchar_t *path);
		bool open(uint32_t creation = CREATE_NEW);
		bool reset();
		bool _write(LogLevel logLevel, const wchar_t *message, ...);

		template <typename ...Args>
		bool write(const wchar_t *message, Args const& ...args)
		{
			return _write(LogLevel::Write, message, string::Argument(args)...);
		}

		template <typename ... Args>
		bool exception(const wchar_t *message, Args const& ...args)
		{
			string what;

			try
			{
				if(auto eptr = std::current_exception())
				{
					std::rethrow_exception(eptr);
				}
			}
			catch(const std::exception &ex)
			{
				what.append_format(L"%hs", ex.what());
			}
			catch(const std::string &ex)
			{
				what.append_format(L"%hs", ex.c_str());
			}
			catch(const char *ex)
			{
				what.append_format(L"%hs", ex);
			}
			catch(const wchar_t *ex)
			{
				what.append_format(L"%s", ex);
			}
			catch(...)
			{
				what.append(L"Unknown exception");
			}

			if(message) what = (message + what).move();
			return _write(LogLevel::Error, what.c_str(), string::Argument(args)...);
		}

		template <typename ...Args>
		bool exception(const char *message, Args const& ...args)
		{
			string what;

			try
			{
				if(auto eptr = std::current_exception())
					std::rethrow_exception(eptr);
			}
			catch(const std::exception &ex)
			{
				what.append_format(L"%hs", ex.what());
			}
			catch(const std::string &ex)
			{
				what.append_format(L"%hs", ex.c_str());
			}
			catch(const char *ex)
			{
				what.append_format(L"%hs", ex);
			}
			catch(const wchar_t *ex)
			{
				what.append_format(L"%s", ex);
			}
			catch(...)
			{
				what.append(L"Unknown exception");
			}

			if(message)
				what.append_format(L" %hs", message);

			return _write(LogLevel::Error, what.c_str(), string::Argument(args)...);
		}

		template <typename ... Args>
		bool error(const wchar_t *message, Args const& ...args)
		{
			return _write(LogLevel::Error, message, string::Argument(args)...);
		}

		template <typename ... Args>
		bool debug(const wchar_t *message, Args const& ...args)
		{
			return _write(LogLevel::Debug, message, string::Argument(args)...);
		}

		template <typename ... Args>
		bool info(const wchar_t *message, Args const& ...args)
		{
			return _write(LogLevel::Info, message, string::Argument(args)...);
		}

		template <typename ...Args>
		bool warning(const wchar_t *message, Args const& ...args)
		{
			return _write(LogLevel::Warning, message, string::Argument(args)...);
		}

		template <typename ...Args>
		bool log(const wchar_t* message, Args const& ...args)
		{
			return _write(LogLevel::None, message, string::Argument(args)...);
		}

		template <typename T, typename ...Args>
		static bool Exception(T const &message, Args const& ...args)
		{
			return Instance().exception(message, args...);
		}

		template <typename ...Args>
		static bool Error(const wchar_t *message, Args const& ...args)
		{
			return Instance()._write(LogLevel::Error, message, args...);
		}

		template <typename ...Args>
		static bool Debug(const wchar_t *message, Args const& ...args)
		{
			return Instance()._write(LogLevel::Debug, message, string::Argument(args)...);
		}

		template <typename ...Args>
		static bool Warning(const wchar_t *message, Args const& ...args)
		{
			return Instance()._write(LogLevel::Warning, message, string::Argument(args)...);
		}

		template <typename ...Args>
		static bool Info(const wchar_t *message, Args const& ...args)
		{
			return Instance()._write(LogLevel::Info, message, string::Argument(args)...);
		}

		template <typename ...Args>
		static bool Log(const wchar_t *message, Args const& ...args)
		{
			return Instance()._write(LogLevel::Write, message, string::Argument(args)...);
		}

		template <typename ...Args>
		static bool Trace([[maybe_unused]]const wchar_t *message, [[maybe_unused]]Args const& ...args)
		{
#ifdef TRACE
			return Instance()._write(LogLevel::Info, message, string::Argument(args)...);
#else
			return true;
#endif
		}

		/*template <typename ...Args>
		bool operator()(const wchar_t *message, Args const& ...args)
		{
			return Instance()._write(LogLevel::Write, message, string::Argument(args)...);
		}*/
	};
}


//#define eprintf(format, ...) fprintf (stderr, format, ##__VA_ARGS__)
//#define eprintf(format, ...) fprintf (stderr, format __VA_OPT__(,) __VA_ARGS__)

#ifdef TRACE
#define __trace(...) Logger::Trace(__VA_ARGS__)
#else
#define __trace(...)
#endif