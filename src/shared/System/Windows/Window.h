#pragma once
#include <dwmapi.h>

namespace Nilesoft
{
	namespace Windows
	{
		constexpr auto WC_MENU = L"#32768";
		constexpr auto WC_DESKTOP = L"#32769";
		constexpr auto WC_Shell_TrayWnd = L"Shell_TrayWnd";
		constexpr auto WC_Shell_SecondaryTrayWnd = L"Shell_SecondaryTrayWnd";
		constexpr auto WC_Composition_DesktopWindowContentBridge = L"Windows.UI.Composition.DesktopWindowContentBridge";
		constexpr auto WC_Start = L"Start";
		constexpr auto WC_ReBarWindow32 = L"ReBarWindow32";
		constexpr auto WC_MSTaskSwWClass = L"MSTaskSwWClass";
		constexpr auto WC_MSTaskListWClass = L"MSTaskListWClass";
		constexpr auto WC_TrayNotifyWnd = L"TrayNotifyWnd";
		constexpr auto WC_WorkerW = L"WorkerW";
		//Windows.UI.Composition.DesktopWindowContentBridge
// Undocumented Menu Messages 

#define WM_FINALDESTROY				0x0070
#define WM_MEASUREITEM_CLIENTDATA	0x0071
// Undocumented Aero Messages
#define WM_UAHDESTROYWINDOW			0x0090
#define WM_UAHDRAWMENU				0x0091
#define WM_UAHDRAWMENUITEM			0x0092
#define WM_UAHINITMENUPOPUP			0x0093	//WM_UAHINITMENU
#define WM_UAHMEASUREMENUITEM		0x0094
#define WM_UAHNCPAINTMENUPOPUP		0x0095
#define WM_UAHUPDATE				0x0096
#define WM_NCUAHDRAWFRAME			0X00AF	//related to themes

// Get hWndMenu first...
//SendMessage(hWndMenu, MN_SELECTITEM, 4, 0);
//PostMessage(hWndMenu, MN_OPENHIERARCHY, 0, 0);
#define MN_SETHMENU					0x01E0
#define MN_GETHMENU					0x01E1
#define MN_SIZEWINDOW				0x01E2	// SetWindowPos
#define MN_OPENHIERARCHY			0x01E3
#define MN_CLOSEHIERARCHY			0x01E4
#define MN_SELECTITEM				0x01E5
#define MN_CANCELMENUS				0x01E6
#define MN_SELECTFIRSTVALIDITEM		0x01E7	// MNFindNextValidItem(); SendMessage(MN_SELECTITEM)
// 0x1E8 - 0x1E9
#define MN_GETPPOPUPMENU			0x01EA  // Get current Item MN_GETPPOPUPMENU(obsolete) 0x01EA
#define MN_FINDMENUWINDOWFROMPOINT	0X01EB
#define MN_SHOWPOPUPWINDOW			0x01EC	// ShowWindow
#define MN_BUTTONDOWN				0X01ED
#define MN_MOUSEMOVE				0x01EE
#define MN_BUTTONUP					0x01EF
#define MN_SETTIMERTOOPENHIERARCHY	0x01F0
#define MN_DBLCLK					0x01F1
#define MN_ACTIVATEPOPUP            0x01F2	// ActivateThisWindow
#define MN_ENDMENU					0x01F3
#define MN_DODRAGDROP               0x01F4
#define MN_LASTPOSSIBLE				0x01FF

#define MENU_DOWN   0xFFFFFFFD
#define MENU_UP     0xFFFFFFFC

#define IE_GETMENU					0x04cc

//
//#define MN_ENDMENU                  0x01F4	//UnlockAndEndMenuState

//#define MN_ENDMENU			WM_USER + 10000
//#define MN_ACTIVATEPOPUP	WM_USER + 10001
/*
#define MN_FIRST                        0x01E0
#define MN_SETHMENU                     (MN_FIRST + 0)
#define MN_GETHMENU                     (MN_FIRST + 1)
#define MN_SIZEWINDOW                   (MN_FIRST + 2)
#define MN_OPENHIERARCHY                (MN_FIRST + 3)
#define MN_CLOSEHIERARCHY               (MN_FIRST + 4)
#define MN_SELECTITEM                   (MN_FIRST + 5)
#define MN_CANCELMENUS                  (MN_FIRST + 6)
#define MN_SELECTFIRSTVALIDITEM         (MN_FIRST + 7)
#define MN_GETPPOPUPMENU                (MN_FIRST + 10)
#define MN_FINDMENUWINDOWFROMPOINT      (MN_FIRST + 11)
#define MN_SHOWPOPUPWINDOW              (MN_FIRST + 12)
#define MN_BUTTONDOWN                   (MN_FIRST + 13)
#define MN_MOUSEMOVE                    (MN_FIRST + 14)
#define MN_BUTTONUP                     (MN_FIRST + 15)
#define MN_SETTIMERTOOPENHIERARCHY      (MN_FIRST + 16)
#define MN_DBLCLK                       (MN_FIRST + 17)
*/
#define MNUS_DEFAULT      0x00000001
#define MNUS_DELETE       0x00000002
#define MNUS_DRAWFRAME    0x00000004

#define MFMWFP_UPARROW      0xFFFFFFFD  /* Warning: Also used to define IDSYS_MNUP */
#define MFMWFP_DOWNARROW    0xFFFFFFFC  /* Warning: Also used to define IDSYS_MNDOWN */

#define IDSYS_MNANIMATE     0x0000FFFBL
#define IDSYS_MNUP          MFMWFP_UPARROW   /* 0xFFFFFFFD */
#define IDSYS_MNDOWN        MFMWFP_DOWNARROW /* 0xFFFFFFFC */
#define IDSYS_MNSHOW		0x0000FFFEL
#define IDSYS_MNHIDE        0x0000FFFFL
// describes the sizes of the menu bar or menu item
		typedef union tagUAHMENUITEMMETRICS
		{
			// cx appears to be 14 / 0xE less than rcItem's width!
			// cy 0x14 seems stable, i wonder if it is 4 less than rcItem's height which is always 24 atm
			struct
			{
				DWORD cx;
				DWORD cy;
			} rgsizeBar[2];
			struct
			{
				DWORD cx;
				DWORD cy;
			} rgsizePopup[4];
		} UAHMENUITEMMETRICS;

