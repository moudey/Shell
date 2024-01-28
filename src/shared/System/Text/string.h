#pragma once

//#include <wchar_t.h>
//#include <minwindef.h>
//#include <strsafe.h>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include "Hash.h"
#include "Char.h"

/*
std::wstring s2ws(const std::string& s)
{
	int slength = (int)s.length() + 1;
	auto len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}

std::string ws2s(const std::wstring& s)
{
	int slength = (int)s.length() + 1;
	auto len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}
*/ 


/*
bool isNumeric(const std::string& string)
{
	std::size_t pos;
	long double value = 0.0;

	try
	{
		value = std::stold(string, &pos);
	}
	catch(std::invalid_argument&)
	{
		return false;
	}
	catch(std::out_of_range&)
	{
		return false;
	}

	return pos == string.size() && !std::isnan(value);
}
#include <regex>
static const std::regex INT_TYPE("[+-]?[0-9]+");
static const std::regex UNSIGNED_INT_TYPE("[+]?[0-9]+");
static const std::regex DOUBLE_TYPE("[+-]?[0-9]+[.]?[0-9]+");
static const std::regex UNSIGNED_DOUBLE_TYPE("[+]?[0-9]+[.]?[0-9]+");

bool isIntegerType(const std::string& str_)
{
  return std::regex_match(str_, INT_TYPE);
}

bool isUnsignedIntegerType(const std::string& str_)
{
  return std::regex_match(str_, UNSIGNED_INT_TYPE);
}

bool isDoubleType(const std::string& str_)
{
  return std::regex_match(str_, DOUBLE_TYPE);
}

bool isUnsignedDoubleType(const std::string& str_)
{
  return std::regex_match(str_, UNSIGNED_DOUBLE_TYPE);
}

bool isNumber( std::string token )
{
	return std::regex_match( token, std::regex( ( "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?" ) ) );
}

bool isNumber(const std::string &token)
{
	return std::regex_match(token, std::regex("(\\+|-)?[0-9]*(\\.?([0-9]+))$"));
}

	bool isNumeric(string str)
	{
	   stringstream stream;
	   double number;

	   stream<<str;
	   stream>>number;

	   return stream.eof();
	}
	bool is_number(const std::string& s)
{
	return( strspn( s.c_str(), "-.0123456789" ) == s.size() );
}

isNumber(const std::string &str) {
  return !str.empty() && str.find_first_not_of("0123456789") == string::npos;
}
bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}
bool isPositiveInteger(const std::string& s)
{
	return !s.empty() &&
		   (std::count_if(s.begin(), s.end(), std::isdigit) == s.size());
}
*/

namespace Nilesoft
{
	namespace Text
	{
		/*
here is no chance to do a secure hash in 64 bits. Even SHA-1 at 160 bit is considered theoretically broken. You should use SHA2-256 if you really care about secure digital signing. If you don't care about security and just want a hash function that avoids non-adversarial collisions just use the following, it is fine:

constexpr uint64 P1 = 7;
constexpr uint64 P2 = 31;

uint64 hash = P1;
for (const char* p = s; *p != 0; p++) {
	hash = hash * P2 + *p;
}

		static unsigned long long llhash(const char *str)
		{
			unsigned long long hash = 5381;
			int c;

			while(c = *str++)
				hash = ((hash << 5) + hash) + c;

			return hash;
		}
		*/

		class string;

		template <class T>
		constexpr bool is_nullptr = std::is_same<nullptr_t, T>::value;

		template <class T>
		constexpr bool is_numeric_type_v = std::is_integral_v<T> || std::is_floating_point_v<T>;

		template <class T>
		constexpr bool is_char_type_v = std::_Is_any_of_v<T, char, wchar_t, Char>;

		template <class T>
		constexpr bool is_string_a_cstr_type_v = std::_Is_any_of_v<T, char*, const char*>;

		template <class T>
		constexpr bool is_string_w_cstr_type_v = std::_Is_any_of_v<T, wchar_t*, const wchar_t*>;

		template <class T>
		constexpr bool is_string_cstr_type_v = is_string_a_cstr_type_v<T> || is_string_w_cstr_type_v<T>;


		template <class T>
		constexpr bool is_string_std_a_type_v = std::_Is_any_of_v<T,
			std::string, std::string_view,
			std::string const, std::string_view const>;

		template <class T>
		constexpr bool is_string_std_w_type_v = std::_Is_any_of_v<T,
			std::wstring, std::wstring_view,
			std::wstring const, std::wstring_view const>;

		template <class T>
		constexpr bool is_string_std_type_v = is_string_std_a_type_v<T> || is_string_std_w_type_v<T>;

		template <class T>
		constexpr bool is_string_w_class_type_v = std::_Is_any_of_v<T,
			string, std::wstring, std::wstring_view,
			string const, std::wstring const, std::wstring_view const>;

		template <class T>
		constexpr bool is_string_class_type_v = std::_Is_any_of_v<T, string, string const>;

		template <class T>
		constexpr bool is_string_a_type_v = is_string_a_cstr_type_v<T> || is_string_std_a_type_v<T>;

		template <class T>
		constexpr bool is_string_w_type_v = is_string_w_cstr_type_v<T> || is_string_std_w_type_v<T> || is_string_class_type_v<T>;

		template <class T>
		constexpr bool is_string_type_v = is_string_a_type_v<T> || is_string_w_type_v<T>;

		template <class T>
		constexpr bool is_string_or_char_type_v = is_string_type_v<T> || is_char_type_v<T>;

		template<typename T>
		struct is_string_decay
			: public std::disjunction<
			std::is_same<char *, typename std::decay_t<T>>,
			std::is_same<const char *, typename std::decay_t<T>>,
			std::is_same<std::string, typename std::decay_t<T>>,
			std::is_same<wchar_t *, typename std::decay_t<T>>,
			std::is_same<const wchar_t *, typename std::decay_t<T>>,
			std::is_same<std::wstring, typename std::decay_t<T>>,
			std::is_same<string, typename std::decay_t<T>>
			>
		{
		};

		template <class T>
		constexpr bool is_string_decay_v = is_string_decay<T>::value;


		template<typename T>
		class autostr
		{
		protected:
			T *ptr;

		public:
			bool empty;
			size_t len;

			autostr()
				: ptr{ nullptr }, len{ 0 }, empty{ true }
			{
			}

			autostr(T *value)
				: ptr{ nullptr }, len{ 0 }, empty{ true }
			{
				set(value);
			}

			autostr(T *value, size_t len)
				: ptr{ nullptr }, len{ 0 }, empty{ true }
			{
				set(value, len);
			}

			autostr(size_t len)
				: ptr{ new T[len + 1]{} }, len{ len }, empty{ false }
			{
			}

			virtual ~autostr() { delete[] ptr; }

			T *resize(size_t _len, bool free = false)
			{
				if(free && ptr)
					delete[] ptr;

				ptr = new T[_len + 1]{};
				len = _len;
				return ptr;
			}

			T *get() { return ptr; }

			T *get() const { return ptr; }
			T at(int i) const { return ptr ? ptr[i] : NULL; }

			T operator [](size_t i) const
			{
				if(ptr && (i < len))
					return ptr[i];
				return NULL;
			}

			bool set(size_t i, T v)
			{
				if(ptr && i < len)
				{
					ptr[i] = v;
					return true;
				}
				return false;
			}

			T *set(T *value)
			{
				size_t length = 0;
				if(auto p = value)
					while(*p++) length++;
				return set(value, length);
			}

			T *set(T *value, size_t length)
			{
				if(value == nullptr)
				{
					if(ptr)
						delete[] ptr;
					ptr = nullptr;
					len = 0;
					empty = true;
				}
				else
				{
					if(len <= length)
					{
						ptr = value;
						ptr[length] = NULL;
					}
					else
					{
						if(ptr)
							delete[] ptr;
						ptr = value;
					}
					len = length;
					empty = len > 0;
				}
				return ptr;
			}

			operator T() const { return ptr ? ptr[0] : NULL; }
			operator T *() const { return ptr; }
			operator T const *() const { return ptr; }
			const autostr &operator =(T *value)
			{
				set(value);
				return *this;
			}
		};

		class string
		{
		protected:
			//std::unique_ptr<wchar_t[]> m_data;
			wchar_t *m_data = nullptr;
			size_t	m_length = 0u;
			size_t	m_capacity = 0u;
			const size_t BUFFERSIZE = 260u;

			static constexpr wchar_t whitespace_chars[]
			{
				0x9, 0xA, 0xB, 0xC, 0xD, 0x20, 0x85,
				0xA0, 0x1680,
				0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005,
				0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x200B,
				0x2028, 0x2029,
				0x3000, 0xFEFF
			};

		protected:

			bool valid() const
			{
				return (m_data && (m_length > 0));
			}

			bool valid(size_t index) const
			{
				return (valid() && (index < m_length));
			}

			string &terminate(size_t index)
			{
				if(!m_data)
					m_length = 0;
				else
				{
					m_length = index;
					if(index >= m_capacity)
						m_length = m_capacity - 1;
					m_data[m_length] = 0;
				}
				return *this;
			}

			void zero(size_t index = 0)
			{
				for(size_t i = index; i < m_capacity; i++)
					m_data[i] = 0;;
			}


			string &capacity(size_t capacity, bool copy)
			{
				if(!m_data || capacity >= m_capacity)
				{
					m_capacity = capacity + size_t(capacity * 0.50);
					if(m_capacity < 16)
						m_capacity = 16;

					if(m_length >= m_capacity)
						m_capacity = m_length + size_t(m_length * 0.50);

					auto o_data = m_data;
					m_data = new wchar_t[m_capacity] {};
					if(o_data)
					{
						if(copy && m_length)
						{
							auto _data = m_data;
							for(size_t i = 0; i < m_length; i++)
								*_data++ = o_data[i];
						}
						else
							m_length = 0;

						delete[] o_data;
					}
					else
						m_length = 0;
				}
				else if(capacity < m_length)
				{
					m_length = capacity - 1;
				}

				if(m_length < m_capacity)
					*(m_data + m_length) = NULL;

				return *this;
			}

