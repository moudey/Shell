#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		constexpr auto NS_COMPUTER = 0xD4FD60C2U;		//::{20D04FE0-3AEA-1069-A2D8-08002B30309D}
		constexpr auto NS_RECYCLE = 0xC8989B8EU;		//::{645FF040-5081-101B-9F08-00AA002F954E}
		constexpr auto NS_CONTROL = 0x0AC95438U;		//::{21EC2020-3AEA-1069-A2DD-08002B30309D}
		constexpr auto NS_NETWORK = 0xFF5E5EDEU;		//::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}
		constexpr auto NS_NETWORKCENTER = 0x3E01EAF8U;	//::{8E908FC9-BECC-40F6-915B-F4CA0E70D03D}
		constexpr auto NS_UINSTALL = 0x207A7697U;		//::{7B81BE6A-CE2B-4676-A29E-EB907A5126C5}
		constexpr auto NS_SYSTEM = 0xE738C518U;			//::{BB06C0E4-D293-4F75-8A90-CB05B6477EEE}
		constexpr auto NS_QUICKACCESS = 0xDB6BDBF5U;	//::{679F85CB-0220-4080-B29B-5540CC05AAB6}
		constexpr auto NS_LIBRARIES = 0x214BFAA6U;		//::{031E4825-7B94-4DC3-B131-E946B44C8DD5}
														//::{031E4825-7B94-4DC3-B131-E946B44C8DD5}\CameraRoll.library-ms
