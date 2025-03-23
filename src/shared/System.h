#pragma once

// C RunTime Header Files
#include <memory>
#include <TCHAR.h>

// Windows Header Files:
#include <windows.h>
#include <Windowsx.h>
#include <strsafe.h>
#include <thread>
#include <sys/timeb.h>
#include <time.h>
#include <ctime>
#include <aclapi.h>
#include <Shlwapi.h>
#include <shlobj.h>

#include <VersionHelpers.h>
#include <string>
#include <string_view>
#include <algorithm>


#include <thread>
#include <mutex>
#include <chrono>
//#include <future>

//#include <filesystem>
//#include <vector>
//#include <deque>
//#include <map>
#include <unordered_map>
#include <random>
#include<appmodel.h>
//#include <commoncontrols.h>
/*
#include <exception>
#include <typeinfo>
#include <stdexcept>
*/

//_WIN64 || _AMD64_ || _M_X64
//_WIN32 || _M_IX86 || _X86_
/*
_M_IX86 Defined for x86 processors.
_M_X64 Defined for x64 processors.
_WIN64 Defined for applications for Win64.
*/

/*struct hresult
{
	HRESULT _value = S_FALSE;
public:
	hresult() = default;
	hresult(HRESULT result) : _value(result) {};
	operator HRESULT() const { return _value; }
	explicit operator bool() const { return  _value >= 0; } // SUCCEEDED(_value)
	bool ok() const { return _value == 0; }
	bool failed() const { return _value < 0; }              // FAILED(_value)
	HRESULT get() const { return _value; }
};

struct lresult
{
	LRESULT _value = S_FALSE;
public:
	lresult() = default;
	lresult(LRESULT result) : _value(result) {};
	operator LRESULT() const { return _value; }
	explicit operator bool() const { return  ok(); }
	bool ok() const { return _value == 0L; }
	bool failed() const { return !ok(); }
	LRESULT get() const { return _value; }
};

struct lstatus
{
	LSTATUS _value = S_FALSE;
public:
	lstatus() = default;
	lstatus(LSTATUS result) : _value(result) {};
	operator LSTATUS() const { return _value; }
	explicit operator bool() const { return  ok(); }
	bool ok() const { return _value == 0; }
	bool failed() const { return !ok(); }
	LSTATUS get() const { return _value; }
};
*/
/*
struct DPI
{
	static constexpr double X = 96.0;
	static constexpr double Y = 96.0;

	double _dpix{ X };
	double _dpiy{ Y };

	DPI(double dpix = X, double dpiy = Y) : _dpix{ dpix }, _dpiy{ dpiy } { }
	
	template<typename T = long>
	T operator ()(auto value) const {
		return calc<T>(value);
	}

	template<typename T = long>
	T calc(auto value) const { return Value<T>(value, _dpix); }

	template<typename T = long>
	static T Value(double value, double dpi = 96)
	{
		return static_cast<T>(std::rint((value * dpi / 96.0)));
	}
};
*/

/*
std::round(context->theme->dpi->scale() * 100.0) / 100.0


round_to(10.0078, 0.001) = 10.008
round_to(10.0078, 0.01) = 10.01
round_to(10.0078, 0.1) = 10
round_to(10.0078, 1) = 10
round_to(10.0078, 2) = 10
round_to(10.0078, 3) = 9
round_to(10.0078, 4) = 12

*/ 

template<typename T = long>
struct TResult
{
	T _value = S_FALSE;
public:
	TResult(T result = S_FALSE) : _value(result) { };
	virtual operator T() const { return _value; }
	virtual operator bool() const { return ok(); }
	virtual bool ok() const { return _value == 0; }
	virtual bool failed() const { return FAILED(_value); }
	virtual bool succeeded() const { return SUCCEEDED(_value); }
	virtual T get() const { return _value; }
};

inline static double round_to(double value, double precision = 1.0)
{
	return std::round(value / precision) *precision;
}

struct COM_INITIALIZER
{
	COM_INITIALIZER(bool init = false, COINIT type = COINIT_APARTMENTTHREADED) : _type(type)
	{
		if(init)
		{
			initialize();
		}
	}

	~COM_INITIALIZER()
	{
		if(SUCCEEDED(_hr))
			::CoUninitialize();
		_hr = E_UNEXPECTED;
	}

	bool initialize(COINIT type)
	{
		_type = type;
		return initialize();
	}

	bool initialize()
	{
		_hr = ::CoInitializeEx(nullptr, _type);
		return is_initialized();
	}

	bool is_initialized()
	{
		//Checks if COM was initialized
		//RETURN:
		//      = true if COM was initialized, or
		//      = false if it was not (check GetLastError() for details)
		if(_hr == S_OK || _hr == S_FALSE)
		{
			return true;
		}

		//Error
		//::SetLastError(_hr);
		return false;
	}

private:
	HRESULT _hr{ E_UNEXPECTED };
	COINIT _type;

	//Copy constructor and assignment operators are not allowed!
	COM_INITIALIZER(COM_INITIALIZER const &) = delete;
	COM_INITIALIZER(COM_INITIALIZER const &&) = delete;
	COM_INITIALIZER &operator=(COM_INITIALIZER const &) = delete;
	COM_INITIALIZER &operator=(COM_INITIALIZER const &&) = delete;
};

template <typename Value>
struct auto_property final
{
public:
	auto_property(auto_property &&) = delete;

	auto_property(auto_property const &) = delete;

	template <typename TValue = Value>
	auto_property(TValue &&defaultValue = {}) : m_BackingField(std::forward<TValue>(defaultValue)) {}

	Value operator()() const noexcept
	{
		return m_BackingField;
	}

	template <typename TValue = Value>
	void operator()(TValue &&newValue)
	{
		m_BackingField = std::forward<TValue>(newValue);
	}
private:
	Value m_BackingField;
};

template<typename T = HGDIOBJ>
struct auto_gdi_deleter
{
	using pointer = T;
	auto operator()(T handle) const -> void {
		if(handle) ::DeleteObject(handle);
	}
};

template<typename T = HGDIOBJ>
using auto_gdi = std::unique_ptr<T, auto_gdi_deleter<T>>;

template<typename T = HANDLE>
struct handle_deleter {
	using pointer = T;
	auto operator()(T handle) const -> void {
		::CloseHandle(handle);
	}
};

template<typename T = HANDLE>
using unique_handle = std::unique_ptr<T, handle_deleter<T>>;