		public:
			// Initialize static member
			static size_t const MAX = (size_t)260;
			const static wchar_t Null = 0;
			constexpr static const wchar_t *EMPTY = L"";
			//static size_t const npos = (size_t)-1;
			static constexpr auto npos { static_cast<size_t>(-1) };
			const wchar_t null = NULL;

			virtual ~string()
			{
				if(m_data) delete[] m_data;
			}

			//explicit string(double);
			string() noexcept {}
			string(nullptr_t) noexcept {}
			string(const string &value) noexcept
			{
				this->assign(value.m_data, value.m_length);
			}

			// moveable
			string(string &&other) noexcept { this->assign(other.move()); }
			string(std::initializer_list<wchar_t> chars) noexcept { assign(chars); }
			string(std::initializer_list<char> chars) noexcept { assign(chars); }
			string(wchar_t *value) noexcept { this->assign(value); }
			string(const wchar_t *value) noexcept { this->assign(value); }
			string(const Char &value) noexcept { this->assign(((wchar_t *)&value), 1); }
			string(const char &value) noexcept { this->assign((wchar_t *)&value, 1); }
			string(const wchar_t &value) noexcept { this->assign(&value, 1); }
			string(const char *value) noexcept { this->assign(ToUnicode(value)); }

			string(std::wstring_view const &value) noexcept
			{
				this->assign(value.data(), value.length());
			}

			string(std::string_view const &value) noexcept
			{
				this->assign(ToUnicode(value));
			}

			string(std::wstring const &value) noexcept
			{
				this->assign(value.c_str(), value.length());
			}

			string(std::string const &value) noexcept
			{
				this->assign(ToUnicode(value));
			}

			template<typename T = std::size_t>
			string(const wchar_t *value, T length) noexcept 
			{ 
				this->assign(value, static_cast<size_t>(length));
			}

			template<typename T = std::size_t>
			string(const char *value, T length) noexcept 
			{ 
				this->assign(ToUnicode(value, static_cast<size_t>(length)).move()); 
			}

			template<typename T = std::size_t>
			string(T count, const wchar_t value) noexcept
			{
				this->capacity<T>(count);
				m_length = count;
				string::Set(m_data, 0, value, m_length);
			}

/*
			template<typename T>
			string(T const &value) noexcept
			{
				if constexpr(std::is_null_pointer_v<T>)
				{
					// nullptr
				}
				else if constexpr(is_char_type_v<T>)
				{
					assign((wchar_t *)&value, 1);
				}
				else if constexpr(is_string_w_type_v<T>)
				{
				//decltype(std::declval<const T &>().length()
					if constexpr(is_string_w_cstr_type_v<T>)
						assign((wchar_t*)value);
					else
						assign((string)value);
				}
				else if constexpr(is_string_a_type_v<T>)
				{
					assign(ToUnicode((std::string_view)value).move());
				}
				else if constexpr(is_numeric_type_v<T>)
				{
					this->capacity(static_cast<size_t>(value));
				}
			}
*/
			template<typename T = size_t>
			string(T capacity) noexcept { this->capacity(static_cast<size_t>(capacity)); }

			// reallocates the size of the memory
			template<typename T = size_t>
			T capacity() const { return (T)m_capacity; }
			
			template<typename T = size_t>
			const string &capacity(T capacity)
			{
				return this->capacity(static_cast<size_t>(capacity), false);
			}

			template<typename T = size_t>
			const string &reserve(T capacity)
			{
				return this->capacity(static_cast<size_t>(capacity), false);
			}
		
			template<typename T = size_t>
			size_t resize(T size) { return length(size).m_length; }
			size_t size() const { return length(); }
			// Gets the number of characters in the current String object.
			
			template<typename T = std::size_t>
			T length() const { return static_cast<T>(m_data ? m_length : 0); }

			template<typename T = std::size_t>
			const string &length(T length)
			{
				auto len = static_cast<T>(length);
				if(len >= m_capacity)
					return capacity(len);
				else if(!empty())
				{
					m_length = len;
					this->terminate(0);
				}
				return *this;
			}

			// Indicates whether a specified string is null, empty, 
			// or consists only of white-space characters.
			bool is_null_whitespace() const
			{
				return string::IsNullOrWhiteSpace(m_data, m_length);
			}

			const wchar_t *clear()
			{
				if(m_data)
				{
					delete[] m_data;
					m_data = nullptr;
				}
				m_capacity = 0;
				m_length = 0;
				return nullptr;
			}

			bool empty() const
			{
				return (m_data == nullptr || m_length == 0);
			}

			template<typename T = size_t>
			const wchar_t &at(T index) const
			{
				return valid(index) ? m_data[static_cast<size_t>(index)] : null;
			}

			// pos is addres of index start
			const wchar_t *c_str(size_t startIndex) const
			{
				if(valid(startIndex))
					return &m_data[startIndex];
				return nullptr;
			}

			const wchar_t *c_str() const
			{
				return valid() ? m_data : nullptr;
			}

			wchar_t *data() { return this->m_data; }
			wchar_t *buffer() { return capacity(BUFFERSIZE).m_data; }
			template<typename T>
			wchar_t *buffer(T length) { return capacity(static_cast<size_t>(length)).m_data; }

			string &release()
			{
				return this->terminate(string::Length(m_data));
			}

			template<typename T = size_t>
			string &release(T length)
			{
				return this->terminate(static_cast<size_t>(length));
			}

			// moveable
			//string &&forward() noexcept { return static_cast<string &&>(*this); }
			string &&move() noexcept { return static_cast<string &&>(*this); }
			string &assign(string &&other) noexcept
			{
				if(this != &other)
				{
					clear();
					m_capacity = other.m_capacity;
					m_length = other.m_length;
					m_data = other.m_data;

					other.m_data = nullptr;
					other.m_length = 0;
					other.m_capacity = 0;
				}
				return *this;
			}

			string &assign(const string &other) noexcept
			{
				if(this != &other)
				{
					clear();
					m_capacity = other.m_capacity;
					m_length = other.m_length;
					m_data = other.m_data;
				}
				return *this;
			}

			string &assign(const std::wstring &other) noexcept
			{
				return assign(other.c_str(), other.length());
			}

			string &assign(const std::wstring_view &other) noexcept
			{
				return assign(other.data(), other.length());
			}

			string &assign(const wchar_t *other, size_t length)
			{
				m_length = 0;
				if(other && length > 0)
				{
					this->capacity(length, false);
					m_length = length;
					for(size_t i = 0; i < length; i++)
						m_data[i] = other[i];
				}

				if(m_data) m_data[m_length] = 0;

				return *this;
			}

			template<typename T = wchar_t>
			string &assign(std::initializer_list<T> chars) noexcept
			{
				auto size = chars.size();
				if(size >= m_capacity)
					capacity(size, false);
				else
					zero(size);

				m_length = size;
				auto temp = m_data;
				for(auto &c : chars) *temp++ = wchar_t(c);
				return *this;
			}

			string &assign(const wchar_t *rhs)
			{
				return assign(rhs, string::Length(rhs));
			}

			string &append(const wchar_t *value, size_t length)
			{
				if(value && *value && length > 0)
				{
					auto newLength = m_length + length;
					this->capacity(newLength, true);
					string::Copy(m_data + m_length, value, length);
					m_length = newLength;
				}
				return *this;
			}

			string &append(const string &value)
			{
				return this->append(value.m_data, value.m_length);
			}

			string &append(std::wstring const &value)
			{
				return this->append(value.c_str(), value.length());
			}

			string &append(std::wstring_view const &value)
			{
				return this->append(value.data(), value.length());
			}

			template<typename T = wchar_t>
			string &append(std::initializer_list<T> chars)
			{
				for(auto &c : chars) append(wchar_t(c));
				return this;
			}

			string &append(const wchar_t *value)
			{
				return this->append(value, string::Length(value));
			}

			string &append(wchar_t value)
			{
				return this->append(&value, 1);
			}

			string &append(char value)
			{
				return this->append((wchar_t *)&value, 1);
			}

			string &append(const Char &value)
			{
				return this->append((wchar_t)value.get());
			}

			string &append_line(const string &value)
			{
				this->append(L"\n", 1);
				return this->append(value, value.length());
			}

			string &append_line(const wchar_t *value = nullptr)
			{
				this->append(L"\n", 1);
				return this->append(value, string::Length(value));
			}

			string &append_format(const wchar_t *format, ...)
			{
				if(!format || *format == 0)
					return *this;
				va_list argList;
				va_start(argList, format);
				this->append(string().format(argList, format));
				va_end(argList);
				return *this;
			}

			string &format(va_list argList, wchar_t const * const format)
			{
				/*
				* 	Text::string str(std::swprintf(nullptr, 0, L"%g", Value.Double));
					std::swprintf(str.data(), str.Length() + 1, L"%g", Value.Double);
				*/
				m_length = 0;
				if(m_data)
					m_data[m_length] = 0;

				if(format && *format)
				{
					auto length = string::FormatLength(format, argList);
					if(length > 0)
					{
						length = string::Format(buffer(length), length + 1, format, argList);
						return this->release(length);
					}
				}
				return *this;
			}

