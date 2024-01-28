#pragma once

namespace Nilesoft
{
	namespace IO
	{
		using namespace Text;

		class Path
		{

		public:
			static const auto Separator = '\\';

			string _path;
			string _root;
			string _dir;
			string _name;
			string _title;
			string _extension;

			Path()
			{
			}

			Path(const string &path) : _path{path}
			{
				split();
			}

			bool split()
			{
				if(!_path.empty())
				{
					auto p = _path.last_index_of(Path::Separator, false);
					if(p == _path.npos|| p == 0)
						_name = _path;
					else
					{
						_dir = _path.substr(0, p == 2 ? p + 1 : p).move();
						_name = _path.substr(p + 1).move();
					}

					if(!_name.empty())
					{
						p = _name.last_index_of(L'.', false);
						if(p == _name.npos || p == 0)
							_title = _name;
						else
						{
							_title = _name.substr(0, p).move();
							_extension = _name.substr(p).move();
						}
					}
					return true;
				}
				return false;
			}

			bool is_absolute() const
			{
				if(_path.length() >= 3)
				{
					if(_path[0u] == L'/')
					{
						if(_path[1u] != L'/')
							return false;
						return true;
					}

					if(_path[0u] == L'\\')
					{
						if(_path[1u] != L'\\')
							return false;
						return true;
					}

					// Maybe "X:\blahblah"?
					if(_path[1u] == L':')
					{
						if(_path[2u] == L'\\' || _path[2u] == L'/')
							return true;
					}
				}
				return false;
			}

			static string OpenFolderDialog(const std::wstring_view &title, HWND hWndOwner = nullptr)
			{
				string res = nullptr;
				IFileDialog *pfd = nullptr;

				if(S_OK == CoCreateInstance(CLSID_FileOpenDialog, nullptr,
											CLSCTX_INPROC_SERVER,
											__uuidof(IFileDialog),
											(void**)&pfd))
				{
					DWORD dwOptions;
					if(S_OK == pfd->GetOptions(&dwOptions))
						pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
					
					pfd->SetTitle(title.data());

					if(S_OK == pfd->Show(hWndOwner))
					{
						IShellItem *psi;
						if(S_OK == pfd->GetResult(&psi))
						{
							wchar_t* name = nullptr;
							if(S_OK == psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &name))
							{
								if(name)
								{
									res = name;
									::CoTaskMemFree(name);
								}
							}
							psi->Release();
						}
					}
					pfd->Release();
				}
				return res.move();
			}

			static string OpenFileDialog(OPENFILENAMEW *ofn)
			{
				if(ofn) {
					if(!ofn->Flags)
						ofn->Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
					if(DLL::Invoke<int>(L"COMDLG32.DLL", "GetOpenFileNameW", ofn) == IDOK)
						return ofn->lpstrFile;
				}
				return nullptr;
			}

			static string OpenFileDialog(const std::wstring_view &title, const wchar_t* lpFilter,
										 const std::wstring_view &initialDir, HWND hwndOwner)
			{
				OPENFILENAMEW ofn = { };
				ofn.lStructSize = sizeof(OPENFILENAMEW);

				if(lpFilter)
					ofn.lpstrFilter = lpFilter;
				else
					ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";

				wchar_t szFile[MAX_PATH] = { };
				ofn.lpstrTitle = title.data();
				ofn.lpstrInitialDir = initialDir.data();
				ofn.hwndOwner = hwndOwner;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				return OpenFileDialog(&ofn).move();
			}

			static string Search(const string &path)
			{
				string fullPath(MAX_PATH);
				auto l = ::SearchPathW(nullptr, path, nullptr, MAX_PATH, fullPath.data(), nullptr);
				return fullPath.release(l).move();
			}

			static string GetKnownFolder(const KNOWNFOLDERID &rfid)
			{
				string res;
				IComPtr<IKnownFolderManager> pkfm;
				if(pkfm.CreateInstance(CLSID_KnownFolderManager, CLSCTX_INPROC_SERVER))
				{
					IComPtr<IKnownFolder> pKnownFolder;
					if(SUCCEEDED(pkfm->GetFolder(rfid, pKnownFolder)))
					{
						wchar_t *lpPath = nullptr;
						if(SUCCEEDED(pKnownFolder->GetPath(KF_FLAG_DEFAULT, &lpPath)) && lpPath)
						{
							res = lpPath;
							::CoTaskMemFree(lpPath);
						}
					}
				}
				return res.move();
			}

