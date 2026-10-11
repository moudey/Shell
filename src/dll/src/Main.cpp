#include <pch.h>
#include "Include/ContextMenu.h"
#include "Library/detours.h"
#include "RegistryConfig.h"
#include <UIAutomation.h>
#include <propkey.h>      // PKEY_AppUserModel_ID (needed when patching a pinned shortcut)
#include <shlobj.h>       // CSIDL_APPDATA / IShellLink
#include <shobjidl.h>     // IStartMenuPinnedList (the official "unpin from taskbar" API)
using namespace Nilesoft::Shell;
using namespace Diagnostics;

//MS also offers EV Code Certificates, which enable immediate acceptance by SmartScreeen of Windows Defender
//https://blogs.msdn.microsoft.com/ie/2012/08/14/microsoft-smartscreen-extended-validation-ev-code-signing-certificates/

#ifdef _DEBUG
#include <crtdbg.h>
#endif
/*
Windows 10
Version Build
-------------
1507	10240
1511	10586
1607	14393
1703	15063
1709	16299
1803	17134
1809	17763
1903	18362
1909	18363
2004	19041
20H2	19042
21H1	19043
21H2	19044
22H2	19045
*/

//#pragma comment(lib, "mincore.lib")
//#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "WindowsCodecs.lib")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "Shlwapi.lib")

#if defined(_M_ARM64)
#pragma comment(lib, "plutosvg-arm64.lib")
#pragma comment(lib, "detours-arm64.lib")
#elif defined(_M_ARM)
#pragma comment(lib, "plutosvg-arm.lib")
#pragma comment(lib, "detours-arm.lib")
#elif defined(_M_X64)
#pragma comment(lib, "plutosvg-x64.lib")
#pragma comment(lib, "detours-x64.lib")
#elif defined(_M_IX86)
#pragma comment(lib, "plutosvg-x86.lib")
#pragma comment(lib, "detours-x86.lib")
#endif

//#pragma optimize("ts", on) 

/*
#if defined(_M_X64)
#pragma comment(linker, "/export:DllCanUnloadNow=_DllCanUnloadNow")
#else
#pragma comment(linker, "/export:DllCanUnloadNow=__DllCanUnloadNow@0")
#endif
*/

#ifdef _DEBUG

std::unordered_map< uint32_t, const wchar_t *> msg_map0 = {
	{ WM_DESTROY, L"WM_DESTROY" },
	{ WM_NCDESTROY, L"WM_NCDESTROY" },
	{ WM_CREATE, L"WM_CREATE" },
	{ WM_NCCREATE, L"WM_NCCREATE" },
	{ WM_PAINT, L"WM_PAINT" },
	{ WM_NCPAINT, L"WM_NCPAINT" },
	{ WM_PRINTCLIENT, L"WM_PRINTCLIENT" },
	{ WM_PRINT, L"WM_PRINT" },
	{ WM_NCCALCSIZE, L"WM_NCCALCSIZE" },
	{ WM_ERASEBKGND, L"WM_ERASEBKGND" },
	{ MN_SELECTITEM, L"MN_SELECTITEM" },
	{ WM_KEYDOWN, L"WM_KEYDOWN" },
	{ WM_WINDOWPOSCHANGED, L"WM_WINDOWPOSCHANGED" },
	{ WM_WINDOWPOSCHANGING, L"WM_WINDOWPOSCHANGING" },
	{ WM_NCHITTEST, L"WM_NCHITTEST" },
	{ WM_TIMER, L"WM_TIMER" },
	{ WM_SIZE, L"WM_SIZE" },
	{ WM_MOVE, L"WM_MOVE" },
	{ MN_SIZEWINDOW, L"MN_SIZEWINDOW" },
	{ MN_DBLCLK, L"MN_DBLCLK" },
	{ MN_CLOSEHIERARCHY, L"MN_CLOSEHIERARCHY" },
	{ MN_SETTIMERTOOPENHIERARCHY, L"MN_SETTIMERTOOPENHIERARCHY" },
	{ MN_FINDMENUWINDOWFROMPOINT, L"MN_FINDMENUWINDOWFROMPOINT" },
	{ MN_ENDMENU, L"MN_ENDMENU" },
	{ MN_CANCELMENUS, L"MN_CANCELMENUS" },
	{ MN_BUTTONDOWN, L"MN_BUTTONDOWN" },
	{ MN_BUTTONUP, L"MN_BUTTONUP" },
	{ WM_UAHDESTROYWINDOW, L"WM_UAHDESTROYWINDOW" },
	{ WM_UAHMEASUREMENUITEM, L"WM_UAHMEASUREMENUITEM" },
	{ WM_UAHINITMENUPOPUP, L"WM_UAHINITMENUPOPUP" },
	{ WM_UAHNCPAINTMENUPOPUP, L"WM_UAHNCPAINTMENUPOPUP" },
	{ WM_UAHDRAWMENU, L"WM_UAHDRAWMENU" },
	{ WM_NCUAHDRAWFRAME, L"WM_NCUAHDRAWFRAME" },
	{ WM_UAHDRAWMENUITEM, L"WM_UAHDRAWMENUITEM" },
	{ WM_ENTERMENULOOP, L"WM_ENTERMENULOOP" },
	{ WM_EXITMENULOOP, L"WM_EXITMENULOOP" },
	{ WM_INITMENU, L"WM_INITMENU" },
	{ WM_INITMENUPOPUP, L"WM_INITMENUPOPUP" },
	{ WM_UNINITMENUPOPUP, L"WM_UNINITMENUPOPUP" },
	{ WM_MENUSELECT, L"WM_MENUSELECT" },
	{ WM_NEXTMENU, L"WM_NEXTMENU" },
	{ WM_MENUCHAR, L"WM_MENUCHAR" },
	{ WM_MEASUREITEM, L"WM_MEASUREITEM" },
	{ WM_DRAWITEM, L"WM_DRAWITEM" },
	{ WM_MENUCOMMAND, L"WM_MENUCOMMAND" },
	{ WM_COMMAND, L"WM_COMMAND" },
	{ WM_SYSCOMMAND, L"WM_SYSCOMMAND" },
	{ WM_MENURBUTTONUP, L"WM_MENURBUTTONUP" },
	{ WM_CAPTURECHANGED, L"WM_CAPTURECHANGED" },
	{ WM_ENTERIDLE, L"WM_ENTERIDLE" },
	{ WM_SETCURSOR, L"WM_SETCURSOR" },
	{ WM_SHOWWINDOW , L"WM_SHOWWINDOW" },
	{ WM_PARENTNOTIFY, L"WM_PARENTNOTIFY"},
	{ WM_MOUSEACTIVATE, L"WM_MOUSEACTIVATE"},
	{ WM_CONTEXTMENU, L"WM_CONTEXTMENU"},
	{ WM_NOTIFY, L"WM_NOTIFY"},
	{ WM_SETFOCUS, L"WM_SETFOCUS"},
	{ WM_KILLFOCUS, L"WM_KILLFOCUS"},
	{ WM_GETOBJECT, L"WM_GETOBJECT"},
	{ WM_ACTIVATE, L"WM_ACTIVATE"},
	{ WM_NCACTIVATE, L"WM_NCACTIVATE"},
	{ WM_ACTIVATEAPP, L"WM_ACTIVATEAPP"},
	{ WM_CHANGEUISTATE, L"WM_CHANGEUISTATE"},
	{ WM_IME_SETCONTEXT, L"WM_IME_SETCONTEXT"},
	{ WM_IME_NOTIFY, L"WM_IME_NOTIFY"},
	{ WM_MOUSEMOVE, L"WM_MOUSEMOVE"},
	{WM_DEVICECHANGE, L"WM_DEVICECHANGE"},
	{WM_IME_REQUEST, L"WM_IME_REQUEST"},
	{MN_GETHMENU, L"MN_GETHMENU"},
	{WM_LBUTTONDOWN, L"WM_LBUTTONDOWN"},
	{WM_RBUTTONDOWN, L"WM_RBUTTONDOWN"},
	{WM_LBUTTONUP, L"WM_LBUTTONUP"},
	{WM_RBUTTONUP, L"WM_RBUTTONUP"},
	{WM_MOUSELEAVE,L"WM_MOUSELEAVE"},
	{WM_USER, L"WM_USER"},
	{WM_APP, L"WM_APP"},
	{ 0, nullptr }
};

#endif

#pragma region Entry Point

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))  // windowsx.h
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))  // windowsx.h

HINSTANCE _hInstance{};
Initializer _initializer;
extern Logger &_log = Logger::Instance();
const Windows::Version *ver = &Windows::Version::Instance();

HANDLE watch_event = nullptr;

WindowsHook _taskbar_mouse;

//std::unordered_map<HMODULE, IATHook> _detours_ci;
IATHook iathook_NtUserTrackPopupMenuEx;
std::vector<IATHook> iathook_TrackPopupMenu; //OverrideFunction
//Detours<decltype(::TrackPopupMenu)> _TrackPopupMenu;
//Detours<decltype(::TrackPopupMenuEx)> _TrackPopupMenuEx;
Detours<decltype(::DllGetClassObject)> _DllGetClassObject;
Detours<decltype(::CoCreateInstance)> _CoCreateInstance;

uint32_t MN_CONTEXTMENU = 0;
std::unordered_map<HWND, Window> _window_taskbar;

LRESULT __stdcall TaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT __stdcall TaskbarProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// Win10 taskbar button area (MSTaskListWClass): hook entry points.
LRESULT __stdcall TaskListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void tasklist_hook_all(uint32_t delay);

__inline auto is_registered() -> bool
{
	//auto_regkey r;
	//return ERROR_SUCCESS == auto_regkey::exists_key(HKCR, HKCR_CLSID_ContextMenu);
	return RegistryConfig::IsRegistered();
}

struct
{
	string path;
	bool explorer{};
	bool contextmenuhandler{};
	HMODULE handle{};

	bool init()
	{
		handle = ::GetModuleHandleW(nullptr);
		if(!(explorer) && path.empty())
		{
			path = Path::Module(nullptr);
			explorer = path.ends_with(def_EXPLORER, true);
		}
		return explorer;
	}

	explicit operator bool() const { return explorer; }

} _loader;

struct taskbar_t
{
	inline static IComPtr<IUIAutomation> _IUIAutomation;
	inline static auto wShell_TrayWnd = ::RegisterWindowMessageW(Windows::WC_Shell_TrayWnd);
	inline static auto wShell_SecondaryTrayWnd = ::RegisterWindowMessageW(Windows::WC_Shell_SecondaryTrayWnd);

	static bool hook(HWND hTaskbar, UINT_PTR id)
	{
		Window window = Window::Find(Windows::WC_Composition_DesktopWindowContentBridge, hTaskbar);
		if(window && !window.is_prop(UxSubclass))
		{
			if(!_window_taskbar.contains(window))
			{
				if(window.subclass(TaskbarSubclassProc, id, hTaskbar))
				{
					window.prop(UxSubclass, CONTEXTMENUSUBCLASS);
					_window_taskbar[window] = window;
					return true;
				}
			}
		}
		return false;
	}

	static void try_hook(HWND hTaskbar, UINT_PTR id, int time = 500)
	{
		if(hTaskbar)
		{
			::SetTimer(hTaskbar, id, time, [](HWND hWnd, UINT, UINT_PTR id, DWORD)
			{
				static int atttempt = 0;
				if(hook(hWnd, id))
					atttempt = 50;
				if(atttempt++ >= 50)
					::KillTimer(hWnd, id);
			});
		}
	}

	static void hook_all()
	{
		::EnumWindows([](HWND hWnd, LPARAM)->BOOL {
			auto atom = Window::Atom(hWnd);
			if(atom == wShell_TrayWnd || atom == wShell_SecondaryTrayWnd)
			{
				Window window = hWnd;
				if(!window.is_prop(UxSubclass))
				{
					if(!_window_taskbar.contains(window))
					{
						auto proc = window.get_long(GWLP_WNDPROC);
						window.prop(UxSubclass, proc);
						window.set_long(GWLP_WNDPROC, TaskbarProc);
						_window_taskbar[window] = window;
						return true;
					}
				}
			}
			return TRUE;
		}, 0);
	}

	static void hook_all(uint32_t delay)
	{
		::SetTimer(::GetShellWindow(), 0, delay, [](HWND hWnd, UINT, UINT_PTR id, DWORD)
		{						
			static int atttempt = 0;
			if(atttempt++ >= 10)
				::KillTimer(hWnd, id);
			
			taskbar_t::hook_all();
		});
	}
	
	static void unhook(HWND hWnd)
	{
		for(auto &window : _window_taskbar)
		{
			if(window.first == hWnd)
			{
				window.second.set_long(GWLP_WNDPROC, window.second.prop(UxSubclass));
				window.second.remove_prop(UxSubclass);
				_window_taskbar.erase(hWnd);
				break;
			}
		}
	}
	static void unhook_all()
	{
		for(auto &window : _window_taskbar)
		{
			window.second.set_long(GWLP_WNDPROC, window.second.prop(UxSubclass));
			window.second.remove_prop(UxSubclass);
			_window_taskbar.erase(window.first);
			break;
		}
		_window_taskbar.clear();
	}