/*
struct hwnd_deleter {
	using pointer = HWND;
	auto operator()(HWND handle) const -> void {
		::DestroyWindow(handle);
	}
};

using unique_hwnd = std::unique_ptr<HWND, hwnd_deleter>;
*/
/*

std::wstring Exception::GetWideMessage() const
{
	using std::tr1::shared_ptr;
	shared_ptr<void> buff;
	LPWSTR buffPtr;
	DWORD bufferLength = FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetErrorCode(),
		0,
		reinterpret_cast<LPWSTR>(&buffPtr),
		0,
		NULL);
	buff.reset(buffPtr, LocalFreeHelper());
	return std::wstring(buffPtr, bufferLength);
}

/////////////////////////////
// ComException

CString FormatMessage(HRESULT result)
{
	CString strMessage;
	WORD facility = HRESULT_FACILITY(result);
	CComPtr<IErrorInfo> iei;
	if (S_OK == GetErrorInfo(0, &iei) && iei)
	{
		// get the error description from the IErrorInfo
		BSTR bstr = NULL;
		if (SUCCEEDED(iei->GetDescription(&bstr)))
		{
			// append the description to our label
			strMessage.Append(bstr);

			// done with BSTR, do manual cleanup
			SysFreeString(bstr);
		}
	}
	else if (facility == FACILITY_ITF)
	{
		// interface specific - no standard mapping available
		strMessage.Append(_T("FACILITY_ITF - This error is interface specific.  No further information is available."));
	}
	else
	{
		// attempt to treat as a standard, system error, and ask FormatMessage to explain it
		CString error;
		CErrorMessage::FormatMessage(error, result); // <- This is just a wrapper for ::FormatMessage, left to reader as an exercise :)
		if (!error.IsEmpty())
			strMessage.Append(error);
	}
	return strMessage;
}

struct hresult : public TResult<HRESULT>
{
public:
	hresult(HRESULT hr = S_FALSE) { _value = hr; }
	explicit operator bool() const override { return  _value >= 0; } // SUCCEEDED(_value)
	bool failed() const override { return _value < 0; } // FAILED(_value)
};

struct lresult : public TResult<LRESULT> { lresult(LRESULT lr = S_FALSE) { _value = lr; } };
struct lstatus : public TResult<LSTATUS> { lstatus(LSTATUS ls = S_FALSE) { _value = ls; } };
*/

namespace System
{
	namespace Math
	{
		template<class T> 
		const T &Min(const T &a, const T &b)
		{
			return (a < b) ? a : b;
		}

		template<class T> 
		const T &Max(const T &a, const T &b)
		{
			return (a < b) ? b : a;
		}

		/*uint32_t number_size(uint32_t value)
		{
			if(value == 0.00)
				return 1u;

			unsigned ret;
			double dval;
			if(value > 0) 
			{
				ret = 0;
				dval = value;
			}
			else 
			{
				// Make room for the minus sign, and proceed as if positive.
				ret = 1;
				dval = -double(value);
			}

			ret +=std::ceil(std::log10(dval + 1.0));

			return ret;
		}*/
	}
}
namespace Nilesoft
{
	enum class PrimitiveType
	{
		Null,
		Object,
		String,
		Array,
		Pointer,
		Char,
		Color,
		Number,
		Boolean,
		Int8,
		Uint8,
		Int16,
		Uint16,
		Int32,
		Uint32,
		Int64,
		Uint64,
		Float,
		Double
	};

	struct Margin
	{
		long left{};
		long top{};
		long right{};
		long bottom{};

		long height() const { return top + bottom; }
		long width() const { return left + right; }
	};
/*
	template <typename Traits>
	class unique_handle
	{
		using pointer = typename Traits::pointer;

		pointer m_value;

		auto close() throw() -> void
		{
			if(*this)
			{
				Traits::close(m_value);
			}
		}

	public:

		unique_handle(unique_handle const &) = delete;
		auto operator=(unique_handle const &)->unique_handle & = delete;

		explicit unique_handle(pointer value = Traits::invalid()) throw() :
			m_value{ value }
		{
		}

		unique_handle(unique_handle &&other) throw() :
			m_value{ other.release() }
		{
		}

		auto operator=(unique_handle &&other) throw() -> unique_handle &
		{
			if(this != &other)
			{
				reset(other.release());
			}

			return *this;
		}

		~unique_handle() throw()
		{
			close();
		}

		explicit operator bool() const throw()
		{
			return m_value != Traits::invalid();
		}

		auto get() const throw() -> pointer
		{
			return m_value;
		}

		auto get_address_of() throw() -> pointer *
		{
			return &m_value;
		}

		auto release() throw() -> pointer
		{
			auto value = m_value;
			m_value = Traits::invalid();
			return value;
		}

		auto reset(pointer value = Traits::invalid()) throw() -> bool
		{
			if(m_value != value)
			{
				close();
				m_value = value;
			}

			return static_cast<bool>(*this);
		}

		auto swap(unique_handle<Traits> &other) throw() -> void
		{
			std::swap(m_value, other.m_value);
		}
	};

	template <typename Traits>
	auto swap(unique_handle<Traits> &left,
			  unique_handle<Traits> &right) throw() -> void
	{
		left.swap(right);
	}

	template <typename Traits>
	auto operator==(unique_handle<Traits> const &left,
					unique_handle<Traits> const &right) throw() -> bool
	{
		return left.get() == right.get();
	}

	template <typename Traits>
	auto operator!=(unique_handle<Traits> const &left,
					unique_handle<Traits> const &right) throw() -> bool
	{
		return left.get() != right.get();
	}

	template <typename Traits>
	auto operator<(unique_handle<Traits> const &left,
				   unique_handle<Traits> const &right) throw() -> bool
	{
		return left.get() < right.get();
	}

	template <typename Traits>
	auto operator>=(unique_handle<Traits> const &left,
					unique_handle<Traits> const &right) throw() -> bool
	{
		return left.get() >= right.get();
	}

	template <typename Traits>
	auto operator>(unique_handle<Traits> const &left,
				   unique_handle<Traits> const &right) throw() -> bool
	{
		return left.get() > right.get();
	}

	template <typename Traits>
	auto operator<=(unique_handle<Traits> const &left,
					unique_handle<Traits> const &right) throw() -> bool
	{
		return left.get() <= right.get();
	}

	struct null_handle_traits
	{
		using pointer = HANDLE;

		static auto invalid() throw() -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) throw() -> void
		{
			::CloseHandle(value);
		}
	};

	struct invalid_handle_traits
	{
		using pointer = HANDLE;

		static auto invalid() throw() -> pointer
		{
			return INVALID_HANDLE_VALUE;
		}

		static auto close(pointer value) throw() -> void
		{
			::CloseHandle(value);
		}
	};

	using null_handle = unique_handle<null_handle_traits>;
	using invalid_handle = unique_handle<invalid_handle_traits>;
*/

