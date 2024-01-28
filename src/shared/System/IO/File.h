#pragma once

namespace Nilesoft
{
	namespace IO
	{
		class File
		{
			public:
				enum class Encoding
				{
					ANSI,//Ansi
					UTF8,//Utf8
					UTF16LE,//Utf16LE
					UTF16BE,//Utf16BE
					Binary
					//UTF32LE,
					//UTF32BE,
				};

		private:
			bool m_write = false;
			Encoding m_encoding = Encoding::ANSI;

		public:
			FILE *Handle;
			string Path;

			enum class Seek
			{
				CUR = 1,
				END = 2,
				SET = 0
			};

		public:
			File() noexcept
				:Path { nullptr }, Handle { nullptr }
			{
			}

			File(const string &path) :
				Path { path }, Handle { nullptr }
			{
			}

			File(const string &path, Encoding encoding) :
				Path { path }, Handle { nullptr }, m_encoding{ encoding }
			{
			}

			File(const File& file) :
				Path { file.Path },
				Handle { file.Handle },
				m_write { file.m_write }
			{
			}

			~File()
			{
				this->Close();
			}

			File &operator =(const File &file)
			{
				this->Handle = file.Handle;
				this->Path = file.Path;
				this->m_write = file.m_write;
				this->m_encoding = file.m_encoding;
				return *this;
			}

			bool Open(const string &path, bool write, Encoding encoding)
			{
				this->Path = path;
				m_write = write;
				m_encoding = encoding;
				return Open();
			}

			bool Open(bool write, Encoding encoding)
			{
				m_write = write;
				m_encoding = encoding;
				return Open();
			}

			bool Open(const string &path, bool write)
			{
				this->Path = path;
				return Open(write, m_encoding);
			}

			bool Open()
			{
				string mode = m_write ? L"w" : L"r";

				if(m_encoding == Encoding::Binary)
				{
					mode += L"b";
				}
				else if(m_encoding == Encoding::UTF8)
				{
					mode += L", ccs=UTF-8";
				}
				else if(m_encoding == Encoding::UTF16LE)
				{
					mode += L", ccs=UTF-16LE";
				}
				else if(m_encoding == Encoding::UTF16BE)
				{
					mode += L", ccs=UNICODE";
				}

				if(0 == ::_wfopen_s(&this->Handle, this->Path, mode))
					return this->Handle != nullptr;
				return false;
			}

			bool OpenReload(bool write = false, Encoding encoding = Encoding::ANSI)
			{
				m_write = write;
				m_encoding = encoding;
				string mode =  L"r";
				if(write)
					mode = L"w";

				if(encoding == Encoding::Binary)
				{
					mode += L"b";
				}
				else if(encoding == Encoding::UTF8)
				{
					mode += L", ccs=UTF-8";
				}
				else if(encoding == Encoding::UTF16LE)
				{
					mode += L", ccs=UTF-16LE";
				}
				else if(encoding == Encoding::UTF16BE)
				{
					mode += L", ccs=UNICODE";
				}

				FILE* newFile = nullptr;
				if(0 == ::_wfreopen_s(&newFile, this->Path, mode, this->Handle))
				{
					this->Handle = newFile;
					return this->Handle != nullptr;
				}
				return false;
			}
			
			template<typename T>
			size_t Write(T buffer)
			{
				return Write(&buffer, 1);
			}

			template<typename T>
			size_t Write(T* buffer)
			{
				return Write(buffer, 1);
			}

			template<typename T>
			size_t Write(T* buffer, size_t count)
			{
				if(this->Handle && m_write)
					return ::fwrite(buffer, sizeof(T), count, this->Handle);
				return 0;
			}

			size_t Write(void* buffer, size_t size, size_t count)
			{
				if(this->Handle && m_write)
					return ::fwrite(buffer, size, count, this->Handle);
				return 0;
			}

			size_t WriteFormat(const wchar_t* format, ...)
			{
				if(this->Handle && m_write)
				{
					if(format && *format)
					{
						size_t res = 0;
						va_list argList;
						va_start(argList, format);
						int length = ::_vscwprintf(format, argList);
						if(length)
						{
							std::unique_ptr<wchar_t[]> buffer (new wchar_t[static_cast<unsigned __int64>(length) + 1]{});
							::vswprintf_s(&buffer[0], length, format, argList);
							res = Write((void*)&buffer[0], sizeof(wchar_t), length);
						}
						va_end(argList);
						return res;
					}
				}
				return 0;
			}

			size_t WriteFormat(const char* format, ...)
			{
				if(this->Handle && m_write)
				{
					if(format && *format)
					{
						size_t res = 0;
						va_list argList;
						va_start(argList, format);
						auto length = (size_t)::_vscprintf(format, argList);
						if(length++ > 0)
						{
							std::unique_ptr<char[]> buffer(new char[length]);
							::vsprintf_s(&buffer[0], length, format, argList);
							buffer[length - 1] = 0;
							res = Write((void*)&buffer[0], sizeof(buffer[0]), length - 1);
						}
						va_end(argList);
						return res;
					}
				}
				return 0;
			}

			bool IsOpen()
			{
				return (Handle != nullptr);
			}

			bool Flush()
			{
				return ::fflush(Handle) == 0;
			}

			template<typename T>
			size_t Read(T* buffer)
			{
				return Read(buffer, 1);
			}

			template<typename T>
			size_t Read(T*& buffer, size_t count)
			{
				if(this->Handle)
					return ::fread(buffer, sizeof(T), count, this->Handle);
				return 0;
			}