			string &format(wchar_t const * const fmt, ...)
			{
				m_length = 0;
				if(m_data) m_data[m_length] = 0;
				if(fmt && *fmt)
				{
					va_list argList;
					va_start(argList, fmt);
					this->format(argList, fmt);
					va_end(argList);
				}
				return *this;
			}
/*

void __cdecl stringEx::Format(std::string& resultString, const TCHAR* fmt_str, ...){
	int final_n, n = ((int)strlen(fmt_str)) * 2; // reserve 2 times as much as the length of the fmt_str
			std::string str;
			std::unique_ptr<char[]> formatted;
			va_list args;
			while(1)
			{
				va_start(args, fmt_str);
				formatted.reset(new TCHAR[n]); // wrap the plain char array into the unique_ptr
				//strcpy_s(&formatted[0], n, fmt_str);
				final_n = vsnprintf_s(&formatted[0], n, _TRUNCATE, fmt_str, args);
				va_end(args);
				if(final_n < 0 || final_n >= n)
					n += abs(final_n - n + 1);
				else
					break;
			}
			resultString = std::string(formatted.get());
	}

std::string TrimLeft(std::string& sVal)
{
	std::string result;
	int nOmitCt = 0;
	for(std::string::iterator i=sVal.begin(); i!=sVal.end(); i++){
		if( *i == ' ' || *i == '\t' || *i == '\r' || *i == '\n' )
			nOmitCt ++;
		else
			break;
		}

	if( nOmitCt == 0 )
		return sVal;
	result = sVal.substr(nOmitCt, sVal.length()-nOmitCt);
	return result;
	}

std::string TrimRight(std::string& sVal){
	std::string result;
	int nOmitCt = 0;
	for(std::string::reverse_iterator i=sVal.rbegin(); i!=sVal.rend(); ++i){
		if( *i == ' ' || *i == '\t' || *i == '\r' || *i == '\n' )
			nOmitCt ++;
		else
			break;
		}

	if( nOmitCt == 0 )
		return sVal;
	result = sVal.substr(0, sVal.length()-nOmitCt);
	return result;
	}

std::string Right(std::string& sVal, int nCt){
	std::string result;
	int len = sVal.length();
	if( len < nCt )
		result = sVal;
	else{
		result = sVal.substr(len - nCt, nCt);
		}
	return result;
}
*/
			string &trim_start()
			{
				if(this->valid())
				{
					auto length = m_length;
					const wchar_t *data = m_data;

					while(length > 0 && Char::IsWhiteSpace(*data))
					{
						data++; 
						length--;
					}

					if(length < m_length)
					{
						m_length = length;
						::memmove(m_data, data, m_length * sizeof(wchar_t));
					}
					m_data[m_length] = 0;
				}
				return *this;
			}

			string &trim_start(const wchar_t *value)
			{
				if(value)
				{
					while(*value && m_length > 0)
					{
						this->trim_start(*value++);
					}
				}
				return *this;
			}


			string &trim_start(wchar_t value)
			{
				if(this->valid())
				{
					auto length = m_length;
					const wchar_t *data = m_data;
					while(length > 0 && value == *data)
					{
						data++; 
						length--;
					}

					if(length < m_length)
					{
						m_length = length;
						::memmove(m_data, data, m_length * sizeof(wchar_t));
					}
					m_data[m_length] = 0;
				}
				return *this;
			}

			string &trim_start(char value)
			{
				return this->trim_start((wchar_t)value);
			}

			string &trim_end()
			{
				if(this->valid())
				{
					while(m_length > 0)
					{
						if(!Char::IsWhiteSpace(m_data[m_length - 1]))
							break;
						m_data[--m_length] = 0;
					}
					m_data[m_length] = 0;
				}
				return *this;
			}

			string &trim_end(const wchar_t *value)
			{
				if(value)
				{
					while(*value && m_length > 0)
					{
						this->trim_end(*value++);
					}
				}
				return *this;
			}

			string &trim_end(wchar_t value)
			{
				if(this->valid())
				{
					while(m_length > 0)
					{
						if(value != m_data[m_length - 1])
							break;
						m_data[--m_length] = 0;
					}
					m_data[m_length] = 0;
				}
				return *this;
			}

			string &trim_end(char value)
			{
				return this->trim_end(wchar_t(value));
			}

			string &trim()
			{
				this->trim_start();
				return this->trim_end();
			}

			string &trim(wchar_t value)
			{
				this->trim_start(value);
				return this->trim_end(value);
			}

			string &trim(char value)
			{
				return this->trim(wchar_t(value));
			}

			string &trim(const wchar_t *value)
			{
				if(value)
				{
					while(*value && m_length > 0)
					{
						this->trim(*value++);
					}
				}
				return *this;
			}

			//pat = 0 trim start+end, 1 = start, 2 = end
			string &trim_cntrl(int pat = 0)
			{
				if(this->valid())
				{
					if(pat == 0 || pat == 1)
					{
						auto length = m_length;
						const wchar_t *data = m_data;

						while(length > 0 && std::iswcntrl(*data))
						{
							data++; length--;
						}

						if(length < m_length)
						{
							m_length = length;
							::memmove(m_data, data, m_length * sizeof(wchar_t));
							m_data[m_length] = 0;
						}
					}

					m_data[m_length] = 0;

					if(pat == 0 || pat == 2)
					{
						while(m_length > 0)
						{
							if(!std::iswcntrl(m_data[m_length - 1]))
								break;
							m_data[--m_length] = 0;
						}
					}
					m_data[m_length] = 0;
				}
				return *this;
			}

			//Locate first occurrence of character
			size_t index_of(wchar_t pattern, size_t startIndex = 0, bool ignoreCase = true) const
			{
				return string::IndexOf(m_data, pattern, startIndex, ignoreCase);
			}

			size_t index_of(const wchar_t *pattern, size_t startIndex = 0, bool ignoreCase = true) const
			{
				return string::IndexOf(m_data, m_length, pattern, string::Length(pattern), startIndex, ignoreCase);
			}

			size_t index_of(const wchar_t *pattern, size_t pattern_len, size_t startIndex, bool ignoreCase = true) const
			{
				return string::IndexOf(m_data, m_length, pattern, pattern_len, startIndex, ignoreCase);
			}

			// Locate last occurrence of character
			size_t last_index_of(const wchar_t pattern, bool ignoreCase = true) const
			{
				return string::LastIndexOf(m_data, m_length, pattern, ignoreCase);
			}

			size_t last_index_of(const wchar_t *pattern, bool ignoreCase = true) const
			{
				return string::LastIndexOf(m_data, m_length, pattern, ignoreCase);
			}

			const wchar_t *find(wchar_t pattern, size_t startIndex = 0, bool ignoreCase = true) const
			{
				return string::Find(m_data, pattern, startIndex, ignoreCase);
			}

			const wchar_t *find(const wchar_t *pattern, size_t startIndex = 0, bool ignoreCase = true) const
			{
				return string::Find(m_data, m_length, pattern, string::Length(pattern), startIndex, ignoreCase);
			}

			const wchar_t *find_last(const wchar_t pattern, bool ignoreCase = true) const
			{
				return string::FindLast(m_data, m_length, pattern, ignoreCase);
			}

			// Locate last occurrence of character
			const wchar_t *find_last(const wchar_t *pattern, bool ignoreCase = true) const
			{
				return string::FindLast(m_data, m_length, pattern, ignoreCase);
			}

			const wchar_t *find_last(const wchar_t *pattern, size_t pattern_length, bool ignoreCase = true) const
			{
				return string::FindLast(m_data, m_length, pattern, pattern_length, ignoreCase);
			}

			const wchar_t *find_last(const string &pattern, bool ignoreCase = true) const
			{
				return string::FindLast(m_data, m_length, pattern.c_str(), pattern.length(), ignoreCase);
			}

			bool contains(const string &pattern, bool ignoreCase = true) const
			{
				return nullptr != this->find(pattern.c_str(), 0, ignoreCase);
			}

			bool contains(const wchar_t *pattern, bool ignoreCase = true) const
			{
				return nullptr != this->find(pattern, 0, ignoreCase);
			}

			bool contains(const wchar_t pattern, bool ignoreCase = true) const
			{
				return nullptr != this->find(pattern, 0, ignoreCase);
			}

			bool starts_with(const wchar_t *pattern, size_t length, bool ignoreCase = true) const
			{
				if(this->empty() || !pattern)
					return false;
				if(length == 0)
					return false;
				if(length > m_length)
					return false;
				else if(m_length == length)
					return equals(pattern, length, ignoreCase);
				return this->index_of(pattern, length, 0, ignoreCase) == 0;
			}

			bool starts_with(const string &pattern, bool ignoreCase = true) const
			{
				return this->starts_with(pattern, pattern.length(), ignoreCase);
			}

			bool starts_with(const wchar_t* pattern, bool ignoreCase = true) const
			{
				return this->starts_with(pattern, string::Length(pattern), ignoreCase);
			}

			bool starts_with(std::initializer_list<const wchar_t *>patterns, bool ignoreCase = true) const
			{
				for(auto pat : patterns)
					if(starts_with(pat, ignoreCase)) return true;
				return false;
			}

			bool starts_with(wchar_t pattern, bool ignoreCase = true) const
			{
				return Char::Equals(front(), pattern, ignoreCase);
			}

			bool starts_with(std::initializer_list<wchar_t>patterns, bool ignoreCase = true) const
			{
				for(auto pat : patterns)
					if(starts_with(pat, ignoreCase)) return true;
				return false;
			}

			bool ends_with(const string &pattern, bool ignoreCase = true) const
			{
				return this->ends_with(pattern, pattern.length(), ignoreCase);
			}

			bool ends_with(const wchar_t *pattern, size_t length, bool ignoreCase = true) const
			{
				if(this->empty() || !pattern)
					return false;
				if(length == 0)
					return (m_length == 0) ? true : false;
				else if(length > m_length)
					return false;
				else if(m_length == length)
					return this->equals(pattern, length, ignoreCase);

				return string::Equals(&m_data[m_length - length], pattern, ignoreCase);
			}

			bool ends_with(const wchar_t *pattern, bool ignoreCase = true) const
			{
				if(this->empty() || !pattern)
					return false;
				return this->ends_with(pattern, string::Length(pattern), ignoreCase);
			}

			bool ends_with(std::initializer_list<const wchar_t *>patterns, bool ignoreCase = true) const
			{
				for(auto pat : patterns)
					if(ends_with(pat, ignoreCase)) return true;
				return false;
			}

			bool ends_with(std::initializer_list<wchar_t>patterns, bool ignoreCase = true) const
			{
				for(auto pat : patterns)
					if(ends_with(pat, ignoreCase)) return true;
				return false;
			}

			bool ends_with(wchar_t pattern, bool ignoreCase = true) const
			{
				return string::Equals(back(), pattern, ignoreCase);
			}

			bool taged(wchar_t value, bool ignoreCase = false) const
			{
				if(this->starts_with(value, ignoreCase))
					return this->ends_with(value, ignoreCase);
				return false;
			}