	// retrieve the HINSTANCE for the current DLL or EXE using this symbol that
	// the linker provides for every module, avoids the need for a global HINSTANCE variable
	// and provides access to this value for static libraries
	extern "C" IMAGE_DOS_HEADER __ImageBase;
	const auto CurrentModule = reinterpret_cast<HMODULE>(&__ImageBase);
	//__inline HMODULE GetCurrentModule() { return reinterpret_cast<HMODULE>(&__ImageBase); }
	
/*
	__inline const HMODULE GetCurrentModule()
	{
#if _MSC_VER < 1300    // earlier than .NET compiler (VC 6.0)

		// Here's a trick that will get you the handle of the module
		// you're running in without any a-priori knowledge:
		// http://www.dotnet247.com/247reference/msgs/13/65259.aspx
		MEMORY_BASIC_INFORMATION mbi{};
		//static int dummy;
		//VirtualQuery(&dummy, &mbi, sizeof(mbi));
		::VirtualQuery(GetCurrentModule, &mbi, sizeof(mbi));
		return  reinterpret_cast<HMODULE>(mbi.AllocationBase);
#else    // VC 7.0

		// from ATL 7.0 sources
		return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
	}
*/

#define maximum_path 32767

/*
	using TCHAR = TCHAR;
	using byte = unsigned char;
	using sbyte = signed char;

	using achar_t = char;
	using wchar = wchar_t;
	using uchar = unsigned char;
	using ubyte = unsigned char;
	using ushort = unsigned short;
	using uint = unsigned int;
	using ulong = unsigned long;

	using dword = unsigned long;
	using word = unsigned short;

	using int8 = __int8;
	using int16 = __int16;
	using int32 = __int32;
	using int64 = __int64;

	using uint8 = unsigned __int8;
	using uint16 = unsigned __int16;
	using uint32 = unsigned __int32;
	using uint64 = unsigned __int64;

#ifdef _M_X64
	using intptr = __int64;
	using uintptr = unsigned __int64;
	using longptr = long long;
	using ulongptr = unsigned long long;
#else
	using intptr = int;
	using uintptr = unsigned int;
	using longptr = long;
	using ulongptr = unsigned long;
#endif
*/

	class CriticalSection
	{
	protected:
		CRITICAL_SECTION _cs;
	public:
		CriticalSection() { ::InitializeCriticalSection(&_cs); }
		CriticalSection(const CRITICAL_SECTION &cs) : _cs{ cs } 
		{
			::InitializeCriticalSection(&_cs);
		}
		virtual ~CriticalSection() { ::DeleteCriticalSection(&_cs); }
		virtual void lock() { ::EnterCriticalSection(&_cs); }
		virtual void unlock() { ::LeaveCriticalSection(&_cs); }
		virtual bool trylock() { return TryEnterCriticalSection(&_cs); }
		operator CRITICAL_SECTION() { return _cs; };
	};

	/*class CriticalSection
	{
	public:
		CriticalSection() { ::InitializeCriticalSection(&_cs); }
		CriticalSection(const CRITICAL_SECTION &section)
			: _cs{ section } {
			::InitializeCriticalSection(&this->_cs);
		}
		// Release resources used by the critical section object.
		virtual ~CriticalSection() { ::DeleteCriticalSection(&_cs); }
		// Request ownership of the critical section.
		void Lock() { ::EnterCriticalSection(&_cs); }
		// Release ownership of the critical section.
		void Unlock() { ::LeaveCriticalSection(&_cs); }
		operator CRITICAL_SECTION() { return _cs; };

	protected:
		CRITICAL_SECTION _cs;
	};*/

	//#define flag_has(f, v) ((f) & (v)) == (v)
	/*
	inline bool flag_has(auto f, auto v)
	{
		return intptr_t(intptr_t(f) & intptr_t(v)) == intptr_t(v);
	}

	template<typename T>
	inline bool flag_has(auto f, std::initializer_list<T> values)
	{
		for(auto v : values)
		{
			if((f & v) == v)
				return true;
		}
		return false;
	}

	template<typename T>
	inline auto flag_remove(auto &f, T v)
	{
		if(flag_has(f, v))
			f &= ~v;
		return f;
	}

	template<typename T>
	inline auto flag_add(auto &f, T v)
	{
		if(!flag_has(f, v))
			f |= v;
		return f;
	}
	*/

	template<typename T = uint32_t>
	struct Flag
	{
		T value;

		Flag(T val) : value(val) {}

		bool has(auto v)
		{
			return T(value & T(v)) == T(v);
		}

		bool has(std::initializer_list<T> values)
		{
			for(auto v : values)
			{
				if(has(v))
					return true;
			}
			return false;
		}

		T add(auto v)
		{
			if(!has(v))
				value |= v;
			return value;
		}

		T add(std::initializer_list<T> values)
		{
			for(auto v : values) add(v);
			return value;
		}

		T remove(T v)
		{
			if(has(v))
				value &= ~v;
			return value;
		}

		bool equals(T other)
		{
			return value == other;
		}

		bool equals(const Flag& other)
		{
			return value == other.value;
		}

		operator T() { return value; }

		static bool set(T *flags, T value)
		{
			if((*flags & value) != value)
			{
				*flags |= value;
				return true;
			}
			return false;
		}
	};

	struct Guid : public GUID
	{
		Guid()
		{
		}

		Guid(const GUID& guid) 
		{
			::memcpy(reinterpret_cast<GUID *>(this), &guid, sizeof(GUID));
		}

		Guid(const wchar_t *guid)
		{
			parse(guid);
		}

		operator GUID() const { return *this; }
		explicit operator bool() const { return !empty(); }

		bool equals(const GUID &other) const
		{
			return 0 == ::memcmp(reinterpret_cast<const GUID *>(this), &other, sizeof(GUID));
		}

		bool equals(std::initializer_list<GUID> others) const
		{
			for(auto &iid : others)
				if(equals(iid)) return true;
			return false;
		}