	static bool is_allowed_area(HWND hTaskbar, const Point &pt)
	{
		bool ret = false;

		if(!_IUIAutomation)
			_IUIAutomation.CreateInstance(__uuidof(CUIAutomation), CLSCTX_INPROC_SERVER);

		if(_IUIAutomation)
		{
			IComPtr<IUIAutomationElement> pIUIAutomationElement;
			if(SUCCEEDED(_IUIAutomation->ElementFromPoint(pt, pIUIAutomationElement)) && pIUIAutomationElement)
			{
				struct elem_t {
					BSTR type = nullptr;
					BSTR name = nullptr;
					BSTR id = nullptr;
					~elem_t() {
						freeString(type);
						freeString(name);
						freeString(id);
					}
					void freeString(BSTR s) {
						if(s) DLL::Invoke(L"OleAut32.dll", "SysFreeString", s);
					}
				} elem;
				
				pIUIAutomationElement->get_CurrentAutomationId(&elem.id);
				pIUIAutomationElement->get_CurrentClassName(&elem.type);
				pIUIAutomationElement->get_CurrentName(&elem.name);
				/*
				id=[TaskbarFrame], type=[Taskbar.TaskbarFrameAutomationPeer], name=[]
				id=[StartButton], type=[ToggleButton], name=[Start]
				id=[SearchButton], type=[ToggleButton], name=[Search]
				id=[SystemTrayIcon], type=[SystemTray.NormalButton], name=[Show Hidden Icons]
				id=[SystemTrayIcon], type=[SystemTray.AccentButton], name=[Network wifi-1 Internet access]
				id=[SystemTrayIcon], type=[SystemTray.OmniButton], name=[Clock 7:04 PM 3/20/2023]
				id=[SystemTrayIcon], type=[SystemTray.ShowDesktopButton], name=[Show Desktop]
				*/
				if(elem.name && elem.type && elem.id)
				{
					if(!::lstrcmpiW(elem.id, L"TaskbarFrame"))
						ret = !::lstrcmpiW(elem.name, L"") && !::lstrcmpiW(elem.type, L"Taskbar.TaskbarFrameAutomationPeer");
					else if(is_secondary(hTaskbar))
					{
						if(!::lstrcmpiW(elem.id, L"SystemTrayIcon"))
							ret = !::lstrcmpiW(elem.type, L"SystemTray.OmniButton");
						//_log.info(L"id=[%s],\t[%s], [%s]", elem.id, elem.type, elem.name);
					}
				}
			}
		}
		return ret;
	}

	static bool is_primary(HWND hTaskbar)
	{
		if(hTaskbar)
		{
			Window taskbar = hTaskbar;
			if(taskbar.hash() == WindowClass::WC_Shell_TrayWnd)
			{
				// Only the primary taskbar has a RebarWindow32
				return taskbar.find(Windows::WC_ReBarWindow32);
			}
		}
		return false;
	}

	static bool is_secondary(HWND hTaskbar)
	{
		return hTaskbar && (Window::class_hash(hTaskbar) == WindowClass::WC_Shell_SecondaryTrayWnd);
	}
};

// ===== Win10 taskbar button (MSTaskListWClass) right-click takeover =====
// Forward declarations: both symbols are defined/declared further down in this file.
extern bool is_in_taskbar;
extern bool in_taskbar_button_menu;
// Defined near the bottom of this file; needed here by the button menu and command.kill_process_tree.
extern std::vector<HWND> taskbar_button_windows;
BOOL WINAPI TrackPopupMenuExProc(HMENU hMenu, uint32_t uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);
// NOTE: do not reuse UxSubclass as the subclass marker: ContextMenu::Initialize() uses
// `if(0 == ::GetPropW(hwnd.owner, UxSubclass))` to detect windows Shell already handled,
// and when it matches, build_system_menuitems is skipped, so the items we pass in are ignored.
constexpr auto TaskListSubclassProp = L"ShellTaskListSubclass";

// Why this works: on Win10 the button menu is a ShellExperienceHost XAML jump list (not a Win32
// menu), so it cannot be hooked. Instead subclass the taskbar button area window, swallow
// WM_CONTEXTMENU and hand our own HMENU to Shell, which snapshots and repaints it with its own theme.
struct taskbar_button_info
{
	bool is_button{};   // true only when the click landed on a real app button
	// NOTE: the pid UIA reports for a taskbar button is the shell process, not the app process.
	int pid{};
	HWND window{};              // main window (first match)
	std::vector<HWND> windows;  // all matched windows (a merged button must be able to close them all)
	string title;   // e.g. "Task Manager - 1 running window"
};

static taskbar_button_info query_taskbar_button(const Point &pt)
{
	taskbar_button_info out;
	if(!taskbar_t::_IUIAutomation)
		taskbar_t::_IUIAutomation.CreateInstance(__uuidof(CUIAutomation), CLSCTX_INPROC_SERVER);
	if(!taskbar_t::_IUIAutomation)
		return out;

	IComPtr<IUIAutomationElement> elem;
	if(FAILED(taskbar_t::_IUIAutomation->ElementFromPoint(pt, elem)) || !elem)
		return out;

	// MSTaskListWClass covers the whole button area, including the gaps between buttons.
	// Only button-like control types count as an app button; clicks in the gaps fall through so
	// explorer can show its own taskbar menu (Shell takes that one over). WARNING: a merged button
	// (app with several windows) is exposed as MenuItem, not Button (measured: Edge with 3 windows).
	CONTROLTYPEID ctype = 0;
	elem->get_CurrentControlType(&ctype);
	if(ctype != UIA_ButtonControlTypeId && ctype != UIA_MenuItemControlTypeId && ctype != UIA_SplitButtonControlTypeId)
		return out;

	BSTR name = nullptr;
	elem->get_CurrentName(&name);
	if(name)
	{
		out.title = name;
		DLL::Invoke(L"OleAut32.dll", "SysFreeString", name);
	}
	int epid = 0;
	elem->get_CurrentProcessId(&epid);
	out.pid = epid;
	out.is_button = !out.title.empty();
	return out;
}

struct taskbar_window_finder
{
	string keyword;   // app name keyword parsed from the button name
	bool file_explorer{};     // File Explorer button: allow matching by window class (it has no AppUserModel properties)
	std::vector<HWND> all;    // all matched windows
};

// Chromium apps (Edge/Chrome) insert zero-width characters into their window titles:
// the real title is "Microsoft<U+200B> Edge", so matching "Microsoft Edge" with wcsstr always fails.
// Strip zero-width characters and turn non-breaking spaces into plain spaces before comparing.
static void normalize_for_match(wchar_t *s)
{
	if(!s)
		return;
	wchar_t *d = s;
	for(const wchar_t *p = s; *p; ++p)
	{
		if(*p == 0x200B || *p == 0x200C || *p == 0x200D || *p == 0xFEFF || *p == 0x2060)
			continue;
		*d++ = (*p == 0x00A0) ? L' ' : *p;
	}
	*d = 0;
}

// A window carries the full identity of its taskbar entry (all public properties, no name guessing):
//   AppUserModel.ID                          - grouping key
//   AppUserModel.RelaunchCommand             - also used for the taskbar AppID. Without it a pinned
//                                              shortcut becomes a second icon (measured Blender: the
//                                              lnk had only ID, target was blender.exe while the window relaunched blender-launcher.exe)
//   AppUserModel.RelaunchDisplayNameResource - taskbar button name (measured: Blender -> 'Blender 5.2')
//   AppUserModel.RelaunchIconResource        - icon
struct window_appmodel
{
	wchar_t aumid[256]{};
	wchar_t relaunch_command[1024]{};
	wchar_t relaunch_display_name[512]{};
	wchar_t relaunch_icon[512]{};
};

static void read_prop_str(IPropertyStore *ps, REFPROPERTYKEY key, wchar_t *out, size_t outLen)
{
	out[0] = 0;
	PROPVARIANT pv{};
	if(SUCCEEDED(ps->GetValue(key, &pv)))
	{
		if(pv.vt == VT_LPWSTR && pv.pwszVal)
			::wcsncpy_s(out, outLen, pv.pwszVal, outLen - 1);
		::PropVariantClear(&pv);
	}
}

static bool get_window_appmodel(HWND hWnd, window_appmodel &out)
{
	if(!hWnd)
		return false;
	IPropertyStore *ps = nullptr;
	if(FAILED(::SHGetPropertyStoreForWindow(hWnd, IID_IPropertyStore, reinterpret_cast<void **>(&ps))) || !ps)
		return false;
	read_prop_str(ps, PKEY_AppUserModel_ID, out.aumid, 256);
	read_prop_str(ps, PKEY_AppUserModel_RelaunchCommand, out.relaunch_command, 1024);
	read_prop_str(ps, PKEY_AppUserModel_RelaunchDisplayNameResource, out.relaunch_display_name, 512);
	read_prop_str(ps, PKEY_AppUserModel_RelaunchIconResource, out.relaunch_icon, 512);
	ps->Release();
	// NOTE: an empty AUMID still counts as success (explorer windows carry no properties at all);
	// the caller has to fall back to the exe path.
	return true;
}

// Look up an app's display name in shell:AppsFolder - that is exactly the taskbar button name.
// The argument may be an AppUserModelID (Microsoft.Windows.Explorer -> "File Explorer") or an
// exe path (Blender registers no AUMID; ...\blender-launcher.exe -> "Blender 5.2").
static bool appsfolder_display_name(const wchar_t *aumid_or_path, wchar_t *out, size_t outLen)
{
	out[0] = 0;
	if(!aumid_or_path || !aumid_or_path[0])
		return false;

	wchar_t spec[1088]{};
	::swprintf_s(spec, 1088, L"shell:AppsFolder\\%s", aumid_or_path);
	IShellItem *item = nullptr;
	if(FAILED(::SHCreateItemFromParsingName(spec, nullptr, IID_IShellItem, reinterpret_cast<void **>(&item))) || !item)
		return false;

	bool ok = false;
	LPWSTR name = nullptr;
	if(SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name)
	{
		::wcsncpy_s(out, outLen, name, outLen - 1);
		ok = out[0] != 0;
		::CoTaskMemFree(name);
	}
	item->Release();
	return ok;
}

// Fallback when the full exe path is not found: AppsFolder entries do not always key on the full
// exe path - cmd.exe is shell:AppsFolder\{1AC14E77-02E7-4E5D-B744-2EB1AE5198B7}\cmd.exe (the
// System32 KNOWNFOLDERID), so querying with C:\Windows\system32\cmd.exe always fails.
// Match by exe file name instead and take that entry's display name (cmd.exe -> "Command Prompt").
// The table is built once and cached.
static const std::map<std::wstring, std::wstring> &appsfolder_name_map()
{
	static std::once_flag once;
	static std::map<std::wstring, std::wstring> cache;
	std::call_once(once, []()
	{
		IShellItem *folder = nullptr;
		if(FAILED(::SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_IShellItem,
												reinterpret_cast<void **>(&folder))) || !folder)
			return;

		IEnumShellItems *en = nullptr;
		if(SUCCEEDED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_IEnumShellItems,
										   reinterpret_cast<void **>(&en))) && en)
		{
			IShellItem *child = nullptr;
			while(en->Next(1, &child, nullptr) == S_OK)
			{
				LPWSTR path = nullptr;
				if(SUCCEEDED(child->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path)) && path)
				{
					const wchar_t *file = ::wcsrchr(path, L'\\');
					file = file ? file + 1 : path;
					if(file[0] && cache.find(file) == cache.end())
					{
						LPWSTR name = nullptr;
						if(SUCCEEDED(child->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name)
						{
							cache[file] = name;
							::CoTaskMemFree(name);
						}
					}
					::CoTaskMemFree(path);
				}
				child->Release();
			}
			en->Release();
		}
		folder->Release();
	});
	return cache;
}

static bool appsfolder_display_name_by_exe_filename(const wchar_t *exe_path, wchar_t *out, size_t outLen)
{
	out[0] = 0;
	if(!exe_path || !exe_path[0])
		return false;

	const wchar_t *file = ::wcsrchr(exe_path, L'\\');
	file = file ? file + 1 : exe_path;
	if(!file[0])
		return false;

	auto &map = appsfolder_name_map();
	auto it = map.find(file);
	if(it == map.end())
		return false;

	::wcsncpy_s(out, outLen, it->second.c_str(), outLen - 1);
	return out[0] != 0;
}

