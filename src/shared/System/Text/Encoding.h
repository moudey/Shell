#pragma once

namespace Nilesoft
{
	namespace Text
	{
		enum class EncodingType
		{
			Unknown = -1,
			ANSI,
			UTF8,
			UTF8BOM,
			UTF16LE,
			UTF16LEBOM,
			UTF16BE,
			UTF16BEBOM,
			UTF32LE,
			UTF32BE,
			UTF7,
			UTF1
		};

/*
BOM Byte order mark

EF BB BF xx - UTF-8
FF FE xx 00 - UTF-16LE
FE FF 00 xx - UTF-16BE
FF FE 00 00 - UTF-32LE
00 00 FE FF - UTF-32BE

UTF-7
2B 2F 76 38
2B 2F 76 39
2B 2F 76 2B
2B 2F 76 2F
2B 2F 76 38 2D

Note that:
	UTF-32BE doesn't start with three NULs so it won't be recognized
	UTF-32LE the first byte is not followed by 3 NULs so it won't be recognized
	UTF-16BE has only 1 NUL in the first 4 bytes so it won't be recognized
	UTF-16LE has only 1 NUL in the first 4 bytes so it won't be recognized
*/
		class Encoding
		{
// If "characterEncoding=ascii" then we assume that all characters have the same length of 1 byte.
// If "characterEncoding=UTF8" then the characters have different lengths (from 1 byte to 4 bytes).
// This table is used as lookup-table to know the length of a character (in byte) based on the
// content of the first byte of the character.
// (note: if you modify this, you must always have XML_utf8ByteTable[0]=0 ).
			static constexpr const char utf8ByteTable[256] =
			{
				//  0 1 2 3 4 5 6 7 8 9 a b c d e f
				0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x00
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x10
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x20
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x30
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x40
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x50
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x60
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x70 End of ASCII range
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x80 0x80 to 0xc1 invalid
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x90
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xa0
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xb0
				1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xc0 0xc2 to 0xdf 2 byte
				2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xd0
				3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xe0 0xe0 to 0xef 3 byte
				4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1 // 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
			};

			static constexpr const char legacyByteTable[256] =
			{
				0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			};

		public:
			static EncodingType GetType(byte* b, size_t l)
			{
				if(!b || l <= 0)
					return EncodingType::Unknown;

				if(l >= 2)
				{
					if((b[0] == 0xff) && (b[1] == 0xfe))
					{
						return EncodingType::UTF16LEBOM;
					}
					else if((b[0] == 0xfe) && (b[1] == 0xff))
					{
						return EncodingType::UTF16BEBOM;
					}
				}

				if(l >= 3)
				{
					if((b[0] == 0xef) && (b[1] == 0xbb) && (b[2] == 0xbf))
					{
						return EncodingType::UTF8BOM;
					}
					else if((b[0] == 0xf7) && (b[1] == 0x64) && (b[2] == 0x4c))
					{
						return EncodingType::UTF1;
					}
				}

				if(l >= 4)
				{
					if((b[0] == 0xff) && (b[1] == 0xfe))
					{
						if(!b[2] && !b[3])
						{
							return EncodingType::UTF32LE;
						}
					}
					else if(b[0] && !b[1] && b[2] && !b[3])
					{
						return EncodingType::UTF16LE;
					}
					else if(!b[0])
					{
						if(b[1] && !b[2] && b[3])
						{
							return EncodingType::UTF16BE;
						}
						else if(!b[1] && (b[2] == 0xfe) && (b[3] == 0xff))
						{
							return EncodingType::UTF32BE;
						}
					}

					if((b[0] == 0x2B) && (b[1] == 0x2F) && (b[2] == 0x76))
					{
						if(l >= 5)
						{
							if((b[3] == 0x38) && (b[4] == 0x2d))
								return EncodingType::UTF7;
						}

						if(b[3] == 0x38)
							return EncodingType::UTF7;
						else if(b[3] == 0x39)
							return EncodingType::UTF7;
						else if(b[3] == 0x2b)
							return EncodingType::UTF7;
						else if(b[3] == 0x2f)
							return EncodingType::UTF7;
					}
				}

				auto et = EncodingType::UTF8;
				size_t i = 0;
				while(i < l)
				{
					switch((int)utf8ByteTable[b[i]])
					{
						case 4:
						{
							i++;
							if((i < l) && (b[i] & 0xC0) != 0x80)
							{
								et = EncodingType::ANSI;
								i = l;
							}
							// 10bbbbbb ?
							break;
						}
						case 3:
						{
							i++;
							if((i < l) && (b[i] & 0xC0) != 0x80)
							{
								et = EncodingType::ANSI;
								i = l;
							}
							// 10bbbbbb ?
							break;
						}
						case 2:
						{
							i++;
							if((i < l) && (b[i] & 0xC0) != 0x80)
							{
								et = EncodingType::ANSI;
								i = l;
							}// 10bbbbbb ?
							break;
						}
						case 1:
						{
							i++;
							break;
						}
						case 0:
						{
							i = l;
							break;
						}
					}
				}
				return et;
			}