		bool equals(const wchar_t *lpGuid) const
		{
			GUID guid{};
			if(S_OK != ::IIDFromString(lpGuid, &guid))
				return false;
			return equals(guid);
		}

		bool equals(std::initializer_list<const wchar_t *> others) const
		{
			for(auto &iid : others)
				if(equals(iid)) return true;
			return false;
		}

		bool empty() const
		{
			if(Data1 == 0 && Data2 == 0 && Data3 == 0)
			{
				for(auto d : Data4)
					if(d != 0) return false;
			}
			return true;
		}

		bool parse(const wchar_t *lpGuid)
		{
			GUID guid{};
			if(S_OK != ::IIDFromString(lpGuid, &guid))
				return false;
			::memcpy(reinterpret_cast<GUID *>(this), &guid, sizeof(GUID));
			return true;
		}

		std::wstring to_string(int format = 0) const
		{
			std::wstring buffer;
			size_t length = 33;
			
			auto parentleft = L"";
			auto parentright = L"";
			auto dash = L"";

			if(format > 0)
			{
				dash = L"-";
				length += 4;
				if(format >= 2)
				{
					length += 2;
					if(format == 3)
					{
						parentleft = L"(";
						parentright = L")";
					}
					else 
					{
						parentleft = L"{";
						parentright = L"}";
					}
				}
			}

			buffer.resize(length);

			//::StringCchPrintfW
			::_swprintf_p(buffer.data(), length, L"%s%-08.8X%s%-04.4X%s%-04.4X%s%-02.2X%-02.2X%s%02.2X%-02.2X%-02.2X%-02.2X%-02.2X%-02.2X%s",
				parentleft, 
					Data1, dash, Data2, dash, Data3, dash, Data4[0], Data4[1], dash,  
					Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7],
				parentright);
			return std::move(buffer);
		}
	};

	class Timer
	{
		using _system_clock = std::chrono::time_point<std::chrono::system_clock>;

	public:
		_system_clock start()
		{
			_startTime = std::chrono::system_clock::now();
			_running = true;
			return _startTime;
		}

		_system_clock stop()
		{
			_endTime = std::chrono::system_clock::now();
			_running = false;
			return _endTime;
		}
		
		intptr_t elapsed()
		{
			std::chrono::time_point<std::chrono::system_clock> endTime;

			if(_running)
			{
				endTime = std::chrono::system_clock::now();
			}
			else
			{
				endTime = _endTime;
			}

			return (intptr_t)std::chrono::duration_cast<std::chrono::milliseconds>(endTime - _startTime).count();
		}

		intptr_t elapsed_microseconds()
		{
			std::chrono::time_point<std::chrono::system_clock> endTime;
			if(_running)
				endTime = std::chrono::system_clock::now();
			else
				endTime = _endTime;
			return (intptr_t)std::chrono::duration_cast<std::chrono::microseconds>(endTime - _startTime).count();
		}

		intptr_t elapsed_milliseconds()
		{
			std::chrono::time_point<std::chrono::system_clock> endTime;

			if(_running)
				endTime = std::chrono::system_clock::now();
			else
				endTime = _endTime;
			return (intptr_t)std::chrono::duration_cast<std::chrono::milliseconds>(endTime - _startTime).count();
		}

		intptr_t elapsed_seconds()
		{
			return elapsed() / 1000;
		}

		intptr_t elapsed_minutes()
		{
			return elapsed_seconds() / 60;
		}

		/*static intptr_t now()
		{
			return static_cast<intptr_t>(_Xtime_get_ticks());
		}*/

		static intptr_t TickCount()
		{
#ifdef _WIN64
			return ::GetTickCount64();
#else
			return (intptr_t)::GetTickCount64();
#endif
		}

	private:
		_system_clock _startTime;
		_system_clock _endTime;
		bool _running = false;
	};

	/*class GC
	{
	protected:
		struct Node
		{
			void *object = nullptr;
			bool array = false;
			Node *prev = nullptr;
			Node *next = nullptr;

			Node(void *object, bool array, Node *next = nullptr, Node *prev = nullptr)
				noexcept : object(object), array(array), next(next), prev(prev)
			{
			}

			~Node()
			{
				if(array)
					delete[] object;
				else
					delete object;

				object = nullptr;
				next = nullptr;
				prev = nullptr;
			}
		};

		Node *_head, *_tail;
		size_t _count = 0;

	protected:

		template<typename T>
		T *push_back(T *object, bool array = false)
		{
			auto n = new Node(object, array);
			if(_head == nullptr)
			{
				_head = n;
			}
			else
			{
				n->prev = tail;
				_tail->next = n;
			}
			_tail = n;
			count++;
			return object;
		}

		template<typename T>
		void pop_back()
		{
			if(_tail)
			{
				auto n = _tail;
				if(_tail->prev)
				{
					_tail = _tail->prev;
					_tail->next = nullptr;
				}
				delete n;
				_count--;
			}
		}

		template<typename T>
		T *remove(T *object, bool delete_ = true)
		{
			auto n = find(object);
			if(n != nullptr)
			{
				if(n->prev)
					n->prev->next = n->next;
				else
					_head = n->next;

				if(delete_)
					delete n;
				_count--;
			}
			return nullptr;
		}

		template<typename T>
		Node *find(T *object)
		{
			for(auto n = _head; n != nullptr;
				n = n->next)
			{
				if(n->object == object)
					return n;
			}
			return nullptr;
		}

	public:

		GC() noexcept
			: _head(nullptr), _tail(nullptr), _count(0)
		{
		}

		~GC()
		{
			clean();
		}


		bool empty()const { return _head == nullptr; }
		bool size() const { return _count; }

		void clean()
		{
			if(_head != nullptr)
			{
				auto n = _head;
				while(n != nullptr)
				{
					auto curr = n;
					n = n->next;
					delete curr;
				}
				_head = nullptr;
			}
			_tail = nullptr;
			_count = 0;
		}

		template<typename T>
		T *alloc()
		{
			return push_back(new T{}, false);
		}

		template<typename T>
		T *alloc(size_t size)
		{
			return push_back(new T[size]{}, true);
		}

		template<typename T>
		T *alloc(T *object, bool array = false)
		{
			return push_back(object, array);
		}

		template<typename T>
		T *attach(T *object, bool array = false)
		{
			return push_back(object, array);
		}

		template<typename T>
		T *detach(T *object)
		{
			return remove(object, false);
		}

		template<typename T>
		T *Free(T *&object)
		{
			return object = remove(object);
		}
	};*/

	// Garbage Collection
	/*class GC
	{
		struct Node
		{
			void *data;
			Node *next;
			Node(void *data) : data{ data }, next{ nullptr } {}
			~Node() { if(data) delete data; }
		};

	private:

		Node *head;
		size_t count;

	public:

		GC() noexcept : head{ nullptr }, count{ 0 } {};
		~GC() noexcept { clean(); }

		void clean()
		{
			while(head)
			{
				auto temp = head;
				head = head->next;
				delete temp;
				count--;
			}
			head = nullptr;
			count = 0;
		}

		size_t size() const { return count; }
		bool empty() const { return head == nullptr; }

		void pop()
		{
			if(head)
			{
				auto temp = head;
				head = temp->next;
				delete temp;
				count--;
			}
		}

		template<class T>
		auto push(T *data)
		{
			auto temp = new Node(reinterpret_cast<void *>(data));
			if(head)
			{
				temp->next = head;
				head = temp;
			}
			else
			{
				head = temp;
			}
			count++;
			return data;
		}

		template<class T>
		auto push_back(T *data)
		{
			auto temp = new Node(reinterpret_cast<void *>(data));
			if(head == nullptr)
			{
				head = temp;
			}
			else
			{
				auto cur = head;
				while(cur)
				{
					if(cur->next == nullptr)
					{
						cur->next = temp;
						break;
					}
					cur = cur->next;
				}
			}
			count++;
			return data;
		}

		void reverse()
		{
			if(head)
			{
				auto parent = head;
				auto me = parent->next;
				auto child = me->next;
				// make parent as tail
				parent->next = nullptr;
				while(child)
				{
					me->next = parent;
					parent = me;
					me = child;
					child = child->next;
				}
				me->next = parent;
				head = me;
			}
		}

		template<class T>
		T *get(size_t index)
		{
			auto currunt = head;
			size_t i = 0;
			while(currunt)
			{
				if(i++ == index)
					return (T *)currunt->data;
				currunt = currunt->next;
			}
			return nullptr;
		}

		template<class T>
		T *peek()
		{
			return head ? reinterpret_cast<T*>(head->data) : nullptr;
		}
	};
	*/

	// Garbage Collection
	template<typename T = void>
	class GC
	{
		struct Node
		{
			T *data;
			Node *next;
			Node(T *data) : data{ data }, next{ nullptr } {}
			~Node() { if(data) delete data; }
		};

	private:

		Node *head;
		size_t count;

	public:

		GC() noexcept : head{ nullptr }, count{ 0 } {};
		~GC() noexcept { clean(); }

		void clean()
		{
			while(head)
			{
				auto temp = head;
				head = head->next;
				delete temp;
				count--;
			}
			head = nullptr;
			count = 0;
		}

		size_t size() const { return count; }
		bool empty() const { return head == nullptr; }

		void pop()
		{
			if(head)
			{
				auto temp = head;
				head = temp->next;
				delete temp;
				count--;
			}
		}

		auto push()
		{
			auto temp = new Node(new T);
			if(head)
			{
				temp->next = head;
				head = temp;
			}
			else
			{
				head = temp;
			}
			count++;
			return temp->data;
		}

		auto push(T *data)
		{
			auto temp = new Node(data);
			if(head)
			{
				temp->next = head;
				head = temp;
			}
			else
			{
				head = temp;
			}
			count++;
			return data;
		}

		auto push_back(T *data)
		{
			auto temp = new Node(data);
			if(head == nullptr)
			{
				head = temp;
			}
			else
			{
				auto cur = head;
				while(cur)
				{
					if(cur->next == nullptr)
					{
						cur->next = temp;
						break;
					}
					cur = cur->next;
				}
			}
			count++;
			return data;
		}

		void reverse()
		{
			if(head)
			{
				auto parent = head;
				auto me = parent->next;
				auto child = me->next;
				// make parent as tail
				parent->next = nullptr;
				while(child)
				{
					me->next = parent;
					parent = me;
					me = child;
					child = child->next;
				}
				me->next = parent;
				head = me;
			}
		}

		T *get(size_t index)
		{
			auto currunt = head;
			size_t i = 0;
			while(currunt)
			{
				if(i++ == index)
					return currunt->data;
				currunt = currunt->next;
			}
			return nullptr;
		}

		T *top()
		{
			return head ? head->data : nullptr;
		}
	};

	static inline void *swab(const void *_src, void *_dest, size_t n)
	{
		const auto *src = (uint8_t *)_src;
		auto dest = (uint8_t *)_dest;
		for(; n > 1; n -= 2)
		{
			dest[0] = src[1];
			dest[1] = src[0];

			dest += 2;
			src += 2;
		}
		return _dest;
	}

	static inline void *swab(void *val, size_t n)
	{
		auto *src = (uint8_t *)val;
		uint8_t c0 = 0, c1 = 0;
		for(; n > 1; n -= 2, src += 2)
		{
			c0 = src[0];
			c1 = src[1];

			src[0] = c1;
			src[1] = c0;
		}
		return val;
	}

	struct MESSAGE
	{
		HWND	hWnd{};
		UINT	uMsg{};
		WPARAM	wParam{};
		LPARAM	lParam{};
		WNDPROC	defWndProc{};

		MESSAGE() = default;

		MESSAGE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			: hWnd{ hWnd }, uMsg{ uMsg }, wParam{ wParam }, lParam{ lParam }
		{
		}

		LRESULT operator()() const
		{
			return invoke();
		}

		LRESULT operator()(WPARAM wp, LPARAM lp)
		{
			wParam = wp;
			lParam = lp;
			return invoke();
		}

		LRESULT invoke() const
		{
			if(defWndProc)
				return ::CallWindowProcW(defWndProc, hWnd, uMsg, wParam, lParam);
			return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
		}
	};

	class Module
	{
		HMODULE _handle{};

	public:
		Module() {}
		Module(HMODULE hModule) : _handle(hModule) {}
		Module(const wchar_t *path) { from(path); }
		virtual ~Module() {}

		operator HMODULE() const { return _handle;}
		explicit operator bool() const { return _handle != nullptr; }

		HMODULE from(const wchar_t *path)
		{
			_handle = ::GetModuleHandleW(path);
			return _handle;
		}

		HMODULE get() const { return _handle; }

		std::wstring path() const
		{
			std::wstring p(260, L'\0');
			if(auto z = ::GetModuleFileNameW(_handle, &p[0], 260); z > 0)
			{
				p.resize(z);
				return std::move(p);
			}
			return {};
		}
	};