		// not really used in our case but part of the other structures
		typedef struct tagUAHMENUPOPUPMETRICS
		{
			DWORD rgcx[4];
			DWORD fUpdateMaxWidths : 2; // from kernel symbols, padded to full dword
		} UAHMENUPOPUPMETRICS;

		// hmenu is the main window menu; hdc is the context to draw in
		typedef struct tagUAHMENU
		{
			HMENU hmenu;
			HDC hdc;
			DWORD dwFlags; // no idea what these mean, in my testing it's either 0x00000a00 or sometimes 0x00000a10
		} UAHMENU;

		// menu items are always referred to by iPosition here
		typedef struct tagUAHMENUITEM
		{
			int iPosition; // 0-based position of menu item in menubar
			UAHMENUITEMMETRICS umim;
			UAHMENUPOPUPMETRICS umpm;
		} UAHMENUITEM;

		// the DRAWITEMSTRUCT contains the states of the menu items, as well as
		// the position index of the item in the menu, which is duplicated in
		// the UAHMENUITEM's iPosition as well
		typedef struct UAHDRAWMENUITEM
		{
			DRAWITEMSTRUCT dis; // itemID looks uninitialized
			UAHMENU um;
			UAHMENUITEM umi;
		} UAHDRAWMENUITEM;

		// the MEASUREITEMSTRUCT is intended to be filled with the size of the item
		// height appears to be ignored, but width can be modified
		typedef struct tagUAHMEASUREMENUITEM
		{
			MEASUREITEMSTRUCT mis;
			UAHMENU um;
			UAHMENUITEM umi;
		} UAHMEASUREMENUITEM;

		struct Compositor
		{
			static bool TransparentArea(HWND hWnd, MARGINS margins = {-1})
			{
				return S_OK == ::DwmExtendFrameIntoClientArea(hWnd, &margins);
			}
		};

		//WindowCompositionAttributeData
		struct WindowCompositionAttribute
		{
			uint32_t attribute{};
			void *data{};
			uint32_t cbSize{};