			static string CurrentDirectory()
			{
				string temp(MAX_PATH);
				return temp.release(::GetCurrentDirectoryW(MAX_PATH, temp.buffer())).move();
			}

			static bool CurrentDirectory(const wchar_t *path)
			{
				return ::SetCurrentDirectoryW(path) != 0;
			}

			static string TempDirectory()
			{
				string temp(MAX_PATH);
				return temp.release(::GetTempPathW(MAX_PATH, temp.buffer())).move();
			}

			static bool IsDesktop(const string &path)
			{
				return path.equals(GetKnownFolder(FOLDERID_Desktop), true);
			}

			static bool IsSlash(const std::wstring_view &start, int nPos)
			{
				auto len = (int)start.length();
				if(len == 0u || nPos >= len || nPos < 0u)
					return false;
				return start[nPos] == '/' || start[nPos] == '\\';
			}

			static bool EndsWithSlash(const std::wstring_view &path)
			{
				return path.back() == Path::Separator || path.back() == '/';
			}

			static const wchar_t* FindFileName(const wchar_t* path)
			{
				while(const wchar_t* slash = wcspbrk(path, L"\\/"))
					path = slash + 1;
				return path;
			}

			static std::wstring_view GetRoot(const std::wstring_view &path)
			{
				if(path[1] == ':' && path[2] == '\\')
					return path.substr(0, 3);
				return std::move(std::wstring_view());
			}

			static bool GetLinkInfo(const std::wstring_view &linkFile, string *path, string *workingDirectory, string *iconLocation = nullptr, int *piicon = nullptr)
			{
				IComPtr<IShellLinkW> sl;
				auto hres = ::CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, sl);
				if(SUCCEEDED(hres))
				{
					IComPtr<IPersistFile> pf;
					hres = sl->QueryInterface(IID_IPersistFile, pf);
					if(SUCCEEDED(hres))
					{
						hres = pf->Load(linkFile.data(), STGM_READ);
						if(SUCCEEDED(hres))
						{
							//hres = sl->Resolve(nullptr, SLR_NOSEARCH | SLR_NOUPDATE |
							//				   SLR_NOTRACK | SLR_NOLINKINFO);
							//if(SUCCEEDED(hres))
							{
								if(path)
								{
									hres = sl->GetPath(path->buffer(MAX_PATH), MAX_PATH, nullptr, SLGP_RAWPATH);
									if(SUCCEEDED(hres))
										path->release();
									else
										path->release(0);
								}

								if(workingDirectory)
								{
									hres = sl->GetWorkingDirectory(workingDirectory->buffer(MAX_PATH), MAX_PATH);
									if(SUCCEEDED(hres))
										workingDirectory->release();
									else
										workingDirectory->release(0);
								}

								if(iconLocation)
								{
									int iicon = 0;
									hres = sl->GetIconLocation(iconLocation->buffer(260), 260, &iicon);
									if(SUCCEEDED(hres))
									{
										iconLocation->release();
										if(piicon)
											*piicon = iicon;
										else
											iconLocation->append_format(L",%d", iicon);
									}
									else
										iconLocation->release(0);
								}
							}
						}
					}
				}
				return hres == S_OK;
			}