			/*
			//Function to convert a Unicode string from platform-specific "wide characters" (wchar_t) to UTF-16.
			static uint32_t ConvertUTF32ToUTF16(wchar_t* source, const uint32_t sourceLength, wchar_t*& destination)
			{
				uint32_t destinationLength = 0;
				wchar_t wcharCharacter;
				uint32_t uniui32Counter = 0;
				wchar_t* pwszDestinationStart = destination;
				wchar_t* sourceStart = source;

				if(0 != destination)
				{
					while(uniui32Counter < sourceLength)
					{
						wcharCharacter = *source++;
						if(wcharCharacter <= 0x0000FFFF)
						{
						  // UTF-16 surrogate values are illegal in UTF-32
							// 0xFFFF or 0xFFFE are both reserved values
							if(wcharCharacter >= 0xD800 &&
							   wcharCharacter <= 0xDFFF)
							{
								*destination++ = 0x0000FFFD;
								destinationLength += 1;
							}
							else
							{
							  // source is a BMP Character
								destinationLength += 1;
								*destination++ = wcharCharacter;
							}
						}
						else if(wcharCharacter > 0x0010FFFF)
						{
						  // U+10FFFF is the largest code point of Unicode Character Set
							*destination++ = 0x0000FFFD;
							destinationLength += 1;
						}
						else
						{
						  // source is a character in range 0xFFFF - 0x10FFFF
							wcharCharacter -= 0x0010000UL;
							*destination++ = (wchar_t)((wcharCharacter >> 10) + 0xD800);
							*destination++ = (wchar_t)((wcharCharacter & 0x3FFUL) + 0xDC00);
							destinationLength += 2;
						}

						++uniui32Counter;
					}

					destination = pwszDestinationStart;
					destination[destinationLength] = '\0';
				}

				source = sourceStart;
				return destinationLength;
			} //function ends
		
			// Convert an ANSI string to a wide Unicode String
			static uint32_t ToUnicode(const char *mstr, uint32_t mstr_length, wchar_t **wstr, uint32_t codePage)
			{
				uint32_t length = 0;
				if(mstr)
				{
					DWORD dwFlags = (codePage == CP_UTF8) ? 0 : MB_PRECOMPOSED;
					length = ::MultiByteToWideChar(codePage, dwFlags, mstr, mstr_length, nullptr, 0);
					if(length > 0 && wstr)
					{
						*wstr = new wchar_t[length + 1] { };
						::MultiByteToWideChar(codePage, dwFlags, mstr, mstr_length, *wstr, length);
					}
				}
				return length;
			}

			static uint32_t ToUnicode(const char *mstr, wchar_t **wstr, uint32_t codePage)
			{
				return ToUnicode(mstr, uint32_t(-1), wstr, codePage);
			}

			// Convert an ANSI string to a wide Unicode String
			static wchar_t *ToUnicode(const char *str, uint32_t codePage = CP_ACP)
			{
				wchar_t *ret = nullptr;
				ToUnicode(str, uint32_t(-1) , &ret, codePage);
				return ret;
			}

			// Convert a wide Unicode string to an UTF8 string
			static uint32_t ToUTF8(const wchar_t*wstr, uint32_t wstr_length, char **utf8)
			{
				uint32_t length = 0;
				if(wstr)
				{
					length = ::WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_length, nullptr, 0, nullptr, nullptr);
					if(length > 0 && utf8)
					{
						*utf8 = new char[length + 1] { };
						::WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_length, *utf8, length, nullptr, nullptr);
					}
				}
				return length;
			}

			static uint32_t ToUTF8(const wchar_t*wstr, char **utf8)
			{
				return ToUTF8(wstr, uint32_t(-1), utf8);
			}

			// Convert an ANSI string to an UTF8 string
			static uint32_t ToUTF8(const char *mstr, uint32_t mstr_length, char **utf8)
			{
				wchar_t *wstr = nullptr;
				auto length = ToUnicode(mstr, mstr_length, &wstr, CP_ACP);
				if(length > 0)
				{
					length = ToUTF8(wstr, length, utf8);
					delete[] wstr;
					return length;
				}
				return 0;
			}

			// Convert ANSI string to an  UTF8 string
			static uint32_t ToUTF8(const char *mstr, char **utf8)
			{
				return ToUTF8(mstr, uint32_t(-1), utf8);
			}

			// Convert an UTF8 string to a wide Unicode String
			static wchar_t *UTF8ToUnicode(const char *utf8)
			{
				return ToUnicode(utf8, CP_UTF8);
			}

			// Convert an wide Unicode string to ANSI string
			static uint32_t ToANSI(const wchar_t*wstr, uint32_t wstr_length, char **mstr)
			{
				uint32_t length = 0;
				if(wstr)
				{
					length = ::WideCharToMultiByte(CP_ACP, 0, wstr, wstr_length, nullptr, 0, nullptr, nullptr);
					if(length > 0 && mstr)
					{
						*mstr = new char[length + 1] { };
						::WideCharToMultiByte(CP_ACP, 0, wstr, wstr_length, *mstr, length, nullptr, nullptr);
					}
				}
				return length;
			}

			// Convert an wide Unicode string to ANSI string
			static char* ToANSI(const wchar_t* wstr)
			{
				char *ret = nullptr;
				ToANSI(wstr, uint32_t(-1), &ret);
				return ret;
			}
			
			// Convert an UTF8 string to a ANSI String
			static uint32_t ToANSI(const char *utf8, uint32_t utf8_length, char **mstr)
			{
				if(utf8)
				{
					wchar_t *wstr = nullptr;
					uint32_t length = ToUnicode(utf8, utf8_length, &wstr, CP_UTF8);
					if(length > 0)
					{
						length = ToANSI(wstr, length, mstr);
						delete[] wstr;
						return length;
					}
				}
				return 0;
			}*/
		};