// The name this window has on the taskbar: AppsFolder by AUMID first, then by exe path.
static bool window_button_name(HWND hWnd, wchar_t *out, size_t outLen)
{
	out[0] = 0;
	window_appmodel am;
	get_window_appmodel(hWnd, am);   // empty strings when unavailable; the exe-path fallback follows
	if(am.aumid[0] && appsfolder_display_name(am.aumid, out, outLen))
		return true;

	DWORD pid = 0;
	::GetWindowThreadProcessId(hWnd, &pid);
	if(!pid)
		return false;
	HANDLE hp = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if(!hp)
		return false;
	wchar_t exe[MAX_PATH]{};
	DWORD size = MAX_PATH;
	bool ok = ::QueryFullProcessImageNameW(hp, 0, exe, &size) != FALSE;
	::CloseHandle(hp);
	if(!ok)
		return false;

	if(appsfolder_display_name(exe, out, outLen))
		return true;

	// Console apps (cmd.exe): AppsFolder does not key them by full exe path, so match by file name.
	return appsfolder_display_name_by_exe_filename(exe, out, outLen);
}

// File Explorer is the only app that needs a special case: its windows carry no AppUserModel
// properties at all (measured AUMID vt=0) and `shell:AppsFolder\C:\Windows\explorer.exe` cannot
// be resolved, yet the taskbar still shows "File Explorer" / "Windows Explorer" by internal Windows rules.
// So this button is recognized by name and its windows by the CabinetWClass fallback.
static bool is_file_explorer_button(const wchar_t *button_name)
{
	if(!button_name || !button_name[0])
		return false;

	wchar_t key[256]{};
	const wchar_t *sep = ::wcsstr(button_name, L" - ");
	size_t n = sep ? static_cast<size_t>(sep - button_name) : ::wcslen(button_name);
	if(n == 0 || n >= 256)
		return false;
	::wcsncpy_s(key, 256, button_name, n);

	// A running button is "File Explorer"; once pinned the lnk display name makes it "Windows Explorer".
	return ::_wcsicmp(key, L"\u6587\u4EF6\u8D44\u6E90\u7BA1\u7406\u5668") == 0 ||
		::_wcsicmp(key, L"Windows \u8D44\u6E90\u7BA1\u7406\u5668") == 0 ||
		::_wcsicmp(key, L"File Explorer") == 0;
}

// A taskbar button name looks like "Task Manager - 1 running window"; take the part before " - " and
// match in three steps: exact title, title substring, same process name.
static BOOL CALLBACK find_app_window_proc(HWND hWnd, LPARAM lParam)
{
	auto f = reinterpret_cast<taskbar_window_finder *>(lParam);
	if(!::IsWindowVisible(hWnd) || ::GetWindow(hWnd, GW_OWNER))
		return TRUE;
	if(::GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
		return TRUE;

	wchar_t title[512]{};
	int tlen = ::GetWindowTextW(hWnd, title, 512);
	if(tlen > 0)
		normalize_for_match(title);

	const wchar_t *btn = f->keyword.c_str();
	const wchar_t *sep = ::wcsstr(btn, L" - ");
	size_t n = sep ? static_cast<size_t>(sep - btn) : ::wcslen(btn);
	if(n == 0 || n >= 256)
		return TRUE;
	wchar_t key[256]{};
	::wcsncpy_s(key, 256, btn, n);
	normalize_for_match(key);

	bool matched = false;
	// 1) exact title  2) title substring
	if(tlen > 0 && ::_wcsicmp(title, key) == 0)
		matched = true;
	else if(tlen > 0 && ::wcsstr(title, key))
		matched = true;
	else
	{
		// 3) same process name (QQ.exe / WeChat.exe ...)
		DWORD pid = 0;
		::GetWindowThreadProcessId(hWnd, &pid);
		if(pid)
		{
			wchar_t path[MAX_PATH]{};
			if(HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid))
			{
				DWORD size = MAX_PATH;
				if(::QueryFullProcessImageNameW(hProcess, 0, path, &size))
				{
					const wchar_t *file = ::wcsrchr(path, L'\\');
					file = file ? file + 1 : path;
					wchar_t base[MAX_PATH]{};
					::wcsncpy_s(base, MAX_PATH, file, MAX_PATH);
					if(wchar_t *dot = ::wcsrchr(base, L'.'))
						*dot = 0;
					matched = ::_wcsicmp(base, key) == 0;
				}
				::CloseHandle(hProcess);
			}
		}
	}

	// 4) match against the name this window has on the taskbar (the shell:AppsFolder display name)
	//    (window AUMID Microsoft.Windows.Explorer -> "File Explorer"; apps without an AUMID are
	//     looked up by exe path, e.g. ...\blender-launcher.exe -> "Blender 5.2").
	//    This step ignores the window title, so apps whose title differs from the button name still match.
	if(!matched)
	{
		wchar_t btn_name[512]{};
		if(window_button_name(hWnd, btn_name, 512))
		{
			normalize_for_match(btn_name);
			matched = ::_wcsicmp(btn_name, key) == 0;
		}
	}

	// 5) Last resort: File Explorer windows have no AppUserModel properties and are absent from
	//    AppsFolder, so they can only be matched by class (two folder windows = two CabinetWClass).
	if(!matched && f->file_explorer)
	{
		wchar_t cls[64]{};
		if(::GetClassNameW(hWnd, cls, 64) > 0 &&
			(::_wcsicmp(cls, L"CabinetWClass") == 0 || ::_wcsicmp(cls, L"ExploreWClass") == 0))
			matched = true;
	}

	if(!matched)
		return TRUE;

	// Collect ALL matching windows: a merged button must be able to close every window at once.
	// Prefer a non-minimized main window: UWP apps often have a minimized ApplicationFrameWindow
	// plus the visible CoreWindow; closing only the former does nothing (measured).
	f->all.push_back(hWnd);
	// WARNING: do not return FALSE early on the first non-minimized window, or windows are missed.
	return TRUE;
}

static void find_app_windows(string const &button_name, std::vector<HWND> &out)
{
	out.clear();
	if(button_name.empty())
		return;

	// Button names look like "Task Manager - 1 running window" / "Microsoft Edge (2) - 1 running window".
	// Take the part before " - "; it may still carry a " (2)" suffix (window count / profile),
	// which appears with multiple Edge profiles and breaks title matching - strip it and retry.
	wchar_t key[256]{};
	const wchar_t *btn = button_name.c_str();
	const wchar_t *sep = ::wcsstr(btn, L" - ");
	size_t n = sep ? static_cast<size_t>(sep - btn) : ::wcslen(btn);
	if(n == 0)
		return;
	if(n >= 256)
		n = 255;
	::wcsncpy_s(key, 256, btn, n);

	taskbar_window_finder f;
	f.keyword = key;
	f.file_explorer = is_file_explorer_button(button_name.c_str());
	::EnumWindows(find_app_window_proc, reinterpret_cast<LPARAM>(&f));
	if(!f.all.empty())
	{
		out = f.all;
		return;
	}

	// Strip a trailing " (number)" and match again
	wchar_t stripped[256]{};
	::wcsncpy_s(stripped, 256, key, 256);
	if(wchar_t *p = ::wcsrchr(stripped, L'('))
	{
		if(p > stripped && *(p - 1) == L' ')
		{
			wchar_t *q = p + 1;
			bool digits = *q != 0;
			for(; *q && *q != L')'; ++q)
			{
				if(*q < L'0' || *q > L'9')
				{
					digits = false;
					break;
				}
			}
			if(digits && *q == L')' && *(q + 1) == 0)
			{
				*(p - 1) = 0;
				taskbar_window_finder g;
				g.keyword = stripped;
				g.file_explorer = f.file_explorer;
				::EnumWindows(find_app_window_proc, reinterpret_cast<LPARAM>(&g));
				out = g.all;
			}
		}
	}
}

// ===== Taskbar pin / unpin =====
// The taskbar merges a pinned shortcut with running windows by AppUserModelID:
// if the .lnk AUMID differs from the window's, pinning produces two icons (that is what QQ does).
// Same for icons: an empty .lnk IconLocation shows a blank icon for some apps (Edge).

static bool taskbar_pinned_dir(wchar_t *out, size_t outLen)
{
	wchar_t appdata[MAX_PATH]{};
	if(!::SHGetSpecialFolderPathW(nullptr, appdata, CSIDL_APPDATA, FALSE))
		return false;
	::swprintf_s(out, outLen, L"%s\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar", appdata);
	return true;
}

// Does this .lnk point at that exe?
static bool lnk_targets(const wchar_t *lnk_path, const wchar_t *exe_path)
{
	IShellLinkW *link = nullptr;
	if(FAILED(::CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, reinterpret_cast<void **>(&link))) || !link)
		return false;
	bool ok = false;
	IPersistFile *pf = nullptr;
	if(SUCCEEDED(link->QueryInterface(IID_IPersistFile, reinterpret_cast<void **>(&pf))) && pf)
	{
		if(SUCCEEDED(pf->Load(lnk_path, STGM_READ)))
		{
			wchar_t target[MAX_PATH]{};
			if(SUCCEEDED(link->GetPath(target, MAX_PATH, nullptr, SLGP_UNCPRIORITY)))
				ok = ::_wcsicmp(target, exe_path) == 0;
		}
		pf->Release();
	}
	link->Release();
	return ok;
}