/*

	wnd cls=‘ShellTabWindowClass’ title=‘title*’
	wnd aaname=‘Explorer Pane’ cls=‘DirectUIHWND’
	wnd aaname=‘Shell Folder View’ cls=‘SHELLDLL_DefView’ title=‘ShellView’
	wnd aaname=‘Items View’ cls=‘DirectUIHWND’

*/
//Windows Explorer on XP, Vista, 7, 8
//hExplorer = WinGetHandle("[REGEXPCLASS:^(Cabinet|Explore)WClass$]")
//CabinetWClass
//ExploreWClass
		enum WindowClass : uint32_t
		{
			WC_NONE = 0,
			WC_APPLICATION = 0x00000001,
			WC_WorkerW = 0xD749CBB6,	//L"WorkerW"
			WC_Progman = 0xC3394119,	//L"Progman"
			//taskbar////
			WC_Shell_TrayWnd = 0x0CF03CA5,	//taskbar
			WC_Shell_SecondaryTrayWnd = 0x3861CCEDU, // Shell_SecondaryTrayWnd
			WC_MSTASKLISTWCLASS = 0x8BFA5681U, // taskbar
			WC_TrayNotifyWnd = 0xFD59B287U,
			
			WC__EDIT = 0x7C96292BU,
			WC__SEARCHEDITBOXFAKEWINDOW = 0xAF224CF8U, // _SearchEditBoxFakeWindow
			WC_BREADCRUMB = 0xDE0145C6U, // Breadcrumb Parent

			WC__STATIC = 0x1C8A8BADU,//STATIC
			WC__BUTTON = 0xF4FF5D81U,
			WC_START = 0x106149D3U,
			WC_LauncherTipWnd = 0x41DE6ECDU, //// Win+X menu
			WC_TrayShowDesktopButtonWClass = 0x27548EC9U,
			WC_PNIHiddenWnd = 0xF53ADFC1U,
			WC_DLGBOX = 0x5D3AD0ABU,	// #32770 WC_DIALOG
			/////////////
			WC_SHELLDLL_DefView = 0xB434F7C2,	//L"SHELLDLL_DefView"
			WC_SysTreeView32 = 0x84265DD4,	//L"SysTreeView32" 49431
			WC_CabinetWClass = 0xA26CA168,	//L"CabinetWClass" 49772
			WC_ExplorerWClass = 0x891808A3,	//L"ExplorerWClass"
			WC_SysListView32 = 0xB9B50F60,	//L"SysListView32" 49414
			WC_NamespaceTreeControl = 0x4683C2C3,	//L"NamespaceTreeControl"
			WC_ApplicationManager = 0x9C10E7BD,//L"ApplicationManager_DesktopShellWindow"
			WC_ShellTabWindowClass = 0x7B66A982U,//ShellTabWindowClass
			//Windows 11
			WC_Windows_UI_Composition_DesktopWindowContentBridge = 0x8122A1C6U,	//Windows.UI.Composition.DesktopWindowContentBridge 49992
			WC_WINDOWS_UI_INPUT_INPUTSITE_WINDOWCLASS = 0x86865AE9U, //Windows.UI.Input.InputSite.WindowClass
			WC_XAML_WINDOWEDPOPUPCLASS = 0x8CB7F0C1U, // Xaml_WindowedPopupClas
			// UI
			WC_DirectUIHWND = 0x43F2554FU,
			WC_DUIViewWndClassName = 0xCD1A7C82U,
			WC_ReBarWindow32 = 0xF75EE10EU,
			WC_TravelBand = 0xDD9E6528U,
			// block window
			WC_BLOCK_IDM_DOWNLOAD_BUTTON_CLASS = 0xC4A83BC9U,
			WC_MSTASKSWWCLASS = 0x2C50536FU,
			WC_TRAYNOTIFYWND = WC_TrayNotifyWnd
		};
		//DirectUIHWND
		//DUIViewWndClassName
		//ShellTabWindowClass
		enum class SelectionMode : int8_t
		{
			None,
			Single,
			MultiUnique,
			MultiSingle,
			Multiple
		};

		enum WINDOW_tag : int32_t
		{
			WINDOW_NONE,
			WINDOW_UI,
			WINDOW_SYSMENU,
			WINDOW_EDIT,
			WINDOW_START,
			WINDOW_TASKBAR,
			WINDOW_DESKTOP,
			WINDOW_EXPLORER,
			WINDOW_EXPLORER_TREE,
			WINDOW_COMPUTER,
			WINDOW_RECYCLEBIN,
			WINDOW_LIBRARIES,
			WINDOW_HOME,
			WINDOW_QUICK_ACCESS
		};

		struct WINDOW
		{
			HMODULE hInstance{};
			Windows::Window handle{};
			uint32_t hash = 0;
			uint32_t id = 0;
			uint32_t parent = 0;
			uint32_t front = 0;
			bool desktop{};
			bool explorer{};
			bool explorer_tree{};
			bool application{};
			bool has_IShellBrowser{};

			void copy(const WINDOW &src)
			{
				::memcpy(this, &src, sizeof(src));
			}

			bool isTaskbar() const
			{
				return id == WINDOW_TASKBAR;
			}
		};

		class Selections
		{
		public:
			struct PathItem
			{
				PathType	Type;
				PathType	Group;
				string		Path;
				string		Raw;
				string		Name;
				string		Title;
				string		Extension;
				bool		ReadOnly = false;
				bool		Hidden = false;
				bool		IsLink = false;

				PathItem() noexcept
					: Type{ PathType::Unknown }, Group{ PathType::Unknown }
				{
				}

				PathItem(const string &path, const string &pathRaw, bool readOnly, bool hidden)
					: Path{ path }, Raw{ pathRaw }, Type{ PathType::Unknown }, Group{ PathType::Unknown },
					ReadOnly{ readOnly }, Hidden{ hidden }
				{
					if(!path.empty())
					{
						Name = Path::Name(path).move();
						Title = Path::Title(path).move();
					}
				}

				PathItem(PathType const &type, PathType const &groupType, const string &path = nullptr)
					: Path{ path }, Type{ type }, Group{ groupType }
				{
					if(!path.empty())
					{
						Name = Path::Name(path).move();
						Title = Path::Title(Name).move();
					}
				}

				~PathItem() {}

				string get(const string &quote = nullptr, const wchar_t* sep = nullptr) const
				{
					string result = Path;
					if(sep) result.replace(L"\\", sep);
					return !quote.empty() ? Quote(result, quote).move() : result.move();
				}

				string get_raw(const string &quote) const
				{
					return !quote.empty() ? Quote(Raw).move() : Raw;
				}

				string get_name(const string &quote) const
				{
					return !quote.empty() ? Quote(Name).move() : Name;
				}

				string get_title(const string &quote) const
				{
					return !quote.empty() ? Quote(Title).move() : Title;
				}

				string get_extension(const string &quote) const
				{
					return !quote.empty() ? Quote(Extension).move() : Extension;
				}

				bool IsDesktop() const { return this->Type == PathType::Desktop; }
				bool IsFile() const { return this->Group == PathType::File; }
				bool IsRoot() const { return this->Type == IO::PathType::Drive; }
				bool IsDirectory() const { return this->Group == PathType::Directory; }
				bool IsNamespace() const { return this->Group == PathType::Namespace; }
				bool IsDVD() const { return this->Type == PathType::DVD; }
				bool IsUSB() const { return this->Type == PathType::USB; }
				bool IsFixed() const { return this->Type == PathType::Fixed; }
				bool IsVHD() const { return this->Type == PathType::VHD; }
				bool IsRemovable() const { return this->Type == PathType::Removable; }
			};

			const wchar_t *Empty = L"";

			std::vector<PathItem*>	Items;

			PathItem		*Front{};
			string			Parent;
			string			ParentRaw;
			string			Directory;
			SelectionMode	Mode = SelectionMode::None;
			size_t			Index = 0U;
			int32_t			Types[FSO_MAX]{ };
			int32_t			front = -1;
			bool			Background{};
			WINDOW			Window{};
			bool			destroy = true;
			IShellBrowser*	ShellBrowser{};

			IShellView *ShellView{};
			IFolderView2 *FolderView{};

			struct
			{
				bool		explorer{};
				bool		taskbar{};
				bool		apps{};
				bool		contextmenuhandler{};
			}loader;

			struct {
				uint32_t FILE = 0;
				uint32_t DIRECTORY = 0;
				uint32_t DRIVE = 0;
				uint32_t NAMESPACE = 0;
				uint32_t operator()() const { return FILE + DIRECTORY + DRIVE + NAMESPACE; }
			}count{};

		private:
			uint32_t _first_extension = 0U;
			uint32_t _multi_extension = 0U;

		public:
			Selections(HWND handle = nullptr)
			{
				Window.handle = handle;
			}

			// destructor cleans up all resources
			~Selections()
			{
				this->Clear();

				//if(ShellBrowser)
				//	ShellBrowser->Release();

				if(ShellView)
					ShellView->Release();

				if(FolderView)
					FolderView->Release();

				ShellBrowser = {};
				ShellView = {};
				FolderView = {};
			}

			Selections(const Selections &other) = delete;
			Selections &operator=(const Selections &other) = delete;

			void Clear();
			void Add(PathItem *pathItem);
			void Add(PathType const &pathType, PathType const &groupType, const string &name);
			
			PathItem *Get(size_t index);
			PathType Type(size_t index = 0);
			PathType GroupType(size_t index);
			string Path(size_t index, const string &quote = nullptr);
			string Path(const string &quote = nullptr);
			string Root(size_t index = 0);
		
			string ShortPath(size_t index = 0);

			string Name(size_t index = 0);
			string Extension(size_t index = 0);
			string FileNameWithoutExtension(size_t index = 0);

			string Sel(const string &quote, const wchar_t *sep = SEP_SPACE, const wchar_t *path_sep = nullptr);
			string FilesName(const string &quote = nullptr, const wchar_t *sep = SEP_SPACE);
			string GetFilesExtension(const string &quote = nullptr, const wchar_t *sep = SEP_SPACE);
			string FilesNameWithoutExtension(const string &quote = nullptr, const wchar_t *sep = SEP_SPACE);
			string SelNamespaces(const string &quote, const wchar_t *sep = SEP_SPACE);
			string SelFiles(const string &quote = nullptr, const wchar_t *sep = SEP_SPACE);
			string DiresName(const string &quote, const wchar_t *sep = SEP_SPACE);
			string SelDirs(const string &quote, const wchar_t *sep = SEP_SPACE);
			string SelDrives(const wchar_t *sep = SEP_SPACE);
			string ShortPaths(const wchar_t *sep = SEP_SPACE);
			string PathsName(const string &quote = nullptr, const wchar_t *sep = SEP_SPACE);

			bool is_back(int type = -1) const;
			/*bool is_back_root() const;
			bool is_back_directory() const;
			bool is_back_namespace() const;
			bool is_back_desktop() const;*/
			bool is_taskbar() const;
			bool is_desktop_window() const;

			bool verify_mode(SelectionMode mode);
			bool verify_types(const FileSystemObjects &fso) const;
			
			void QuerySelectionMode();
			bool QueryShellWindow();
			bool QuerySelected();
			bool Preparing();
			bool Parse(IShellItem *shellItem);
			bool Parse(FileProperties *prop);
			auto Count() const {return Items.size(); };

		private:
			static constexpr const wchar_t *SEP_SPACE = L" ";

			static const SFGAOF FGAOF_DEF =
				SFGAO_FOLDER | SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR |
				SFGAO_REMOVABLE | SFGAO_STREAM | SFGAO_LINK |
				SFGAO_DROPTARGET | SFGAO_HASSTORAGE | SFGAO_CANMONIKER | SFGAO_READONLY | SFGAO_HIDDEN;

		public:
			static string Quote(const string &str, const string &quote = L"\"")
			{
				return (quote + str + quote).move();
			}

			static bool GetFileProperties(IShellItem *shellItem, FileProperties *prop);
			inline static Point point = {};
			static constexpr auto GUID_HOME = 0xA8A544D5U;			//::{F874310E-B6B7-47DC-BC84-B9E6B38F5903}
			static constexpr auto GUID_QUICK_ACCESS = 0x214BFAA6U;	//::{031E4825-7B94-4DC3-B131-E946B44C8DD5}
			static constexpr auto GUID_LIBRARIES = 0xDB6BDBF5U;		//::{679F85CB-0220-4080-B29B-5540CC05AAB6}
		};
	}
}