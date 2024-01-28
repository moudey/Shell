#pragma once

namespace Nilesoft
{
	namespace Text
	{
		class Buffer
		{
		private:

			BYTE*  m_pBuffer;
			size_t m_nBuffer;

		public:

			Buffer()
				: m_pBuffer(nullptr), m_nBuffer(0)
			{
			}

			Buffer(const Buffer& other)
				: m_pBuffer(nullptr)
			{
				this->Copy(other);
			}


			Buffer(const BYTE* other, size_t length)
				: m_pBuffer(nullptr), m_nBuffer(0)
			{
				this->Copy(other, length);
			}

			Buffer(const TCHAR* hexString)
				: m_pBuffer(nullptr), m_nBuffer(0)
			{
				this->Copy(hexString);
			}

			Buffer::~Buffer()
			{
				this->Clear();
			}

		public:

			bool Copy(const Buffer& other)
			{
				return this->Copy(other.m_pBuffer, other.m_nBuffer);
			}

			bool Copy(const Buffer* other)
			{
				if(other != nullptr)
				{
					return this->Copy(other->m_pBuffer, other->m_nBuffer);
				}
				else
				{
					return false;
				}
			}

			bool Copy(const BYTE* other, size_t length)
			{
				if(other != nullptr && this->Resize(length))
				{
					::CopyMemory(this->m_pBuffer, other, this->m_nBuffer);
					return true;
				}
				else
				{
					return false;
				}
			}

			bool Copy(const TCHAR* hexString)
			{
				return this->HexStringToByteArray(hexString);
			}

			bool Resize(size_t length)
			{
				// TODO: Optimize resizing in the sense of: (if length <= m_nBuffer) -> don't delete.
				this->Clear();
				if(length > 0)
				{
					this->m_pBuffer = new BYTE[length];
					if(this->m_pBuffer != nullptr)
					{
						this->m_nBuffer = length;
						::ZeroMemory(this->m_pBuffer, this->m_nBuffer);
					}
				}
				return (this->m_pBuffer != nullptr);
			}

			void Buffer::Clear()
			{
				if(this->m_pBuffer != nullptr) { delete[] this->m_pBuffer; }
				this->m_pBuffer = nullptr;
				this->m_nBuffer = 0;
			}

		public: // Properties

			inline BYTE* Bytes()
			{
				return this->m_pBuffer;
			}

			inline size_t Length()
			{
				return this->m_nBuffer;
			}

			inline bool IsEmpty()
			{
				return !((this->m_nBuffer != 0) && (this->m_nBuffer > 0));
			}

		public: // Operators

			inline operator BYTE*()
			{
				return this->Bytes();
			}

			inline Buffer& operator = (const TCHAR* hexString)
			{
				this->Copy(hexString);
				return *this;
			}

			inline Buffer& operator = (const Buffer& other)
			{
				this->Copy(other);
				return *this;
			}

			inline Buffer& operator = (const Buffer* other)
			{
				this->Copy(other);
				return *this;
			}

		private:

			bool HexStringToByteArray(const TCHAR* hexString)
			{
				if(hexString == nullptr || hexString[0] == TEXT('\0')) { return false; }

				size_t length = _tcslen(hexString);
				if(length % 2 != 0) { return false; }

				if(!this->Resize(length / 2)) { return false; }

				TCHAR* string = const_cast<TCHAR*>(hexString);
				BYTE*  buffer = this->m_pBuffer;
				TCHAR  current = _toupper(*string);

				while(*string != TEXT('\0'))
				{
					if(!isxdigit(*string))
					{
						this->Clear();
						return false;
					}

					// Add high byte part.
					if(current >= TEXT('0') && current <= TEXT('9'))
					{
						*buffer = (BYTE)((current - TEXT('0')) << 4);
					}
					else if((current >= TEXT('A') && current <= TEXT('F')))
					{
						*buffer = (BYTE)((current - TEXT('A') + 10) << 4);
					}
					current = _toupper(*++string); // Next character.

												  // Add low byte part.
					if(current >= TEXT('0') && current <= TEXT('9'))
					{
						*buffer |= (current - TEXT('0'));
					}
					else if((current >= TEXT('A') && current <= TEXT('F')))
					{
						*buffer |= (current - TEXT('A') + 10);
					}
					current = _toupper(*++string); // Next character.

					buffer++; // Next byte.
				}
				return true;
			}
		};
	}
}