			template<typename T>
			size_t Read(T** buffer, size_t size, size_t count)
			{
				if(this->Handle)
					return ::fread(*buffer, size, count, this->Handle);
				return 0;
			}


			size_t Read(void* buffer, size_t size, size_t count)
			{
				if(this->Handle)
					return ::fread(buffer, size, count, this->Handle);
				return 0;
			}

			int GetChar()
			{
				if(this->Handle)
					return ::fgetc(this->Handle);
				return EOF;
			}

			bool IsEnd()
			{
				if(this->Handle)
					return ::feof(this->Handle) != 0;
				return true;
			}

			bool Seek(long offset, Seek seek)
			{
				if(this->Handle)
					return ::fseek(this->Handle, offset, (int)seek) == 0;
				return false;
			}

			bool SeekPos(long offset) { return Seek(offset, Seek::CUR); }
			void SeekStart() { if(this->Handle) ::rewind(this->Handle); }
			bool SeekEnd() { return Seek(0, Seek::END); }

			long Size()
			{
				long res = -1;
				if(this->Handle) {
					this->SeekEnd();
					res = ::ftell(this->Handle);
					this->SeekStart();
				}
				return res;
			}

			__int64 Size64()
			{
				__int64 res = -1;
				if(this->Handle) {
					this->SeekEnd();
					res = ::_ftelli64(this->Handle);
					this->SeekStart();
				}
				return res;
			}

			bool Close()
			{
				bool res = true;
				if(this->Handle)
					res = ::fclose(this->Handle) != -1;
				this->Handle = nullptr;
				return res;
			}

			operator FILE *(void) { return Handle; }

			static intptr_t LastWriteTime(const wchar_t* path)
			{
				/*auto hFile = ::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
										  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
				unsigned res = 0;
				if(hFile != INVALID_HANDLE_VALUE)
				{
					FILETIME lastWriteTime{};
					if(::GetFileTime(hFile, nullptr, nullptr, &lastWriteTime))
						res = lastWriteTime.dwHighDateTime + lastWriteTime.dwLowDateTime;
					::CloseHandle(hFile);
				}
				return res;*/
#ifdef _WIN64
				struct _stat64 st = {};
				if(0 == ::_wstat64(path, &st))
					return st.st_mtime;
#else
				struct _stat32 st = {};
				if(0 == ::_wstat32(path, &st))
					return st.st_mtime;
#endif
				return 0;
			}

			static bool Copy(const wchar_t* from, const wchar_t* to)
			{
				File from_file(from);
				if(from_file.Open(false, Encoding::Binary)) {
					auto z = from_file.Size();
					auto buf = new wchar_t[z] { };
					if(from_file.Read(buf, sizeof(wchar_t), z)) {
						File to_file(to);
						if(to_file.Open(true, Encoding::Binary)) {
							to_file.Write(buf, sizeof(wchar_t), z);
						}
					}
				}
			}
		
			static EncodingType GetFileEncoding(const wchar_t* path)
			{
				const size_t size = 4;
				unsigned char buffer[size] = { };
				FILE *file = nullptr;
				if(0 == ::_wfopen_s(&file, path, L"rb") && file)
				{
					if(size == ::fread(buffer, sizeof(buffer[0]), size, file))
					{
						return Text::Encoding::GetType(buffer, 4);
					}
					::fclose(file);
				}
				return EncodingType::Unknown;
			}

			static std::wstring ReadText(const std::wstring_view &path, uint32_t count = uint32_t (-1))
			{
				try
				{
					File file(path);
					if(file.Open(false, File::Encoding::Binary))
					{
						size_t numbytes = count == uint32_t(-1) ? file.Size() : count;
						if(numbytes > 0)
						{
							std::unique_ptr<byte[]> buf(new byte[numbytes + 4]{ 0 });
							numbytes = file.Read(&buf[0], sizeof(byte), numbytes);
							size_t headerSz = 0;
							auto encodeType = Text::Encoding::GetType(buf.get(), numbytes);
							switch(encodeType)
							{
								case EncodingType::ANSI:
								{
									return std::move(Unicode::FromANSI((char *)buf.get(), uint32_t(numbytes)));
								}
								// UTF-8
								case EncodingType::UTF8:
								case EncodingType::UTF8BOM:
								{
									if(encodeType == EncodingType::UTF8BOM)
										headerSz = 3;

									auto utf8 = (char *)(buf.get() + headerSz);
									return std::move(Unicode::FromUTF8(utf8, numbytes - headerSz));
								}
								// UTF-16
								case EncodingType::UTF16LE:
								case EncodingType::UTF16LEBOM:
								case EncodingType::UTF16BE:
								case EncodingType::UTF16BEBOM:
								{
									if(encodeType == EncodingType::UTF16BEBOM ||
									   encodeType == EncodingType::UTF16LEBOM)
									{
										headerSz = 2;
									}

									auto sz = (numbytes - headerSz);
									auto buffer2 = buf.get() + headerSz;

									if(encodeType == EncodingType::UTF16BE ||
									   encodeType == EncodingType::UTF16BEBOM)
									{
										swab(buffer2, sz);
									}

									const size_t length = (sz / sizeof(wchar_t)) - 1;

									std::unique_ptr<wchar_t[]> buffer(new wchar_t[length + 4]{ 0 });
									return (wchar_t*)::memcpy(&buffer[0], buffer2, sz);
								}
								case EncodingType::UTF32LE:
								case EncodingType::UTF32BE:
									break;
							}
						}
					}
				}
				catch(...)
				{
				}
				return {};
			}
		};

	}
}
