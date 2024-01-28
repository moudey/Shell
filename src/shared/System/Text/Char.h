#pragma once

#include <cwctype>

namespace Nilesoft
{
	namespace Text
	{
		/*
		const int x = 32;

		// Converts a string to uppercase
		char *toUpperCase(char *a)
		{
			for(int i = 0; a[i] != '\0'; i++)
				a[i] = a[i] & ~x;

			return a;
		}

		// Converts a string to lowercase
		char *toLowerCase(char *a)
		{
			for(int i = 0; a[i] != '\0'; i++)
				a[i] = a[i] | x;

			return a;
		}
		*/
		class Char
		{
		private:
			unsigned short ch = 0;

		public:
			const static unsigned short Null = 0;

		public:
			Char() : ch(0) {}

			Char(const Char& c) : ch(c.ch) {}

			template<typename T>
			Char(T value) 
			{
				if constexpr(std::is_null_pointer_v<T>)
					ch = 0;
				else if constexpr(std::is_integral_v<T> ||
								  std::is_floating_point_v<T>)
				{
					ch = (unsigned short)(value);
				}
			}

			~Char() {}
			//
			// operator
			//
			operator Char() const { return *this; }
			
			template<typename T>
			operator T() const {
				if constexpr(std::is_null_pointer_v<T>)
					return nullptr;
				else if constexpr(std::is_integral_v<T> ||
								  std::is_floating_point_v<T>)
				{
					return (T)(ch);
				}
			}

			//
			// operator Logical 
			//
			template<typename T>
			bool operator==(const T& c) const { return ch == (unsigned short)(c); }
			template<typename T>
			bool operator!=(const T& c) const { return ch != (unsigned short)(c); }
			template<typename T>
			bool operator>=(const T& c) const { return ch >= (unsigned short)(c); }
			template<typename T>
			bool operator<=(const T& c) const { return ch <= (unsigned short)(c); }
			template<typename T>
			bool operator<(const T& c) const { return ch < (unsigned short)(c); }
			template<typename T>
			bool operator>(const T& c) const { return ch > (unsigned short)(c); }
			//
			// Bitwise
			//
			template<typename T>
			Char& operator|(const T& c) { ch |= (unsigned short)(c); return *this; }
			template<typename T>
			Char& operator|=(const T& c) { ch |= (unsigned short)(c); return *this; }
			template<typename T>
			Char& operator&(const T& c) { ch = (unsigned short)(ch & short(c)); return *this; }
			template<typename T>
			Char& operator&=(const T& c) { ch &= (unsigned short)(c); return *this; }
			template<typename T>
			Char& operator^(const T& c) { ch ^= (unsigned short)(c); return *this; }
			template<typename T>
			Char& operator^=(const T& c) { ch ^= (unsigned short)(c); return *this; }
			//
			// Additive
			//
			//template<typename T>
			//Char& operator+(const T& c) { ch += short(c); return *this; }
			//template<typename T>
			//Char& operator-(const T& c) { ch -= short(c); return *this; }
			
			template<typename T>
			Char& operator=(const T& c) { ch = (unsigned short)(c); return(*this); }
			
			const short get() const { return ch; }
			short get() { return ch; }

			template<typename T = int>
			T hex_toint() { return HexToInt(ch); }

			auto hash(uint32_t hash) { return Hash(hash, ch); }
			
			Char& tolower()
			{
				ch = ToLower(ch);
				return *this;
			}
			
			Char& toupper()
			{
				ch = ToUpper(ch);
				return *this;
			}

			bool equals(char c, bool ignorCase = true)
			{
				return Char::Equals(ch, unsigned short(c), ignorCase);
			}

			bool equals(wchar_t c, bool ignorCase = true)
			{
				return Char::Equals(ch, unsigned short(c), ignorCase);
			}

			bool equals(const Char& c, bool ignorCase = true)
			{
				return Char::Equals(ch, c.ch, ignorCase);
			}

			template<typename T>
			bool any_of(std::initializer_list<T> chs, bool ignorCase = false)
			{
				for(auto c : chs)
				{
					if(Char::Equals(ch, (unsigned short)c, ignorCase))
						return true;
				}
				return false;
			}

