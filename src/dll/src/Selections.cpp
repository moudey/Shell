#include <pch.h>

//Enable Narrow Classic Context Menu on Windows 10
// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\FlightedFeatures\ImmersiveContextMenu:0
//Windows Explorer on XP, Vista, 7, 8
//hExplorer = WinGetHandle("[REGEXPCLASS:^(Cabinet|Explore)WClass$]")
//CabinetWClass
//ExploreWClass

//// The taskbar on the second screen
//hWnd_TaskBar_SecondScreen = FindWindowA("Shell_SecondaryTrayWnd", NULL);

namespace Nilesoft
{
	namespace Shell
	{
		void Selections::Clear()
		{
			this->Mode = SelectionMode::None;
			this->Index = 0;
			this->Parent = nullptr;
			this->Front = nullptr;
			this->front = -1;
			this->_first_extension = 0;
			this->_multi_extension = 0;
			this->Background = false;

			for(auto it : Items) delete it;
			this->Items.clear();

			::memset(&Window, false, sizeof(this->Window));
			::memset(&Types, false, sizeof(this->Types));
		}

		void Selections::Add(PathItem *pathItem)
		{
			this->Items.push_back(pathItem);
		}

		void Selections::Add(PathType const &pathType, PathType const &groupType, const string &name)
		{
			this->Items.push_back(new PathItem(pathType, groupType, name));
		}

		Selections::PathItem *Selections::Get(size_t index)
		{
			if(index >= this->Items.size())
				return nullptr;
			return Items[index];
		}

		PathType Selections::Type(size_t index)
		{
			if(auto pItem = this->Get(index); pItem)
				return pItem->Type;
			return PathType::Unknown;
		}

		PathType Selections::GroupType(size_t index)
		{
			if(auto pItem = this->Get(index); pItem)
				return pItem->Group;
			return PathType::Unknown;
		}

		string Selections::Path(size_t index, const string &quote)
		{
			if(auto pItem = this->Get(index); pItem)
				return pItem->get(quote).move();
			return Empty;
		}

		string Selections::Path(const string &quote)
		{
			return this->Path(0, quote).move();
		}

		string Selections::Root(size_t index)
		{
			if(auto pItem = this->Get(index); pItem)
			{
				auto sel = &pItem->Path;
				if(sel->at(1) == L':' && sel->at(2) == L'\\')
					return sel->substr(0, 3).move();;
			}
			return nullptr;
		}

		string Selections::Sel(const string &quote, const wchar_t *sep, const wchar_t *path_sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(path_sep && *path_sep)
					sel.append(pathItem->get(quote).replace(L"\\", path_sep));
				else
					sel.append(pathItem->get(quote));

				sel.append(sep);
			}
			return sel.trim_end(sep).move();
		}