			enum
			{
				WCA_UNDEFINED = 0,
				WCA_NCRENDERING_ENABLED = 1,
				WCA_NCRENDERING_POLICY = 2,
				WCA_TRANSITIONS_FORCEDISABLED = 3,
				WCA_ALLOW_NCPAINT = 4,
				WCA_CAPTION_BUTTON_BOUNDS = 5,
				WCA_NONCLIENT_RTL_LAYOUT = 6,
				WCA_FORCE_ICONIC_REPRESENTATION = 7,
				WCA_EXTENDED_FRAME_BOUNDS = 8,
				WCA_HAS_ICONIC_BITMAP = 9,
				WCA_THEME_ATTRIBUTES = 10,
				WCA_NCRENDERING_EXILED = 11,
				WCA_NCADORNMENTINFO = 12,
				WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
				WCA_VIDEO_OVERLAY_ACTIVE = 14,
				WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
				WCA_DISALLOW_PEEK = 16,
				WCA_CLOAK = 17,
				WCA_CLOAKED = 18,
				WCA_ACCENT_POLICY = 19,
				WCA_FREEZE_REPRESENTATION = 20,
				WCA_EVER_UNCLOAKED = 21,
				WCA_VISUAL_OWNER = 22,
				WCA_HOLOGRAPHIC = 23,
				WCA_EXCLUDED_FROM_DDA = 24,
				WCA_PASSIVEUPDATEMODE = 25,
				WCA_USEDARKMODECOLORS = 26,
				WCA_LAST = 27,
				DWMWA_SYSTEMBACKDROP_TYPE = 38,	// Build >= 22523
				DWMWA_MICA_EFFECT = 1029
			};

			bool set(HWND hWnd)
			{
				return set(hWnd, this);
			}

			bool get(HWND hWnd)
			{
				return get(hWnd, this);
			}

			static bool set(HWND hWnd, WindowCompositionAttribute *wca)
			{
				return DLL::User32<BOOL>("SetWindowCompositionAttribute", hWnd, wca);
			}

			static bool get(HWND hWnd, WindowCompositionAttribute *wca)
			{
				return DLL::User32<BOOL>("GetWindowCompositionAttribute", hWnd, wca);
			}

			/*
			BOOL enable = TRUE;
			WINDOWCOMPOSITIONATTRIBDATA CompositionAttribute{};
			CompositionAttribute.Attrib = WCA_EXCLUDED_FROM_LIVEPREVIEW;
			CompositionAttribute.pvData = &enable;
			CompositionAttribute.cbData = sizeof(BOOL);
			DwmSetWindowCompositionAttribute(hwnd, &CompositionAttribute);
			*/
		};

		struct AccentPolicy
		{
			uint32_t state{};
			uint32_t flags{};
			uint32_t color{};
			uint32_t animation{};

			// Affects the rendering of the background of a window.
			enum State
			{
				Default = -1,
				Disabled,				// Default value. Background is black.
				Gradient,				// Background is GradientColor, alpha channel ignored.
				TransparentGradient,	// Background is GradientColor.
				BlurBehind,				// Background is GradientColor, with blur effect.
				AcrylicBlurBehind,		// Background is GradientColor, with acrylic blur effect. RS4 1803
				HostBackdrop,			// Unknown. RS5 1809
				Invalid					// Unknown. Seems to draw background fully transparent.
			};

			enum Flags : uint32_t
			{
				NoneBordr = 0x00,
				GradientColor = 0x02, //Set to 2 to tell GradientColor is used, rest is unknown
				Luminosity = 0x02,
				LeftBorder = 0x20,
				TopBorder = 0x40,
				RightBorder = 0x80,
				BottomBorder = 0x100,
				AllBorder = (LeftBorder | TopBorder | RightBorder | BottomBorder),
				
				AllowSetWindowRgn = 0x10,	//0x10, 0xFE; 16, 25, 140
				FullScreen = 0xFF,
				FullScreen1 = 0xFFFFFFFF
			};

			AccentPolicy(HWND handle) 
				: _handle(handle)
			{
			}

			// Blur is not useful in high contrast mode
			// Windows build version less than 16299 does not support acrylic effect
			// 1903 = 18362
			// 1803 = 17134
			// 1709 = 16299
			// 18362 TintLuminosityOpacity
			// flags = 0x10;// 0x10, 0xFE; 16, 25,140
			bool set()
			{
				wca.attribute = WindowCompositionAttribute::WCA_ACCENT_POLICY;
				wca.data = this;
				wca.cbSize = _size;
				return wca.set(_handle);
			}

			bool set(uint32_t _state, uint32_t _flags = 0, uint32_t _color = 0)
			{
				this->state = _state;
				this->flags = _flags;
				this->color = _color;
				return set();
			}

			bool set(HWND hWnd)
			{
				_handle = hWnd;
				return set();
			}