			bool taged(std::initializer_list<wchar_t> tags, bool ignoreCase = false) const
			{
				for(auto c : tags)
				{
					if(this->starts_with(c, ignoreCase))
						return this->ends_with(c, ignoreCase);
				}
				return false;
			}

			wchar_t *begin() { return m_data; }
			const wchar_t *begin() const { return m_data; }

			wchar_t *end() { return m_data + m_length; }
			const wchar_t *end() const { return m_data + m_length; }

			wchar_t front() const { return at(0); }

			bool front(wchar_t value, bool ignoreCase = true) const
			{
				return string::Equals(at(0), value, ignoreCase);
			}

			wchar_t back() const { return at(m_length - 1); }

			bool back(wchar_t value, bool ignoreCase = true) const
			{
				return string::Equals(at(m_length - 1), value, ignoreCase);
			}

			string pad_left(size_t totalWidth, wchar_t paddingChar)
			{
				string str(totalWidth, paddingChar);
				return (str + *this).move();
			}

			string pad_left(size_t totalWidth)
			{
				return this->pad_left(totalWidth, L' ').move();
			}

			string pad_right(size_t totalWidth, wchar_t paddingChar)
			{
				string str(totalWidth, paddingChar);
				return (*this + str).move();
			}

			string pad_right(size_t totalWidth)
			{
				return this->pad_right(totalWidth, L' ').move();
			}

			string &toupper()
			{
				if(valid())
					m_data = string::InternalToUpper(m_data);
				return *this;
			}

			string &tolower()
			{
				if(valid())
					m_data = string::InternalTolower(m_data);
				return *this;
			}

			string &capitalize()
			{
				if(valid())
				{
					//bool cap = true;
					for(size_t i = 0u; i < m_length; i++)
					{
						m_data[i] = Char::ToLower(m_data[i]);

						if(i == 0 || m_data[i - 1] == L' ' || !Char::IsLetterOrDigit(m_data[i - 1]))
							m_data[i] = Char::ToUpper(m_data[i]);
							
						/*
						if(Char::IsLower(m_data[i]) && cap == true)
						{
							m_data[i] = Char::ToUpper(m_data[i]);
							cap = false;
						}
						else if(Char::IsWhiteSpace(m_data[i]))
							cap = true;*/
					}
				}
				return *this;
			}

			bool equals(const std::wstring_view &value, bool ignoreCase = true) const
			{
				return this->equals(value.data(), value.length(), ignoreCase);
			}

			bool equals(const std::wstring &value, bool ignoreCase = true) const
			{
				return this->equals(value.c_str(), value.length(), ignoreCase);
			}

			bool equals(const string &value, bool ignoreCase = true) const
			{
				return this->equals(value.m_data, value.m_length, ignoreCase);
			}

			bool equals(const wchar_t &value, bool ignoreCase = true) const
			{
				if(!m_data && !value)
					return true;
				if(m_data && m_length == 1)
					return Char::Equals(m_data[0], value, ignoreCase);
				return false;
			}

			bool equals(const wchar_t *value, bool ignoreCase = true) const
			{
				return string::Equals(m_data, m_length, value, string::Length(value), ignoreCase);
			}

			bool equals(const wchar_t *value, size_t length, bool ignoreCase = true) const
			{
				return string::Equals(m_data, m_length, value, length, ignoreCase);
			}

			bool equals(std::initializer_list<string> strs, bool ignoreCase = true) const
			{
				for(auto &str : strs)
					if(this->equals(str.m_data, str.m_length, ignoreCase))
						return true;
				return false;
			}

			bool equals(std::initializer_list<const wchar_t *> strs, bool ignoreCase = true) const
			{
				for(auto &str : strs)
					if(this->equals(str, ignoreCase))
						return true;
				return false;
			}

			bool equals(std::initializer_list<wchar_t> chs, bool ignoreCase = true) const
			{
				for(auto &c : chs)
					if(this->equals(c, ignoreCase))
						return true;
				return false;
			}
			const wchar_t *get() const
			{
				return valid() ? m_data : nullptr;
			}

			string &set(wchar_t value, size_t index = size_t(-1))
			{
				if(m_length == 0)
					return *this;
				else if(index >= m_length)
					index = m_length - 1;

				//if(verify(index))
				m_data[index] = value;
				return *this;
			}

			const string &set(const wchar_t *value)
			{
				return this->assign(value, string::Length(value));
			}

			string left(size_t length) const
			{
				return this->substr(0, length).move();
			}

			string right(size_t length) const
			{
				return this->substr(m_length - length, length).move();
			}

			string substr(size_t start_index) const
			{
				return this->substr(start_index, m_length - start_index).move();
			}

			string substr(size_t start_index, size_t length) const
			{
				// optimize case of returning entire string
				if((start_index == 0) && (length >= m_length))
					return(*this);

				if(length == 0)
					return nullptr;

				if(!this->valid(start_index))
					return nullptr;

				if((start_index + length) > m_length)
					return nullptr;
				autostr<wchar_t> result(length);
				string::Copy<wchar_t>(result, m_data + start_index, length);
				return result.get();
			}

			const wchar_t *substr2(size_t start_index, size_t length) const
			{
				// optimize case of returning entire string
				if((start_index == 0) && (length >= m_length))
					return(*this);

				if(length == 0)
					return nullptr;

				if(!this->valid(start_index))
					return nullptr;

				if((start_index + length) > m_length)
					return nullptr;
				return m_data + start_index;
			}

			// Replace all occurrences of character 'chOld' with character 'chNew'
			string &replace(const wchar_t &oldChar, const wchar_t &newChar, bool ignoreCase = true)
			{
				if(valid())
				{
					for(size_t i = 0; i < m_length; i++)
					{
						if(Char::Equals(oldChar, m_data[i], ignoreCase))
							m_data[i] = newChar;
					}
				}
				return *this;
			}

			// Replace all occurrences of string 'oldValue' with string 'newValue'
			string &replace(const string &old_value, const string &new_value, bool ignoreCase = true)
			{
				if(!valid() || old_value.empty())
					return *this;
				return this->replace(old_value.c_str(), old_value.length(),
									 new_value, new_value.length(), ignoreCase);
			}

			// Replace all occurrences of string 'oldValue' with string 'newValue'
			string &replace(const wchar_t *old_value, const wchar_t *new_value, bool ignoreCase = true)
			{
				if(!valid() || !old_value)
					return *this;
				return this->replace(old_value, string::Length(old_value),
									 new_value, string::Length(new_value), ignoreCase);
			}

			// Replace all occurrences of string 'oldValue' with string 'newValue'
			/*
			
			string &replace2(const wchar_t *old_value, size_t old_length, const wchar_t *new_value, size_t new_length, bool ignoreCase = true)
			{
				if(!valid() || !old_value)
					return *this;

				size_t tok_index{};
				const wchar_t *tok, *oldstr;
				size_t loop = 0u;

				while(nullptr != (tok = string::Find(m_data, m_length, old_value, old_length, tok_index, ignoreCase)))
				{
					oldstr = m_data;
					auto oldstr_len = m_length;
					m_length = size_t(oldstr_len - (old_length + new_length));
					
					if((int64_t)m_length < 0)
						m_length = old_length + new_length;

					m_capacity = size_t(m_length + 10);
				
					m_data = new wchar_t[m_capacity] {};
					tok_index = size_t(tok - oldstr);

					string::Copy(m_data, oldstr, tok_index);
					string::Copy(m_data + tok_index, new_value, new_length);
					string::Copy(m_data + tok_index + new_length, tok + old_length, oldstr_len - old_length - tok_index);

					m_data[m_length] = 0;

					delete[] oldstr;

					tok_index += new_length;

					if(++loop >= m_length)
						break;
				}
				return *this;
			}

string& Replace(const wchar_t* pszOld, const wchar_t *pszNew, bool h)
			{
				// can't have empty or NULL lpszOld
				// nSourceLen is in XCHARs
				int nSourceLen = string::Length(pszOld);

				if(nSourceLen == 0)
					return *this;

				// nReplacementLen is in XCHARs
				int nReplacementLen = string::Length(pszNew);

				// loop once to figure out the size of the result string
				int nCount = 0;
				{
					const wchar_t *pszStart = m_data;
					const wchar_t *pszEnd = pszStart + m_length;
					while(pszStart < pszEnd)
					{
						const wchar_t *pszTarget;
						while((pszTarget = Find(pszStart, pszOld)) != NULL)
						{
							nCount++;
							pszStart = pszTarget + nSourceLen;

						}
						pszStart += string::Length(pszStart) + 1;
					}
				}

				// if any changes were made, make them
				if(nCount > 0)
				{
					// if the buffer is too small, just
					//   allocate a new buffer (slow but sure)
					int nOldLength = m_length;
					int nNewLength = nOldLength + (nReplacementLen - nSourceLen) * nCount;
					const wchar_t *pszBuffer = buffer(__max(nNewLength, nOldLength));
					const wchar_t *pszStart = pszBuffer;
					const wchar_t *pszEnd = pszStart + nOldLength;

					// loop again to actually do the work
					while(pszStart < pszEnd)
					{
						wchar_t *pszTarget = nullptr;

						while((pszTarget = (wchar_t *)Find(pszStart, pszOld)) != NULL)
						{
							int nBalance = nOldLength - int(pszTarget - pszBuffer + nSourceLen);
							memmove_s(pszTarget + nReplacementLen, nBalance * sizeof(wchar_t), pszTarget + nSourceLen, nBalance * sizeof(wchar_t));
							memcpy_s(pszTarget, nReplacementLen * sizeof(wchar_t), pszNew, nReplacementLen * sizeof(wchar_t));
							pszStart = pszTarget + nReplacementLen;
							pszTarget[nReplacementLen + nBalance] = 0;
							nOldLength += (nReplacementLen - nSourceLen);
						}
						pszStart += string::Length(pszStart) + 1;
					}
					release(nNewLength);
				}
				return *this;
			}

			string &replace3(const wchar_t *old_value, size_t old_length, const wchar_t *new_value, size_t new_length, bool ignoreCase = true)
			{
				if(!valid() || !old_value)
					return *this;

				string ret;

				for(size_t i = 0;;)
				{
					auto tok = string::Find(m_data, m_length, old_value, old_length, i, ignoreCase);
					if(tok)
					{
						auto tok_index = size_t(tok - m_data);
						//ret.append(substr2(i, tok_len), tok_len);
						//ret.append(m_data + i, tok_len);
						ret.append(substr(i, tok_index));

						if(new_length == 0)
							i = tok_index + 1;
						else
						{
							ret.append(new_value, new_length);
							i = tok_index + new_length;
						}
					}
					else
					{
						ret.append(substr(i + (old_length - new_length)));
						//ret.append(m_data + i + (old_length - new_length), m_length - i);
						break;
					}
				}

				*this = ret.move();

				return *this;
			}
			*/
/*
std::wstring replace(std::wstring const &original,
		   std::wstring const &before,
		   std::wstring const &after)
{
	std::wstring retval;
	auto end = original.end();
	auto current = original.begin();
	auto next =	std::search(current, end, before.begin(), before.end());

	while(next != end) {
		retval.append(current, next);
		retval.append(after);
		current = next + before.size();
		next = std::search(current, end, before.begin(), before.end());
	}

	retval.append(current, next);

	return retval;
}
*/
			/*static const wchar_t *_StrStr(const wchar_t *str, const wchar_t *substring, bool ignoreCase = true)
			{
				const wchar_t *a = 0, *b = substring;

				if(*b == 0)
					return str;

				for(; *str != 0; str += 1)
				{
					if(*str != *b)
					{
						if(!ignoreCase)
							continue;
						if(towupper(*str) != towupper(*b))
							continue;
					}

					a = str;

					while(true)
					{
						if(*b == 0)
							return str;

						if(*a++ != *b++)
						{
							if(!ignoreCase)
								break;
							if(towupper(*a++) != towupper(*b++))
								break;
						}
					}
					b = substring;
				}
				return nullptr;
			}*/