			//path.lnk.create(path, target, args, icon, dir, run, admin, comment)
			static bool CreateLnk(const wchar_t* path, const wchar_t *taget, const wchar_t *args,
								   const wchar_t *iconPath = nullptr, int iconIndex = 0, const wchar_t * workingDirectory = nullptr, int showCmd = SW_NORMAL, const wchar_t *comment = nullptr, bool runas = false)
			{
				if(!taget || !path)
					return false;

				IComPtr<IShellLinkW> sl;
				auto hres = ::CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, sl);
				if(SUCCEEDED(hres))
				{
					IComPtr<IPersistFile> pf;
					hres = sl->QueryInterface(IID_IPersistFile, pf);
					if(SUCCEEDED(hres))
					{
						if(taget)
							hres = sl->SetPath(taget);
						
						if(args)
							hres = sl->SetArguments(args);

						if(iconPath == nullptr)
							iconPath = taget;
						
						if(iconPath)
							hres = sl->SetIconLocation(iconPath, iconIndex);

						sl->SetShowCmd(showCmd);

						if(workingDirectory)
							hres = sl->SetWorkingDirectory(workingDirectory);

						if(comment)
							hres = sl->SetDescription(comment);
						
						if(runas)
						{
							IComPtr<IShellLinkDataList> sldl;
							hres = pf->QueryInterface(IID_IShellLinkDataList, sldl);
							if(SUCCEEDED(hres))
							{
								DWORD dwFlags = 0;
								sldl->GetFlags(&dwFlags);

								if(!(dwFlags & SLDF_RUNAS_USER))
								{
									dwFlags |= SLDF_RUNAS_USER;
									sldl->SetFlags(SLDF_RUNAS_USER);
								}
							}
						}
						return S_OK == pf->Save(path, FALSE);
					}
				}
				return hres == S_FALSE;
			}

			static string Long(const std::wstring_view &path, bool bExpandEnvs)
			{
				string sPath = path;
				auto len = sPath.length();
				if(len < 1)
					return sPath.move();

				wchar_t fullPath[_MAX_PATH] = { 0 };
				wchar_t* lpPart;

				//                                         GetFullPathName  GetLongPathName
				// Convert to fully qualified form              Yes               No
				//    (Including .)
				// Convert /, //, \/, ... to \                  Yes               No
				// Handle ., .., ..\..\..                       Yes               No
				// Convert 8.3 names to long names              No                Yes
				// Fail when file/directory does not exist      No                Yes
				//
				// Fully qualify/normalize name using GetFullPathName.

				// Expand environment variables:
				// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
				wchar_t expandedPath[_MAX_PATH] = { 0 };
				const wchar_t* lpcszPath = nullptr;

				if(bExpandEnvs && wcschr(sPath.c_str(), L'%'))
				{
					if(::ExpandEnvironmentStringsW(sPath, expandedPath, _MAX_PATH))
					{
						lpcszPath = expandedPath;
					}
				}
				else
					lpcszPath = sPath;

				if(!lpcszPath)
					return path;

				if(::GetFullPathNameW(lpcszPath, _MAX_PATH, fullPath, &lpPart))
				{
					//_tcscpy(fullPath, sPath.GetString());
					//::StringCchCopyW(fullPath, sPath.length(), sPath);
					string::Copy(fullPath, sPath.c_str(), sPath.length());
				}

				// We are done if this is not a short name.
				if(::wcschr(fullPath, L'~') == nullptr)
					return fullPath;

				// We have to do it the hard way because GetLongPathName is not
				// available on Win9x and some WinNT 4

				// The file/directory does not exist, use as much long name as we can
				// and leave the invalid stuff at the end.
				string sLong;
				wchar_t* ptr = fullPath;
				wchar_t* end = nullptr;

				// Skip to \ position     d:\abcd or \\host\share\abcd
				// indicated by ^           ^                    ^
				if(::lstrlenW(ptr) > 2)
					end = ::wcschr(fullPath + 2, Path::Separator);
				if(end && !_wcsnicmp(fullPath, L"\\\\", 2))
					end = wcschr(end + 1, Path::Separator);

				if(!end)
					return fullPath;

				*end = 0;
				sLong += ptr;
				ptr = &end[1];

				// now walk down each directory and do short to long name conversion
				while(ptr)
				{
					end = wcschr(ptr, Path::Separator);
					// zero-terminate current component
					// (if we're at end, its already zero-terminated)
					if(end)
						*end = 0;

					string sTemp(sLong.move());
					sTemp += Path::Separator;
					sTemp += ptr;

					// advance to next component (or set ptr==0 to flag end)
					ptr = (end ? end + 1 : 0);

					// (Couldn't get info for just the directory from CFindFile)
					WIN32_FIND_DATAW ffd;
					HANDLE h = ::FindFirstFileW(sTemp, &ffd);
					if(h == INVALID_HANDLE_VALUE)
					{
						sLong = sTemp.move();
						if(ptr)
						{
							sLong += Path::Separator;
							sLong += ptr;
						}
						return sLong;
					}
					sLong += Path::Separator;
					sLong += ffd.cFileName;
					::FindClose(h);
				}
				return sLong.move();
			}

			static string FixSeparator(const std::wstring_view &path, const wchar_t* separator = L"\\")
			{
				string fixed(path.length() + 1);
				bool last = false;
				for(size_t i = 0; i < path.length(); i++)
				{
					if(path[i] == L'/' || path[i] == L'\\')
					{
						if(!last) fixed.append(separator);
						last = true;
					}
					else
					{
						last = false;
						fixed.append(path[i]);
					}
				}
				return fixed.move();
			}

			static string Combine(const std::wstring_view &path, const std::wstring_view &subpath, const wchar_t separator = L'\\')
			{
				if(path.empty() || subpath.empty()) return subpath;

				string ret(path);

				auto f = subpath.front() == separator;
				auto b = path.back() == separator;

				if(f && b)
					ret.trim_end(separator);
				else if(!f && !b)
					ret.append(separator);

				return (ret + subpath).move();
			}

			static string Join(const std::wstring_view &path, const std::wstring_view &subpath, const wchar_t separator = L'\\')
			{
				return Combine(path, subpath, separator).move();
			}

			static string Join(std::initializer_list<const wchar_t*> list, const wchar_t separator = L'\\')
			{
				string result;
				for(auto p : list)
				{
					if(p)
					{
						result = Combine(result, p, separator).move();
					}
				}
				return result.move();
			}

			static size_t FindLastSepatartor(const std::wstring_view &path)
			{
				auto p = path.find_last_of(L'\\');
				if(p == path.npos)
					return path.find_last_of(L'/');
				return p;
			}

			static string Parent(const std::wstring_view &path)
			{
				if(path.length() > 3)
				{
					auto p = FindLastSepatartor(path);
					if(p != path.npos && p > 0)
					{
						return path.substr(0, p == 2 ? p + 1 : p);
					}
					return nullptr;
				}
				return path;
			}

			static string GetNS(const std::wstring_view &path)
			{
				auto p = FindLastSepatartor(path);
				if(p != path.npos)
				{
					if(p >= 0 && path[p + 1])
						return &path[p + 1];
				}
				return path;
			}

			static string Name(const std::wstring_view &path)
			{
				if(path.length() == 3 && path[1] == L':')
					return path.substr(0, 1);

				auto p = FindLastSepatartor(path);
				p = p == path.npos ? 0 : p + 1;

				return path.substr(p);
			}

			/*static string Title(const std::wstring_view &path)
			{
				if(path.length() == 3 && path[1] == ':')
					return path.substr(0, 1);

				auto p = path.find_last_of(Path::Separator);
				p = p == path.npos ? 0 : p + 1;

				auto pdot = path.find_last_of(L'.');
				if(pdot != path.npos && pdot > 0)
					return path.substr(p, pdot - p);

				return path.substr(p);
			}*/

			static string Title(const std::wstring_view &path)
			{
				if(path.length() == 3 && path[1] == L':')
					return path.substr(0, 1);

				auto p = path.find_last_of(Path::Separator);
				p = p == path.npos ? 0 : p + 1;

				string name = path.substr(p);

				p = name.last_index_of(L'.', false);
				if(p == 0 || p == name.npos)
					return name.move();

				return name.substr(0, p).move();
			}

			static std::wstring_view Extension(const std::wstring_view &path)
			{
				if(path.length() == 3 && path[1] == L':')
					return {};

				auto p = FindLastSepatartor(path);
				p = p == path.npos ? 0 : p + 1;

				string name = path.substr(p);

				p = name.last_index_of(L'.', false);
				if(p > 0 && p < name.npos)
				{
					return name.substr(p).move();
				}
				return {};
			}

			/*static std::wstring_view Extension(const std::wstring_view &path)
			{
				if(path.length() == 3 && path[1] == ':')
					return {};

				auto p = path.find_last_of(Path::Separator);
				p = p == path.npos ? 0 : p + 1;

				auto name = path.substr(p);
				if(!name.empty())
				{
					p = name.find_last_of(L'.');
					if(p != name.npos)
						return name.substr(p);
				}
				return {};
			}*/
			
			static bool IsAbsolute(const std::wstring_view &path)
			{
				if(path.length() >= 3)
				{
					if(path[0] == L'/')
					{
						if(path[1] != L'/')
							return false;
						return true;
					}

					if(path[0] == L'\\')
					{
						if(path[1] != L'\\')
							return false;
						return true;
					}

					// Maybe "X:\blahblah"?
					if(path[1] == L':')
					{
						if(path[2] == L'\\' || path[2] == L'/')
							return true;
					}
				}
				return false;
			}

			static bool IsRelative(const std::wstring_view &path)
			{
				if(path.length() >= 2)
				{
					if(path[0] == L'/')
					{
						if(path[1] != L'/')
							return true;
						return false;
					}

					if(path[0] == L'\\')
					{
						if(path[1] != L'\\')
							return true;
						return false;
					}

					// Maybe "X:\blahblah"?
					if(path[1] != L':')
						return true;
				}
				return false;
			}

			/*static int ParseIconLocation(string &path)
			{
				int result = 0;
				if(path.length() > 3)
				{
					intptr_t p = path.last_index_of(',', false);
					if(p > 0)
					{
						result = (int)string::ToInt(&path.c_str()[p + 1], 0LL);
						path = path.substr(0, p);
					}

					if(path[1] != ':')
						path = Search(path).move();
				}
				return result;
			}*/

			static int ParseMUILocation(string &path)
			{
				int result = 0;
				if(path.length() > 3)
				{
					if(path[0] != L'@')
						path.remove(0, 1);

					intptr_t p = path.last_index_of(L',', false);
					if(p > 0)
					{
						result = (int)string::ToInt(&path.c_str()[p + 1], 0LL);
						path = path.substr(0, p);
					}

					if(path[1] != L':')
						path = Search(path).move();
				}
				return result;
			}

			static bool IsCLSID(const std::wstring_view &path)
			{
				if(path.length() >= 40 /*&& path.Length() == 40*/) //40
					return false;
				return ((path[0] == L':' && path[1] == L':' && path[2] == L'{'));
			}

			static bool IsNameSpace(const std::wstring_view &path)
			{
				return IsCLSID(path);
			}

			static bool IsRoot(const std::wstring_view &path)
			{
				if(path.length() != 3)
					return false;
				return (path[1] == L':' && path[2] == Path::Separator);
			}

			static bool IsDrive(const std::wstring_view &path)
			{
				return IsRoot(path);
			}

			static bool IsDrivePrefix(const wchar_t *const path)
			{
				// test if _First points to a prefix of the form X:
				// pre: _First points to at least 2 wchar_t instances
				// pre: Little endian
				auto value = *(uint32_t *)path;
				value &= 0xFFFF'FFDFu; // transform lowercase drive letters into uppercase ones
				value -= (static_cast<unsigned int>(L':') << (sizeof(wchar_t) * CHAR_BIT)) | L'A';
				return value < 26;
			}

			static string GetCLSID(const std::wstring_view &path)
			{
				/*
				if(IsCLSID(path))
				{
					auto classid = path.find_last(L'\\', false);
					if(classid && classid[1])
						return classid;
				}
				*/
				if(IsCLSID(path))
				{
					auto p = path.find_last_of(Path::Separator);
					if(p != path.npos)
						return path.substr(p + 1);
				}
				return path;
			}

			static DWORD FileSize(const wchar_t *path)
			{
				auto_handle hFile = ::CreateFileW(path, GENERIC_READ,
												  FILE_SHARE_READ, nullptr, OPEN_EXISTING,
												  FILE_ATTRIBUTE_NORMAL, nullptr);
				return hFile ? ::GetFileSize(hFile, nullptr) : 0;
			}

			static bool IsExe(const std::wstring_view &path)
			{
				auto ext = Path::Extension(path);
				if(!ext.empty())
				{
					const wchar_t* lpszExtensions[6] = { L".exe", L".com", L".cmd",
						L".bat", L".cpl", L".scr" };
					for(auto it: lpszExtensions)
					{
						if(string::Equals(ext.data(), ext.length(), it, 4))
							return true;
					}
				}
				return false;
			}

			static bool IsFile(DWORD dwAttributes)
			{
				return (dwAttributes != INVALID_FILE_ATTRIBUTES && (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)));
			}

			static bool IsDirectory(DWORD dwAttributes)
			{
				return (dwAttributes != INVALID_FILE_ATTRIBUTES && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY));
			}

			static bool IsAttribNameSpace(DWORD dwAttributes)
			{
				return (dwAttributes != INVALID_FILE_ATTRIBUTES && (!(dwAttributes & FILE_ATTRIBUTE_VIRTUAL)));
			}

			// Checks if a file exists (accessible)
			static bool Exists(const std::wstring_view &path)
			{
				auto attr = ::GetFileAttributesW(path.data());
				return IsFile(attr) || IsDirectory(attr);
			}

			// Checks if a file exists (accessible)
			static bool IsFileExists(const std::wstring_view &path)
			{
				auto attr = ::GetFileAttributesW(path.data());
				//return (attr != INVALID_FILE_ATTRIBUTES && (!(attr & FILE_ATTRIBUTE_DIRECTORY)));
				if(attr == INVALID_FILE_ATTRIBUTES)
				{
					auto errval = ::GetLastError();
					return (errval != ERROR_FILE_NOT_FOUND)
						&& (errval != ERROR_PATH_NOT_FOUND)
						&& (errval != ERROR_INVALID_NAME)
						&& (errval != ERROR_INVALID_DRIVE)
						&& (errval != ERROR_NOT_READY)
						&& (errval != ERROR_INVALID_PARAMETER)
						&& (errval != ERROR_BAD_PATHNAME)
						&& (errval != ERROR_BAD_NETPATH);
				}
				return true;
			}

			/*static bool FileExists(const wchar_t* filename)
			{
				if(filename && *filename)
				{
					HANDLE hFile = ::CreateFileW(filename,
												GENERIC_READ,
												FILE_SHARE_READ, nullptr,
												OPEN_EXISTING,
												FILE_ATTRIBUTE_NORMAL, nullptr);
					if(INVALID_HANDLE_VALUE != hFile)
					{
						::CloseHandle(hFile);
						return true;
					}
				}
				return  false;
			}*/

			static bool IsDirectoryExists(const std::wstring_view &path)
			{
				auto attr = ::GetFileAttributesW(path.data());
				return (attr != UINT32_MAX && (attr & FILE_ATTRIBUTE_DIRECTORY));
			}

			static bool Delete(const std::wstring_view &path)
			{
				auto attr = ::GetFileAttributesW(path.data());
				if(IsFile(attr))
					return ::DeleteFileW(path.data()) != 0;
				else if(IsDirectory(attr))
					return ::RemoveDirectoryW(path.data()) != 0;
				return false;
			}

			static bool SafeDeleteFile(const std::wstring_view &path)
			{
				if(::PathFileExistsW(path.data()))
				{
					::SetFileAttributesW(path.data(), FILE_ATTRIBUTE_NORMAL);
					return ::DeleteFileW(path.data());
				}
				return true;
			}

			static bool IsDirectoryEmpty(const std::wstring_view &path)
			{
				auto attr = ::GetFileAttributesW(path.data());
				//if(IsAttribFile(attr))
				//	return GetFileSizeW(pszPath) != 0;
				if(IsDirectory(attr))
				{
					typedef BOOL(WINAPI *PathIsEmpty)(const wchar_t*);
					if(auto pathIsEmpty = DLL::Get<PathIsEmpty>(
						L"Shlwapi.dll", "PathIsDirectoryEmptyW"))
					{
						return pathIsEmpty(path.data());
					}
				}
				return false;
			}