			bool get(HWND hWnd)
			{
				_handle = hWnd;
				wca.attribute = WindowCompositionAttribute::WCA_ACCENT_POLICY;
				wca.data = this;
				wca.cbSize = _size;
				return wca.get(hWnd);
			}

			bool get()
			{
				return get(_handle);
			}

		private:
			HWND _handle{};
			WindowCompositionAttribute wca{};
			static constexpr auto _size = sizeof(uint32_t) * 4;
		};

		class Window
		{
			HWND _handle{};
			LONG_PTR _style{};
			LONG_PTR _exStyle{};
			SUBCLASSPROC _pfnSubclass{};
			/*
static AccentState SelectAccentState(AcrylicAccentState state = AcrylicAccentState.Default)
{
// set window acrylic effect
AccentState value = state switch
{
// Blur the window background only for Windows 10
// Switch AccentState according to OS version
AcrylicAccentState.Default => SystemInfo.Version.Value switch
{
// Disable acrylic effect in Windows11 environment
var version when version >= VersionInfos.Windows11_Preview => AccentState.ACCENT_ENABLE_GRADIENT,
// On Windows 10 1903 or later, if ACCENT_ENABLE_ACRYLICBLURBEHIND is used, it will not follow mouse operations such as dragging windows.
// Work around this by disabling ACCENT_ENABLE_ACRYLICBLURBEHIND only while moving/resizing the window
//var version when version >= VersionInfos.Windows10_1903 => AccentState.ACCENT_ENABLE_BLURBEHIND,
var version when version >= VersionInfos.Windows10_1809 => AccentState.ACCENT_ENABLE_ACRYLICBLURBEHIND,
var version when version >= VersionInfos.Windows10 => AccentState.ACCENT_ENABLE_BLURBEHIND,
_ => AccentState.ACCENT_ENABLE_TRANSPARENTGRADIENT,
},
AcrylicAccentState.Disabled => AccentState.ACCENT_DISABLED,
AcrylicAccentState.Gradient => AccentState.ACCENT_ENABLE_GRADIENT,
AcrylicAccentState.TransparentGradient => AccentState.ACCENT_ENABLE_TRANSPARENTGRADIENT,
AcrylicAccentState.BlurBehind => AccentState.ACCENT_ENABLE_BLURBEHIND,
AcrylicAccentState.AcrylicBlurBehind => AccentState.ACCENT_ENABLE_ACRYLICBLURBEHIND,
_ => throw new InvalidOperationException(),
};
		*/
		public:
			WINDOWINFO Info{};

		public:

			Window() : _handle{ nullptr } { }
			Window(HWND handle) : _handle{ handle } { }
			Window(const Window &other) { _handle = other._handle; }

			virtual ~Window() {}

			operator HWND() const { return _handle; }
			explicit operator bool() const { return _handle != nullptr; }

			HWND get() const { return _handle; }
			bool get_Info() { return ::GetWindowInfo(_handle, &Info); }

			bool is_window() const {
				return _handle ? ::IsWindow(_handle) : false;
			}

			bool is_visible() const {
				return _handle ? ::IsWindowVisible(_handle) : false;
			}

			template<typename T = LONG_PTR>
			T get_style()
			{
				_style = get_long(GWL_STYLE);
				return T(_style);
			}

			template<typename T = LONG_PTR>
			T set_style(T value)
			{
				_style = value;
				return set_long(GWL_EXSTYLE, _style);
			}

			template<typename T = LONG_PTR>
			T get_ex_style()
			{
				_exStyle = get_long(GWL_EXSTYLE);
				return T(_exStyle);
			}

			template<typename T = LONG_PTR>
			T set_ex_style(auto value)
			{
				_exStyle = value;
				return set_long<T>(GWL_EXSTYLE, value);
			}

			bool is_style(auto style) const
			{
				auto s = reinterpret_cast<LONG_PTR>(style);
				return (_style & s) == s;
			}

			bool add_style(auto value)
			{
				if(!is_style(value))
				{
					_style |= value;
					return 0 != set_style(_style);
				}
				return false;
			}

			bool remove_style(auto value)
			{
				if(is_style(value))
				{
					_style &= ~value;
					return 0 != set_style(_style);
				}
				return false;
			}

			auto instance() const
			{
				return get_long<HINSTANCE>(GWLP_HINSTANCE);
			}

			auto get_wndproc() const
			{
				return get_long<WNDPROC>(GWLP_WNDPROC);
			}