			static const wchar_t *_StrStr(const wchar_t *pszFirst, const wchar_t *pszSrch, bool ignoreCase = true)
			{
				wchar_t *cp = (wchar_t *)pszFirst, *s1, *s2;

				while(*cp)
				{
					s1 = cp;
					s2 = (wchar_t *)pszSrch;

					while(*s1 && *s2 && (ignoreCase ? (towupper(*s1) - towupper(*s2)) : (*s1 - *s2)) == 0)
						s1++, s2++;

					if(*s2 == L'\0')
						return cp;
					cp++;
				}

				return nullptr;
			}

			string &replace(const wchar_t *old_value, size_t old_length, const wchar_t *new_value, size_t new_length, bool ignoreCase = true)
			{
				if(!valid() || !old_value)
					return *this;

				// using deque for pop_front
				std::deque<const wchar_t *> positions;

				for(const wchar_t *cur = m_data, *pos = _StrStr(cur, old_value, ignoreCase); pos; cur = pos + 1, pos = _StrStr(cur, old_value, ignoreCase))
					positions.push_back(pos);

				string n;
				auto newstr = n.buffer(m_length + new_length * positions.size() + 1);
				//auto newstr = new wchar_t[m_length + new_length * positions.size() + 1];

				auto dst = newstr;
				auto src = m_data;

				while(src <= m_data + m_length)
				{
					if(!positions.empty() && src == positions.front())
					{
						Copy(dst, new_value, new_length);
						dst += new_length;
						src += old_length;
						positions.pop_front();
					}
					else
						*dst++ = *src++;
				}
				*this = n.release().move();
				return *this;
			}

			size_t remove_chars(const wchar_t *pattern, bool ignoreCase = true)
			{
				size_t count = 0;
				if(valid() && pattern)
				{
					auto src = m_data, dst = m_data;
					for(size_t i = 0; i < m_length; i++)
					{
						if(string::Find(pattern, *src, 0, ignoreCase))
						{
							src++;
							count++;
						}
						else
							*dst++ = *src++;
					}
					*dst = 0;
					m_length -= count;
				}
				return count;
			}

			size_t remove_char(const wchar_t &pattern)
			{
				size_t count = 0;
				if(valid() && pattern)
				{
					size_t r = 0;
					for(size_t i = 0; i < m_length; i++)
					{
						if(m_data[i] != pattern)
							m_data[r++] = m_data[i];
						else
							count++;
					}
					m_data[r] = 0;
					m_length -= count;
				}
				return count;
			}

			string &remove(size_t index, size_t count = -1)
			{
				size_t newLength = m_length;
				if(valid(index) && count != 0)
				{
					newLength = index;
					if(count != -1)
					{
						if(index + count > m_length)
							count = m_length - index;
						
						newLength = m_length - count;

						size_t nCharToCopy = m_length - (index + count) + 1;
						::memmove_s(m_data + index,
									size_t(nCharToCopy * sizeof(wchar_t)),
									m_data + index + count,
									size_t(nCharToCopy * sizeof(wchar_t)));
					}
				}
				return this->terminate(newLength);
			}

			string &remove_left(size_t count)
			{
				auto index = m_length - count;
				if(index > m_length)
					index = m_length;
				return this->terminate(index);
			}

			string &insert(size_t index, const wchar_t &ch)
			{
				if(valid(index))
				{
					auto newLength = m_length + 1;
					this->capacity(newLength, true);
					// move existing bytes down
					::memmove_s(m_data + index + 1, (size_t(newLength) - index) * sizeof(wchar_t),
								m_data + index, (size_t(newLength) - index) * sizeof(wchar_t));
					m_data[index] = ch;
					this->terminate(newLength);
				}
				return *this;
			}

			string &insert(size_t index, const wchar_t *value)
			{
				size_t index_t = index;
				if(valid(index) && value)
				{
					auto length = string::Length(value);
					auto newLength = m_length;
					if(length > 0)
					{
						auto size = sizeof(wchar_t);
						newLength += length;
						this->capacity(newLength, true);
						// move existing bytes down
						::memmove_s(m_data + index_t + length,
									(newLength - index_t - length + 1) * size,
									m_data + index_t,
									(newLength - index_t - length + 1) * size);

						::memcpy_s(m_data + index_t,
								   length * size, value, length * size);
						this->terminate(newLength);
					}
				}
				return *this;
			}

			bool is_number() const
			{
				if(this->empty())
					return false;
				for(auto &c : *this)
				{
					if(!c) return false;
					if(!(c >= 0x30 && c <= 0x39)) return false;
				}
				return true;
			}

			template<typename T = double>
			bool parse_number(T *value, T default_value = 0)
			{
				return string::ParseNumber<T, wchar_t>(m_data, value, default_value);
			}

			template<typename T = __int64>
			T to_int(T default_value = 0) const 
			{ 
				return string::ToInt<T, wchar_t>(m_data, default_value);
			}

			auto hash() const 
			{
				//return valid() ? string::Hash(m_data) : 0u;
				return string::Hash(m_data);
			}

			/*size_t split(std::vector<string> &dst, wchar_t delim)
			{
				if(this->empty())
					return 0U;

				size_t i = 0, c = 0;
				wchar_t ch = m_data[i++];
				string value;

				while(ch)
				{
					if(delim == ch)
						dst.push_back(value.move());
					else
						value += ch;
					ch = m_data[i++];
				}

				if(c) dst.push_back(string((const wchar_t *)word, c).move());

				return dst.size();
			}*/

			size_t split(std::vector<string> &list, wchar_t delim, bool allow_skip = true) const
			{
				if(this->empty()) return 0U;

				string value;
				for(size_t i = 0; i < m_length; i++)
				{
					auto ch = m_data[i];
					if(delim == ch)
					{
						if(allow_skip && delim == m_data[i + 1])
							i++;
						else if(!value.trim().empty())
						{
							list.emplace_back(value.move());
							continue;
						}
					}
					value += ch;
				}

				if(!value.trim().empty())
					list.emplace_back(value.move());

				return list.size();
			}
#pragma region operator

			// operator

			template<typename T=size_t>
			wchar_t &operator [](T index)
			{
				return m_data[static_cast<size_t>(index)];
			}

			const wchar_t &operator [](size_t index) const
			{
				return at(index);
			}

			// operator nullptr_t() const { return  0; }
			operator Char() const { return (Char)(valid() ? m_data[0] : 0); }
			operator char() const { return valid() ? (char)m_data[0] : 0; }
			operator wchar_t() const { return valid() ? m_data[0] : 0; }
			operator wchar_t *() const { return valid() ? m_data : nullptr; }
			operator wchar_t const *() const { return valid() ? m_data : nullptr; }
			operator string() const { return *this; }

			operator std::wstring() const
			{
				return valid() ? std::move(std::wstring(m_data, m_length)) : std::move(std::wstring());
			}

			operator std::wstring_view() const
			{
				return valid() ? std::move(std::wstring_view(m_data, m_length)) : std::move(std::wstring_view());
			}

			explicit operator bool() const { return !empty(); }
			bool operator !() const { return empty(); }

			template<typename T>
			operator T() const { return T(valid() ? m_data : EMPTY); }

			// Assignment Operators
			// operator =

			string &operator =(nullptr_t) { zero(); m_length = 0; return *this; }

			// copy
			string &operator =(const string &rhs)
			{
				return (this == &rhs) ? *this : assign(rhs.m_data, rhs.m_length);
			}
			// moveable assignment
			string &operator =(string &&rhs) noexcept { return this->assign(rhs.move()); }

			string &operator =(const std::wstring &rhs)
			{
				return assign(rhs.c_str(), rhs.length());
			}

			string &operator =(const std::wstring_view &rhs)
			{
				return assign(rhs.data(), rhs.length());
			}

			//string &operator +=(const wchar_t *rhs) { return append(rhs); }
			//string &operator +=(const string &rhs) { return append(rhs); }

			//  operator +=
		//	string &operator +=(nullptr_t) { return *this; }

			template<typename T>
			string &operator +=(const T &rhs) { return append(rhs); }