// The pinned items actually shown on the taskbar live in HKCU\...\Explorer\Taskband\Favorites
// (REG_BINARY). The .lnk files under User Pinned\TaskBar are only source material: unpinning leaves
// the file behind (a ghost item), so scanning that folder alone reports a wrong "pinned" state
// (measured: Microsoft Edge.lnk present, not in Favorites, no Edge button - running Edge was treated as pinned).
// The blob is searched for the lnk file name in UTF-16LE; the offset may be odd or even, so both alignments are tried.
static bool is_pinned_lnk_active(const wchar_t *lnk_path)
{
	const wchar_t *name = ::wcsrchr(lnk_path, L'\\');
	name = name ? name + 1 : lnk_path;
	size_t nlen = ::wcslen(name);
	if(nlen == 0 || nlen > 128)
		return false;

	HKEY key = nullptr;
	if(::RegOpenKeyExW(HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Taskband", 0, KEY_READ, &key) != ERROR_SUCCESS)
		return false;

	DWORD type = 0, size = 0;
	if(::RegQueryValueExW(key, L"Favorites", nullptr, &type, nullptr, &size) != ERROR_SUCCESS || size < nlen * 2)
	{
		::RegCloseKey(key);
		return false;
	}
	std::vector<BYTE> blob(size);
	bool read = ::RegQueryValueExW(key, L"Favorites", nullptr, &type, blob.data(), &size) == ERROR_SUCCESS;
	::RegCloseKey(key);
	if(!read)
		return false;

	auto fold = [](BYTE c) -> BYTE { return (c >= 'A' && c <= 'Z') ? static_cast<BYTE>(c + 32) : c; };

	for(int align = 0; align < 2; ++align)
	{
		for(size_t i = static_cast<size_t>(align); i + nlen * 2 <= size; i += 2)
		{
			bool hit = true;
			for(size_t j = 0; j < nlen; ++j)
			{
				const wchar_t wc = name[j];
				if(fold(blob[i + j * 2]) != fold(static_cast<BYTE>(wc & 0xFF)) ||
					fold(blob[i + j * 2 + 1]) != fold(static_cast<BYTE>((wc >> 8) & 0xFF)))
				{
					hit = false;
					break;
				}
			}
			if(hit)
				return true;
		}
	}
	return false;
}

// Find this exe's pinned shortcut under User Pinned\TaskBar (only real ones, no ghost items).
static bool find_pinned_lnk(const wchar_t *exe_path, wchar_t *out, size_t outLen)
{
	wchar_t dir[MAX_PATH]{};
	if(!taskbar_pinned_dir(dir, MAX_PATH))
		return false;

	wchar_t pattern[MAX_PATH]{};
	::swprintf_s(pattern, MAX_PATH, L"%s\\*.lnk", dir);
	WIN32_FIND_DATAW fd{};
	HANDLE h = ::FindFirstFileW(pattern, &fd);
	if(h == INVALID_HANDLE_VALUE)
		return false;
	bool found = false;
	do
	{
		wchar_t full[MAX_PATH]{};
		::swprintf_s(full, MAX_PATH, L"%s\\%s", dir, fd.cFileName);
		if(lnk_targets(full, exe_path) && is_pinned_lnk_active(full))
		{
			::wcsncpy_s(out, outLen, full, outLen - 1);
			found = true;
			break;
		}
	} while(::FindNextFileW(h, &fd));
	::FindClose(h);
	return found;
}

// Read the target path of a .lnk. Some pinned shortcuts have no target (the File Explorer
// lnk only sets IconLocation=%windir%\explorer.exe); then this returns false.
static bool lnk_target(const wchar_t *lnk_path, wchar_t *out, size_t outLen)
{
	out[0] = 0;
	IShellLinkW *link = nullptr;
	if(FAILED(::CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, reinterpret_cast<void **>(&link))) || !link)
		return false;
	bool ok = false;
	IPersistFile *pf = nullptr;
	if(SUCCEEDED(link->QueryInterface(IID_IPersistFile, reinterpret_cast<void **>(&pf))) && pf)
	{
		if(SUCCEEDED(pf->Load(lnk_path, STGM_READ)))
		{
			wchar_t target[MAX_PATH]{};
			if(SUCCEEDED(link->GetPath(target, MAX_PATH, nullptr, SLGP_UNCPRIORITY)) && target[0])
			{
				::wcsncpy_s(out, outLen, target, outLen - 1);
				ok = true;
			}
		}
		pf->Release();
	}
	link->Release();
	return ok;
}

// Read the localized display name of a .lnk as Explorer shows it (without extension).
// Key case: "File Explorer (2).lnk" is shown as "File Explorer" while the taskbar button
// name uses that display name, so the on-disk file name does not match.
static bool lnk_display_name(const wchar_t *lnk_path, wchar_t *out, size_t outLen)
{
	out[0] = 0;
	IShellItem *item = nullptr;
	if(FAILED(::SHCreateItemFromParsingName(lnk_path, nullptr, IID_IShellItem, reinterpret_cast<void **>(&item))) || !item)
		return false;

	bool ok = false;
	LPWSTR name = nullptr;
	if(SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name)
	{
		::wcsncpy_s(out, outLen, name, outLen - 1);
		ok = out[0] != 0;
		::CoTaskMemFree(name);
	}
	item->Release();
	return ok;
}

// A pinned button that is not running yields no window and UIA gives only a name (no
// AutomationId, no AUMID), so the button name has to be matched against the .lnk files.
// Three steps: lnk file name == button name, then without a trailing " (number)", then lnk display name / target stem.
// A match still has to pass is_pinned_lnk_active (ghost items do not count as pinned).
static bool find_pinned_lnk_by_name(const wchar_t *button_name, wchar_t *lnkOut, size_t lnkLen, wchar_t *exeOut, size_t exeLen)
{
	if(!button_name || !button_name[0])
		return false;

	// Button name looks like "Task Manager - 1 running window"; take the part before " - ".
	wchar_t key[256]{};
	const wchar_t *sep = ::wcsstr(button_name, L" - ");
	size_t n = sep ? static_cast<size_t>(sep - button_name) : ::wcslen(button_name);
	if(n == 0 || n >= 256)
		return false;
	::wcsncpy_s(key, 256, button_name, n);
	normalize_for_match(key);

	// Then strip a trailing " (number)" (the taskbar numbers duplicates, e.g. "Microsoft Edge (2)").
	wchar_t stripped[256]{};
	::wcsncpy_s(stripped, 256, key, 256);
	if(wchar_t *p = ::wcsrchr(stripped, L'('))
	{
		if(p > stripped && *(p - 1) == L' ')
		{
			wchar_t *q = p + 1;
			bool digits = *q != 0;
			for(; *q && *q != L')'; ++q)
			{
				if(*q < L'0' || *q > L'9')
				{
					digits = false;
					break;
				}
			}
			if(digits && *q == L')' && *(q + 1) == 0)
				*(p - 1) = 0;
		}
	}

	wchar_t dir[MAX_PATH]{};
	if(!taskbar_pinned_dir(dir, MAX_PATH))
		return false;

	wchar_t pattern[MAX_PATH]{};
	::swprintf_s(pattern, MAX_PATH, L"%s\\*.lnk", dir);
	WIN32_FIND_DATAW fd{};
	HANDLE h = ::FindFirstFileW(pattern, &fd);
	if(h == INVALID_HANDLE_VALUE)
		return false;

	bool found = false;
	do
	{
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		wchar_t full[MAX_PATH]{};
		::swprintf_s(full, MAX_PATH, L"%s\\%s", dir, fd.cFileName);

		// (1) lnk file name without extension
		wchar_t stem[MAX_PATH]{};
		::wcsncpy_s(stem, MAX_PATH, fd.cFileName, MAX_PATH);
		if(wchar_t *dot = ::wcsrchr(stem, L'.'))
			*dot = 0;
		normalize_for_match(stem);

		bool matched = ::_wcsicmp(stem, key) == 0;
		if(!matched && stripped[0])
			matched = ::_wcsicmp(stem, stripped) == 0;

		// (2) localized display name ("File Explorer" <-> File Explorer (2).lnk)
		if(!matched)
		{
			wchar_t disp[MAX_PATH]{};
			if(lnk_display_name(full, disp, MAX_PATH))
			{
				normalize_for_match(disp);
				matched = ::_wcsicmp(disp, key) == 0 || (stripped[0] && ::_wcsicmp(disp, stripped) == 0);
			}
		}

		// (3) target stem without extension
		if(!matched)
		{
			wchar_t target[MAX_PATH]{};
			if(lnk_target(full, target, MAX_PATH))
			{
				const wchar_t *file = ::wcsrchr(target, L'\\');
				file = file ? file + 1 : target;
				wchar_t base[MAX_PATH]{};
				::wcsncpy_s(base, MAX_PATH, file, MAX_PATH);
				if(wchar_t *dot = ::wcsrchr(base, L'.'))
					*dot = 0;
				matched = ::_wcsicmp(base, key) == 0 || (stripped[0] && ::_wcsicmp(base, stripped) == 0);
			}
		}

		if(matched)
		{
			// An unpinned ghost item that only left a file behind does not count as pinned
			if(!is_pinned_lnk_active(full))
				continue;

			::wcsncpy_s(lnkOut, lnkLen, full, lnkLen - 1);
			if(exeOut && exeLen && !lnk_target(full, exeOut, exeLen))
				exeOut[0] = 0;
			found = true;
			break;
		}
	} while(::FindNextFileW(h, &fd));
	::FindClose(h);
	return found;
}

static void set_lnk_prop(IPropertyStore *ps, REFPROPERTYKEY key, const wchar_t *value)
{
	if(!value || !value[0])
		return;
	PROPVARIANT pv{};
	pv.vt = VT_LPWSTR;
	pv.pwszVal = const_cast<LPWSTR>(value);
	ps->SetValue(key, pv);
}

// After a successful pin, fix up the shortcut that was just created:
// (1) write the WHOLE set of AppUserModel properties (ID + RelaunchCommand + RelaunchDisplayNameResource
//     + IconResource). The AUMID alone is not enough: the taskbar AppID also uses RelaunchCommand,
//     so without it the pinned item splits from the running window (measured Blender: lnk had only ID,
//     target blender.exe, window relaunch blender-launcher.exe). (2) set the icon location to avoid a blank icon.
static void fix_pinned_shortcut(const wchar_t *lnk_path, const wchar_t *exe_path, const window_appmodel &am)
{
	IShellLinkW *link = nullptr;
	if(FAILED(::CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, reinterpret_cast<void **>(&link))) || !link)
		return;
	IPersistFile *pf = nullptr;
	if(SUCCEEDED(link->QueryInterface(IID_IPersistFile, reinterpret_cast<void **>(&pf))) && pf)
	{
		if(SUCCEEDED(pf->Load(lnk_path, STGM_READWRITE)))
		{
			link->SetIconLocation(exe_path, 0);
			IPropertyStore *ps = nullptr;
			if(SUCCEEDED(link->QueryInterface(IID_IPropertyStore, reinterpret_cast<void **>(&ps))) && ps)
			{
				set_lnk_prop(ps, PKEY_AppUserModel_ID, am.aumid);
				set_lnk_prop(ps, PKEY_AppUserModel_RelaunchCommand, am.relaunch_command);
				set_lnk_prop(ps, PKEY_AppUserModel_RelaunchDisplayNameResource, am.relaunch_display_name);
				set_lnk_prop(ps, PKEY_AppUserModel_RelaunchIconResource, am.relaunch_icon);
				ps->Commit();
				ps->Release();
			}
			pf->Save(lnk_path, TRUE);
		}
		pf->Release();
	}
	link->Release();
}

// Invoke a verb of the shell context menu by its canonical, language-independent name ("TaskbarPin").
//
// Why ShellExecuteEx(verb) cannot be used directly:
//   on Win10 the "TaskbarPin" verb exists but a programmatic call is silently ignored
//   (the old "pin to taskbar" even returns ERROR_NO_ASSOCIATION) - Microsoft's user-intent guard.
//   Clicking "Pin to taskbar" in Explorer works because Explorer goes through IContextMenu::InvokeCommand.
//   This function replicates that path, which is why the pin really takes effect.
static bool invoke_shell_verb(const wchar_t *path, const wchar_t *verb_name)
{
	if(!path || !verb_name)
		return false;

	PIDLIST_ABSOLUTE pidl = nullptr;
	if(FAILED(::SHParseDisplayName(path, nullptr, &pidl, 0, nullptr)) || !pidl)
		return false;

	bool ok = false;
	IShellFolder *parent = nullptr;
	PCUITEMID_CHILD child = nullptr;
	if(SUCCEEDED(::SHBindToParent(pidl, IID_IShellFolder, reinterpret_cast<void **>(&parent), &child)) && parent)
	{
		IContextMenu *cm = nullptr;
		if(SUCCEEDED(parent->GetUIObjectOf(nullptr, 1, &child, IID_IContextMenu, nullptr, reinterpret_cast<void **>(&cm))) && cm)
		{
			HMENU menu = ::CreatePopupMenu();
			if(menu && SUCCEEDED(cm->QueryContextMenu(menu, 0, 1, 0x7FFF, CMF_NORMAL)))
			{
				// When the target verb is missing, log the enumerated verb names: the verb set differs per object
				// ("Unpin from taskbar" only exists on pinned .lnk files), which pinpoints later failures.
				wchar_t seen[512]{};
				int count = ::GetMenuItemCount(menu);
				for(int i = 0; i < count; ++i)
				{
					UINT id = ::GetMenuItemID(menu, i);
					if(id == 0 || id == static_cast<UINT>(-1))
						continue;

					wchar_t verb[128]{};
					if(SUCCEEDED(cm->GetCommandString(id - 1, GCS_VERBW, nullptr, reinterpret_cast<char *>(verb), 128)))
					{
						if(verb[0])
						{
							size_t used = ::wcslen(seen);
							if(used > 0 && used < 500)
								seen[used++] = L',';
							for(const wchar_t *p = verb; *p && used < 505; ++p)
								seen[used++] = *p;
							seen[used] = 0;
						}
						if(::_wcsicmp(verb, verb_name) == 0)
						{
							CMINVOKECOMMANDINFOEX ci{};
							ci.cbSize = sizeof(ci);
							ci.fMask = CMIC_MASK_UNICODE | CMIC_MASK_FLAG_NO_UI;
							ci.lpVerb = MAKEINTRESOURCEA(id - 1);
							ci.lpVerbW = MAKEINTRESOURCEW(id - 1);
							ci.nShow = SW_SHOWNORMAL;
							ok = SUCCEEDED(cm->InvokeCommand(reinterpret_cast<LPCMINVOKECOMMANDINFO>(&ci)));
							break;
						}
					}
				}
				if(!ok && !seen[0])
					_log.info(L"[BTN] verb '%s' not found on '%s' (no verbs)", verb_name, path);
				else if(!ok)
					_log.info(L"[BTN] verb '%s' not found on '%s' (verbs: %s)", verb_name, path, seen);
			}
			if(menu)
				::DestroyMenu(menu);
			cm->Release();
		}
		parent->Release();
	}
	::CoTaskMemFree(pidl);
	return ok;
}

// Microsoft's official "unpin from taskbar / Start menu" API (CLSID_StartMenuPin, implemented in shell32.dll).
// It only offers RemoveFromList: there is no official pin API (to keep programs from changing the
// taskbar), so pinning uses the shell verb and unpinning falls back to this API.
// Measured case: File Explorer (2).lnk has no taskbarunpin verb at all
// (the enumerated set is only open/opencontaining/OpenNewWindow/cut/copy/link/delete/properties),
// yet it really is pinned in Taskband\Favorites, so only this API can unpin it.
static bool start_menu_remove_from_list(const wchar_t *path)
{
	if(!path || !path[0])
		return false;

	IStartMenuPinnedList *pinned = nullptr;
	if(FAILED(::CoCreateInstance(CLSID_StartMenuPin, nullptr, CLSCTX_INPROC_SERVER, IID_IStartMenuPinnedList, reinterpret_cast<void **>(&pinned))) || !pinned)
	{
		_log.info(L"[BTN] StartMenuPin: CoCreateInstance failed");
		return false;
	}

	IShellItem *item = nullptr;
	HRESULT hr = ::SHCreateItemFromParsingName(path, nullptr, IID_IShellItem, reinterpret_cast<void **>(&item));
	if(FAILED(hr) || !item)
	{
		_log.info(L"[BTN] StartMenuPin: SHCreateItemFromParsingName('%s') hr=0x%08X", path, (unsigned)hr);
		pinned->Release();
		return false;
	}

	hr = pinned->RemoveFromList(item);
	_log.info(L"[BTN] StartMenuPin::RemoveFromList('%s') hr=0x%08X", path, (unsigned)hr);
	item->Release();
	pinned->Release();
	return SUCCEEDED(hr);
}