namespace wrt
{
	/*
	//#include <winrt/base.h>
	template <typename T>
	struct com_ptr
	{
		using type = impl::abi_t<T>;

		com_ptr(std::nullptr_t = nullptr) noexcept {}

		com_ptr(void *ptr, take_ownership_from_abi_t) noexcept : m_ptr(static_cast<type *>(ptr))
		{
		}

		com_ptr(com_ptr const &other) noexcept : m_ptr(other.m_ptr)
		{
			add_ref();
		}

		template <typename U>
		com_ptr(com_ptr<U> const &other) noexcept : m_ptr(other.m_ptr)
		{
			add_ref();
		}

		template <typename U>
		com_ptr(com_ptr<U> &&other) noexcept : m_ptr(std::exchange(other.m_ptr, {}))
		{
		}

		~com_ptr() noexcept
		{
			release_ref();
		}

		com_ptr &operator=(com_ptr const &other) noexcept
		{
			copy_ref(other.m_ptr);
			return*this;
		}

		com_ptr &operator=(com_ptr &&other) noexcept
		{
			if(this != &other)
			{
				release_ref();
				m_ptr = std::exchange(other.m_ptr, {});
			}

			return*this;
		}

		template <typename U>
		com_ptr &operator=(com_ptr<U> const &other) noexcept
		{
			copy_ref(other.m_ptr);
			return*this;
		}

		template <typename U>
		com_ptr &operator=(com_ptr<U> &&other) noexcept
		{
			release_ref();
			m_ptr = std::exchange(other.m_ptr, {});
			return*this;
		}

		explicit operator bool() const noexcept
		{
			return m_ptr != nullptr;
		}

		auto operator->() const noexcept
		{
			return m_ptr;
		}

		T &operator*() const noexcept
		{
			return *m_ptr;
		}

		T *get() const noexcept
		{
			return m_ptr;
		}

		T **put() noexcept
		{
			return &m_ptr;
		}

		void **put_void() noexcept
		{
			return reinterpret_cast<void **>(put());
		}

		void attach(type *value) noexcept
		{
			release_ref();
			*put() = value;
		}

		type *detach() noexcept
		{
			return std::exchange(m_ptr, {});
		}

		friend void swap(com_ptr &left, com_ptr &right) noexcept
		{
			std::swap(left.m_ptr, right.m_ptr);
		}

		template <typename To>
		auto as() const
		{
			return impl::as<To>(m_ptr);
		}

		template <typename To>
		auto try_as() const noexcept
		{
			return impl::try_as<To>(m_ptr);
		}

		template <typename To>
		void as(To &to) const
		{
			to = as<impl::wrapped_type_t<To>>();
		}

		hresult as(guid const &id, void **result) const noexcept
		{
			return m_ptr->QueryInterface(id, result);
		}

	private:

		void copy_ref(type *other) noexcept
		{
			if(m_ptr != other)
			{
				release_ref();
				m_ptr = other;
				add_ref();
			}
		}

		void add_ref() const noexcept
		{
			if(m_ptr)
			{
				const_cast<std::remove_const_t<type> *>(m_ptr)->AddRef();
			}
		}

		void release_ref() noexcept
		{
			if(m_ptr)
			{
				unconditional_release_ref();
			}
		}

		__declspec(noinline) void unconditional_release_ref() noexcept
		{
			std::exchange(m_ptr, {})->Release();
		}

		template <typename U>
		friend struct com_ptr;

		type *m_ptr{};
	};
*/
/*
	template <typename T>
	auto get_abi(com_ptr<T> const &object) noexcept
	{
		return object.get();
	}

	template <typename T>
	auto put_abi(com_ptr<T> &object) noexcept
	{
		return object.put_void();
	}

	template <typename T>
	auto detach_abi(com_ptr<T> &object) noexcept
	{
		return object.detach();
	}

	template <typename T>
	bool operator==(com_ptr<T> const &left, com_ptr<T> const &right) noexcept
	{
		return get_abi(left) == get_abi(right);
	}

	template <typename T>
	bool operator==(com_ptr<T> const &left, std::nullptr_t) noexcept
	{
		return get_abi(left) == nullptr;
	}

	template <typename T>
	bool operator==(std::nullptr_t, com_ptr<T> const &right) noexcept
	{
		return nullptr == get_abi(right);
	}

	template <typename T>
	bool operator!=(com_ptr<T> const &left, com_ptr<T> const &right) noexcept
	{
		return !(left == right);
	}

	template <typename T>
	bool operator!=(com_ptr<T> const &left, std::nullptr_t) noexcept
	{
		return !(left == nullptr);
	}

	template <typename T>
	bool operator!=(std::nullptr_t, com_ptr<T> const &right) noexcept
	{
		return !(nullptr == right);
	}

	template <typename T>
	bool operator<(com_ptr<T> const &left, com_ptr<T> const &right) noexcept
	{
		return get_abi(left) < get_abi(right);
	}

	template <typename T>
	bool operator>(com_ptr<T> const &left, com_ptr<T> const &right) noexcept
	{
		return right < left;
	}

	template <typename T>
	bool operator<=(com_ptr<T> const &left, com_ptr<T> const &right) noexcept
	{
		return !(right < left);
	}

	template <typename T>
	bool operator>=(com_ptr<T> const &left, com_ptr<T> const &right) noexcept
	{
		return !(left < right);
	}*/
}
	template<class T>
	class IComPtr
	{
		bool _release = true;
	public:
		T *pointer = nullptr;//IUnknown*

		IComPtr(bool release = true) noexcept : _release(release) {}

		IComPtr(const IComPtr<T> &other) : pointer(other.pointer)
		{
			if(pointer) ((IUnknown *)pointer)->AddRef();
		}

		IComPtr(IComPtr<T> &&other) noexcept = default;

		explicit IComPtr(T *ptr) : pointer(ptr)
		{
			if(pointer) ((IUnknown *)pointer)->AddRef();
		}

		template<typename Q>
		IComPtr(const IComPtr<Q> &other)
		{
			if(other->QueryInterface(pointer) != S_OK)
				pointer = nullptr;
		}

		IComPtr(nullptr_t) {}

		~IComPtr() {
			if(_release)
				this->release();
		}

		IComPtr<T> &operator=(T *rhs)
		{
			if(pointer != rhs)
			{
				this->release();
				pointer = rhs;
			}
			return *this;
		}

		IComPtr<T> &operator=(const IComPtr<T> &rhs)
		{
			this->swap(rhs);
			return *this;
		}

		template<typename Q = T>
		IComPtr<T> &operator=(IComPtr<Q> &&rhs) noexcept
		{
			if(this != (IComPtr<T>*)&rhs || pointer != (T*)rhs.pointer)
			{
				this->release();
				pointer = (T*)rhs.pointer;
				rhs.pointer = nullptr;
			}
			return *this;
		}

		template<typename Q = T>
		void swap(IComPtr<Q> &other) throw()
		{
			std::swap(this->pointer, (T*)other.pointer);
		}

		unsigned long addref()
		{
			return pointer ? ((IUnknown *)pointer)->AddRef() : 0;
		}

		unsigned long release()
		{
			return pointer ? ((IUnknown *)pointer)->Release() : 0;
		}

		bool operator==(const IComPtr<T> &other)
		{
			return this->pointer == other.pointer;
		}

		bool operator!=(const IComPtr<T> &other)
		{
			return !((*this) == other);
		}

		template<typename Q = T>
		Q *get() const { return (Q*)pointer; }

		template<typename Q = T>
		Q *getx()  { return (Q *)pointer; }


		operator T *()const { return pointer; }
		T &operator*()const { return *pointer; }
		T *operator->()const { return pointer; }

		explicit operator bool() const { return pointer != nullptr; }

	//	template<typename Q = T>
	//	operator Q *()const { return (Q *)pointer; }

		T* set_release(bool b)
		{
			_release = b;
			return *this;
		}

		template<typename Q = void> 
		operator Q **()
		{
			this->release();
			return (Q **)&pointer;
		}

		void **reset()
		{
			this->release();
			return (void **)&pointer;
		}


		template<typename TYPE = void>
		bool QueryServiceProvider(REFGUID sid, TYPE **lppOut)
		{
			TResult hr = E_FAIL;
			if(lppOut)
			{
				*lppOut = nullptr;
				if(pointer)
				{
					IComPtr<IServiceProvider> sp;
					hr = ((IUnknown*)pointer)->QueryInterface(IID_IServiceProvider, sp);
					if(hr.succeeded())
					{
						hr = sp->QueryService(sid, __uuidof(TYPE), (void**)lppOut);
					}
				}
			}
			return hr;
		}

		/*
		template<typename TYPE>
		TYPE *QueryService(REFGUID guidService)
		{
			TYPE *ppvOut = nullptr;
			if(S_OK == QueryService(pointer, guidService, __uuidof(TYPE), (void **)&ppvOut) && (ppvOut && *ppvOut))
				return ppvOut;
			return nullptr;
		}

		template<typename TYPE>
		bool QueryService(REFGUID guidService, TYPE **ppvOut)
		{
			return S_OK == QueryService(pointer, guidService, ppvOut) && (ppvOut && *ppvOut);
		}*/

		bool QueryServiceProvider(REFGUID guidService, REFIID riid, IUnknown **ppvOut)
		{
			return S_OK == QueryServiceProvider(pointer, guidService, riid, ppvOut) && (ppvOut && *ppvOut);
		}

		bool CreateInstance(REFIID rclsid, DWORD dwClsContext = CLSCTX_LOCAL_SERVER)
		{
			return SUCCEEDED(::CoCreateInstance(rclsid, nullptr, dwClsContext, __uuidof(T), (void **)&pointer));
		}

		bool CreateInstance(REFIID rclsid, REFIID riid, DWORD dwClsContext = CLSCTX_LOCAL_SERVER)
		{
			return SUCCEEDED(::CoCreateInstance(rclsid, nullptr, dwClsContext, riid, (void **)&pointer));
		}

		/*bool CreateInstance(REFIID rclsid, IUnknown *pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_LOCAL_SERVER)
		{
			return S_OK == ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void **)&pointer);
		}
		*/
		bool is_null() const { return pointer == nullptr; }
		bool ok() const  { return pointer != nullptr; }

		template<typename T>
		static bool Ok(T *object, HRESULT hRet)
		{
			return (hRet == S_OK) && (object != nullptr);
		}

		template<typename T>
		static bool Succeeded(T *object, HRESULT hRet)
		{
			return (hRet == S_OK) && (object != nullptr);
		}

		template<typename T>
		static bool Failed(T *object, HRESULT hRet)
		{
			return (hRet != S_OK) && (object == nullptr);
		}

		template<typename TYPE = void>
		static HRESULT QueryService(IUnknown *lpUnknown, REFGUID sid, TYPE **lppOut)
		{
			TResult hr = E_FAIL;
			if(lppOut)
			{
				*lppOut = nullptr;
				if(lpUnknown)
				{
					IComPtr<IServiceProvider> sp;
					hr = lpUnknown->QueryInterface(IID_IServiceProvider, sp);
					if(hr && !sp.is_null())
					{
						hr = sp->QueryService(sid, __uuidof(TYPE), (void **)lppOut);
					}
				}
			}
			return hr;
		}

		template<typename T = void>
		static HRESULT QueryService(IUnknown *lpUnknown, REFGUID sid, REFIID riid, T **lppOut)
		{
			TResult hr = E_FAIL;
			if(lppOut)
			{
				*lppOut = nullptr;
				if(lpUnknown)
				{
					IComPtr<IServiceProvider> sp;
					hr = lpUnknown->QueryInterface(IID_IServiceProvider, sp);
					if(hr && !sp.is_null())
					{
						hr = sp->QueryService(sid, riid, (void **)lppOut);
					}
				}
			}
			return hr;
		}


		/*
		template<typename TYPE>
		static bool CreateInstance(TYPE **ppv, REFCLSID rclsid, LPUNKNOWN pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_LOCAL_SERVER)
		{
			return S_OK == ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(TYPE), (void**)ppv);
		}*/
	};

	template<typename T>
	class CoPtr
	{
		T *ptr;
	public:

		CoPtr() noexcept : ptr(nullptr) {}
		CoPtr(const CoPtr<T> &obj) : ptr(obj.ptr)
		{
		}

		explicit CoPtr(T *ptr) : ptr(ptr)
		{
		}

		~CoPtr() { this->Release(); }

		const CoPtr &operator=(T *ptr)
		{
			this->ptr = ptr;
			return *this;
		}

		void Release()
		{
			if(ptr)
				ptr->Release();
			ptr = nullptr;
		}

		T *Get() const { return ptr; }
		bool operator==(const CoPtr &other)
		{
			return this->ptr == other.ptr;
		}

		bool operator!=(const CoPtr &other)
		{
			return !((*this) == other);
		}

		T &operator*()const { return *ptr; }
		T *operator->()const { return ptr; }
		operator T *()const { return ptr; }
		operator T **()
		{
			return &ptr;
		}

		operator void **()
		{
			return (void **)&ptr;
		}

		bool IsNull() { return ptr == nullptr; }
	};

	namespace Drawing {}

	struct Monitor
	{
		HMONITOR handle{};
		RECT  rcMonitor{};
		RECT rcWork{};
		DWORD dwFlags{};

		Monitor() {}

		Monitor(HWND hWnd) { from_window(hWnd); }
		Monitor(POINT const &pt) { from_point(pt); }

		explicit operator bool() const { return handle != nullptr; }
		explicit operator HMONITOR() const { return handle; }

		Monitor& from_window(HWND hWnd)
		{
			if(hWnd) handle = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			return *this;
		}

		Monitor& from_point(POINT const &pt)
		{
			handle = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
			return *this;
		}

		Monitor& from_point(RECT const &rc)
		{
			handle = ::MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
			return *this;
		}

		bool info()
		{
			if(handle)
			{
				MONITORINFO mi{ sizeof mi };
				if(::GetMonitorInfoW(handle, &mi))
				{
					rcMonitor = mi.rcMonitor;
					rcWork = mi.rcWork;
					dwFlags = mi.dwFlags;
					return true;
				}
			}
			return false;
		}

		bool is_primary() const { return (dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY; }
	};


	namespace IO {}
	namespace Text {}
	namespace Diagnostics {}
	namespace Security {}
	namespace Windows { namespace Forms {} }

	static const auto typeid_bool = typeid(bool).hash_code();
	static const auto typeid_char = typeid(char).hash_code();
	static const auto typeid_wchar = typeid(wchar_t).hash_code();

	static const auto typeid_int8 = typeid(int8_t).hash_code();
	static const auto typeid_uint8 = typeid(uint8_t).hash_code();

	static const auto typeid_int16 = typeid(int16_t).hash_code();
	static const auto typeid_uint16 = typeid(uint16_t).hash_code();

	static const auto typeid_int32 = typeid(int32_t).hash_code();
	static const auto typeid_uint32 = typeid(uint32_t).hash_code();

	static const auto typeid_int64 = typeid(int64_t).hash_code();
	static const auto typeid_uint64 = typeid(uint64_t).hash_code();

	static const auto typeid_short = typeid(short).hash_code();
	static const auto typeid_ushort = typeid(unsigned short).hash_code();

	static const auto typeid_long = typeid(long).hash_code();
	static const auto typeid_ulong = typeid(unsigned long).hash_code();

	static const auto typeid_float = typeid(float).hash_code();
	static const auto typeid_double = typeid(double).hash_code();
	static const auto typeid_long_double = typeid(long double).hash_code();
}