			auto set_wndproc(auto value) const
			{
				return set_long<WNDPROC>(GWLP_WNDPROC, value);
			}

			template<typename T = LONG_PTR>
			T get_long(int index) const
			{
				return (T)::GetWindowLongPtrW(_handle, index);
			}

			template<typename T = LONG_PTR>
			T set_long(int index, auto value) const
			{
				return (T)::SetWindowLongPtrW(_handle, index, (LONG_PTR)value);
			}

			string class_name() const
			{
				return class_name(_handle).move();
			}

			uint32_t class_hash() const
			{
				return class_hash(_handle);
			}

			Window ancestor(uint32_t gaFalg = GA_ROOTOWNER) const {
				return Ancestor(_handle, gaFalg);
			}

			template<typename T = LRESULT>
			auto send(uint32_t msg, auto wparam = 0, auto lparam = 0)
			{
				return (T)::SendMessageW(_handle, msg, (WPARAM)wparam, (LPARAM)lparam);
			}

			template<typename T = BOOL>
			auto post(uint32_t msg, auto wparam = 0, auto lparam = 0)
			{
				return (T)::PostMessageW(_handle, msg, (WPARAM)wparam, (LPARAM)lparam);
			}


			bool is_prop(const wchar_t *name)
			{
				return ::GetPropW(_handle, name) != nullptr;
			}

			template<typename T = HANDLE>
			bool prop(const wchar_t *name, T value)
			{
				return ::SetPropW(_handle, name, reinterpret_cast<HANDLE>(value));
			}

			template<typename T = HANDLE>
			T prop(const wchar_t *name)
			{
				return reinterpret_cast<T>(::GetPropW(_handle, name));
			}
			
			template<typename T = HANDLE>
			T remove_prop(const wchar_t *name)
			{
				return reinterpret_cast<T>(::RemovePropW(_handle, name));
			}

			HWND find(const wchar_t *name) const
			{
				return _handle ? Find(name, _handle) : nullptr;
			}

			uint32_t hash() const
			{
				return class_name(_handle).hash();
			}

			Window parent() const
			{
				return Parent(_handle);
			}

			BOOL is_subclassed() const
			{
				return _pfnSubclass != nullptr;
			}

			BOOL subclass(SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, auto dwRefData)
			{
				if(pfnSubclass && ::SetWindowSubclass(_handle, pfnSubclass, uIdSubclass, (DWORD_PTR)dwRefData))
				{
					_pfnSubclass = pfnSubclass;
					return true;
				}
				return false;
			}

			BOOL unsubclass(UINT_PTR uIdSubclass = 0)
			{
				if(_pfnSubclass && ::RemoveWindowSubclass(_handle, _pfnSubclass, uIdSubclass))
				{
					_pfnSubclass = nullptr;
					return true;
				}
				return false;
			}

			BOOL unsubclass(SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass) const
			{
				return ::RemoveWindowSubclass(_handle, pfnSubclass, uIdSubclass);
			}

			template<typename T>
			BOOL get_subclass(UINT_PTR uIdSubclass, T *dwRefData)
			{
				return ::GetWindowSubclass(_handle, _pfnSubclass, uIdSubclass, static_cast<DWORD_PTR *>(dwRefData));
			}

			DWORD get_threadId(DWORD * processId = nullptr) const
			{
				return ::GetWindowThreadProcessId(_handle, processId);
			}

			uint16_t atom() const {
				return ::GetClassWord(_handle, GCW_ATOM);
			}

			/////////////////////////////////////////

			template<typename T = LONG_PTR>
			static T Style(HWND hWnd)
			{
				return reinterpret_cast<T>(::GetWindowLongPtrW(hWnd, GWL_STYLE));
			}

			template<typename T = LONG_PTR>
			static T Style(HWND hWnd, auto value)
			{
				return reinterpret_cast<T>(::SetWindowLongPtrW(hWnd, GWL_STYLE, reinterpret_cast<LONG_PTR>(value)));
			}

			template<typename T = LONG_PTR>
			static T ExStyle(HWND hWnd, auto value)
			{
				return reinterpret_cast<T>(::SetWindowLongPtrW(hWnd, GWL_EXSTYLE, reinterpret_cast<LONG_PTR>(value)));
			}