// Runs on a background thread: QueryContextMenu loads many shell extensions and must not block
// explorer's UI thread. 'lnk' is the pinned shortcut path (may be empty); the unpin verb lives there.
static void set_taskbar_pin_async(string path, string lnk, HWND hWnd, bool pin)
{
	std::thread([path, lnk, hWnd, pin]()
	{
		::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

		// Before pinning, capture the whole AppUserModel property set of the running window and write it into
		// the shortcut afterwards. The AUMID alone is not enough (the AppID also uses RelaunchCommand), otherwise it becomes two icons.
		window_appmodel am;
		if(pin)
		{
			get_window_appmodel(hWnd, am);

			// File Explorer is the exception: explorer windows carry no AppUserModel property at all,
			// yet the pinned shortcut needs this set to merge with the running window (measured: after writing
			// it and restarting explorer the pinned item merged with the folder window). These are the values
			// Windows itself uses: Microsoft.Windows.Explorer in AppsFolder displays as "File Explorer".
			const wchar_t *file = ::wcsrchr(path.c_str(), L'\\');
			file = file ? file + 1 : path.c_str();
			if(!am.aumid[0] && ::_wcsicmp(file, L"explorer.exe") == 0)
			{
				::wcsncpy_s(am.aumid, 256, L"Microsoft.Windows.Explorer", 255);
				if(!am.relaunch_command[0])
					::wcsncpy_s(am.relaunch_command, 1024, L"\"C:\\Windows\\explorer.exe\"", 1023);
				if(!am.relaunch_display_name[0])
					::wcsncpy_s(am.relaunch_display_name, 512, L"\u6587\u4EF6\u8D44\u6E90\u7BA1\u7406\u5668", 511);
			}
		}

		// The "unpin from taskbar" verb only exists on pinned .lnk files (measured: msedge.exe,
		// DeepSeek Harness.exe, notepad.exe and plain .lnk files do not have it), so calling it on an exe
		// enumerates nothing - that was the root cause of unpin working only sometimes.
		// Therefore try the .lnk first and fall back to the exe path.
		bool ok = false;
		if(!pin && !lnk.empty())
			ok = invoke_shell_verb(lnk.c_str(), L"taskbarunpin");
		if(!ok)
			ok = invoke_shell_verb(path.c_str(), pin ? L"TaskbarPin" : L"TaskbarUnpin");
		// Some pinned .lnk files have no taskbarunpin verb at all (measured: File Explorer), so fall
		// back to the official API (unpin only).
		if(!ok && !pin)
			ok = start_menu_remove_from_list(lnk.empty() ? path.c_str() : lnk.c_str());
		_log.info(L"[BTN] %s exe='%s' lnk='%s' -> %s",
			pin ? L"TaskbarPin" : L"TaskbarUnpin", path.c_str(), lnk.c_str(), ok ? L"ok" : L"failed");

		// After a successful pin, patch the shortcut: AUMID + icon location
		if(pin && ok)
		{
			for(int i = 0; i < 25; ++i)
			{
				::Sleep(200);
				wchar_t lnk[MAX_PATH]{};
				if(find_pinned_lnk(path.c_str(), lnk, MAX_PATH))
				{
					fix_pinned_shortcut(lnk, path.c_str(), am);
					_log.info(L"[BTN] patched lnk aumid='%s' relaunch='%s' name='%s'",
						am.aumid, am.relaunch_command, am.relaunch_display_name);
					break;
				}
			}
		}
		::CoUninitialize();
	}).detach();
}

// "Close window" requests a close, it does NOT kill the process (killing loses unsaved data).
//   1) UIA WindowPattern.Close(): the standard close semantics (like clicking the title-bar X).
//      UWP / modern apps only honour this one - the Settings window ignored WM_CLOSE,
//      WM_SYSCOMMAND/SC_CLOSE and SendMessage(WM_CLOSE) (measured).
//   2) Fallback for plain Win32 windows: WM_CLOSE.
// UIA calls can be slow, so this runs on a background thread to keep explorer's UI responsive.
static bool close_window_via_uia(HWND hWnd)
{
	if(!hWnd || !taskbar_t::_IUIAutomation)
		return false;

	IComPtr<IUIAutomationElement> elem;
	if(FAILED(taskbar_t::_IUIAutomation->ElementFromHandle(hWnd, elem)) || !elem)
		return false;

	IUnknown *unk = nullptr;
	if(FAILED(elem->GetCurrentPattern(UIA_WindowPatternId, &unk)) || !unk)
		return false;

	bool ok = false;
	IUIAutomationWindowPattern *wp = nullptr;
	if(SUCCEEDED(unk->QueryInterface(__uuidof(IUIAutomationWindowPattern), reinterpret_cast<void **>(&wp))) && wp)
	{
		ok = SUCCEEDED(wp->Close());
		wp->Release();
	}
	unk->Release();
	return ok;
}

static void close_app_window_async(HWND hWnd)
{
	if(!hWnd)
		return;
	std::thread([hWnd]()
	{
		if(close_window_via_uia(hWnd))
			return;
		::PostMessageW(hWnd, WM_CLOSE, 0, 0);
	}).detach();
}

// Reduce the matched windows to the top-level windows to close: de-duplicate by GA_ROOT
// (UWP exposes a CoreWindow and its ApplicationFrameWindow host, which are the same window).
static void distinct_top_windows(const std::vector<HWND> &in, std::vector<HWND> &out)
{
	out.clear();
	for(HWND w : in)
	{
		HWND root = ::GetAncestor(w, GA_ROOT);
		if(!root)
			root = w;
		bool dup = false;
		for(HWND t : out)
		{
			if(t == root)
			{
				dup = true;
				break;
			}
		}
		if(!dup)
			out.push_back(root);
	}
}

// Bring a window to the foreground. Why this is so verbose:
//   explorer runs at medium integrity while a window like Task Manager (run as admin) is high:
//   - SetForegroundWindow is often refused by the foreground lock (caller is not the foreground);
//   - the usual AttachThreadInput fallback needs the target thread's input queue and UIPI blocks it
//     across integrity levels. So try them in increasing order of cost; SwitchToThisWindow, the
//     legacy taskbar API, is last. Minimized windows only need ShowWindow (no input queue), which is why they come up but background ones do not.
extern "C" __declspec(dllimport) void WINAPI SwitchToThisWindow(HWND hwnd, BOOL fUnknown);

static bool activate_top_window(HWND hWnd)
{
	if(!hWnd || !::IsWindow(hWnd))
		return false;

	if(::IsIconic(hWnd))
		::ShowWindow(hWnd, SW_RESTORE);
	else
		::ShowWindow(hWnd, SW_SHOW);

	if(::GetForegroundWindow() == hWnd)
		return true;
	if(::SetForegroundWindow(hWnd))
		return true;

	// Fallback 1: attach to the current foreground thread's input queue (same integrity only; UIPI blocks elevated targets)
	if(auto fg = ::GetForegroundWindow())
	{
		const auto tid_fg = ::GetWindowThreadProcessId(fg, nullptr);
		const auto tid_me = ::GetCurrentThreadId();
		if(tid_fg && tid_fg != tid_me && ::AttachThreadInput(tid_me, tid_fg, TRUE))
		{
			const bool ok = ::SetForegroundWindow(hWnd) != FALSE;
			::AttachThreadInput(tid_me, tid_fg, FALSE);
			if(ok)
				return true;
		}
	}

	// Fallback 2: simulate an ALT keypress - the foreground lock is released on keyboard input, works for elevated windows too
	::keybd_event(VK_MENU, 0, 0, 0);
	::keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
	if(::SetForegroundWindow(hWnd))
		return true;

	// Fallback 3: the legacy API the taskbar uses internally
	::SwitchToThisWindow(hWnd, TRUE);
	return ::GetForegroundWindow() == hWnd;
}

static void show_taskbar_button_menu(HWND hWnd, const Point &pt, taskbar_button_info &info)
{
	find_app_windows(info.title, info.windows);
	info.window = info.windows.empty() ? nullptr : info.windows.front();

	HMENU menu = ::CreatePopupMenu();
	if(!menu)
		return;

	const UINT id_pin = 0x1001;
	const UINT id_close = 0x1002;
	const UINT id_unpin = 0x1003;
	const UINT id_activate = 0x1004;   // app-name item: clicking it activates the window (or launches the app)

	// Non-ASCII literals are written as \u escapes so the file stays independent of the compiler's
	// source encoding (see the L"\u56FA..." literals below).
	const wchar_t *text_pin = L"\u56FA\u5B9A\u5230\u4EFB\u52A1\u680F";                       // Pin to taskbar
	const wchar_t *text_unpin = L"\u4ECE\u4EFB\u52A1\u680F\u53D6\u6D88\u56FA\u5B9A";         // Unpin from taskbar
	const wchar_t *text_close = L"\u5173\u95ED\u7A97\u53E3";                                 // Close window
	const wchar_t *text_close_all = L"\u5173\u95ED\u6240\u6709\u7A97\u53E3";                 // Close all windows

	auto title = info.title.empty() ? string(L"Application") : info.title;
	// The app-name item is clickable: with a running window it is brought to the foreground (same as
	// clicking the button); otherwise the app is launched. Previously the id was 0 and nothing happened.
	::AppendMenuW(menu, MF_STRING, id_activate, title.c_str());
	::AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

	// Pin / unpin: show one based on the current state; remember the exe / pinned .lnk for the action.
	wchar_t exe_path[MAX_PATH]{};
	bool has_exe = false;
	if(info.window)
	{
		DWORD pid = 0;
		::GetWindowThreadProcessId(info.window, &pid);
		if(pid)
		{
			if(HANDLE hp = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid))
			{
				DWORD size = MAX_PATH;
				if(::QueryFullProcessImageNameW(hp, 0, exe_path, &size))
					has_exe = true;
				::CloseHandle(hp);
			}
		}
	}

	// A not-running (pinned-only) button gives no exe above, so look the .lnk up by button name -
	// and take its target as the exe (lnk files like File Explorer's have no target, only the .lnk).
	wchar_t pinned_lnk[MAX_PATH]{};
	bool has_pinned_lnk = false;
	if(!has_exe)
	{
		has_pinned_lnk = find_pinned_lnk_by_name(info.title.c_str(), pinned_lnk, MAX_PATH, exe_path, MAX_PATH);
		if(has_pinned_lnk && exe_path[0])
			has_exe = true;
	}
	// Running pinned app: look the pinned .lnk up from the exe (the unpin verb lives on the .lnk).
	if(!has_pinned_lnk && has_exe)
		has_pinned_lnk = find_pinned_lnk(exe_path, pinned_lnk, MAX_PATH);

	if(has_pinned_lnk)
		::AppendMenuW(menu, MF_STRING, id_unpin, text_unpin);
	else if(has_exe)
		::AppendMenuW(menu, MF_STRING, id_pin, text_pin);

	// "Close window" is only offered when a window really exists; otherwise it would do nothing.
	// With several windows (merged button) it becomes "Close all windows" and closes them at once.
	std::vector<HWND> close_targets;
	distinct_top_windows(info.windows, close_targets);
	if(!close_targets.empty())
		::AppendMenuW(menu, MF_STRING, id_close, close_targets.size() > 1 ? text_close_all : text_close);

	_log.info(L"[BTN] pid=%d window=0x%x windows=%d class='%s' title='%s' exe='%s' lnk='%s'",
		info.pid, (unsigned)(ULONG_PTR)info.window, static_cast<int>(close_targets.size()),
		info.window ? Window::class_name(info.window).c_str() : L"<null>",
		info.title.c_str(), exe_path, pinned_lnk);

	// The menu must take the foreground/focus first: otherwise keyboard navigation does not work, the
	// first mouse click only activates it, and any other process taking focus closes it (returns 0).
	if(auto root = ::GetAncestor(hWnd, GA_ROOT))
		::SetForegroundWindow(root);
	::SetForegroundWindow(hWnd);

	// Used by command.kill_process_tree: the top-level windows of this button, de-duplicated by GA_ROOT.
	taskbar_button_windows = close_targets;

	is_in_taskbar = true;
	in_taskbar_button_menu = true;
	auto cmd = TrackPopupMenuExProc(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, hWnd, nullptr);
	in_taskbar_button_menu = false;
	is_in_taskbar = false;

	_log.info(L"[BTN] command=%d", static_cast<int>(cmd));

	if(cmd == id_activate)
	{
		if(info.window)
		{
			const bool ok = activate_top_window(info.window);
			// Log success and the actual foreground window, so "clicked but nothing happened" is easy to diagnose.
			_log.info(L"[BTN] activate window=0x%x ok=%d fg=0x%x exe='%s'",
					  (unsigned)(ULONG_PTR)info.window, ok ? 1 : 0,
					  (unsigned)(ULONG_PTR)::GetForegroundWindow(), exe_path);
		}
		else
		{
			// Not running: launch it. Prefer the pinned .lnk (it keeps the original working dir / arguments).
			const wchar_t *target = has_pinned_lnk ? pinned_lnk : (has_exe ? exe_path : nullptr);
			if(target)
			{
				_log.info(L"[BTN] launch '%s'", target);
				::ShellExecuteW(nullptr, nullptr, target, nullptr, nullptr, SW_SHOWNORMAL);
			}
		}
	}
	else if(cmd == id_close && !close_targets.empty())
	{
		// The inner Windows.UI.Core.CoreWindow of UWP apps (Settings, Store, ...) ignores SC_CLOSE, so the
		// close message must go to the top-level host ApplicationFrameWindow (already reduced by GA_ROOT).
		// With several windows each one gets its own close request; the app can still show save/cancel dialogs.
		_log.info(L"[BTN] close %d window(s)", static_cast<int>(close_targets.size()));
		for(HWND t : close_targets)
			close_app_window_async(t);
	}
	else if(cmd == id_pin && has_exe)
	{
		_log.info(L"[BTN] pin '%s'", exe_path);
		set_taskbar_pin_async(exe_path, string(), info.window, true);
	}
	else if(cmd == id_unpin && has_pinned_lnk)
	{
		_log.info(L"[BTN] unpin exe='%s' lnk='%s'", exe_path, pinned_lnk);
		set_taskbar_pin_async(has_exe ? string(exe_path) : string(), string(pinned_lnk), info.window, false);
	}

	::DestroyMenu(menu);
}