			// Logical Operators
			//bool operator ==(nullptr_t) { return !valid(); }
			//bool operator ==(const string &rhs) { return equals(rhs, false); }
			//bool operator ==(const wchar_t *rhs) { return equals(rhs, false); }

			//bool operator !=(nullptr_t) { return (m_data != nullptr); }
			//bool operator !=(const string &rhs) { return !equals(rhs, false); }
			//bool operator !=(const wchar_t *rhs) { return equals(rhs, false); }

			//  operator +
			friend string operator +(const string &lhs, const string &rhs) { return string(lhs).append(rhs).move(); }
			//friend string operator +(const string & left, const nullptr_t&) { return left; }

			template<typename T>
			friend string operator +(const string &lhs, const T &rhs) { return string(lhs).append(rhs).move(); }

			template<typename T>
			friend string operator +(const T &lhs, const string &rhs) { return string(lhs).append(rhs).move(); }

			// Logical Operators
			friend bool operator ==(nullptr_t, const string &rhs) { return rhs.length() == 0; }
			friend bool operator ==(const string &lhs, nullptr_t) { return lhs.length() == 0; }
			friend bool operator ==(const string &lhs, const string &rhs) { return lhs.equals(rhs, false); }
			friend bool operator !=(const string &lhs, const string &rhs) { return !lhs.equals(rhs, false); }
			friend bool operator !=(const string &lhs, nullptr_t) { return lhs.length() != 0; }
			friend bool operator !=(nullptr_t, const string &rhs) { return rhs.length() != 0; }
			

#pragma endregion

			template<typename T>
			static bool IsNullOrWhiteSpace(T const *value)
			{
				if(value && *value)
				{
					auto buf = value;
					while(buf)
					{
						if(!Char::IsWhiteSpace(*buf++))
							return false;
					}
				}
				return true;
			}

			template<typename T>
			static bool IsNullOrWhiteSpace(T const *value, size_t len)
			{
				if(value && *value)
				{
					for(size_t i = 0; i < len; i++)
					{
						auto c = &value[i];
						if(*c == 0)
							break;
						if(!Char::IsWhiteSpace(*c))
							return false;
					}
				}
				return true;
			}

			template<typename T>
			static bool IsNullOrEmpty(T const *value)
			{
				return (!value || *value == NULL);
			}

			template<typename T>
			static size_t Length(T const *str)
			{
				size_t length = 0;
				if(str) while(*str++) length++;

				return length;
			}

			template<typename T>
			static T *Set(T *str, size_t index, T value, size_t count)
			{
				if(str)
				{
					for(size_t i = index; i < count; i++)
						str[i] = value;
				}
				return str;
			}

			template<typename T>
			static T *Set(T *str, T value, size_t count)
			{
				return string::Set(str, 0, value, count);
			}

			// convert to others
			static wchar_t *Convert(wchar_t *dst, const char *src)
			{
				if(!dst || !src || *src == NULL)
					return nullptr;
				wchar_t *s = dst;
				while(*src)
					*s++ = (wchar_t)*src++;
				*s = NULL;
				return dst;
			}

			static wchar_t *Convert(wchar_t *dst, const char *src, size_t count)
			{
				if((count <= 0) || !dst || !src || (*src == NULL))
					return nullptr;
				auto str = dst;
				for(size_t i = 0; i < count && *src; i++)
					*str++ = (wchar_t)*src++;
				*str = NULL;
				return dst;
			}

			static char *Copy(char *dst, const char *src)
			{
				if(dst && src && *src != NULL)
				{
					auto s = dst;
					while(*src) *s++ = *src++;
					*s = NULL;
					return dst;
				}
				return nullptr;
			}

			static wchar_t *Copy(wchar_t *dst, const wchar_t *src)
			{
				if(dst && src && *src != NULL)
				{
					auto s = dst;
					while(*src) *s++ = *src++;
					*s = L'\0';
					return dst;
				}
				return nullptr;
			}

			/*template<typename T>
			static T *Copy(T const *src, size_t count)
			{
				if(src)
				{
					auto dst = new T * [count + 1]{ };
					return Copy(dst, src, count);
				}
				return nullptr;
			}*/

			template<typename T>
			static T *Copy(T *dst, T const *src, size_t count)
			{
				if(dst && src && *src != NULL)
				{
					auto s = dst;
					while(*src)
					{
						*s++ = *src++;
						if(--count == 0)
							break;
					}
					*s = L'\0';
					return dst;
				}
				return nullptr;
			}

			template<typename T>
			static T *Copy(T *dst, size_t dst_index, T const *src, size_t src_index)
			{
				return string::Copy(dst + dst_index, src + src_index);
			}

			template<typename T>
			static T *Copy(T *dst, size_t dst_index, T const *src, size_t src_index, size_t count)
			{
				return string::Copy(dst + dst_index, src + src_index, count);
			}

			template<typename T>
			static bool Equals(T const *value1, size_t value1_length,
							   T const *value2, size_t value2_length,
							   bool ignoreCase = true)
			{
				auto l1 = (value1_length == -1) ? string::Length(value1) : value1_length;
				auto l2 = (value2_length == -1) ? string::Length(value2) : value2_length;

				if(l1 == l2)
				{
					if(l1 == 0)
						return true;

					size_t sz = l1 * sizeof(value1[0]);
					if(ignoreCase)
						return (0 == ::_memicmp(value1, value2, sz));
					else
						return (0 == ::memcmp(value1, value2, sz));
				}

				return false;
			}

			template<typename T>
			static bool Equals(T const *value1, T const *value2, size_t count, bool ignoreCase)
			{
				if(value1 && value2)
				{
					auto sz = count * sizeof(T);
					if(ignoreCase)
						return (0 == ::_memicmp(value1, value2, sz));
					else
						return (0 == ::memcmp(value1, value2, sz));
				}
				return false;
			}

			template<typename T>
			static bool Equals(T const *value1, T const *value2, bool ignoreCase = true)
			{
				return Equals(value1, size_t(-1), value2, size_t(-1), ignoreCase);
			}

			static bool Equals(const string &value1, const string &value2, bool ignoreCase = true)
			{
				return string::Equals(value1.c_str(), value1.length(),
									  value2.c_str(), value2.length(), ignoreCase);
			}

			template<typename T>
			static bool Equals(T const *value, std::initializer_list<T const *> values, bool ignoreCase)
			{
				for(auto val : values)
				{
					if(string::Equals(value, size_t(-1), val, size_t(-1), ignoreCase)) return true;
				}
				return false;
			}

			static bool Equals(string const &value, std::initializer_list<string> values, bool ignoreCase = true)
			{
				for(auto const &val : values)
				{
					if(string::Equals(value.c_str(), value.length(), val.c_str(), val.length(), ignoreCase)) return true;
				}
				return false;
			}

			template<typename T>
			static bool Contains(T const *value, T const *pattren, bool ignoreCase = true)
			{
				return (string::Find(value, pattren, 0, ignoreCase) != nullptr);
			}

			template<typename T>
			static T const *Find(T const *value, T pattern, size_t start_index = 0, bool ignoreCase = true)
			{
				auto str = value + start_index;
				if(str && pattern)
				{
					for(; *str; str++)
					{
						if(Char::Equals(*str, pattern, ignoreCase))
							return str;
					}
				}
				return nullptr;
			}

			template<typename T>
			static T const *Find(T const *str, T const *pattern, size_t start_index = 0, bool ignoreCase = true)
			{
				return string::Find(str, string::Length(str), pattern, string::Length(pattern), start_index, ignoreCase);
			}

			template<typename T>
			static T const *Find(T const *str, size_t str_length, T const *pattern, size_t start_index = 0, bool ignoreCase = true)
			{
				return string::Find(str, str_length, pattern, string::Length(pattern), start_index, ignoreCase);
			}

			template<typename T>
			static T const *Find(T const *value, size_t value_length, T const *pattern, size_t pattern_length, size_t start_index = 0, bool ignoreCase = true)
			{
				if(!value_length || !pattern_length)
					return nullptr;

				if(pattern_length > value_length)
					return nullptr;

				if(start_index >= value_length)
					return nullptr;

				if((start_index + pattern_length) > value_length)
					return nullptr;

				if(!value || !pattern)
					return nullptr;

				T const *v = nullptr;
				const auto sz = pattern_length * sizeof(T);
				for(auto i = start_index; i < value_length; i++)
				{
					v = value + i;
					if(ignoreCase)
					{
						if(0 == ::_memicmp(v, pattern, sz))
							return v;
					}
					else if(0 == ::memcmp(v, pattern, sz))
						return v;
				}
				return nullptr;
			}

			template<typename T>
			static T const *Find0(T const *value, size_t value_length, T const *pattern, size_t pattern_length, size_t start_index = 0, bool ignoreCase = true)
			{
				if(!value_length || !pattern_length)
					return nullptr;

				if(pattern_length > value_length)
					return nullptr;

				if(start_index >= value_length)
					return nullptr;

				if((start_index + pattern_length) > value_length)
					return nullptr;

				if(!value || !pattern)
					return nullptr;

				T const *v = nullptr;
				const auto sz = pattern_length * sizeof(T);
				for(auto i = start_index; i < value_length; i++)
				{
					v = value + i;
					if(ignoreCase)
					{
						if(0 == ::_memicmp(v, pattern, sz))
							return v;
					}
					else if(0 == ::memcmp(v, pattern, sz))
						return v;
				}
				return nullptr;
			}

			template<typename T>
			static T const *FindLast(T const *value, T pattern, bool ignoreCase = true)
			{
				return FindLast(value, Length(value), pattern, ignoreCase);
			}

			template<typename T>
			static T const *FindLast(T const *value, size_t length, T pattern, bool ignoreCase = true)
			{
				T const *result = nullptr;
				if(value && pattern && length > 0)
				{
					for(intptr_t i = length - 1; i >= 0; i--)
					{
						if(Char::Equals(value[i], pattern, ignoreCase))
						{
							result = value + i;
							break;
						}
					}
				}
				return result;
			}

			template<typename T>
			static T const *FindLast(T const *value, T const *pattern, bool ignoreCase = true)
			{
				return FindLast(value, string::Length(value), pattern, string::Length(pattern), ignoreCase);
			}