		class Unicode
		{
		public:
			// Convert a wide Unicode string to an UTF8 string
			static std::string utf8_encode(const std::wstring &wstr)
			{
				if(wstr.empty()) return std::string();
				int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
				std::string strTo(size_needed, 0);
				WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
				return strTo;
			}

			// Convert an UTF8 string to a wide Unicode String
			static std::wstring utf8_decode(const std::string &str)
			{
				if(str.empty()) return std::wstring();
				int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
				std::wstring wstrTo(size_needed, 0);
				MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
				return wstrTo;
			}

			// Convert an ANSI string to a wide Unicode String
			static size_t From(const char *ansi, size_t ansi_length, wchar_t **unicode, uint32_t codePage)
			{
				int length = 0;
				if(ansi)
				{
					DWORD dwFlags = (codePage == CP_UTF8) ? MB_ERR_INVALID_CHARS : 0;
					length = ::MultiByteToWideChar(codePage, dwFlags, ansi, static_cast<int>(ansi_length), nullptr, 0);
					if(length > 0 && unicode)
					{
						*unicode = new wchar_t[static_cast<size_t>(length) + 1] { };
						::MultiByteToWideChar(codePage, dwFlags, ansi, static_cast<int>(ansi_length), *unicode, length);
					}
				}
				return static_cast<size_t>(length);
			}

			static std::wstring From(const char *str, size_t str_length, uint32_t codePage)
			{
				if(str)
				{
					DWORD dwFlags = (codePage == CP_UTF8) ? MB_ERR_INVALID_CHARS : 0;
					auto length = ::MultiByteToWideChar(codePage, dwFlags, str, static_cast<int>(str_length), nullptr, 0);
					if(length > 0)
					{
						std::wstring w(length, L'\0');
						length = ::MultiByteToWideChar(codePage, dwFlags, str, static_cast<int>(str_length), &w[0], length);
						w.resize(length);
						return std::move(w);
					}
				}
				return L"";
			}