/*
			static string TempDirectory()
			{
				string temp(MAX_PATH);
				auto len =::GetTempPathW(MAX_PATH, temp.buffer());
				return temp.release(len).move();
			}
			*/
			static PathType PathType(const std::wstring_view &path)
			{
				if(path.empty())
					return PathType::Unknown; // none
				else if(IsNameSpace(path))
					return PathType::Namespace; // namespace
				else if(IsRoot(path))
					return PathType::Drive; // root
				else if(IsDirectoryExists(path))
					return PathType::Directory; // dirctory
				else if(IsFileExists(path))
					return PathType::File; // is file
				return PathType::Unknown; // none
			}

			static string Short(const std::wstring_view &path)
			{
				string buf(MAX_PATH);
				auto len =::GetShortPathNameW(path.data(), buf.buffer(), MAX_PATH);
				return buf.release(len).move();
			}

			static string Long(const std::wstring_view &path)
			{
				string r = path;
				auto length = ::GetLongPathNameW(path.data(), nullptr, 0);
				if(length == 0 || length == path.length())
					return path;

				::GetLongPathNameW(path.data(), r.buffer(length), length);
				return r.release(length).move();
			}

			static string Full(const std::wstring_view &path)
			{
				if(path.empty())
					return nullptr;

				string long_path = path;

				auto ccLong = ::GetLongPathNameW(path.data(), nullptr, 0);
				if(ccLong > (path.length() + 1))
				{
					long_path.resize(ccLong);
					ccLong = ::GetLongPathNameW(path.data(), long_path.data(), ccLong);
				}

				auto ccFull = ::GetFullPathNameW(long_path, 0, nullptr, nullptr);
			//	if(ccFull == ccLong)
			//		return long_path.move();

				string full_path(ccFull + 1);
				if(0 == ::GetFullPathNameW(long_path, ccFull, full_path.data(), nullptr))
					return long_path.move();
				return full_path.release(ccFull).move();
			}

			static string RemoveExtension(const std::wstring_view &path)
			{
				auto ext = Path::Extension(path);
				if(!ext.empty())
				{
					return path.substr(0, path.length() - ext.length());
				}
				return path;
			}

			static const string& RemoveBackSlash(string& path)
			{
				return path.trim_end(Path::Separator);
			}

			static const string& AddBackSlash(string &path)
			{
				if(!path.back(Path::Separator, false))
					path.append(Path::Separator);
				return path;
			}

			static int ParseLocation(string &path, string &id)
			{
				int type = 0;
				if(path.length() > 3) 
				{
					intptr_t p = path.last_index_of(L',', false);
					if(p > 3) 
					{
						string id0 = path.substr(p + 1).trim().move();
						path = path.substr(0, p).move();
						if(id0.starts_with(L'-', false)) 
						{
							id0.remove(0, 1);
							p = id0.index_of(L'-', false);
							if(p > 0) 
							{
								id = id0.substr(0, p).move();
								id0.remove(0, p);
								type = 6;
								if(id0.equals(L"-ico"))
									type = 3;
								else if(id0.equals(L"-bmp"))
									type = 4;
								else if(id0.equals(L"-png"))
									type = 5;
							}
							else 
							{
								id = id0;
								type = 2;
							}
						}
						else
						{
							id = id0;
							type = 2;
						}
					}
					else {
						type = 1;
					}
				}
				return type;
			}

			static bool ParseLocation(string &path, int &index)
			{
				bool res = false;
				if(path.length() > 3) 
				{
					auto p = path.last_index_of(L',', false);
					if(p > 0)
					{
						index = (int)string::ToInt(&path.c_str()[p + 1], 0);
						path = path.substr(0, p).move();
						res = true;
					}
					//if(path.Get(1) != L':')
					//	path = Path::Search(path, path.Length());
				}
				return res;
			}

			static int ParseLocation(string &path)
			{
				int result = 0;
				if(path.length() > 3)
				{
					intptr_t p = path.last_index_of(L',', false);
					if(p > 0)
					{
						result = (int)string::ToInt(&path.c_str()[p + 1], 0LL);
						path = path.substr(0, p).move();
					}

					//if(path.Get(1) != L':')
					//	path = Path::Search(path, path.Length());
				}
				return result;
			}

			static string Module(HMODULE handle)
			{
				string path(MAX_PATH);
				auto len = ::GetModuleFileNameW(handle, path.data(), MAX_PATH);
				if(len > MAX_PATH)
				{
					path.capacity(len + 1);
					len = ::GetModuleFileNameW(handle, path.data(), len);
				}
				return path.release(len).move();
			}

			static HMODULE GetCurrentModule()
			{
				return CurrentModule;
			}
		};
	}
}
