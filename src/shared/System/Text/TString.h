#pragma once

namespace Nilesoft
{
	namespace Text
	{
		template <class T>
		class String
		{
			T *string;
		public:
			String(T *str) : string(str) {}
			String(const T *str) : string((wchar_t*)str) {}
			~String() { string = nullptr; }

			void Set(T *str) { string = str; }

			//operator T() { return string; }
			operator TCHAR*() { return (TCHAR *)(string); }
			operator const TCHAR() { return (const TCHAR *)(string); }

			const String<T>& operator=(T *src)
			{
				string = src;
				return(*this);
			}

			const String<T>& operator=(const String<T> &src)
			{
				string = (wchar_t*)src.ToString();
				return(*this);
			}

			const String<T>& operator=(const T* src)
			{
				string = src;
				return(*this);
			}

			bool IsEmpty()
			{
				return string::IsNullOrEmpty(string);
			}

			int Length()
			{
				return string::Length(string);
			}

			bool Equle(const T* str, bool ignoreCase = true)
			{
				return string::Equals(string, str, ignoreCase);
			}

			bool Equle(const T* str1, const T* str2, const T* str3 = nullptr)
			{
				return (Equle(str1) || Equle(str2) || Equle(str3));
			}

			bool Find(T wMatch)
			{
				return string::Find(string, wMatch) != nullptr;
			}

			bool Find(const T* pszMatch)
			{
				return string::Find(string, pszMatch) != nullptr;
			}

			int ToInt()
			{
				return string::ToInt<int, T>(string);
			}

			int ToInt(int defaultValue = -1)
			{
				return string::ToInt<int, T>(string, defaultValue);
			}

			int HexToInt(int defaultValue = -1)
			{
				return string::HexToInt(string, defaultValue);
			}

			/*bool IsValid(int nLength = -1)
			{
				return string::IsValid(pszBlock, nLength);
			}*/

			int Hash()
			{
				return string::Hash(string);
			}

			const T& ToString()
			{
				return string;
			}
		};
	}
}
