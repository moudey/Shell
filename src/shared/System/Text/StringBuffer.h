#pragma once

namespace Nilesoft
{
	namespace Text
	{
		class StringBuffer
		{
		private:
			TCHAR* m_pBuffer;
			size_t m_nLength;

		public:
			StringBuffer() noexcept
				: m_pBuffer(nullptr), m_nLength(0)
			{
			}

			StringBuffer(size_t length)
				: m_pBuffer(nullptr), m_nLength(0)
			{
				this->Resize(length);
			}

			StringBuffer(const TCHAR* buffer)
				: m_pBuffer(nullptr), m_nLength(0)
			{
				this->Copy(buffer);
			}

			StringBuffer(StringBuffer& buffer)
				: m_pBuffer(nullptr), m_nLength(0)
			{
				this->Copy(buffer);
			}

			StringBuffer(StringBuffer* buffer)
				: m_pBuffer(nullptr), m_nLength(0)
			{
				this->Copy(buffer);
			}

			~StringBuffer()
			{
				if(this->m_pBuffer)
					delete[] this->m_pBuffer;
				this->m_pBuffer = nullptr;
				this->m_nLength = 0;
			}

			TCHAR* Resize(size_t length)
			{
				if(length > this->m_nLength)
				{
					delete[] this->m_pBuffer;
					this->m_pBuffer = new TCHAR[length + 1]{};

					if(this->m_pBuffer != nullptr)
					{
						this->m_nLength = length;
					}
					else
					{
						this->m_nLength = 0;
					}
				}

				if(this->m_pBuffer != nullptr && this->m_nLength > 0)
				{
					this->m_pBuffer[0] = TEXT('\0');
				}

				return this->m_pBuffer;
			}

			bool Copy(const TCHAR* buffer)
			{
				if(nullptr != buffer && nullptr != this->Resize(_tcslen(buffer)))
				{
					return (0 == _tcscpy_s(this->m_pBuffer, this->m_nLength + 1, buffer));
				}
				else
				{
					return false;
				}
			}

			bool Copy(StringBuffer& buffer)
			{
				if(nullptr != this->Resize(buffer.Length()))
				{
					return (0 == _tcscpy_s(this->m_pBuffer, this->m_nLength + 1, buffer.Buffer()));
				}
				else
				{
					return false;
				}
			}

			bool Copy(StringBuffer* buffer)
			{
				if(nullptr != buffer)
				{
					return this->Copy(*buffer);
				}
				else
				{
					return false;
				}
			}


		public:

			inline size_t Length()
			{
				return this->m_nLength;
			}

			inline TCHAR* Buffer()
			{
				return this->m_pBuffer;
			}

			inline StringBuffer& operator=(const TCHAR* buffer)
			{
				this->Copy(buffer);
				return *this;
			}

			inline StringBuffer& operator=(StringBuffer& buffer)
			{
				this->Copy(buffer);
				return *this;
			}

			inline StringBuffer& operator=(StringBuffer* buffer)
			{
				this->Copy(buffer);
				return *this;
			}
		};
	}
}