			template<typename T>
			static T const *FindLast(T const *value, size_t value_length, T const *pattern, bool ignoreCase = true)
			{
				return FindLast(value, value_length, pattern, string::Length(pattern), ignoreCase);
			}

			template<typename T>
			static T const *FindLast(T const *value, size_t value_length, T const *pattern, size_t pattern_length, bool ignoreCase = true)
			{
				if(!value_length || !pattern_length)
					return nullptr;

				if(pattern_length > value_length)
					return nullptr;

				if(!value || !pattern)
					return nullptr;

				T const *v = nullptr;
				auto sz = pattern_length * sizeof(T);
				auto i = (value_length == pattern_length) ? 0 : value_length - pattern_length;
				for(; i >= 0 && i < value_length; i--)
				{
					v = value + i;
					if(ignoreCase)
					{
						if(0 == ::_memicmp(v, pattern, sz))
							return v;
					}
					else if(0 == ::memcmp(v, pattern, sz))
						return v;
				}
				return nullptr;
			}

			template<typename T>
			static size_t IndexOf(T const *value, T const &pattern, size_t start_index, bool ignoreCase = true)
			{
				auto result = string::Find(value, pattern, start_index, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t IndexOf(T const *value, size_t value_len, T const &pattern, size_t pattern_len, size_t start_index, bool ignoreCase = true)
			{
				auto result = string::Find(value, value_len, pattern, pattern_len, start_index, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t IndexOf(T const *value, T const *pattern, size_t start_index, bool ignoreCase = true)
			{
				auto result = string::Find(value, pattern, start_index, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t IndexOf(T const *value, size_t value_len, T const *pattern, size_t pattern_len, size_t start_index, bool ignoreCase = true)
			{
				if(!value || !pattern || (value_len == 0) || (start_index >= value_len))
					return npos;
				auto result = string::Find(value, value_len, pattern, pattern_len, start_index, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t LastIndexOf(T const *value, size_t value_length, T pattern, bool ignoreCase = true)
			{
				auto result = string::FindLast(value, value_length, pattern, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t LastIndexOf(T const *value, T pattern, bool ignoreCase = true)
			{
				auto result = string::FindLast(value, pattern, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t LastIndexOf(T const *value, T const *pattern, bool ignoreCase = true)
			{
				auto result = string::FindLast(value, pattern, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t LastIndexOf(T const *value, size_t value_length, T const *pattern, bool ignoreCase = true)
			{
				auto result = string::FindLast(value, value_length, pattern, string::Length(pattern), ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			template<typename T>
			static size_t LastIndexOf(T const *value, size_t value_length, T const *pattern, size_t pattern_length, bool ignoreCase = true)
			{
				auto result = string::FindLast(value, value_length, pattern, pattern_length, ignoreCase);
				return result ? size_t(result - value) : npos;
			}

			static string Capitalize(const wchar_t *value, size_t length)
			{
				if(!value || length == 0)
					return nullptr;
				return string(value, length).capitalize().move();
			}

			static string Capitalize(const wchar_t *value)
			{
				return Capitalize(value, Length(value)).move();
			}

			static string Capitalize(const string &value)
			{
				return string::Capitalize(value, value.length()).move();
			}

			static string ToUpper(const wchar_t *value, size_t length)
			{
				if(!value || length == 0)
					return nullptr;
				return string(value, length).toupper().move();
			}

			static string ToUpper(const wchar_t *value)
			{
				return string::ToUpper(value, Length(value)).move();
			}

			static string ToUpper(const string &value)
			{
				return string::ToUpper(value, value.length()).move();
			}

			static string ToLower(const wchar_t *value, size_t length)
			{
				if(!value || length == 0)
					return nullptr;
				return string(value, length).tolower().move();
			}

			static string ToLower(const wchar_t *value)
			{
				return string::ToLower(value, Length(value)).move();
			}

			static string ToLower(const string &value)
			{
				return string::ToLower(value, value.length()).move();
			}

			template<typename T>
			static T *Duplicate(T const *value)
			{
				return string::Duplicate(value, Length(value));
			}

			template<typename T>
			static T *Duplicate(T const *value, size_t length)
			{
				return string::Copy(new T[length + 1]{}, value, length);
			}

			template<typename T>
			static T *Alloc(size_t length)
			{
				return string::Set(new T[length]{}, 0, length);
			}

			template<typename T>
			static T *Realloc(T *str, size_t length, bool deleteOld = true)
			{
				auto newstr = string::Alloc<T>(length);
				if(str)
				{
					string::Copy<T>(newstr, str);
					if(deleteOld)
						delete[] str;
				}
				return newstr;
			}

			template<typename T>
			static T *Append(T *dst, T const *str)
			{
				auto ret = dst;
				if(dst && str)
				{
					// Find the end of the  string
					while(*dst)
						dst++;
					while(*str)
						*dst++ = *str++;
					*dst = NULL;
				}
				return ret;
			}

			template<typename T>
			static T *Append(T *dst, T const *str, size_t length)
			{
				auto ret = dst;
				if(length > 0 && dst && str)
				{
					// Find the end of the  string
					while(*dst)
						dst++;
					while(length-- && *str)
						*dst++ = *str++;
					*dst = NULL;
				}
				return ret;
			}

			template<typename T>
			static T *Reverse(T *str)
			{
				auto start = str;
				if(str)
				{
					auto left = str;
					T ch;
					// Find the end of the  string
					while(*str++)
						;
					str -= 2;
					while(left < str)
					{
						ch = *left;
						*left++ = *str;
						*str-- = ch;
					}
				}
				return start;
			}

			template <typename T>
			static T Argument(T value) noexcept
			{
				return value;
			}

			template <typename T>
			static T const *Argument(string const &value) noexcept
			{
				return value.c_str();
			}

			template <typename T>
			static T const *Argument(std::basic_string<T> const &value) noexcept
			{
				return value.c_str();
			}

			template <typename ... Args>
			static int32_t StringPrintF(char *const buffer, const size_t bufferCount,
								   char const *const format, Args const & ... args) noexcept
			{
				return snprintf(buffer, bufferCount, format, Argument(args) ...);
			}

			template <typename ... Args>
			static int32_t StringPrintF(wchar_t *const buffer, const size_t bufferCount,
								   wchar_t const *const format, Args const & ... args) noexcept
			{
				return swprintf(buffer, bufferCount, format, Argument(args) ...);
			}

			template <typename T, typename ... Args>
			static int32_t Format(std::basic_string<T> &buffer, T const *const format, Args const & ... args)
			{
				auto size = StringPrintF(nullptr, 0, format, args ...);
				if(size > 0)
				{
					buffer.resize(size);
					StringPrintF(&buffer[0], buffer.size() + 1, format, args ...);
				}
				return size;
			}

			template <typename ... Args>
			static int32_t Format(string &buffer, const wchar_t *format, Args const& ...args)
			{
				auto size = StringPrintF(nullptr, 0, format, args ...);
				if(size > 0)
				{
					buffer.capacity(size);
					StringPrintF(buffer.data(), size_t(buffer.size() + 1), format, Argument(args)...);
				}
				return size;
			}

			static string ToString(wchar_t const * const format, ...)
			{
				string result;
				if(format && *format)
				{
					va_list argList;
					va_start(argList, format);
					result.format(argList, format);
					va_end(argList);
				}
				return result.move();
			}

			static string ToString(int n)
			{
				return ToString(L"%d", n).move();
			}

			/// <summary>
			/// Returns a string representation of the value of this Guid instance, according to the provided format specifier.
			/// </summary>
			/// <param name="guid"></param>
			/// <param name="format">
			/// 0=32 digits:
			/// 1=32 digits separated by hyphens:
			/// 2=32 digits separated by hyphens, enclosed in braces:
			/// 3=32 digits separated by hyphens, enclosed in parentheses:
			/// </param>
			/// <returns></returns>
			static string ToString(const GUID &guid, int format = 0)
			{
				string d = L"";
				string parentleft = L"";
				string parentright = L"";

				if(format > 0)
				{
					d = L"-";

					if(format == 2)
					{
						parentleft = L"{";
						parentright = L"}";
					}
					else if(format == 3)
					{
						parentleft = L"(";
						parentright = L")";
					}
				}

				string fmt = 
					parentleft + 
					L"%-08.8X" + d + 
					L"%-04.4X" + d + 
					L"%-04.4X" + d + 
					L"%-02.2X%-02.2X" + d + 
					L"%02.2X%-02.2X%-02.2X%-02.2X%-02.2X%-02.2X" + 
					parentright;

				return string::ToString(fmt.c_str(),
										guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],
										guid.Data4[1], guid.Data4[2], guid.Data4[3], 
										guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]).move();
			}

			static string TimeFormat(wchar_t const * const format = L"y.m.d-H.M.S")
			{
				SYSTEMTIME lt = { };
				::GetLocalTime(&lt);
				return TimeFormat(&lt, format).move();
			}

			static string TimeFormat(SYSTEMTIME *time, wchar_t const *format = L"y.m.d-H.M.S")
			{
				if(!format || !format[0])
					return nullptr;

				string s(string::Length(format) * 2);
				auto padding = L"%02u";
				auto c = *format;
				while(c)
				{
					switch(c)
					{
						case L't':	// time 1:15:20 AM
							s.append_format(L"%u:%02u:%02u %s",
											((time->wHour + 11) % 12 + 1), time->wMinute, time->wSecond,
											time->wHour >= 12 ? L"PM" : L"AM");
							break;
						case L'T':	// time 13:15:20
							s.append_format(L"%02u:%02u:%02u", time->wHour, time->wMinute, time->wSecond);
							break;
						case L'D':	// data 04/11/2018
							s.append_format(L"%02u/%02u/%02u", time->wDay, time->wMonth, time->wYear);
							break;
						case L'p':	// AM or PM
						case L'P':
							s.append(time->wHour >= 12 ? L"PM" : L"AM");
							break;
						case L's':	// seconds (000-999)
							s.append_format(L"%03u", time->wMilliseconds);
							break;
						case L'S':	// seconds (00-59)
							s.append_format(padding, time->wSecond);
							break;
						case L'M':	// minute (00-59)
							s.append_format(padding, time->wMinute);
							break;
						case L'H':	// hour of the day (00-23)
							s.append_format(padding, time->wHour);
							break;
						case L'h':	// hour of the day (01-12)
							s.append_format(padding, (time->wHour + 11) % 12 + 1);
							break;
						case L'd':	// day of the month (01-31) 
							s.append_format(padding, time->wDay);
							break;
						case L'w':// day of the week (1-7) 
							s.append_format(L"%u", time->wDayOfWeek + 1);
							break;
						case L'm':	// month of the year (01-12)
							s.append_format(padding, time->wMonth);
							break;
						case L'Y':	// two-digit year representation (00-99)
							s.append_format(padding, time->wYear % 100);
							break;
						case L'y':	// four-digit year representation
							s.append_format(L"%04u", time->wYear);
							break;
						default:
							s.append(c);
							break;
					}
					c = *++format;
				}
				return s.move();
			}

			static string NewGuid(int format = 0)
			{
				GUID guid{};
				if(S_OK == ::CoCreateGuid(&guid))
					return string::ToString(guid, format).move();
				return nullptr;
			}

			template<typename NumType = double, typename T = wchar_t>
			static bool ParseNumber(const T *str, NumType *value, NumType default_value = 0)
			{
				auto s = str;
				if(!s || !s[0])
				{
					if(value) *value = default_value;
					return false;
				}

				int count = 0;
				double val{};
				bool sign = false;
				bool isfloat = false;

				if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
				{
					s++; //skip 0
					s++; // skip x
					while(std::isxdigit(*s))
					{
						auto v = std::iswdigit(*s) ? (*s - '0') : ((*s & 0xdf) - 'A' + 10);
						val = double((intptr_t(val) << 4) | v);
						s++;
						count++;
					}

					if(value)
					{
						if(count == 0)
						{
							*value = (NumType)val;
							return false;
						}

						*value = (NumType)val;
					}
					return true;
				}

				if(*s == '-' || *s == '+')
				{
					sign = *s++ == '-';
				}

				int mantissa = 1;
				while(std::isdigit(*s))
				{
					val = val * 10.0 + *s++ - '0';
					mantissa = mantissa * 10;
					count++;
				}

				if(isfloat)
				{
					val = val / (double)mantissa;
				}
				else if(*s == '.' && std::isdigit(s[1]))
				{
					s++;// skip '.'
					double mantissa_val = 0;
					mantissa = 1;
					while(std::isdigit(*s))
					{
						mantissa_val = mantissa_val * 10.0 + (*s++ - '0');
						mantissa = mantissa * 10;
					}
					val += mantissa_val / (double)mantissa;
				}

				if(sign && val == 0) sign = false;

				if(value)
				{
					if(count == 0)
					{
						*value = default_value;
						return false;
					}

					*value = NumType(sign ? -val : val);
				}
				return true;
			}

			template<typename NumType = int32_t, typename T = wchar_t>
			static NumType ToInt(T const *str, NumType default_value = 0)
			{
				int len = 0;
				NumType minus = 1, sum = 0;

				if(str)
				{
					while(*str == ' ' || *str == '\t')
						str++;

					if(str[0] == '-')
					{
						minus = -1;
						str++;
					}

					while(*str >= '0' && *str <= '9')
					{
						sum *= 10;
						sum += NumType(*str++ - '0');
						len++;
					}
				}
				return len ? NumType(sum * minus) : default_value;
			}

			template<typename T>
			static int ToOctal(T ch)
			{
				int c = int(ch);
				if(c <= '7' && c >= '0')
					c -= '0';
				else if(c <= 'h' && c >= 'a')
					c -= 'a';
				else c = 0;
				return c;
			}

			template<typename T>
			static size_t ParseHexadecimal(const T *s)
			{
				size_t n = 0;
				if(s)
				{
					int i = 0;
					if(s[0] == '0')
					{
						if(s[1] == 'x' || s[1] == 'X')
							i = 2;
					}
					for(int v = 0; s[i]; i++)
					{
						if('a' <= s[i] && s[i] <= 'f') { v = s[i] - 97 + 10; }
						else if('A' <= s[i] && s[i] <= 'F') { v = s[i] - 65 + 10; }
						else if('0' <= s[i] && s[i] <= '9') { v = s[i] - 48; }
						else break;
						n *= 16;
						n += v;
					}
				}
				return n;
			}

			static auto Hash(uint32_t hash, wchar_t value)
			{
				return Hash::Generate(hash, value);
			}

			static auto Hash(wchar_t value)
			{
				return Hash::Generate(value, true);
			}

			static uint32_t Hash(const wchar_t *value)
			{
				return Hash::Generate(value);
			}

			static const wchar_t *TrimStartWhiteSpace(const wchar_t *value)
			{
				auto data = value;
				if(value)
				{
					while(Char::IsWhiteSpace(*data))
						data++;
				}
				return data;
			}

			/*static auto Hash(const string &value, bool trim = false)
			{
				return Hash::Generate(TrimStartWhiteSpace(value));
			}*/

			static const wchar_t *HashToString(size_t hash, string *hashString)
			{
				return hashString->format(L"0x%x", hash).c_str();
			}

			static const string HashToString(size_t hash)
			{
				return string().format(L"0x%x", hash).move();
			}

			static size_t FormatLength(const wchar_t *format, va_list argList)
			{
				//return std::vswprintf(nullptr, 0, format, argList);
				return ::_vscwprintf(format, argList);
			}

			static size_t Format(wchar_t *buffer, size_t length, const wchar_t *format, va_list argList)
			{
				//return std::vswprintf(buffer, length, format, argList);
				return ::vswprintf_s(buffer, length, format, argList);
			}

			static size_t Format(wchar_t *data, size_t length, size_t maxlength, const wchar_t *format, va_list argList)
			{
				return ::_vsnwprintf_s(data, length, maxlength, format, argList);
			}


			////////////////ENCODING//////////////////

			static std::string ToANSI(const wchar_t *wstr, int length = -1)
			{
				if(wstr)
				{
					auto l = ::WideCharToMultiByte(CP_ACP, 0, wstr, length, nullptr, 0, nullptr, nullptr);
					if(l > 0)
					{
						std::string str(l, '\0');
						::WideCharToMultiByte(CP_ACP, 0, wstr, length, &str[0], l, nullptr, nullptr);
						return str;
					}
				}
				return "";
			}

			static std::string ToUTF8(const wchar_t *wstr, int length = -1)
			{
				if(wstr)
				{
					auto len = ::WideCharToMultiByte(CP_UTF8, 0, wstr, length, nullptr, 0, nullptr, nullptr);
					if(len > 0)
					{
						std::string utf8(len, '\0');
						::WideCharToMultiByte(CP_UTF8, 0, wstr, length, &utf8[0], len, nullptr, nullptr);
						return std::move(utf8);
					}
				}
				return "";
			}

			static string ToUnicode(const std::string_view &str)
			{
				return ToUnicode(str.data(), str.length()).move();
			}

			static string ToUnicode(const char *str, size_t length)// = size_t(-1))
			{
				if(str)
				{
					auto len = ::MultiByteToWideChar(CP_ACP, 0, str, (int)length, nullptr, 0);
					if(len > 0)
					{
						string wstr(len + 1);
						len = ::MultiByteToWideChar(CP_ACP, 0, str, (int)length, wstr.data(), len);
						return wstr.release(len).move();
					}
				}
				return nullptr;
			}

			static string Extract(uint32_t id)
			{
				return Extract(CurrentModule, id).move();
			}

			static string Extract(HMODULE module, uint32_t id)
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

			static string Extract(const wchar_t *path, uint32_t id)
			{
				auto module = ::GetModuleHandleW(path);
				bool unload = false;
				if(!module)
				{
					module = ::LoadLibraryExW(path, nullptr, LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
					unload = module != nullptr;
				}

				if(module == nullptr)
					return nullptr;
				
				string str = Extract(module, id).move();

				if(unload)
					::FreeLibrary(module);

				return str.move();
			}

			static string Extract(const wchar_t *path)
			{
				string p = path;
				auto id = ParseMUILocation(p);
				if(id < 0)
					id *= -1;
				return Extract(p.c_str(), (uint32_t)id);
			}

			static int ParseMUILocation(string &path)
			{
				int result = -1;
				if(path.length() > 3)
				{
					if(path[0] == L'@')
						path.trim_start(L'@');

					intptr_t p = path.last_index_of(',', false);
					if(p != npos && p > 0)
					{
						result = (int)string::ToInt(&path.c_str()[p + 1], 0LL);
						path = path.substr(0, p).move();
					}
				}
				return result;
			}

			static string url_decode(const string &url)
			{
				string result;
				for(auto i = url.begin(), n = url.end(); i != n; ++i)
				{
					if(*i == L'%')
					{
						if(i[1] && i[2])
						{
							result += wchar_t(Char::HexToInt(i[1]) << 4 | Char::HexToInt(i[2]));
							i += 2;
						}
					}
					else if(*i == L'+')
						result += L' ';
					else
						result += *i;
				}
				return result.move();
			}
		protected:
			/*void 
stringEx::MakeUpper(std::string& str){
	for(std::string::iterator i=str.begin(); i!=str.end(); ++i){
		*i = toupper(*i);
		}
	};
    
void
stringEx::MakeLower(std::string& str){
	for(std::string::iterator i=str.begin(); i!=str.end(); ++i){
		*i = tolower(*i);
		}
	};
			*/
			static wchar_t *InternalTolower(wchar_t *str)
			{
				if(str)
				{
					for(wchar_t *cp = str; *cp; ++cp)
						*cp = Char::ToLower(*cp);
				}
				return str;
			}

			static wchar_t *InternalToUpper(wchar_t *str)
			{
				if(str)
				{
					for(wchar_t *cp = str; *cp; ++cp)
						*cp = Char::ToUpper(*cp);
				}
				return str;
			}
		};
	}
}