// Implementation of command.kill_process_tree (called via extern from FuncExpression.cpp):
// Kill the process tree of the windows this button menu refers to. Order matters: expand all
// descendants from the Toolhelp snapshot first, then terminate leaves first - killing parents first lets Windows reparent the children and the tree leaks.
bool kill_taskbar_button_process_tree()
{
	std::vector<uint32_t> roots;
	for(auto hwnd : taskbar_button_windows)
	{
		if(!::IsWindow(hwnd))
			continue;
		DWORD pid = 0;
		::GetWindowThreadProcessId(hwnd, &pid);
		if(pid == 0)
			continue;
		bool seen = false;
		for(auto r : roots)
		{
			if(r == pid)
			{
				seen = true;
				break;
			}
		}
		if(!seen)
			roots.push_back(pid);
	}

	if(roots.empty())
	{
		_log.info(L"[KILL] no window/pid for taskbar button (windows=%d)", static_cast<int>(taskbar_button_windows.size()));
		return false;
	}

	_log.info(L"[KILL] root pid=%u", roots.front());

	auto snapshot = Diagnostics::Process::EnumInfo();

	std::vector<uint32_t> order;
	std::function<void(uint32_t)> collect = [&](uint32_t pid)
	{
		for(auto done : order)
			if(done == pid)
				return;
		for(auto &p : snapshot)
		{
			if(p.th32ProcessID != 0 && p.th32ParentProcessID == pid && p.th32ProcessID != pid)
				collect(p.th32ProcessID);
		}
		order.push_back(pid);
	};

	for(auto r : roots)
		collect(r);

	int killed = 0;
	for(auto pid : order)
	{
		if(auto hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, pid))
		{
			if(::TerminateProcess(hProcess, 0))
				killed++;
			::CloseHandle(hProcess);
		}
	}

	_log.info(L"[KILL] roots=%d processes=%d killed=%d",
			  static_cast<int>(roots.size()), static_cast<int>(order.size()), killed);
	return killed > 0;
}

LRESULT __stdcall TaskListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto proc = reinterpret_cast<WNDPROC>(::GetPropW(hWnd, TaskListSubclassProp));
	if(!proc)
		return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);

	switch(uMsg)
	{
		case WM_CONTEXTMENU:
		{
			auto pt = Point::CursorPos();
			auto info = query_taskbar_button(pt);
			if(!info.is_button)
				break;   // gap between buttons: fall through to explorer/Shell's own taskbar menu
			_log.info(L"[BTN] WM_CONTEXTMENU cursor=(%d,%d)", pt.x, pt.y);
			show_taskbar_button_menu(hWnd, pt, info);
			return 0;   // swallow only for real app buttons, preventing the XAML jump list
		}
	}
	return ::CallWindowProcW(proc, hWnd, uMsg, wParam, lParam);
}

// Hook ONE taskbar. The main taskbar (Shell_TrayWnd) and secondary taskbars
// (Shell_SecondaryTrayWnd) share the same window chain: ReBarWindow32 -> MSTaskSwWClass -> MSTaskListWClass.
static bool tasklist_hook_window(HWND tray)
{
	if(!tray)
		return false;
	auto rebar = ::FindWindowExW(tray, nullptr, Windows::WC_ReBarWindow32, nullptr);
	if(!rebar)
		return false;
	auto sw = ::FindWindowExW(rebar, nullptr, Windows::WC_MSTaskSwWClass, nullptr);
	if(!sw)
		return false;
	auto list = ::FindWindowExW(sw, nullptr, Windows::WC_MSTaskListWClass, nullptr);
	if(!list)
		return false;
	if(::GetPropW(list, TaskListSubclassProp))
		return true;   // already hooked

	auto proc = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW(list, GWLP_WNDPROC));
	::SetPropW(list, TaskListSubclassProp, reinterpret_cast<HANDLE>(proc));
	::SetWindowLongPtrW(list, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TaskListProc));
	_log.info(L"[BTN] hooked taskbar list hwnd=0x%x tray=0x%x",
			  (unsigned)(ULONG_PTR)list, (unsigned)(ULONG_PTR)tray);
	return true;
}

// Hook ALL taskbars: the main one plus every secondary taskbar (one Shell_SecondaryTrayWnd per screen on Win10).
static int tasklist_hook_all_now()
{
	int hooked = 0;
	if(tasklist_hook_window(::FindWindowW(Windows::WC_Shell_TrayWnd, nullptr)))
		hooked++;

	for(HWND prev = nullptr;;)
	{
		auto sec = ::FindWindowExW(nullptr, prev, Windows::WC_Shell_SecondaryTrayWnd, nullptr);
		if(!sec)
			break;
		if(tasklist_hook_window(sec))
			hooked++;
		prev = sec;
	}
	return hooked;
}

// Retry a few times after startup (the taskbar may not exist yet when explorer starts; 'delay' is
// the retry interval), then switch to a permanent watchdog: when the taskbar window is recreated
// in place (DPI change, monitor plug/unplug, theme switch) the subclass is dropped and re-installed
// here; otherwise the button menu silently falls back to the native XAML jump list. Only real re-hooks are logged.
static void tasklist_hook_all(uint32_t delay)
{
	std::thread([delay]()
	{
		for(int i = 0; i < 15; ++i)
		{
			::Sleep(delay);
			if(tasklist_hook_all_now() > 0)
				break;
		}
		_log.info(L"[BTN] tasklist hook ready");

		for(;;)
		{
			::Sleep(3000);
			tasklist_hook_all_now();
		}
	}).detach();
}