			template<typename T>
			bool any_of(const T *chs, bool ignorCase = false)
			{
				if(chs)
				{
					while(*chs)
					{
						if(Char::Equals(ch, (unsigned short)(*chs++), ignorCase))
							return true;
					}
				}
				return false;
			}

			bool eof() const { return ch == Null; }
			bool isnull() const { return ch == Null; }
			bool islower()const { return std::iswlower(ch); }
			bool isupper()const { return std::iswupper(ch); }
			bool isdigit()const { return std::iswdigit(ch); }
			bool isxdigit()const { return std::isxdigit(ch); }
			bool isalpha() const { return std::iswalpha(ch); }
			bool isalnum() const { return std::iswalnum(ch); }
			bool isspace() const { return std::iswspace(ch); }
			bool isblank() const { return std::iswblank(ch); }
			bool isident() const { return IsIdent(ch); }
			bool isidentdigit() const { return IsIdentOrDigit(ch); }
			//bool isquote()const { return ch == 0x22 || ch == 0x27 || ch == 0x60; }// '"' | ''' | '`'
			bool isquote() const { return ch == 0x22 || ch == 0x27; }// '"' | '''
			// single quote 
			bool issquote()const { return ch == 0x27; }	// '
			// double quote
			bool isdquote()const { return ch == 0x22; }	// "

			bool isnewline() const { return ch == 0x0a; }		// \n	new line
			bool isopen_brackets() const { return ch == '(' || ch == '{'; }
			bool isclose_brackets() const { return ch == ')' || ch == '}'; }
			//
			//
			bool isopen_paren()const { return ch == 0x28; }	// (
			bool isclose_paren() const { return ch == 0x29; }	// )
			bool isopen_curly() const { return ch == 0x7b; }	// {
			bool isclose_curly()const { return ch == 0x7d; }	// }
			bool isopen_square() const { return ch == 0x5b; }	// [
			bool isclose_square()const { return ch == 0x5d; } // ]

			bool IsSingleQuote()const { return ch == 0x27; } // '
			bool IsDoubleQuote() const { return ch == 0x22; }	// "
			bool IsComma()const { return ch == 0x2c; }		// ,
			bool IsDot()const { return ch == 0x2e; }			// .
			bool IsBackSlash() const { return ch == 0x5c; }	// '\'
			bool IsPercent()const { return ch == 0x25; }		// %
			bool IsDollar() const { return ch == 0x24; }		// $
			bool IsQues()const { return ch == 0x3f; }		// ?
			bool IsAt()const { return ch == 0x40; }			// @
			bool IsHash() const { return ch == 0x23; }		// #
			bool IsDash() const { return ch == 0x2d; }		// -
			bool IsUnderline() const { return ch == 0x5f; }	// _
			bool IsColon() const { return ch == 0x3a; }		// :
			bool IsSemicolon()const { return ch == 0x3b; }	// ;
			bool IsCR() const { return ch == 0x0d; }			// \r	carriage return
			bool IsTab() const { return ch == 0x09; }			// \t	horizontal tab
			// logic
			bool IsEquals()const { return ch == 0x3d; }		// =
			bool IsNot() const { return ch == 0x21; }			// !
			bool IsTilde() const { return ch == 0x7e; }		// ~
			bool IsAnd() const { return ch == 0x26; }			// &
			bool IsOr() const { return ch == 0x7c; }			// |
			bool IsGreater() const { return ch == 0x3e; }		// >
			bool IsLess()const { return ch == 0x3c; }		// <
			// arithmetic
			bool IsPlus() const { return ch == 0x2b; }		// +
			bool IsMinus() const { return ch == 0x2d; }		// -
			bool IsStar() const { return ch == 0x2a; }		// *
			bool IsSlash()const { return ch == 0x2f; }		// /
			bool IsMod() const { return ch == 0x25; }			// %
			bool IsHat() const { return ch == 0x5e; }			// ^

