#pragma once

#include <VersionHelpers.h>

namespace Nilesoft
{
	namespace IO
	{
		enum class PathType : int
		{
			Unknown = -1,
			Namespace,
			File,
			Desktop,
			Directory,
			Drive,
			Fixed,
			Removable,
			Floppy,
			DVD,
			VHD,
			USB,
			Remote,
			Taskbar,
			Back,
			Back_Namespace,
			Back_Directory,
			Back_Drive,
			Back_Fixed,
			Back_Removable,
			Back_DVD,
			Back_VHD,
			Back_USB,
			Back_Remote,
			Computer,
			Recyclebin
		};

		class SmartFileMapping
		{
		public:
			SmartFileMapping(void* v) { m_v = v; }
			~SmartFileMapping(void) { if(m_v) ::UnmapViewOfFile(m_v); }
			operator LPVOID(void) { return m_v; }
		private:
			void* m_v = nullptr;
		};

		// File properties
		class FileProperties
		{
		public:
			string	Path;
			string	PathRaw;

			BOOL	FileSystem = FALSE;
			BOOL	FileSysAnceStor = FALSE;
			BOOL	Folder = FALSE;
			BOOL	Namespace = FALSE;
			BOOL	File = FALSE;
			BOOL	Dir = FALSE;
			BOOL	Removable = FALSE;
			BOOL	Link = FALSE;
			BOOL	Stream = FALSE;
			BOOL	DropTarget = FALSE;
			BOOL	HasStorage = FALSE;
			BOOL	CanMoniker = FALSE;
			BOOL	IsFile = FALSE;
			BOOL	IsDir = FALSE;
			BOOL	IsNamespace = FALSE;
			BOOL	Background = FALSE;
			BOOL	ReadOnly = FALSE;
			BOOL	Hidden = FALSE;
			SFGAOF	Attr = 0;

			FileProperties() noexcept
			{
			}
		};

		static inline PathType GetPathType(PathType pathType)
		{
			switch(pathType)
			{
				case PathType::Desktop:
					return PathType::Desktop;

				case PathType::Namespace:
					return PathType::Namespace;

				case PathType::Drive:
				case PathType::Fixed:
				case PathType::VHD:
				case PathType::DVD:
				case PathType::Floppy:
				case PathType::USB:
				case PathType::Removable:
					return PathType::Drive;

				case PathType::Directory:
					return PathType::Directory;

				case PathType::File:
					return PathType::File;
				
				case PathType::Remote:
					return PathType::Remote;

				default:
					return PathType::Unknown;
			}
		}

		static inline int GetTypeOfDrive(const string &pPath)
		{
			int busType = BusTypeUnknown;
			if(pPath.empty())
				return busType;
			//Open the disk for reading (must be an administrator)
			auto_handle device = ::CreateFileW(string(LR"(\\?\)" + pPath.left(2)),
										GENERIC_READ, FILE_SHARE_READ,
										nullptr, OPEN_EXISTING, 0, nullptr);
			if(device)
			{
				DWORD dwOutBytes;
				STORAGE_DEVICE_DESCRIPTOR desc = {};
				STORAGE_PROPERTY_QUERY query = {};
				query.PropertyId = StorageDeviceProperty;
				query.QueryType = PropertyStandardQuery;
				if(::DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY, 
									 &query, sizeof(query),
									 &desc, sizeof(desc),
									 &dwOutBytes, nullptr))
				{
					busType = desc.BusType;
				}
			}
			return busType;
		}

		static inline bool IsCOMLibrary(const wchar_t* pszFilePath)
		{
			DLL lib(pszFilePath);
			return (lib.is_func("DllRegisterServer") &&
					lib.is_func("DllUnregisterServer"));
		}