HRESULT __stdcall CoCreateInstanceHook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv)
{
	auto is_local_server = (dwClsContext & CLSCTX_LOCAL_SERVER) != 0;
	auto is_inproc_server = (dwClsContext & CLSCTX_INPROC_SERVER) != 0;

	if(_initializer.cache && (is_inproc_server || is_local_server))
	{
		auto hr = E_NOINTERFACE; //CLASS_E_CLASSNOTAVAILABLE;
		auto process = true;

		if(rclsid == IID_FileExplorerContextMenu /*&& riid == {706461D1-AC5F-4730-BFE3-CAC6CAD5EF5E}*/)
		{
			if(_initializer.cache->settings.priority)
			{
				Context context;
				Object obj = context.Eval(_initializer.cache->settings.priority).move();
				if(obj.is_number())
					process = obj;
			}

			if(process)
				return hr;
		}
		else
		{
			/*auto xx = Guid(L"{f3d06e7c-1e45-4a26-847e-f9fcdee59be0}");

			if(xx.equals({riid,rclsid}))
			{
				MB(L"0000000000000");
			}*/

			//if(riid== IID_IExplorerCommandProvider)
			//	_log.info(L"IID_IExplorerCommandProvider %s", Guid(rclsid).to_string(2).c_str());

			//if(riid == IID_IExecuteCommand)
			//	_log.info(L"IID_IExecuteCommand %s", Guid(rclsid).to_string(2).c_str());
			/*
			if(riid == IID_IExplorerCommandState)
			{
				hr = _CoCreateInstance.invoke(rclsid, pUnkOuter, dwClsContext, riid, ppv);

				auto cc = (IExplorerCommand *)*ppv;

				IExplorerCommand *f = 0;
				cc->QueryInterface(IID_IExplorerCommand, (void**)&f);
				//printto;rotate90;rotate270;setwallpaper;slideshow

				LPWSTR p;
				f->GetTitle(0, &p);
				_log.info(L"IExplorerCommandState %s", p);
				return hr;
			}
		*/
			auto is_UWP = false;
			process = false;
			/*
			if(Guid(rclsid).equals(L"{E6950302-61F0-4FEB-97DB-855E30D4A991}"))
			{
				*ppv = new ExplorerCommandBase;
				return S_OK;
			}
			*/
			if(is_inproc_server)
				process = riid == IID_IContextMenu || riid == IID_IContextMenu2 || riid == IID_IContextMenu3 || riid == IID_IExplorerCommand;
			else
			{
				process = riid == IID_IExplorerCommand;
				is_UWP = is_local_server && process;
			}

			if(process)
			{
				Context context;
				this_item _this; context._this = &_this;
				_this.is_uwp = is_UWP;
				_this.clsid = Guid(rclsid).to_string(2);

				bool test = Keyboard::IsKeyDown(VK_MENU);
				if(test)
				{
					Timer t;
					t.start();
					hr = _CoCreateInstance.invoke(rclsid, pUnkOuter, dwClsContext, riid, ppv);
					t.stop();
					auto elapsed = (int)t.elapsed_milliseconds();
					_log.write(L"%d%s\t%s\t%s\r\n", elapsed, elapsed > 0 ? L"ms" : L"" , _this.clsid.c_str(), is_UWP ? L"UWP":L"");
					return hr;
				}

				for(auto si : _initializer.cache->statics)
				{
					if(si->clsid.empty() || (si->where && !context.eval_bool(si->where)))
						continue;

					if(si->has_clsid)
					{
						for(auto &id : si->clsid)
						{
							if(id.equals(rclsid))
							{
								if(test && *ppv)
								{
									((IUnknown *)*ppv)->Release();
									*ppv = nullptr;
								}
								return E_NOINTERFACE;
							}
						}
					}
				}

				if(test)
					return hr;
			}
		}
	}

	return _CoCreateInstance.invoke(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

STDAPI WINAPI DllGetClassObjectHook(REFCLSID rclsid, _In_ REFIID riid, LPVOID *ppv)
{
	auto hr = CLASS_E_CLASSNOTAVAILABLE;
	if(IID_FileExplorerContextMenu == rclsid/* || rclsid == IID_FileExplorerCommandBar*/)
		return hr;
	hr = _DllGetClassObject.invoke(rclsid, riid, ppv);
	return hr;
}

#define TPM_SYSMENU	0x0200L
bool is_in_taskbar;
// True while the self-built taskbar "app button" menu is open (see show_taskbar_button_menu).
// ContextMenu uses it to tag this menu with FSO_TASKBAR_BUTTON instead of FSO_TASKBAR, so config
// items can target the button menu with where=window.is_taskbar_button while existing taskbar
// items ("Go to", imports/taskbar.nss, ...) do not leak into it.
bool in_taskbar_button_menu;
// Top-level windows of the current button menu (filled before the menu pops up); used by command.kill_process_tree.
std::vector<HWND> taskbar_button_windows;

BOOL WINAPI TrackPopupMenuProc(HMENU hMenu, uint32_t uFlags, int x, int y, int nReserved, HWND hWnd, const RECT *prcRect);
BOOL WINAPI TrackPopupMenuExProc(HMENU hMenu, uint32_t uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);

BOOL WINAPI NtUserTrackPopupMenu(HMENU hMenu, uint32_t uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm, int tfunc)
{
#ifdef _DIJA
	return (BOOL)XXX::OnContextMenu(hWnd, x, y, hMenu);;
#endif

	auto result = FALSE;
	auto invoked = false;

	auto invoke = [&](HMENU hmenu, uint32_t flag, Point pt)
	{
		if(!invoked)
		{
			if(tfunc == 0)
				result = ::TrackPopupMenu(hmenu, flag, pt.x, pt.y, 0, hWnd, nullptr);
			else if(iathook_NtUserTrackPopupMenuEx.installed())
				result = iathook_NtUserTrackPopupMenuEx.invoke<decltype(TrackPopupMenuExProc)>(hmenu, flag, pt.x, pt.y, hWnd, lptpm);
			else
				result = ::TrackPopupMenuEx(hmenu, flag, pt.x, pt.y, hWnd, lptpm);
			invoked = true;
		}
	};

	__try
	{
		__try
		{

			if(!is_registered())
			{
				__trace(L"TrackPopupMenu unregistered");
				__leave;
			}

			//cs.lock();
			auto has_inited = 0; Initializer::OnState(is_in_taskbar);

			if(!_initializer.Status.Disabled)
			{
				// is injected from explorer
				if(!_initializer.Status.Loaded)
				{
					if(_initializer.has_error() || has_inited)
						__leave; //goto skip;
					_initializer.init();
				}

				auto ctx = ContextMenu::CreateAndInitialize(hWnd, hMenu, { x, y }, _loader.explorer, _loader.contextmenuhandler);
				if(ctx != nullptr)
				{

					Flag<uint32_t> flag(uFlags);
					//Selections::point = { x, y };
					//	flag.remove(TPM_RETURNCMD);
					flag.remove(TPM_NONOTIFY);
					//	flag.remove(TPM_NOANIMATION);

				//	flag.add(TPM_HORPOSANIMATION);

				//	BOOL v = 1;
				//	SystemParametersInfoW(SPI_SETMENUANIMATION, 0, &v, SPIF_SENDCHANGE);
				//	v = 0;
				//	SystemParametersInfoW(SPI_GETMENUFADE, 0, &v, SPIF_SENDCHANGE);
					
					if(ctx->is_layoutRTL == 1)
						flag.add(TPM_LAYOUTRTL);
					else if(ctx->is_layoutRTL == 0)
						flag.remove(TPM_LAYOUTRTL);

					auto z = ctx->_theme.border.size + ctx->_theme.border.size;

					if(ctx->Selected.Window.parent == WINDOW_TASKBAR)
					{
						HWND hTaskbar = hWnd;
						if(ctx->Selected.Window.id == WINDOW_START)
							hTaskbar = ::WindowFromPoint({ x, y });
						else
							hTaskbar = ::GetAncestor(hWnd, GA_ROOTOWNER);

						if(!hTaskbar)
							hTaskbar = hWnd;

						Monitor monitor(hTaskbar);
						monitor.info();

						Rect rcW = monitor.rcWork;
						Rect rcM = monitor.rcMonitor;
						Rect rc = hTaskbar;

						if(ctx->Selected.Window.id == WINDOW_START)
						{
							if(rc.top > 0)
								y = (rc.top + 5) - (z + ctx->_theme.border.padding.height());
							else if(rc.left == 0)
								y = rc.bottom;
							else
								x = rc.left;
						}
						else
						{
							if(rcM.top == rc.top && rcM.bottom == rc.bottom) // Vertical
							{
								if(rcM.left == rc.left)
									x = rc.right;
								else if(rcM.right == rc.right)
									x = rc.left - 20;
							}
							else if(rcM.left == rc.left && rcM.right == rc.right) // Horizontal
							{
								if(rcM.top == rc.top)
									y = rc.bottom;
								else if(rcM.bottom == rc.bottom)
									y = (rc.top + 5) - (z + ctx->_theme.border.padding.height());
							}
						}
					}
					else
					{
						y -= z;
					}
					
					invoke(ctx->MenuHandle(), flag, { x, y });
				
					// v = 0;
					// SystemParametersInfoW(SPI_SETMENUANIMATION, 0, &v, SPIF_SENDCHANGE);
					result = ctx->InvokeCommand(result);
					__leave;
				}
			}
		}
		except
		{
#ifdef _DEBUG
			_log.exception(__func__);
#endif
		}
	}
	__finally
	{
		invoke(hMenu, uFlags, { x, y });
		_loader.contextmenuhandler = false;
		is_in_taskbar = false;
		return result;
	}
}

BOOL WINAPI TrackPopupMenuProc(HMENU hMenu, uint32_t uFlags, int x, int y, [[maybe_unused]] int nReserved, HWND hWnd, [[maybe_unused]] const RECT *prcRect)
{
	//_log.write(L"%s\n", Window::class_name(hWnd).c_str());
	return NtUserTrackPopupMenu(hMenu, uFlags, x, y, hWnd, nullptr, 0);
}

//win32u.dll win10 1607	Redstone 1 14393
BOOL WINAPI TrackPopupMenuExProc(HMENU hMenu, uint32_t uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm)
{
	//_log.write(L"%s\n", Window::class_name(hWnd).c_str());
	return NtUserTrackPopupMenu(hMenu, uFlags, x, y, hWnd, lptpm, 1);
}

bool ShowTaskbarContextMenu(HWND hTaskbar, const Point &pt, uint32_t uMsg)
{
	auto ret = false;
	//if(taskbar_t::is_allowed_area(hTaskbar, pt))
	{
		struct menu {
			HMENU handle{};
			menu() { handle = ::CreatePopupMenu(); }
			~menu() { if(handle) ::DestroyMenu(handle); }
		}_menu;

		if(_menu.handle)
		{
			is_in_taskbar = true;
			//::SetForegroundWindow(hTaskbar);
			auto uFlags = TPM_RETURNCMD | TPM_RIGHTBUTTON | (::GetSystemMetrics(SM_MIDEASTENABLED) ? TPM_LAYOUTRTL : 0);
			if(uMsg == WM_CONTEXTMENU)
			{
				//_log.info(L"************");
				TrackPopupMenuExProc(_menu.handle, uFlags, pt.x, pt.y, hTaskbar, nullptr);
			}
			else 
			{
				iathook_NtUserTrackPopupMenuEx.invoke<decltype(TrackPopupMenuExProc)>(_menu.handle, uFlags, 0, 0, hTaskbar, nullptr);
			}
			::PostMessageW(hTaskbar, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
			ret = true;
		}
	}
	return ret;
}

LRESULT __stdcall TaskbarProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto wndproc = (WNDPROC)::GetPropW(hWnd, UxSubclass);
	
	if(wndproc == nullptr)
		return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);

	if(uMsg == WM_CONTEXTMENU)
	{
		auto pt = Point::CursorPos();
		if(taskbar_t::is_allowed_area(hWnd, pt))
		{
			if(ShowTaskbarContextMenu(hWnd, pt, wParam == 0 && lParam == 0 ? WM_RBUTTONDOWN : WM_CONTEXTMENU))
				return 0;
		}
	}
	else if(uMsg == WM_MOUSEACTIVATE)
	{
		auto msg = HIWORD(lParam);
		if(msg != WM_RBUTTONDOWN)
			return wndproc(hWnd, uMsg, wParam, lParam);

		auto pt = Point::CursorPos();

		if(taskbar_t::is_allowed_area(hWnd, pt))
		{
			int disabled_taskbar = 0;
			if(RegistryConfig::get(L"\\disable", L"taskbar", disabled_taskbar) && disabled_taskbar == 1)
				return wndproc(hWnd, uMsg, wParam, lParam);

			if(msg == WM_RBUTTONDOWN)
			{
				Keyboard kb;
				if(auto count = kb.get_keys_excloude_contextmenu(); count > 0)
				{
					if(kb.key_ctrl())
					{
						if(count == 2 && kb.key_win())
							return wndproc(hWnd, uMsg, wParam, lParam);

						if(count == 1 && !_initializer.has_error())
						{
							if(!_initializer.Status.Loaded)
								_initializer.OnState();
						}
					}
				}
			}

			if(_initializer.has_error())
			{
				_initializer.OnState();
				if(_initializer.has_error())
					return wndproc(hWnd, uMsg, wParam, lParam);
			}
			else
			{
				if(_initializer.Status.Disabled)
					return wndproc(hWnd, uMsg, wParam, lParam);
			}

			if(!is_registered())
			{
				return wndproc(hWnd, uMsg, wParam, lParam);
			}

			::SetFocus(hWnd);
			::SendMessageW(hWnd, WM_CONTEXTMENU, 0, 0);

			return MA_ACTIVATEANDEAT;
		}
	}
	else if(uMsg == WM_SETTINGCHANGE)
	{
		if(SPI_SETWORKAREA == wParam && ::GetSystemMetrics(SM_CMONITORS) > 1)
		{
			taskbar_t::hook_all(1000);
		}
	}
	else if(uMsg == WM_DESTROY)
	{
		taskbar_t::unhook(hWnd);
		taskbar_t::hook_all(1000);
	}

	return wndproc(hWnd, uMsg, wParam, lParam);
}

LRESULT __stdcall TaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, [[maybe_unused]] UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	if(uMsg == WM_CONTEXTMENU)
	{
		auto hTaskbar = reinterpret_cast<HWND>(dwRefData);
		auto pt = Point::CursorPos();
		if(taskbar_t::is_allowed_area(hTaskbar, pt))
		{
			if(ShowTaskbarContextMenu(hTaskbar, pt, wParam == 0 && lParam == 0? WM_RBUTTONDOWN : WM_CONTEXTMENU))
				return 0;			
		}
	}
	else if(uMsg == WM_MOUSEACTIVATE)
	{
		auto lret = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
		auto msg = HIWORD(lParam);

		if(/*msg != WM_LBUTTONDOWN && */msg != WM_RBUTTONDOWN)
			return lret;

		auto hTaskbar = reinterpret_cast<HWND>(dwRefData);
		auto pt = Point::CursorPos();

		if(taskbar_t::is_allowed_area(hTaskbar, pt))
		{
			int disabled_taskbar = 0;
			if(RegistryConfig::get(L"\\disable", L"taskbar", disabled_taskbar) && disabled_taskbar == 1)
				return lret;

			if(msg == WM_RBUTTONDOWN)
			{
				Keyboard kb;
				if(auto count = kb.get_keys_excloude_contextmenu(); count > 0)
				{
					if(kb.key_ctrl())
					{
						if(count == 2 && kb.key_win())
							return lret;

						if(count == 1 && !_initializer.has_error())
						{
							if(!_initializer.Status.Loaded)
								_initializer.OnState();
						}
					}
				}
			}

			if(_initializer.has_error())
			{
				if(msg != WM_RBUTTONDOWN)
					return lret;

				_initializer.OnState();

				if(_initializer.has_error())
					return lret;
			}
			else
			{
				if(_initializer.Status.Disabled)
					return lret;
			}

			if(!is_registered())
			{
				_taskbar_mouse.unhook();
				taskbar_t::unhook(hWnd);
			//	for(auto &window : _window_taskbar)
			//		window.second.remove_prop(UxSubclass);
				return lret;
			}

			if(msg == WM_RBUTTONDOWN)
			{
				_taskbar_mouse.hook(WH_MOUSE, [](int nCode, WPARAM wParam, LPARAM lParam)->LRESULT
				{
					//auto lret = _taskbar_mouse.callnext(nCode, wParam, lParam);
					if(nCode == HC_ACTION && wParam == WM_RBUTTONUP)
					{
						auto mh = reinterpret_cast<MOUSEHOOKSTRUCT *>(lParam);
						Window window = ::GetParent(mh->hwnd);
						if(window.is_prop(UxSubclass))
						{
							_taskbar_mouse.unhook();
							::SetFocus(window);
							ShowTaskbarContextMenu(window, mh->pt, WM_CONTEXTMENU);
							return 1;
						}
					}
					return _taskbar_mouse.callnext(nCode, wParam, lParam);
				}, hWnd);
				
				return MA_ACTIVATEANDEAT;
			}

			//if(msg != WM_RBUTTONDOWN)
			//	::SetFocus(hWnd);

			/*
			if(msg == WM_RBUTTONDOWN)
			{
				::SetFocus(hWnd);
				//ShowTaskbarContextMenu(hWnd, pt, WM_RBUTTONDOWN);
				SendMessageW(hWnd, WM_CONTEXTMENU, 0, 0);
			}
			*/
		}
		return lret;
	}
	else if(uMsg == WM_SETTINGCHANGE)
	{
		if(SPI_SETWORKAREA == wParam && ::GetSystemMetrics(SM_CMONITORS) > 1)
		{
			taskbar_t::hook_all(1000);
		}
	}
	else if(uMsg == WM_DESTROY)
	{
		taskbar_t::unhook(hWnd);
		taskbar_t::hook_all(1000);
	}

	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//integrate 
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	try
	{
		if(dwReason == DLL_PROCESS_ATTACH)
		{
			::DisableThreadLibraryCalls(hInstance);
			//InitCommonControls();
			/*INITCOMMONCONTROLSEX icex{};
			// Load the ToolTip class from the DLL.
			icex.dwSize = sizeof(icex);
			icex.dwICC = ICC_BAR_CLASSES;
			InitCommonControlsEx(&icex);
			*/

			_hInstance = hInstance;
			_initializer.HInstance = _hInstance;

#if _DEBUG
			// detect memory leaks
			_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG
						   | _CRTDBG_CHECK_ALWAYS_DF
						   | _CRTDBG_LEAK_CHECK_DF
						   | _CRTDBG_ALLOC_MEM_DF);

			//::_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
			_loader.init();

			if(!_loader.explorer && !_loader.path.ends_with(L"\\shell.exe"))
			{
				
				//	if(_loader.path.ends_with(L"\\firefox.exe") || _loader.path.ends_with(L"\\devenv.exe"))
				//		return 0;
				
				int disabled_3rdparty = 0;
				if(RegistryConfig::get(L"\\disable", L"3rdparty", disabled_3rdparty) && disabled_3rdparty == 1)
					return 0;

				/*if(RegistryConfig::IsFolderExtensions())
				{
					if(!hasImportTrackPopupMenu())
						return 0;
				}*/
			}

			//if(!loader)
			//	_log.info(L"Load '%s'", Path::Module(nullptr).c_str());

			if(_initializer.init(_hInstance))
			{
				// DisableComExceptionHandling
				IComPtr<IGlobalOptions> globalOptions;
				if(globalOptions.CreateInstance(CLSID_GlobalOptions, CLSCTX_SERVER))
					globalOptions->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE_ANY);

				_initializer.process.hModule = _loader.handle;
				_initializer.process.path = Path::Module(_loader.handle).move();
				_initializer.process.name = Path::Title(_initializer.process.path).move();
				_initializer.process.id = ::GetCurrentProcessId();
				_initializer.process.handle = ::GetCurrentProcess();

				//_log.info(L"%s", _initializer.process.name.c_str());

				ContextMenu::RegisterLayer();

				iathook_NtUserTrackPopupMenuEx
					.init(L"user32.dll", "win32u.dll", "NtUserTrackPopupMenuEx", TrackPopupMenuExProc)
					.install();

				auto hook = []()
				{
					__trace(L"hook all the modules in '%s' process", _initializer.process.name.c_str());

					auto user32 = "user32.dll";

					// hooh all the modules in this process.
					for(auto hModule : Process::Modules(_initializer.process.handle))
					{
						if(_hInstance == hModule)
							continue;

						bool module_exists = false;
						for(const auto &m : iathook_TrackPopupMenu)
						{
							if(m._hModule == hModule)
							{
								module_exists = true;
								break;
							}
						}

						if(!module_exists)
						{
							iathook_TrackPopupMenu.emplace_back(hModule, user32, ::TrackPopupMenu, TrackPopupMenuProc).install();
							iathook_TrackPopupMenu.emplace_back(hModule, user32, ::TrackPopupMenuEx, TrackPopupMenuExProc).install();
						}
					}
				};

				//_TrackPopupMenu.init(::TrackPopupMenu, TrackPopupMenuProc).hook();
				//_TrackPopupMenuEx.init(::TrackPopupMenuEx, TrackPopupMenuExProc).hook();
				//_CoCreateInstance.init(::CoCreateInstance, CoCreateInstanceProc).hook();
				
				if(_loader.explorer)
				{
					//_log.info(_loader.path);

					_CoCreateInstance.Begin();
					//_DllGetClassObject.init(hModule, "DllGetClassObject", DllGetClassObjectHook).hook();
					_CoCreateInstance.init(::CoCreateInstance, CoCreateInstanceHook).hook();
					//auto m = &_detours_ci[hModule];
					//if(!m->installed())
					//m->init(hModule, "api-ms-win-core-com-l1-1-0.dll", ::CoCreateInstance, CoCreateInstanceProc).install();
					_CoCreateInstance.Commit();

					if(!iathook_NtUserTrackPopupMenuEx.installed())
					{
						std::thread([=]() { ::Sleep(2000); hook(); }).detach();
					}

					if(ver->IsWindows11OrGreater())
					{

						/*
						auto keyCLSID = Registry::ClassesRoot.OpenSubKey(L"CLSID\\" CLS_FileExplorerContextMenu "\\TreatAs", false, false);
						if(!(keyCLSID && keyCLSID.GetString(nullptr).equals(CLS_ContextMenu)))
						{
							auto hModule = ::GetModuleHandleW(Windows_UI_FileExplorer);

							if(!hModule)
								hModule = ::LoadLibraryW(Windows_UI_FileExplorer);

							if(hModule)
							{
								auto r = _CoCreateInstance.Begin();
								_log.info(L"%x", r);
								//_DllGetClassObject.init(hModule, "DllGetClassObject", DllGetClassObjectHook).hook();
								_CoCreateInstance.init(::CoCreateInstance, CoCreateInstanceHook).hook();
								//auto m = &_detours_ci[hModule];
								//if(!m->installed())
								//m->init(hModule, "api-ms-win-core-com-l1-1-0.dll", ::CoCreateInstance, CoCreateInstanceProc).install();
								_CoCreateInstance.Commit();
							}
						}
						keyCLSID.Close();
						*/

						taskbar_t::hook_all(1000);
					}
					else
					{
						// Win10: install the taskbar button area (MSTaskListWClass) hook.
						tasklist_hook_all(1000);
					}
				}
				else if(!iathook_NtUserTrackPopupMenuEx.install())
				{
					hook();
				}
			}
			//IsRegistered
			/*
			IID iid{};
			CLSIDFromString(L"{23170F69-40C1-278A-1000-000100020000}", &iid);
			Guid guid = iid;
			MB(guid.to_string(2).c_str());
			*/
			//UuidFromStringW

			return TRUE;
		}
		else if(dwReason == DLL_PROCESS_DETACH)
		{
			if(!is_registered())
			{
				if(_loader.explorer)
				{
					// Perform any necessary cleanup.
					_taskbar_mouse.unhook();
					taskbar_t::unhook_all();
					//detour_NtUserTrackPopupMenuEx.uninstall(true);

					if(ver->IsWindows11OrGreater())
					{
						_CoCreateInstance.Begin();
						//_DllGetClassObject.unhook();
						_CoCreateInstance.unhook();
						_CoCreateInstance.Commit();
					}
				}

				for(auto &d : iathook_TrackPopupMenu)
					d.uninstall(true);

				//_TrackPopupMenu.unhook();
				//_TrackPopupMenuEx.unhook();
				//_CoCreateInstance.unhook();

				ContextMenu::UnRegisterLayer();
			}

			taskbar_t::_IUIAutomation.release();

			_log.close();

			//::SendNotifyMessageW(HWND_BROADCAST, WM_NULL, 0, 0);	

			return TRUE;
		}
	}
	catch(...) // handle all exceptions
	{
#ifdef _DEBUG
		_log.exception(__func__);
		_log.close();
#endif
	}
	return FALSE;
}