			static string class_name(HWND hWnd)
			{
				if(hWnd)
				{
					string cls(0xFF, '\0');
					if(auto length = ::GetClassNameW(hWnd, cls.data(), 0xFF); length > 0)
						return cls.release(length).move();
				}
				return nullptr;
			}

			static uint32_t class_hash(HWND hWnd)
			{
				if(hWnd)
				{
					if(string cls = class_name(hWnd).move(); !cls.empty())
						return cls.hash();
				}
				return 0u;
			}

			static uint32_t hash(HWND hWnd)
			{
				return class_name(hWnd).hash();
			}

			auto static get_hInstance(HWND hWnd)
			{
				return reinterpret_cast<HINSTANCE>(::GetWindowLongPtrW(hWnd, GWLP_HINSTANCE));
			}

			static bool IsPopupMenu(HWND hWnd, bool isTopLevelWindow = false)
			{
				if(::GetClassLongPtrW(hWnd, GCW_ATOM) == 32768)
					return true;
				wchar_t pszClass[100]{};
				if(auto length = ::GetClassNameW(hWnd, pszClass, 100); length == 6) //#32768
				{
					if(0 == ::memcmp(pszClass, WC_MENU, 6 * sizeof(wchar_t)))
					{
						return isTopLevelWindow ? DLL::User32<BOOL>("IsTopLevelWindow", hWnd) : true;
					}
				}
				return false;
			}

			static HWND Find(wchar_t const* lpszClass, HWND hParent = nullptr)
			{
				return ::FindWindowExW(hParent, nullptr, lpszClass, nullptr);
			}

			static HWND FindTaskbar()
			{
				return Find(Windows::WC_Shell_TrayWnd);
			}
			
			static Window Parent(HWND handle) {
				return ::GetParent(handle);
			}

			static Window Ancestor(HWND handle, uint32_t gaFalg = GA_ROOTOWNER) {
				return ::GetAncestor(handle, gaFalg);
			}

			static Window FromPoint(const POINT &pt) {
				return ::WindowFromPoint(pt);
			}

			static Window ActiveWindow() {
				return ::GetActiveWindow();
			}
			
			static Window ForegroundWindow() {
				return ::GetForegroundWindow();
			}
			
			static uint16_t Atom(HWND hWnd) {
				return ::GetClassWord(hWnd, GCW_ATOM);
			}

			static bool Atom(HWND hWnd, std::initializer_list<uint16_t> atom_list) {
				for(auto const &atom : atom_list)
					if(atom == Atom(hWnd)) return true;
				return false;
			}

			/*
			typedef BOOL(WINAPI *pfnIsTopLevelWindow)(HWND hWnd);
			static auto IsTopLevelWindow = (pfnIsTopLevelWindow)::GetProcAddress(::GetModuleHandleW(L"User32"), "IsTopLevelWindow");

			inline static bool IsPopupMenu(HWND hWnd)
			{
				if(::GetClassLongPtrW(hWnd, GCW_ATOM) == 32768)
					return true;
				wchar_t pszClass[MAX_PATH + 1]{};
				auto length = ::GetClassNameW(hWnd, pszClass, MAX_PATH);
				if(length == 6) //#32768
					return DLL::User32<BOOL>("IsTopLevelWindow", hWnd) && _tcscmp(pszClass, WC_MENU) == 0;
				return false;
			}

			inline static bool IsWindowUseDarkMode(HWND hwnd)
			{
				constexpr auto DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
				BOOL bResult{ FALSE };
				::DwmGetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &bResult, sizeof(bResult));
				return bResult;
			}

			inline static bool IsAncestorUseDarkMode(GUITHREADINFO gti)
			{
				HWND hwndMenuRoot = ::GetAncestor(gti.hwndMenuOwner, GA_ROOT);
				return IsWindowUseDarkMode(hwndMenuRoot);
			}

			inline static bool IsMenuUseDarkMode(HWND hwnd)
			{
				GUITHREADINFO gti = { sizeof(GUITHREADINFO), };
				DWORD tid = ::GetWindowThreadProcessId(hwnd, nullptr);

				if(tid && ::GetGUIThreadInfo(tid, &gti))
				{
					return IsWindowUseDarkMode(gti.hwndMenuOwner);   // || IsAncestorUseDarkMode(gti);
				}

				return false;
			}
			*/
			static GUITHREADINFO GetGUIThread(DWORD dwThreadId)
			{
				GUITHREADINFO gti = { sizeof(gti) };
				if(::GetGUIThreadInfo(dwThreadId, &gti))
					return gti;
				return {};
			}