			static std::wstring From(const std::string_view& str, uint32_t codePage = CP_ACP)
			{
				return std::move(From(str.data(), str.length(), codePage));
			}

			static std::wstring FromAnsi(const std::string_view &str)
			{
				return std::move(From(str.data(), str.length(), CP_ACP));
			}

			static size_t From(const char *mstr, wchar_t **wstr, uint32_t codePage)
			{
				return From(mstr, size_t(-1), wstr, codePage);
			}

			// Convert an ANSI string to a wide Unicode String
			static wchar_t *From(const char *str, uint32_t codePage = CP_ACP)
			{
				wchar_t *ret = nullptr;
				From(str, uint32_t(-1), &ret, codePage);
				return ret;
			}

			// Convert an UTF8 string to a wide Unicode String
			static size_t FromANSI(const char *mstr, size_t length, wchar_t **wstr)
			{
				return From(mstr, length, wstr, CP_ACP);
			}

			// Convert an UTF8 string to a wide Unicode String
			static std::wstring FromANSI(const char *mstr, size_t length)
			{
				wchar_t *wstr = nullptr;
				From(mstr, length, &wstr, CP_ACP);
				return wstr;
			}

			static wchar_t *FromANSI(const char *mstr)
			{
				return From(mstr, CP_ACP);
			}

			// Convert an UTF8 string to a wide Unicode String
			static size_t FromUTF8(const char *utf8, size_t length, wchar_t **wstr)
			{
				return From(utf8, length, wstr, CP_UTF8);
			}

			// Convert an UTF8 string to a wide Unicode String
			static std::wstring FromUTF8(const char *utf8, size_t length)
			{
				wchar_t *wstr = nullptr;
				From(utf8, length, &wstr, CP_UTF8);
				return wstr;
			}

			// Convert an UTF8 string to a wide Unicode String
			static wchar_t *FromUTF8(const char *utf8)
			{
				return From(utf8, CP_UTF8);
			}

			static size_t ToANSI(const wchar_t*wstr, char **mstr)
			{
				return ToANSI(wstr, size_t (-1), mstr);
			}

			static size_t ToANSI(const wchar_t*wstr, size_t wstr_length, char **mstr)
			{
				size_t length = 0;
				if(wstr)
				{
					length = (size_t)::WideCharToMultiByte(CP_ACP, 0, wstr, int(wstr_length), nullptr, 0, nullptr, nullptr);
					if(length > 0 && mstr)
					{
						*mstr = new char[static_cast<unsigned __int64>(length) + 1] { };
						::WideCharToMultiByte(CP_ACP, 0, wstr, int(wstr_length), *mstr, (int)length, nullptr, nullptr);
					}
				}
				return length;
			}

			static std::string ToANSI(const wchar_t*wstr, int wstr_length = -1)
			{
				std::string str;
				if(wstr)
				{
					auto length = ::WideCharToMultiByte(CP_ACP, 0, wstr, wstr_length, nullptr, 0, nullptr, nullptr);
					if(length > 0)
					{
						str.reserve(length);
						length = ::WideCharToMultiByte(CP_ACP, 0, wstr, wstr_length, &str[0], length, nullptr, nullptr);
						str.resize(length);
					}
				}
				return str;
			}