			//
			// static functions
			//
			template<typename T>
			static bool Equals(T const& first, T const & second, bool ignoreCase = true)
			{
				if(first == second)
					return true;
				if(ignoreCase)
				{
					int c1 = int(first), c2 = int(second);
					if(c1 >= 0x41 && c1 <= 0x5a)
						c1 = c1 - 0x41 + 0x61;
					if(c2 >= 0x41 && c2 <= 0x5a)
						c2 = c2 - 0x41 + 0x61;
					return c1 == c2;
				}
				return false;
			}

			template<typename T>
			static bool IsWhiteSpace(const T& ch)
			{
				switch(int(ch))
				{
					case 0x08:	// 0xa0 BREAK
					case 0x09:	// 0x09 HORIZONTAL TAB
					case 0x0a:	// 0x0a LINE FEED
					case 0x0b:	// 0x0b VERTICAL TAB
					case 0x0c:	// 0x0c FORM FEED
					case 0x0d:	// 0x0d CARRIAGE RETURN
					case 0x20:	// SPACE
					case 0x85:	// 0x85 NEXT LINE
					case 0xa0:	// 0xa0 NO-BREAK SPACE
						return true;
				}
				return false;
			}

			template<typename T>
			static bool IsBlank(const T& ch)
			{
				return std::iswblank(ch);//(ch == 0x20) || (ch == 0x09)
			}

			template<typename T>
			static bool IsLatin(const T& ch)
			{
				return ch <= 0xff;
			}

			template<typename T>
			static bool IsAscii(const T& ch)
			{
				return ch <= 0x7f;
			}

			template<typename T>
			static bool IsLetter(const T& ch)
			{
				return std::iswalpha(ch);
				//return (ch >= 'a' &&  ch <= 'z') || (ch >= 'A' && ch <= 'Z');
			}

			template<typename T>
			static bool IsDigit(const T& ch)
			{
				return std::iswdigit(ch);
			}

			template<typename T>
			static bool IsNumber(const T& ch)
			{
				return std::iswdigit(ch);
			}

			template<typename T>
			static bool IsHexDigit(const T& ch)
			{
				return std::iswxdigit(ch);
			}

			template<typename T>
			static bool IsLetterOrDigit(const T& ch)
			{
				return std::iswalnum(ch);
			}

			template<typename T>
			static bool IsUpper(const T& ch)
			{
				return std::iswupper(ch);
			}

			template<typename T>
			static bool IsLower(const T& ch)
			{
				return std::iswlower(ch);
			}

			template<typename T>
			static bool IsSymbol(const T& ch)
			{
				return !(std::iswalnum(ch));
			}

			template<typename T>
			static bool IsIdent(const T& ch)
			{
				return (std::iswalpha(ch) || (ch == 0x5f));
			}

			template<typename T>
			static bool IsIdentOrDigit(const T& ch)
			{
				return std::iswalnum(ch) || (ch == 0x5f);
			}

			template<typename T>
			static T ToLower(const T& ch)
			{
				if(std::iswupper(ch))
					return (T)std::towlower(ch); //T(((unsigned short)ch) - 0x41 + 0x61);
				return ch;
			}

			template<typename T>
			static T ToUpper(const T& ch)
			{
				if(std::iswlower(ch))
					return (T)std::towupper(ch);// T(((unsigned short)ch) - 0x61 + 0x41);
				return ch;
			}

			template<typename charType>
			static charType ToAscii(const charType & ch)
			{
				//return std::iswascii(0)
				return charType(int(ch) & 0x7F);
			}

			template< typename T = int, typename charType = wchar_t>
			static T ToInt(const charType & ch)
			{
				return (T)(int(ch) - 0x30);
			}

			template<typename T = int, typename charType = wchar_t>
			static T HexToInt(const charType & ch)
			{
				if(std::iswdigit(ch))
					return ToInt(ch);
				else if(std::iswxdigit(ch))
					return (T)((int(ch) & 0xdf) - 0x41 + 10);
				return (T)0;
			}

			template<typename charType>
			static uint32_t Hash(uint32_t hash, const charType & ch) { return Hash::Generate(hash, ch); }
			
			template<typename charType>
			static uint32_t Hash(const charType & ch) { return Hash::Generate(ch, true); }
		};
	}
}