//IID_FolderExtensions
//#pragma comment(linker, "/export:DllGetClassObject=DllGetClassObject")
_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, [[maybe_unused]] _In_ REFIID riid, [[maybe_unused]] _Outptr_ LPVOID FAR *ppv)
{
	auto hr = E_NOTIMPL;// CLASS_E_CLASSNOTAVAILABLE,E_NOTIMPL, E_OUTOFMEMORY, and E_UNEXPECTED,E_INVALIDARG
	Guid iid = rclsid;
	//Guid iid2 = riid;
	if(ppv) *ppv = nullptr;

	__try
	{
		if(iid.equals(IID_FolderExtensions))
			return E_NOTIMPL;

		if(!iid.equals({ IID_ContextMenu, IID_IconOverlay }))
			return hr;

		if(Initializer::Status.Disabled)
			return hr;

		if(!is_registered())
			return hr;
		
		if(_initializer.has_error())
			return hr;
		
		if(rclsid == IID_ContextMenu)
		{
			_loader.contextmenuhandler = true;
			Selections::point.GetCursorPos();
		}

		if(!_initializer.Status.Loaded)
			_initializer.init();
	}
	except
	{
#ifdef _DEBUG
		_log.exception(__func__);
#endif
	}
	_log.close();

	return hr;
}

//DllCanUnloadNow. COM calls this function to determine whether the object is serving any clients. 
//If not, the system can unload the DLL and free the associated memory.
// Check if we can unload the component from the memory.
// NOTE: The component can be unloaded from the memory when its reference 
// count is zero (i.e. nobody is still using the component).
//#pragma comment(linker, "/export:DllCanUnloadNow=DllCanUnloadNow")
__control_entrypoint(DllExport)
STDAPI DllCanUnloadNow(void)
{
	if(!_loader.explorer || !is_registered())
		return S_OK;
	return S_FALSE;
	/*
	if(ContextMenu::Processes.size() == 0)
	{
		int p = 0;
		EnumThreadWindows(GetCurrentThreadId(), [](HWND, LPARAM lp) {
		(*((int *)lp))++;
			return 0;
		}, (LPARAM)&p);

		//if(p)MBF(L"%d", p);
		return p > 0 ? S_FALSE : S_OK;
	}

	//if(_loader.explorer)
	//	return release() ? S_OK : S_FALSE;
	return ContextMenu::Processes.size() == 0 ? S_OK : S_FALSE;
	//return S_OK;
	*/
}

// Register the COM server and the context menu handler.
HRESULT Register(bool reg)
{
	try
	{
		if(reg)
		{
			Path::Delete(_log.path());
			//_log.reset();
		}

		if(!ver->IsWindows7OrGreater())
		{
			//windows compatibility
			_log.error(L"%s", string::Extract(_hInstance, IDS_WINDOWS_COMPATIBILITY).c_str());
			return ERROR_EXE_MACHINE_TYPE_MISMATCH;//ERROR_DS_VERSION_CHECK_FAILURE
		}

		auto is_elevated = Security::Elevation::IsElevated();

		if(!is_elevated)
		{
			// Missing administrative privileges!
			string msg = string::Extract(_hInstance, IDS_ADMIN_PRIVILEGES).move();
			//You will need to provide administrator permission to run this Shell
			_log.error(L"%s", msg.c_str());
			return ERROR_ACCESS_DENIED;
		}

		string path = Path::Module(_hInstance).move();

		if(reg)
		{
			string dir = Path::Parent(path).move();
			if(!dir.empty())
				Security::Permission::SetFile(dir.c_str());
			//_log.reset();
			//IO::Path::Delete(_log.path());
		}

		string msg;

		if(reg)
		{
			//_log.create();
			REGOP regop;
			regop.REGISTER = regop.CONTEXTMENU = regop.ICONOVERLAY = true;

			if(!RegistryConfig::Register(path, regop))
			{
				msg = string::Extract(_hInstance, IDS_REGISTER_NOT_SUCCESS).move();
				_log.error(L"%s", msg.c_str());
				return S_FALSE;
			}
			msg = string::Extract(_hInstance, IDS_REGISTER_SUCCESS).move();
		}
		else
		{
			if(!RegistryConfig::IsRegistered())
				return S_OK;

			if(!RegistryConfig::Unregister())
			{
				msg = string::Extract(_hInstance, IDS_UNREGISTER_NOT_SUCCESS).move();
				_log.error(L"%s", msg.c_str());
				return S_FALSE;;
			}
			msg = string::Extract(_hInstance, IDS_UNREGISTER_SUCCESS).move();
		}

		_log.info(L"%s", msg.c_str());

		_log.close();

		return S_OK;
	}
	catch(...)
	{
#ifdef _DEBUG
		_log.exception(__func__);
#endif
	}
	_log.close();
	return S_FALSE;
}
/*
//  Register the COM server and the context menu handler.
__control_entrypoint(DllExport)
STDAPI DllRegisterServer()
{
	return Register(true);
}

__control_entrypoint(DllExport)
STDAPI DllUnregisterServer()
{
	return Register(false);
}
*/
/*
STDAPI DllInstall(BOOL bInstall, _In_opt_ PCWSTR pszCmdLine)
{
	MBF(L"%d %s", bInstall, pszCmdLine);
	return E_NOTIMPL;
}
*/

/*
#include <Shldisp.h>

CoInitialize(NULL);
// Create an instance of the shell class
IShellDispatch4 *pShellDisp = NULL;
HRESULT sc = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID *) &pShellDisp );
// Show the desktop
sc = pShellDisp->ToggleDesktop();
// Restore the desktop
sc = pShellDisp->ToggleDesktop();
pShellDisp->Release();
 */

#pragma endregion

uint32_t ImmersiveColor::GetColorByColorType(uint32_t colorType)
{
	auto colorSet = GetImmersiveUserColorSetPreference(false, false);
	return GetImmersiveColorFromColorSetEx(colorSet, colorType, false, 0);
}

uint32_t ImmersiveColor::GetColorByName(const wchar_t *name)
{
	auto colorSet = GetImmersiveUserColorSetPreference(false, false);
	auto colorType = GetImmersiveColorTypeFromName(name);
	return GetImmersiveColorFromColorSetEx(colorSet, colorType, false, 0);
}

uint32_t ImmersiveColor::GetImmersiveUserColorSetPreference(bool bForceCheckRegistry, bool bSkipCheckOnFail)
{
	return DLL::Invoke<uint32_t>(hUxTheme, 98, bForceCheckRegistry, bSkipCheckOnFail);
}

uint32_t ImmersiveColor::GetImmersiveColorFromColorSetEx(uint32_t dwImmersiveColorSet, uint32_t dwImmersiveColorType, bool bIgnoreHighContrast, uint32_t dwHighContrastCacheMode)
{
	return DLL::Invoke<uint32_t>(hUxTheme, 95, dwImmersiveColorSet, dwImmersiveColorType, bIgnoreHighContrast, dwHighContrastCacheMode);
}

uint32_t ImmersiveColor::GetImmersiveColorTypeFromName(const wchar_t *name)
{
	return DLL::Invoke<uint32_t>(hUxTheme, 96, name);
}

uint32_t ImmersiveColor::GetImmersiveColorSetCount()
{
	return DLL::Invoke<uint32_t>(hUxTheme, 94);
}

bool ImmersiveColor::IsSupported()
{
	return  ::IsCompositionActive() && DLL::IsFunc(hUxTheme, 95);
}