		string Selections::PathsName(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(!pathItem->Name.empty())
				{
					sel.append(pathItem->get_name(quote));
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::FilesName(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == PathType::File and !pathItem->Name.empty())
				{
					sel.append(pathItem->get_name(quote));
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::GetFilesExtension(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == PathType::File and !pathItem->Extension.empty())
				{
					sel.append(pathItem->get_extension(quote));
					sel.append(sep);					
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::FilesNameWithoutExtension(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == PathType::File and !pathItem->Title.empty())
				{
					sel.append(pathItem->get_title(quote));
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::SelFiles(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == IO::PathType::File)
				{
					sel.append(pathItem->get(quote));
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::SelNamespaces(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == IO::PathType::Namespace)
				{
					sel.append(pathItem->get(quote));
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::DiresName(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == PathType::Directory and !pathItem->Name.empty())
				{
					sel.append(pathItem->get_name(quote));
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::SelDirs(const string &quote, const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == IO::PathType::Directory)
				{
					sel.append(pathItem->get(quote));
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::SelDrives(const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				if(pathItem->Type == IO::PathType::Drive)
				{
					sel.append(pathItem->Path);
					sel.append(sep);
				}
			}
			return sel.trim_end(sep).move();
		}

		string Selections::ShortPaths(const wchar_t *sep)
		{
			string sel;
			for(auto pathItem : Items)
			{
				sel.append(Path::Short(pathItem->Path));
				sel.append(sep);
			}
			return sel.trim_end(sep).move();
		}

		string Selections::ShortPath(size_t index)
		{
			if(auto pathItem = this->Get(index); pathItem)
				return Path::Short(pathItem->Path).move();
			return nullptr;
		}

		string Selections::Name(size_t index)
		{
			if(auto pathItem = this->Get(index); pathItem)
				return pathItem->Name;
			return nullptr;
		}

		string Selections::Extension(size_t index)
		{
			if(auto pathItem = this->Get(index); pathItem and pathItem->IsFile())
				return pathItem->Extension;
			return nullptr;
		}

		string Selections::FileNameWithoutExtension(size_t index)
		{
			if(auto pathItem = this->Get(index); pathItem and pathItem->IsFile())
				return pathItem->Title;
			return nullptr;
		}

		bool Selections::is_back(int type) const
		{
			return (type == -1) ? Types[FSO_BACK] : Types[FSO_BACK] == type;
		}
/*
		bool Selections::is_back_root() const
		{
			return Types[FSO_BACK] == FSO_DRIVE;
		}

		bool Selections::is_back_directory() const
		{
			return Types[FSO_BACK] == FSO_DIRECTORY;
		}

		bool Selections::is_back_namespace() const
		{
			return Types[FSO_BACK] == FSO_NAMESPACE;
		}

		bool Selections::is_back_desktop() const
		{
			return Types[FSO_DESKTOP];
		}
*/
		bool Selections::is_desktop_window() const
		{
			return Window.desktop;
		}

		bool Selections::is_taskbar() const
		{
			return Window.id == WINDOW_TASKBAR;
		}

		void Selections::QuerySelectionMode()
		{
			if(Background)
			{
				Mode = SelectionMode::Single;
			}
			else
			{
				auto const &c = count();
				if(c == 0)
					Mode = SelectionMode::None;
				else if(c == 1)
					Mode = SelectionMode::Single;
				else// if(c > 1)
				{
					Mode = SelectionMode::Multiple;

					auto drives = 0u;
					{
						drives += Types[FSO_DRIVE_FIXED];
						drives += Types[FSO_DRIVE_VHD];
						drives += Types[FSO_DRIVE_USB];
						drives += Types[FSO_DRIVE_DVD];
						drives += Types[FSO_DRIVE_REMOVABLE];
						drives += Types[FSO_DRIVE_REMOTE];
					}

					auto namespaces = 0u;
					{
						namespaces += Types[FSO_NAMESPACE_COMPUTER];
						namespaces += Types[FSO_NAMESPACE_RECYCLEBIN];
					}

					auto types = 0u;
					{
						types += Types[FSO_DIRECTORY];
						types += Types[FSO_FILE];
						types += (drives > 0) + (namespaces > 0);
					}

					if(types == 1)
					{
						Mode = SelectionMode::MultiUnique;

						if(Types[FSO_FILE])
							types += _multi_extension > 0;
						else if(Types[FSO_NAMESPACE] || namespaces)
							types += (namespaces == 0) || (namespaces > 1) || Types[FSO_NAMESPACE];
						else if(Types[FSO_DRIVE] || drives)
							types += (drives == 0) || (drives > 1) || Types[FSO_DRIVE];

						if(types > 1)
							Mode = SelectionMode::MultiSingle;
					}
				}
			}
		}

		bool Selections::verify_mode(SelectionMode mode)
		{
			bool result = true;
			if(Types[FSO_TASKBAR]) 
				result = true;
			else if(mode == SelectionMode::Single)
				result = Mode == SelectionMode::Single;
			else if(mode == SelectionMode::MultiSingle)
				result = Mode != SelectionMode::Multiple;
			else if(mode == SelectionMode::MultiUnique)
				result = Mode != SelectionMode::Multiple && Mode != SelectionMode::MultiSingle;
			return result;
		}

		bool Selections::verify_types(const FileSystemObjects &fso) const
		{
			if(fso.any_types)
				return true;

			if(!fso.defined && Count() == 0)
				return false;

			if(Items.empty() && !(Types[FSO_START] + Types[FSO_TASKBAR] + Types[FSO_EDIT] + Types[FSO_TITLEBAR]))
			{
				return false;
			}

			if(fso.count == 0)
				return true;
			
			if(fso.exclude)
			{
				for(int32_t i = 0; i < FSO_SIZE; i++)
				{
					if(Types[i] && fso.Types[i] == FileSystemObjects::EXCLUDE)
						return false;
				}
				return true;
			}

			//taskbar,start,edit,titlebar,
			struct tagFSO
			{
				// sel count == 0
				bool titlebar;
				bool edit;
				bool start;
				bool taskbar;
				// sel count > 0
				bool desktop;
				bool file;
				bool dirctory;
				bool drive;
				bool ns;
			} _fso{};

			int32_t res = 0;
			int32_t drives = 0;
			int32_t namespaces = 0;
			int32_t backs = 0;
			
			_fso.titlebar = Types[FSO_TITLEBAR];
			_fso.edit = Types[FSO_EDIT];
			_fso.start = Types[FSO_START];
			_fso.taskbar = Types[FSO_TASKBAR];
			_fso.desktop = Types[FSO_DESKTOP];
			_fso.file = Types[FSO_DIRECTORY];
			_fso.dirctory = Types[FSO_DIRECTORY];
			_fso.ns = Types[FSO_NAMESPACE];
			_fso.drive = Types[FSO_DRIVE];

			if(fso.Types[FSO_DRIVE] > 0)
			{
				drives += Types[FSO_DRIVE];
				drives += Types[FSO_DRIVE_FIXED];
				drives += Types[FSO_DRIVE_USB];
				drives += Types[FSO_DRIVE_VHD];
				drives += Types[FSO_DRIVE_REMOVABLE];
				drives += Types[FSO_DRIVE_REMOTE];
			}

			if(fso.Types[FSO_NAMESPACE] > 0)
			{
				namespaces += Types[FSO_NAMESPACE];
				namespaces += Types[FSO_NAMESPACE_COMPUTER];
				namespaces += Types[FSO_NAMESPACE_RECYCLEBIN];
			}

			if(Items.size() > 1)
			{
				if(!Background && fso.Types[FSO_COUNT] > 0)
				{
					if(Types[FSO_COUNT] > fso.Types[FSO_COUNT])
					{
						int c = (drives > 0) + (namespaces > 0) + _fso.titlebar + _fso.edit + _fso.start + _fso.taskbar + _fso.desktop + _fso.file + _fso.dirctory;
						if(c > fso.Types[FSO_COUNT] || (drives + namespaces) == 0)
						{
							return false;
						}
					}
				}
			}

			if(Background)
			{
				if(fso.Types[FSO_BACK] > 0)
				{
					backs += Types[FSO_BACK];
					backs += Types[FSO_BACK_DIRECTORY];
					backs += Types[FSO_BACK_DRIVE];
					backs += Types[FSO_BACK_DRIVE_FIXED];
					backs += Types[FSO_BACK_DRIVE_USB];
					backs += Types[FSO_BACK_DRIVE_VHD];
					backs += Types[FSO_BACK_DRIVE_REMOVABLE];
					backs += Types[FSO_BACK_DRIVE_REMOTE];
					backs += Types[FSO_BACK_NAMESPACE];
					backs += Types[FSO_BACK_NAMESPACE_COMPUTER];
					backs += Types[FSO_BACK_NAMESPACE_RECYCLEBIN];
				}

				if(fso.Types[FSO_BACK_DRIVE] > 0)
				{
					backs += Types[FSO_BACK_DRIVE];
					backs += Types[FSO_BACK_DRIVE_FIXED];
					backs += Types[FSO_BACK_DRIVE_USB];
					backs += Types[FSO_BACK_DRIVE_VHD];
					backs += Types[FSO_BACK_DRIVE_REMOVABLE];
					backs += Types[FSO_BACK_DRIVE_REMOTE];
				}

				if(fso.Types[FSO_BACK_NAMESPACE] > 0)
				{
					backs += Types[FSO_BACK_NAMESPACE];
					backs += Types[FSO_BACK_NAMESPACE_COMPUTER];
					backs += Types[FSO_BACK_NAMESPACE_RECYCLEBIN];
				}
			}

			res += drives;
			res += namespaces;
			res += backs;

			if(res == 0)
			{
				for(int32_t i = 0; i < FSO_SIZE; i++)
					res += (fso.Types[i] > 0) && (Types[i] > 0);
			}

			return res > 0;
		}

		bool Selections::Preparing()
		{
			if(!Items.empty())
			{
				Front = Items[0];

				QuerySelectionMode();

				if(Front->IsNamespace())
				{
					this->front = FSO_NAMESPACE;
					if(Front->Raw.contains(L"Microsoft.Base.DevQueryObjects"))
						return false;
				}
				else if(Front->IsRoot())
					this->front = FSO_DRIVE;
				else if(Front->IsFile())
					this->front = FSO_FILE;
				else if(Front->IsDirectory())
					this->front = FSO_DIRECTORY;
				else if(Front->IsDVD())
					this->front = FSO_DRIVE_DVD;
				else if(Front->IsUSB())
					this->front = FSO_DRIVE_USB;
				else if(Front->IsVHD())
					this->front = FSO_DRIVE_VHD;
				else if(Front->IsRemovable())
					this->front = FSO_DRIVE_REMOVABLE;
				else if(Front->IsFixed())
					this->front = FSO_DRIVE_FIXED;
				else
					this->front = uint32_t(-1);

				auto parent_hash = ParentRaw.hash();

				if(parent_hash == NS_RECYCLE)
					Window.id = WINDOW_RECYCLEBIN;
				else if(parent_hash == NS_COMPUTER)
					Window.id = WINDOW_COMPUTER;
				else if(parent_hash == NS_LIBRARIES)
					Window.id = WINDOW_LIBRARIES;

				if(Parent.empty() || Window.explorer_tree)
				{
					if(Path::IsDesktop(Front->Path))
					{
						Types[FSO_DESKTOP] = true;
						Types[FSO_DIRECTORY] = false;
						Types[FSO_BACK_DIRECTORY] = false;
					}
				}
				//FOLDERID_Desktop
				//search-ms:
				else if(Parent.starts_with(L"search-ms:"))
				{
					Parent = Path::Parent(Front->Path).move();
					ParentRaw = Path::Parent(Front->Raw).move();
				}

				/*if(Background)
				{
					if(ParentRaw.starts_with(L"::{031E4825-7B94-4DC3-B131-E946B44C8DD5}"))
					{
						Window.id = WINDOW_LIBRARIES;
					}
				}*/

				if(this->Background || Types[FSO_BACK] || Types[FSO_DESKTOP])
					Directory = Front->Path;
				else
					Directory = Parent;

				::SetCurrentDirectoryW(Directory);
			}

			Types[FSO_BACK] = this->Background;

			//string s;
			for(int32_t i = 0; i < FSO_BACK; i++)
			{
				Types[FSO_COUNT] += Types[i];
				//if(Types[i])	s.append_format(L"%s\n", fs_types[i].name);
			}

			return true;
		}

		bool Selections::Parse(IShellItem *shellItem)
		{
			FileProperties fprop;
			if(Selections::GetFileProperties(shellItem, &fprop))
				return Parse(&fprop);
			return false;
		}

		bool Selections::Parse(FileProperties *prop)
		{
			std::unique_ptr<Selections::PathItem> pathItem;
			try
			{
				auto path = &prop->PathRaw;
				if(path->length() < 3)
					return false;

				pathItem.reset(new Selections::PathItem(prop->Path, prop->PathRaw,
														 prop->ReadOnly, prop->Hidden));
				pathItem->IsLink = prop->Link;

				if(path->length() == 3)
				{
					pathItem->ReadOnly = true;
					pathItem->Type = PathType::Drive;
					pathItem->Group = PathType::Drive;

					auto dtype = ::GetDriveTypeW(path->c_str());

					if(dtype == DRIVE_NO_ROOT_DIR)
						goto error;
					else if(dtype == DRIVE_FIXED)
					{
						pathItem->Type = PathType::Fixed;
						if(Background)
							Types[FSO_BACK_DRIVE_FIXED] = true;
						else
							Types[FSO_DRIVE_FIXED] = true;
					}
					else if(dtype == DRIVE_CDROM)
					{
						pathItem->Type = PathType::DVD;
						if(Background)
							Types[FSO_BACK_DRIVE_DVD] = true;
						else
							Types[FSO_DRIVE_DVD] = true;
					}
					else if(dtype == DRIVE_REMOTE)
					{
						pathItem->Type = PathType::Remote;
						if(Background)
							Types[FSO_BACK_DRIVE_REMOTE] = true;
						else
							Types[FSO_DRIVE_REMOTE] = true;
					}
					else if(dtype == DRIVE_REMOVABLE)
					{
						int busType = IO::GetTypeOfDrive(*path);
						switch(busType)
						{
							case BusTypeVirtual:
							case BusTypeFileBackedVirtual:
							{
								pathItem->Type = PathType::VHD;

								if(Background)
								{
									Types[FSO_BACK_DRIVE_VHD] = true;
								}
								else
								{
									Types[FSO_DRIVE_VHD] = true;
								}
								break;
							}
							case BusTypeUsb:
							{
								pathItem->Type = PathType::USB;
								if(Background)
									Types[FSO_BACK_DRIVE_USB] = true;
								else
									Types[FSO_DRIVE_USB] = true;
								break;
							}
							default:
							{
								pathItem->Type = PathType::Removable;
								if(Background)
									Types[FSO_BACK_DRIVE_REMOVABLE] = true;
								else
									Types[FSO_DRIVE_REMOVABLE] = true;
							}
							break;
						}
					}
					else
					{
						if(Background)
							Types[FSO_BACK_DRIVE] = true;
						else
							Types[FSO_DRIVE] = true;
					}
					count.DRIVE++;
				}
				/*else if(prop->Folder && prop->Removable)
				{
					Types[FSO_REMOVABLE]++;
					if(isBackground)
					{
						Types[FSO_BACKG] = FSO_BACK_DRIVE;
					}
				}*/
				else if(prop->IsNamespace)
				{
					pathItem->ReadOnly = true;
					pathItem->Group = pathItem->Type = PathType::Namespace;

					auto hash = path->hash();

					if(hash == NS_COMPUTER)
						Types[FSO_NAMESPACE_COMPUTER] = true;
					else if(hash == NS_RECYCLE)
						Types[FSO_NAMESPACE_RECYCLEBIN] = true;
					else
					{
						if(this->Background)
							Types[FSO_BACK_NAMESPACE] = true;
						else
							Types[FSO_NAMESPACE] = true;
					}
					count.NAMESPACE++;
				}
				else if(prop->IsDir)
				{
					pathItem->Group = pathItem->Type = PathType::Directory;
					if(this->Background)
					{
						if(Window.desktop)
						{
							pathItem->ReadOnly = true;
							pathItem->Type = PathType::Desktop;
							Types[FSO_DESKTOP] = true;
						}
						else
						{
							Types[FSO_BACK_DIRECTORY] = true;
						}
					}
					else
					{
						Types[FSO_DIRECTORY] = true;
					}
					count.DIRECTORY++;
				}
				else if(prop->IsFile)
				{
					uint32_t hash = 0;
					pathItem->Group = pathItem->Type = PathType::File;
					if(pathItem->Name.last_index_of(L'.', false) != 0)
					{
						pathItem->Extension = pathItem->Name.find_last(L'.', false);
					}
					hash = Hash::Generate(pathItem->Extension);
					if(Types[FSO_FILE] == 0)
						_first_extension = hash;
					else if(_first_extension != hash)
						_multi_extension++;

					Types[FSO_FILE] = true;
					count.FILE++;
				}
				else
				{
					goto error;
				}

				Items.push_back(pathItem.release());
				return true;
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
		error:
			return false;
		}

		bool Selections::GetFileProperties(IShellItem *shellItem, FileProperties *prop)
		{
			bool res = false;
			wchar_t *path_raw = nullptr;
			if(shellItem && S_OK == shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path_raw))
			{
				if(path_raw)
				{
					prop->PathRaw = path_raw;
					::CoTaskMemFree(path_raw);
				}

				SFGAOF attr{};//SHCIDS_ALLFIELDS
				shellItem->GetAttributes(FGAOF_DEF, &attr);

				prop->Folder = (attr & SFGAO_FOLDER);
				prop->FileSystem = (attr & SFGAO_FILESYSTEM);
				prop->Removable = (attr & SFGAO_REMOVABLE);
				prop->FileSysAnceStor = (attr & SFGAO_FILESYSANCESTOR);
				prop->Stream = (attr & SFGAO_STREAM);
				prop->Link = (attr & SFGAO_LINK);
				prop->DropTarget = (attr & SFGAO_DROPTARGET);
				prop->HasStorage = (attr & SFGAO_HASSTORAGE);
				prop->CanMoniker = (attr & SFGAO_CANMONIKER);
				prop->ReadOnly = (attr & SFGAO_READONLY);
				prop->Hidden = (attr & SFGAO_HIDDEN);
				prop->Attr = attr;

				wchar_t *path = nullptr;
				if(prop->FileSystem)
				{
					if(S_OK == shellItem->GetDisplayName(SIGDN_FILESYSPATH, &path))
					{
						if(path)
						{
							prop->Path = path;
							::CoTaskMemFree(path);
						}
					}
				}
				else if(S_OK == shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEEDITING, &path))
				{
					if(path)
					{
						prop->Path = path;
						::CoTaskMemFree(path);
					}
				}

				if(prop->Stream)
				{
					bool f = prop->Stream && prop->HasStorage && prop->CanMoniker;
					prop->IsFile = prop->FileSystem || (SFGAO_STREAM == attr) || f;
				}
				else if(prop->Folder)
				{
					if(prop->FileSystem)
						prop->IsDir = TRUE;
					else
					{
						prop->IsNamespace = TRUE;
					}
					/*else if(prop->PathRaw.StartsWith(L"::{", 3, false))
					{
						prop->IsNamespace = TRUE;//(SFGAO_FOLDER == attr);
					}
					else
					{
						prop->IsDir = TRUE;
					}*/
				}
				else
				{
					if(!prop->PathRaw.starts_with(L"::{", 3, false))
						return false;
					prop->IsNamespace = TRUE;
				}
				res = true;
			}
			return res;
		}

		// Get IShellBrowser object interface associated an hwnd
		IShellBrowser *GetIShellBrowser(HWND hWnd, bool checked = true)
		{
			IShellBrowser *result{};
			__try
			{
				auto lres = ::SendMessageW(hWnd, WM_GETISHELLBROWSER, 0, 0);
				if(!lres && !checked)
					result = reinterpret_cast<IShellBrowser *>(::GetWindowLongPtrW(hWnd, DWLP_MSGRESULT));
				else if(lres)
				{
					if(lres == TRUE)
						result = reinterpret_cast<IShellBrowser *>(::GetWindowLongPtrW(hWnd, DWLP_MSGRESULT));
					else
						result = reinterpret_cast<IShellBrowser *>(lres);
				}
				
				//if(result) result->GetWindow(&hWnd);
			}
			except
			{
				result = nullptr;
			}
			return result;
		}

		bool Selections::QuerySelected()
		{
			try
			{
				if(Window.handle == nullptr)
					return false;

				/*
				IComPtr<IShellWindows> sw;	
				//if(S_OK == ::CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER, __uuidof(IShellWindows), sw))
				if(sw.CreateInstance(CLSID_ShellWindows))
				{
					IComPtr<IShellBrowser> sb;
					HWND current_window = nullptr;
					long sb_count = 0;

					if((S_OK != sw->get_Count(&sb_count)) || sb_count == 0)
					{
						if(!Window.desktop)
							return false;
					}

					if(Window.desktop)
					{
						IComPtr<IDispatch> disp;
						// Get the desktop shell window
						VARIANT v = { };
						if(S_OK != sw->FindWindowSW(&v, &v,
													SWC_DESKTOP, (long *)&current_window,
													SWFO_NEEDDISPATCH, disp))
						{
							return false;
						}

						//current_window = WC_Pogrman
						if(!disp.QueryServiceProvider<IShellBrowser>(SID_STopLevelBrowser, sb))
							return false;
					}
					else
					{
						bool found_window = false;
						VARIANT i = { }; i.vt = VT_I4;
						for(i.lVal = 0; i.lVal < sb_count; i.lVal++)
						{
							IComPtr<IDispatch> disp;
							if(S_OK != sw->Item(i, disp))
								continue;

							if(!disp.QueryServiceProvider<IShellBrowser>(SID_STopLevelBrowser, sb))
								continue;
							
							if(S_OK != sb->GetWindow(&current_window))
								continue;

							//current_window == WC_ShellTabWindowClass
							auto hwnd = Window.handle;
							while(hwnd)
							{
								MB(hwnd.class_name());
								if(current_window == hwnd.get())
								{
									found_window = true;
									break;
								}
								hwnd = hwnd.parent();
							}

							if(found_window)
								break;
						}

						if(!found_window)
							return false;
					}

					ShellBrowser = sb.set_release(0);
				}
				*/

				HWND current_window{};
				
				if(!Window.has_IShellBrowser)
					return false;

				if(Window.desktop)
				{
					// WorkerW, Progman, GetShellWindow
					current_window = Window.handle.parent();

					// Get IShellBrowser interface for current hWnd
					ShellBrowser = GetIShellBrowser(current_window, false);
					if(ShellBrowser == nullptr)
					{
						current_window = ::GetShellWindow();
						ShellBrowser = GetIShellBrowser(current_window, false);
					}
				}
				else if(Window.id >= WINDOW_DESKTOP)
				{
					// ShellTabWindowClass, SysTreeView32
					current_window = Window.handle;
					while(current_window)
					{
						// Get IShellBrowser interface for current hWnd
						ShellBrowser = GetIShellBrowser(current_window, true);
						if(ShellBrowser) break;
						current_window = ::GetParent(current_window);
					}
				}

			//	if(ShellBrowser)
			//		ShellBrowser->AddRef();
				

			//	Logger::Info(L"%x %s %x", current_window, Window::class_name(current_window).c_str(), sb);

				if(ShellBrowser == nullptr)
					return false;

				if(Window.id == WINDOW_EXPLORER)
				{
					IComPtr<IShellView> sv;

					if(S_OK != ShellBrowser->QueryActiveShellView(sv))
						return false;

					IComPtr<IFolderView2> fv;
					if(S_OK != sv->QueryInterface(IID_IFolderView2, fv))
						return false;

					IComPtr<IShellItemArray> siaFolder;
					if(S_OK != fv->GetFolder(IID_IShellItemArray, siaFolder))
						return false;

					IComPtr<IShellItem2> si;
					if(S_OK != siaFolder->GetItemAt(0, si))
						return false;

					FileProperties folderProp;
					Selections::GetFileProperties(si, &folderProp);
					
					IComPtr<IShellItemArray> sia;
					if(S_OK == fv->GetSelection(FALSE, sia))
					{
						DWORD sel_count = 0;
						if(S_OK != sia->GetCount(&sel_count))
							return false;

						Items.reserve(sel_count);

						for(DWORD i = 0; i < sel_count; i++)
						{
							IComPtr<IShellItem> item;
							if(S_OK == sia->GetItemAt(i, item) && item)
								Parse(item);
						}

						Parent = folderProp.Path;
						ParentRaw = folderProp.PathRaw;

						return !Items.empty();
					}
					
					if(folderProp.Folder)
					{
						if(folderProp.FileSystem || folderProp.FileSysAnceStor)
							folderProp.Background = TRUE;
						else
							folderProp.Background = folderProp.DropTarget;

						if(Items.empty() && !folderProp.Background)
						{
							auto h = folderProp.PathRaw.hash();
							if(h == GUID_HOME or h == GUID_QUICK_ACCESS or h == GUID_LIBRARIES)
							{
								Window.id = (h == GUID_HOME) ? WINDOW_HOME : (h == GUID_QUICK_ACCESS) ? WINDOW_QUICK_ACCESS : WINDOW_LIBRARIES;
								folderProp.Background = TRUE;
							}
						}
					}

					if(Window.desktop)
					{
						Parent = Path::Parent(folderProp.Path).move();
						ParentRaw = Path::Parent(folderProp.PathRaw).move();
					}
					else
					{
						IComPtr<IShellItem> sip;
						if(S_OK == si->GetParent(sip))
						{
							FileProperties fp_parent;
							if(Selections::GetFileProperties(sip, &fp_parent))
							{
								//if(fpParent.IsDir)
								Parent = fp_parent.Path.move();
								ParentRaw = fp_parent.PathRaw.move();
							}
						}
					}

					if(folderProp.Background)
					{
						this->Background = true;
						this->Parse(&folderProp);
						//return true;
					}
					return true;// add all ui
				}
				else if(Window.explorer_tree)
				{
					IComPtr<INameSpaceTreeControl> nstc;
					IComPtr<IServiceProvider> sp;
					
					TResult hr = ShellBrowser->QueryInterface(IID_IServiceProvider, sp);
					if(!hr.succeeded())
						return false;

					hr = sp->QueryService(SID_SNavigationPane, IID_INameSpaceTreeControl, nstc);
					if(!hr.succeeded())
						return false;

					this->Background = false;

					//POINT pt {};
					//::GetCursorPos(&pt);
					//::ScreenToClient(Window.handle, &pt);
					//auto pos = ::GetMessagePos();
					//pt = { GET_X_LPARAM(pos),GET_Y_LPARAM(pos) };

					auto pt = point;
					::ScreenToClient(Window.handle, &pt);
					IComPtr<IShellItem> si;
					if(S_OK != nstc->HitTest(&pt, si))
					{
						//if(S_OK != nstc->HitTest(&point, si))
						{
							/*if(Windows::Version::Instance().IsWindows11OrGreater())
							{
								if(S_OK == nstc->GetSelectedItems(sia))
								{
									DWORD count = 0;
									if(S_OK == sia->GetCount(&count) && count > 0)
									{
										sia->GetItemAt(0, si);
									}
								}
							}*/
						}
						Window.id = WINDOW_UI;
					}

					if(si)
					{
						auto ret = Parse(si);
						IComPtr<IShellItem> sip;
						if(S_OK == si->GetParent(sip))
						{
							FileProperties fp_parent;
							if(Selections::GetFileProperties(sip, &fp_parent))
							{
								Parent = fp_parent.Path.move();
								ParentRaw = fp_parent.PathRaw.move();
							}
						}

						if(ret)
							return ret;
					}
					Window.id = WINDOW_UI;
				}
			}
			catch(...)
			{
				ShellBrowser = nullptr;
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return false;
		}

		/*
TrayShowDesktopButtonWClass
NotificationsMenuOwner // Notification Button
LauncherTipWnd // Win+X menu
MultitaskingViewFrame // Multitask Button

		*/
		bool Selections::QueryShellWindow()
		{
			auto window = &Window.handle;
			try
			{
			//	if(!*window)
			//		*window = ::GetActiveWindow();
			
			//	if(!window->is_window() || !window->is_visible())
			//		return false;
				Hash cur_hash(window->class_hash());
				//MB(window->class_name());

				if(!cur_hash) return false;
				// Edit >> ComboBox >> ComboBoxEx32 >> #32770 >> Notepad
				// Edit >> ComboBox >> ComboBoxEx32 >> msctls_progress32 >> Address Band Root >> ReBarWindow32 >> WorkerW >> #32770 >> Notepad
				// Breadcrumb Parent >> msctls_progress32 >> Address Band Root >> ReBarWindow32 >> WorkerW >> #32770 >> Notepad
				// WorkerW >> #32770 >> Notepad
				/*
ComboBox > #32770 > Notepad
ComboBox > #32770 > Notepad
msctls_progress32 > #32770 > Notepad
CtrlNotifySink > #32770 > Notepad

DirectUIHWND >> SHELLDLL_DefView > #32770 > Notepad
Edit >> ComboBox > #32770 > Notepad
Breadcrumb Parent >> msctls_progress32 > #32770 > Notepad
Edit >> ComboBox > #32770 > Notepad
				*/

				Window.hash = cur_hash;

				auto hParent = window->parent();
				Hash cur_Parent = hParent.class_hash();

				if(cur_hash.equals({ WC_SHELLDLL_DefView, WC_SysListView32, WC__STATIC, WC_ShellTabWindowClass/*, WC_CabinetWClass, WC_ExplorerWClass*//*, WC_DirectUIHWND*/ }))
				{
					Window.id = WINDOW_EXPLORER;
					Window.explorer = true;

					if(hParent)
					{
						if(cur_Parent.equals({ WC_Progman, WC_WorkerW }))
							Window.desktop = true;
					}
					else if(cur_hash.equals(WC__STATIC))
						Window.id = WINDOW_UI;
					else
						return false;

					Window.has_IShellBrowser = true;

					return true;
				}
				else if(cur_hash.equals(WC_SysTreeView32))
				{
					if(!cur_Parent.equals(WC_NamespaceTreeControl))
					{
						auto active = window->ActiveWindow();
						auto activeParent = active.parent();

						if(!activeParent)
							return false;

						Hash active_hash = active.class_hash();
						if(!active_hash.equals({ WC_CabinetWClass, WC_ExplorerWClass, WC_Progman, WC_WorkerW }))
						{
							return false;
						}
					}
					Window.has_IShellBrowser = true;
					Window.explorer_tree = true;
					Window.id = WINDOW_EXPLORER_TREE;
					return true;
				}
				else if(cur_hash.equals({ WC__EDIT, WC__SEARCHEDITBOXFAKEWINDOW }))
				{
					Window.id = WINDOW_EDIT;
					Types[FSO_EDIT] = 1;
					Types[FSO_COUNT]++;
					return true;
				}
				else
				{
					Hash root_owner_hash;
					bool from_root_owner = false;
					bool from_taskbar = cur_hash.equals({ WC_Shell_TrayWnd, WC_Shell_SecondaryTrayWnd,
										WC_START, WC_LauncherTipWnd,
										WC_PNIHiddenWnd,  WC__BUTTON, WC_TrayNotifyWnd, WC_TrayShowDesktopButtonWClass });
					if(!from_taskbar)
					{
						if(auto hRoot_owner = window->ancestor(); hRoot_owner)
						{
							root_owner_hash = hRoot_owner.class_hash();
							from_root_owner = root_owner_hash.equals({ WC_Shell_TrayWnd, WC_Shell_SecondaryTrayWnd });
						}
					}

					if(from_taskbar or from_root_owner)
					{
						Window.id = WINDOW_TASKBAR;
						if(cur_hash.equals({ WC_START, WC_LauncherTipWnd }))
						{
							Window.id = WINDOW_START;
							Types[FSO_START] = 1;
							Types[FSO_COUNT]++;
						}
						else if(from_root_owner)
							Window.hash = root_owner_hash;
						Window.parent = WINDOW_TASKBAR;
						return true;
					}

					Module module(L"explorerframe.dll");
					if(Window.hInstance == module)
					{
						if(hParent)
						{
							// system menu
							if(cur_hash.equals(WC_ReBarWindow32) && cur_Parent.equals(WC_WorkerW))
							{
								Window.id = WINDOW_SYSMENU;
								Types[FSO_TITLEBAR] = 1;
								Types[FSO_COUNT]++;
								return true;
							}

							if(cur_hash.equals(WC_WorkerW) && cur_Parent.equals(WC_CabinetWClass))
							{
								Window.id = WINDOW_SYSMENU;
								Types[FSO_TITLEBAR] = 1;
								Types[FSO_COUNT]++;
								return true;
							}

							// toolbar
							//if(cur_hash.equals(WC_TravelBand) && cur_Parent.equals(WC_ReBarWindow32))
							//	return false;
						}
						else if(cur_hash.equals(WC_CabinetWClass))
						{
							Window.id = WINDOW_SYSMENU;
							Types[FSO_TITLEBAR] = 1;
							Types[FSO_COUNT]++;
							return true;
						}
					}
					
					module.from(L"shcore.dll");

					if(Window.hInstance == module)
					{
						Window.id = WINDOW_SYSMENU;
						Types[FSO_TITLEBAR] = 1;
						Types[FSO_COUNT]++;
						return true;
					}

					//if(!cur.equals(WC_CabinetWClass))
					if(loader.contextmenuhandler)
						goto ui;

					if(loader.explorer)
						goto ui;

					auto &wnd = Window.handle;
					while(wnd)
					{
						if(WC_DLGBOX == wnd.class_hash())
							goto ui;
						wnd = wnd.parent();
					}
				}

				return false;
			ui:
				Window.id = WINDOW_UI;
				return true;
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return false;
		}
	}
}