			static size_t ToUTF8(const wchar_t*wstr, size_t wstr_length, char **utf8)
			{
				int length = 0;
				if(wstr)
				{
					length = (size_t)::WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, wstr, (int)wstr_length, nullptr, 0, nullptr, nullptr);
					if(length > 0 && utf8)
					{
						*utf8 = new char[static_cast<unsigned __int64>(length) + 1] { };
						::WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, wstr, (int)wstr_length, *utf8, length, nullptr, nullptr);
					}
				}
				return static_cast<size_t>(length);
			}
		};

		class UTF8
		{
		public:

			static void utf8toWStr(std::wstring &dest, const std::string &src)
			{
				dest.clear();
				wchar_t w = 0;
				int bytes = 0;
				wchar_t err = L' ';// L'�';
				for(size_t i = 0; i < src.size(); i++)
				{
					unsigned char c = (unsigned char)src[i];
					if(c <= 0x7f)
					{//first byte
						if(bytes)
						{
							dest.push_back(err);
							bytes = 0;
						}
						dest.push_back((wchar_t)c);
					}
					else if(c <= 0xbf)
					{//second/third/etc byte
						if(bytes)
						{
							w = ((w << 6) | (c & 0x3f));
							bytes--;
							if(bytes == 0)
								dest.push_back(w);
						}
						else
							dest.push_back(err);
					}
					else if(c <= 0xdf)
					{//2byte sequence start
						bytes = 1;
						w = c & 0x1f;
					}
					else if(c <= 0xef)
					{//3byte sequence start
						bytes = 2;
						w = c & 0x0f;
					}
					else if(c <= 0xf7)
					{//3byte sequence start
						bytes = 3;
						w = c & 0x07;
					}
					else
					{
						dest.push_back(err);
						bytes = 0;
					}
				}
				if(bytes)
					dest.push_back(err);
			}

			static void Utf16ToUtf8(std::string &dest, const wchar_t *src, const size_t length)
			{
				dest.clear();
				for(size_t i = 0; i < length; i++)
				{
					short w = src[i];
					if(w <= 0x7f)
						dest.push_back((char)w);
					else if(w <= 0x7ff)
					{
						dest.push_back(0xc0 | ((w >> 6) & 0x1f));
						dest.push_back(0x80 | (w & 0x3f));
					}
					else if(w <= 0xffff)
					{
						dest.push_back(0xe0 | ((w >> 12) & 0x0f));
						dest.push_back(0x80 | ((w >> 6) & 0x3f));
						dest.push_back(0x80 | (w & 0x3f));
					}
					else if(w <= 0x10ffff)
					{
						dest.push_back(0xf0 | ((w >> 18) & 0x07));
						dest.push_back(0x80 | ((w >> 12) & 0x3f));
						dest.push_back(0x80 | ((w >> 6) & 0x3f));
						dest.push_back(0x80 | (w & 0x3f));
					}
					else
						dest.push_back('?');
				}
			}

			static void Utf16ToUtf8(std::string &dest, const std::wstring &src)
			{
				Utf16ToUtf8(dest, src.c_str(), src.length());
			}

			static std::string Utf16ToUtf8(const std::wstring &src)
			{
				std::string dest;
				Utf16ToUtf8(dest, src.c_str(), src.length());
				return std::move(dest);
			}

			static std::string Utf16ToUtf8(const wchar_t * const src, const size_t length)
			{
				std::string dest;
				Utf16ToUtf8(dest, src, length);
				return std::move(dest);
			}

			// Convert a wide Unicode string to an UTF8 string
			static size_t FromUnicode(const wchar_t *wstr, const size_t wstr_length, char **utf8)
			{
				const int wlength = static_cast<int>(wstr_length);
				int length = 0;
				if(wstr)
				{
					length = ::WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, wstr, wlength, nullptr, 0, nullptr, nullptr);

					if(length > 0 && utf8)
					{
						*utf8 = new char[static_cast<size_t>(length) + 1]{ };
						::WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, wstr, wlength, *utf8, length, nullptr, nullptr);
					}
				}
				return static_cast<size_t>(length);
			}

			static size_t FromUnicode(const wchar_t*wstr, char **utf8)
			{
				return FromUnicode(wstr, size_t(-1), utf8);
			}

			// Convert an ANSI string to an UTF8 string
			static size_t FromANSI(const char *mstr, size_t mstr_length, char **utf8)
			{
				wchar_t *wstr = nullptr;
				auto length = Unicode::From(mstr, mstr_length, &wstr, CP_ACP);
				if(length > 0)
				{
					length = FromUnicode(wstr, length, utf8);
					delete[] wstr;
					return length;
				}
				return 0;
			}

			// Convert ANSI string to an  UTF8 string
			static size_t FromANSI(const char *mstr, char **utf8)
			{
				return FromANSI(mstr, size_t(-1), utf8);
			}

			// Convert an UTF8 string to a wide Unicode String
			static size_t ToUnicode(const char *utf8, size_t length, wchar_t** wstr)
			{
				return Unicode::From(utf8, length, wstr, CP_UTF8);
			}

			// Convert an UTF8 string to a wide Unicode String
			static wchar_t *ToUnicode(const char *utf8)
			{
				return Unicode::From(utf8, CP_UTF8);
			}

			static std::string From(const std::string& str)
			{
				std::string text;
				if(!is_utf8((byte*)str.c_str()))
				{
					int size = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), nullptr, 0);
					std::wstring utf16_str(size, '\0');

					::MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), &utf16_str[0], size);

					int utf8_size = ::WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
						(int)utf16_str.length(), nullptr, 0, nullptr, nullptr);

					std::string utf8_str(utf8_size, '\0');
					::WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
						(int)utf16_str.length(), &utf8_str[0], utf8_size, 0, 0);
					text = utf8_str;
				}
				return text;
			}

			static std::string From(std::wstring const& str)
			{
				int size = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(),
					(int)str.length(), nullptr, 0, nullptr, nullptr);
				std::string text(size, '\0');
				::WideCharToMultiByte(CP_UTF8, 0, str.c_str(),
					(int)str.length(), &text[0], size, nullptr, nullptr);
				return text;
			}

			static bool is_utf8(const unsigned char *buf)
			{
				if(!buf) return true;

				int num = 0;

				while(*buf != 0x00)
				{
					if((*buf & 0x80) == 0x00)
					{
						// U+0000 to U+007F 
						num = 1;
					}
					else if((*buf & 0xE0) == 0xC0)
					{
						// U+0080 to U+07FF 
						num = 2;
					}
					else if((*buf & 0xF0) == 0xE0)
					{
						// U+0800 to U+FFFF 
						num = 3;
					}
					else if((*buf & 0xF8) == 0xF0)
					{
						// U+10000 to U+10FFFF 
						num = 4;
					}
					else
						return false;

					buf += 1;
					for(int i = 1; i < num; ++i)
					{
						if((*buf & 0xC0) != 0x80)
							return false;
						buf += 1;
					}
				}

				return true;
			}

			static bool IsValid(const unsigned char *buf)
			{
				if(!buf) return true;

				unsigned int cp;
				int num;

				while(*buf != 0x00)
				{
					if((*buf & 0x80) == 0x00)
					{
						// U+0000 to U+007F 
						cp = (*buf & 0x7F);
						num = 1;
					}
					else if((*buf & 0xE0) == 0xC0)
					{
						// U+0080 to U+07FF 
						cp = (*buf & 0x1F);
						num = 2;
					}
					else if((*buf & 0xF0) == 0xE0)
					{
						// U+0800 to U+FFFF 
						cp = (*buf & 0x0F);
						num = 3;
					}
					else if((*buf & 0xF8) == 0xF0)
					{
						// U+10000 to U+10FFFF 
						cp = (*buf & 0x07);
						num = 4;
					}
					else
						return false;

					buf += 1;
					for(int i = 1; i < num; ++i)
					{
						if((*buf & 0xC0) != 0x80)
							return false;
						cp = (cp << 6) | (*buf & 0x3F);
						buf += 1;
					}

					if((cp > 0x10FFFF) || ((cp >= 0xD800) && (cp <= 0xDFFF)) ||
						((cp <= 0x007F) && (num != 1)) ||
					   ((cp >= 0x0080) && (cp <= 0x07FF) && (num != 2)) ||
					   ((cp >= 0x0800) && (cp <= 0xFFFF) && (num != 3)) ||
					   ((cp >= 0x10000) && (cp <= 0x1FFFFF) && (num != 4)))
						return false;
				}
				return true;
			}

			static bool utf8_check_is_valid(const unsigned char* buf, int length)
			{
				int c, i, ix, n, j;
				for(i = 0, ix = length; i < ix; i++)
				{
					c = buf[i];
					//if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
					if(0x00 <= c && c <= 0x7f) n = 0; // 0bbbbbbb
					else if((c & 0xE0) == 0xC0) n = 1; // 110bbbbb
					else if(c == 0xed && i < (ix - 1) && (buf[i + 1] & 0xa0) == 0xa0) return false; //U+d800 to U+dfff
					else if((c & 0xF0) == 0xE0) n = 2; // 1110bbbb
					else if((c & 0xF8) == 0xF0) n = 3; // 11110bbb
					//else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
					//else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
					else return false;
					for(j = 0; j < n && i < ix; j++)
					{ // n bytes matching 10bbbbbb follow ?
						if((++i == ix) || (((unsigned char)buf[i] & 0xC0) != 0x80))
							return false;
					}
				}
				return true;
			}

			static bool is__utf8(const char *string)
			{
				if(!string)
					return false;

				const unsigned char * bytes = (const unsigned char *)string;
				while(*bytes)
				{
					if((// ASCII
					  // use bytes[0] <= 0x7F to allow ASCII control characters
					   bytes[0] == 0x09 ||
					   bytes[0] == 0x0A ||
					   bytes[0] == 0x0D ||
					   (0x20 <= bytes[0] && bytes[0] <= 0x7E)
					   )
					   )
					{
						bytes += 1;
						continue;
					}

					if((// non-overlong 2-byte
						(0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
					   (0x80 <= bytes[1] && bytes[1] <= 0xBF)
					   )
					   )
					{
						bytes += 2;
						continue;
					}

					if((// excluding overlongs
					   bytes[0] == 0xE0 &&
					   (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
					   (0x80 <= bytes[2] && bytes[2] <= 0xBF)
					   ) ||
					   (// straight 3-byte
					   ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
					   bytes[0] == 0xEE ||
					   bytes[0] == 0xEF) &&
					   (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
					   (0x80 <= bytes[2] && bytes[2] <= 0xBF)
					   ) ||
					   (// excluding surrogates
					   bytes[0] == 0xED &&
					   (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
					   (0x80 <= bytes[2] && bytes[2] <= 0xBF)
					   )
					   )
					{
						bytes += 3;
						continue;
					}

					if((// planes 1-3
					   bytes[0] == 0xF0 &&
					   (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
					   (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
					   (0x80 <= bytes[3] && bytes[3] <= 0xBF)
					   ) ||
					   (// planes 4-15
					   (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
					   (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
					   (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
					   (0x80 <= bytes[3] && bytes[3] <= 0xBF)
					   ) ||
					   (// plane 16
					   bytes[0] == 0xF4 &&
					   (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
					   (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
					   (0x80 <= bytes[3] && bytes[3] <= 0xBF)
					   )
					   )
					{
						bytes += 4;
						continue;
					}
					return false;
				}
				return true;
			}
		};

		class ANSI
		{
		public:

			// Convert an wide Unicode string to ANSI string
			static size_t FromUnicode(const wchar_t*wstr, size_t wstr_length, char **mstr)
			{
				uint32_t length = 0;
				if(wstr)
				{
					length = ::WideCharToMultiByte(CP_ACP, 0, wstr, (int32_t)wstr_length, nullptr, 0, nullptr, nullptr);
					if(length > 0 && mstr)
					{
						*mstr = new char[static_cast<unsigned __int64>(length) + 1] { };
						::WideCharToMultiByte(CP_ACP, 0, wstr, (int32_t)wstr_length, *mstr, length, nullptr, nullptr);
					}
				}
				return (size_t)length;
			}

			static std::string FromUnicode(const wchar_t*wstr, size_t wstr_length)
			{
				std::string str;
				if(wstr)
				{
					auto length = ::WideCharToMultiByte(CP_ACP, 0, wstr, (int)wstr_length, nullptr, 0, nullptr, nullptr);
					if(length > 0)
					{
						str.reserve(length);
						length = ::WideCharToMultiByte(CP_ACP, 0, wstr, (int)wstr_length, &str[0], length, nullptr, nullptr);
						str.resize(length);
					}
				}
				return str;
			}

			// Convert an wide Unicode string to ANSI string
			static char *FromUnicode(const wchar_t*wstr)
			{
				char *ret = nullptr;
				FromUnicode(wstr, size_t(-1), &ret);
				return ret;
			}

			// Convert an UTF8 string to a ANSI String
			static size_t FromUTF8(const char *utf8, size_t utf8_length, char **mstr)
			{
				if(utf8)
				{
					wchar_t *wstr = nullptr;
					auto length = Unicode::From(utf8, utf8_length, &wstr, CP_UTF8);
					if(length > 0)
					{
						length = FromUnicode(wstr, length, mstr);
						delete[] wstr;
						return length;
					}
				}
				return 0;
			}

			// Convert an UTF8 string to a wide Unicode String
			static size_t ToUnicode(const char *mstr, size_t length, wchar_t **wstr)
			{
				return Unicode::From(mstr, length, wstr, CP_ACP);
			}

			static wchar_t *ToUnicode(const char *mstr)
			{
				return Unicode::From(mstr, CP_ACP);
			}
		};
	}
}