//template <class T>
//constexpr bool is_numeric = (std::is_integral_v<T> || std::is_floating_point_v<T>) && !(std::is_same<char, T>::value || std::is_same<wchar_t, T>::value);

/*
				template<typename T>
	Object(T const &obj) noexcept
	{
		if constexpr(std::is_null_pointer_v<T>)
			Value.Type = PrimitiveType::Null;
		else if constexpr(is_string_w_type_v<T> || is_char_type_v<T>)
			assign((string)obj);
		else if constexpr(is_string_a_type_v<T>)
			assign(((string)Unicode::FromAnsi(obj)).move());
		else if constexpr(is_numeric<T>)
			assign((double)obj);
		else if constexpr(std::is_pointer_v<T>)
			*this = (Object *)obj;
		else
			assign((string)obj);
	}
*/

template<typename T = LRESULT>
inline static auto SendMSG(HWND hWnd, uint32_t msg, auto wparam, auto lparam)
{
	return (T)::SendMessageW(hWnd, msg, (WPARAM)wparam, (LPARAM)lparam);
}

template<typename T = BOOL>
inline static auto PostMSG(HWND hWnd, uint32_t msg, auto wparam, auto lparam)
{
	return (T)::PostMessageW(hWnd, msg, (WPARAM)wparam, (LPARAM)lparam);
}