		static inline bool SetDateTime(HANDLE hFile, SYSTEMTIME *lpCreationTime, SYSTEMTIME *lpLastAccessTime, SYSTEMTIME *lpLastWriteTime)
		{
			bool result = false;
			if(hFile != INVALID_HANDLE_VALUE)
			{
				FILETIME ftCreate{};
				FILETIME ftAccess{};
				FILETIME ftWrite{};

				if(lpCreationTime)
				{
					::SystemTimeToFileTime(lpCreationTime, &ftCreate);
					::LocalFileTimeToFileTime(&ftCreate, &ftCreate);
				}

				if(lpLastAccessTime)
				{
					::SystemTimeToFileTime(lpLastAccessTime, &ftAccess);
					::LocalFileTimeToFileTime(&ftAccess, &ftAccess);
				}

				if(lpLastWriteTime)
				{
					::SystemTimeToFileTime(lpLastWriteTime, &ftWrite);
					::LocalFileTimeToFileTime(&ftWrite, &ftWrite);
				}

				result = ::SetFileTime(hFile,
									   lpCreationTime ? &ftCreate : nullptr,
									   lpLastAccessTime ? &ftAccess : nullptr,
									   lpLastWriteTime ? &ftWrite : nullptr);
			}
			return result;
		}

		static inline bool SetDateTime(const wchar_t *path, SYSTEMTIME *lpCreationTime, SYSTEMTIME *lpLastAccessTime, SYSTEMTIME *lpLastWriteTime)
		{
			bool result = false;
			auto hFile = ::CreateFileW(path, FILE_WRITE_ATTRIBUTES,
									   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
									   nullptr, OPEN_EXISTING, 0, nullptr);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				result = SetDateTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
				::CloseHandle(hFile);
			}
			return result;
		}

		static inline bool GetDateTime(HANDLE hFile, SYSTEMTIME *lpCreationTime, SYSTEMTIME *lpLastAccessTime, SYSTEMTIME *lpLastWriteTime)
		{
			bool result = false;
			if(hFile != INVALID_HANDLE_VALUE)
			{
				FILETIME ftCreate{};
				FILETIME ftAccess{};
				FILETIME ftWrite{};
				result = ::GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
				if(result)
				{
					if(lpCreationTime)
					{
						::FileTimeToLocalFileTime(&ftCreate, &ftCreate);
						::FileTimeToSystemTime(&ftCreate, lpCreationTime);
					}

					if(lpLastAccessTime)
					{
						::FileTimeToLocalFileTime(&ftAccess, &ftAccess);
						::FileTimeToSystemTime(&ftAccess, lpLastAccessTime);
					}

					if(lpLastWriteTime)
					{
						::FileTimeToLocalFileTime(&ftWrite, &ftWrite);
						::FileTimeToSystemTime(&ftWrite, lpLastWriteTime);
					}
				}
			}
			return result;
		}

		static inline bool GetDateTime(const wchar_t *path, SYSTEMTIME *lpCreationTime, SYSTEMTIME *lpLastAccessTime, SYSTEMTIME *lpLastWriteTime)
		{
			bool result = false;
			auto hFile = ::CreateFileW(path, FILE_READ_ATTRIBUTES,
									   FILE_SHARE_READ | FILE_SHARE_DELETE,
									   nullptr, OPEN_EXISTING, 0, nullptr);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				result = GetDateTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
				::CloseHandle(hFile);
			}
			return result;
		}

		static inline bool DateTime(bool set, HANDLE hFile, SYSTEMTIME *lpCreationTime, SYSTEMTIME *lpLastAccessTime, SYSTEMTIME *lpLastWriteTime)
		{
			if(set)
				return SetDateTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
			return GetDateTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
		}

		static inline bool DateTime(bool set, const wchar_t *path, SYSTEMTIME *lpCreationTime, SYSTEMTIME *lpLastAccessTime, SYSTEMTIME *lpLastWriteTime)
		{
			if(set)
				return SetDateTime(path, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
			return GetDateTime(path, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
		}
	}
}