			static GUITHREADINFO GetGUIThread(auto hWnd)
			{
				return GetGUIThread(::GetWindowThreadProcessId(hWnd, nullptr));
			}

			struct Dwm
			{
				static bool IsCompositionEnabled()
				{
					BOOL enabled = FALSE;
					::DwmIsCompositionEnabled(&enabled);
					return enabled;
				}

				template<typename T = DWORD>
				static T get(HWND hWnd, DWMWINDOWATTRIBUTE attribute)
				{
					T value{};
					::DwmGetWindowAttribute(hWnd, attribute, &value, sizeof(value));
					return value;
				}

				template<typename T = DWORD>
				static bool set(HWND hWnd, DWORD attribute, T attributeValue)
				{
					auto result = ::DwmSetWindowAttribute(hWnd, attribute, (void*)&attributeValue, sizeof(T));
					return (result == S_OK);
				}

				static uint32_t AccentColor()
				{
					DWORD col = 0;
					BOOL opaque = FALSE;
					uint32_t newcolor{};
					uint8_t a, r, g, b;

					if(S_OK == ::DwmGetColorizationColor(&col, &opaque))
					{
						a = uint8_t(col >> 24);
						r = uint8_t(col >> 16);
						g = uint8_t(col >> 8);
						b = uint8_t(col);

						newcolor = RGB(
							round(r * (a / 255) + 255 - a),
							round(g * (a / 255) + 255 - a),
							round(b * (a / 255) + 255 - a)
						);
					}
					return newcolor;
				}

				static bool IsNonClientRenderingEnabled(HWND hWnd)
				{
					return get(hWnd, DWMWINDOWATTRIBUTE::DWMWA_NCRENDERING_ENABLED);
				}

				// Disable/ENABLED non-client area rendering on the window.
				static bool NonClientRendering(HWND hWnd, DWMNCRENDERINGPOLICY NCRP = DWMNCRP_DISABLED)
				{
					return set(hWnd, DWMWINDOWATTRIBUTE::DWMWA_NCRENDERING_POLICY, NCRP);
				}

				// Extend frame on the bottom of client area
				static bool ExtendIntoClientArea(HWND hWnd, const MARGINS &margins = {-1})
				{
					return S_OK == ::DwmExtendFrameIntoClientArea(hWnd, &margins);
				}


				/// <summary>
				//	DEFAULT = 0
				//	DONOTROUND = 1
				//	ROUND = 2
				//	ROUNDSMALL = 3
				/// </summary>
				static bool BorderRadius(HWND hWnd, uint32_t corner = 0)
				{
					const auto DWMWA_WINDOW_CORNER_PREFERENCE = 33U;
					return set(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, corner);
				}

				static bool BorderColor(HWND hWnd, uint32_t color)
				{
					const auto DWMWA_BORDER_COLOR = 34U;
					return set(hWnd, DWMWA_BORDER_COLOR, color);
				}

				static bool ImmersiveDarkMode(HWND hWnd, BOOL enable = TRUE)
				{
					const auto DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
					return set(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, enable);
				}

				static bool BorderlessDropShadow(HWND hWnd, int ShadowSize = 1)
				{
					return ExtendIntoClientArea(hWnd, { 0, 0, 0, ShadowSize });
				}
			};

			class Transparency
			{
			public:

				// Enable Blur Behind and apply to the entire client area
				static bool WindowSheetOfGlass(HWND hWnd)
				{
					return Dwm::ExtendIntoClientArea(hWnd);
				}

				// Enable Blur Behind and apply to the entire client area
				static bool BlurBehind(HWND hWnd, bool enabled = true)
				{
					DWM_BLURBEHIND bb { };
					bb.dwFlags = DWM_BB_ENABLE;
					bb.fEnable = enabled;
					// Apply Blur Behind
					return S_OK == ::DwmEnableBlurBehindWindow(hWnd, &bb);
				}

				static bool Transparent(HWND hWnd, int capcity = 80)
				{
					if(::IsWindow(hWnd))
					{
						auto exStyle = ::GetWindowLongW(hWnd, GWL_EXSTYLE);
						if((exStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
						{
							::SetWindowLongW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
						}
						return ::SetLayeredWindowAttributes(hWnd, 0xff00ff, BYTE((255 * capcity) / 100), LWA_ALPHA);
					}
					return false;
				}

			};
		};
	}
}