#include "System\auto_ptr.h"
#include "System\MemoryManager.h"
#include "System\Diagnostics\Thread.h"
#include "System\DLL.h"


// string classes
#include "System\Text\StringBuffer.h"
#include "System\Text\Encoding.h"
#include "System\Text\string.h"
#include "System\Text\TString.h"
#include "System\Text\Text.h"

//static const auto typeid_string = typeid(Nilesoft::Text::string).hash_code();
//static const auto typeid_const_string = typeid(const Nilesoft::Text::string).hash_code();
//static const auto typeid_wstring = typeid(std::wstring).hash_code();
//static const auto typeid_const_wstring = typeid(const std::wstring).hash_code();

#include "System\Log.h"
#include "System\Exception.h"

// Collections
#include "System\Collections\Array.h"
#include "System\Collections\LList.h"
#include "System\Collections\List.h"
#include "System\Collections\StringList.h"

#include "System\Object.h"

#include "System\Environment.h"
#include "System\Windows\Registry.h"

#include "System\CommandLine.h"
#include "System\Windows\Window.h"
#include "System\Windows\Forms\MessageBox.h"
#include "System\Windows\Windows.h"

#include "System\Diagnostics\Process.h"
#include "System\Diagnostics\Shell.h"
#include "System\Windows\Registry.h"
#include "System\Security\Permission.h"
#include "System\Security\Elevation.h"

#include "System\IO\IO.h"
#include "System\IO\Path.h"
#include "System\IO\File.h"

#include "System\Drawing\Drawing.h"
#include "System\Drawing\Color.h"
#include "System\Drawing\Brush.h"
#include "System\Drawing\DC.h"
#include "System\Drawing\Bitmap.h"
#include "System\Drawing\Icon.h"

#include <wincodec.h>
#include <d2d1.h>
#include <dwrite.h>

#include "System\Drawing\WICImagingFactory.h"
#include "System\Drawing\Image.h"
#include "System\Drawing\Drawing.h"

