#include <pch.h>
#include "Include/ContextMenu.h"
#include "Library/detours.h"
#include "RegistryConfig.h"
#include <UIAutomation.h>
using namespace Nilesoft::Shell;
using namespace Diagnostics;

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

//#define _DIJA

#ifdef _DIJA

namespace XXX
{
#define MN_SELECTITEM           0x01E5
#define WM_FINALDESTROY         0x0070

#define CYMENU          30      // Height of a single color pick
#define CXMENU          150     // Width of our fake menu
#define CYMARGINMENU    2
#define CXMARGINMENU    2

#define CYMARGINMENU    2
#define CXPADDING		4

#define CXIMAGE		    20
#define CYIMAGE		    20

#define CXPOPUP		    20

#define POPUPMARGIN		1

#define BORDERPADDING   10

#define IDSYS_MNSHOW    0x0000FFFEL
#define UI_TIMERTOOPENHIERARCHY 200


	inline std::wstring to_format(const wchar_t *format, ...)
	{
		wchar_t msg[1024] = { 0 };
		va_list args;
		va_start(args, format);
		::vswprintf_s(msg, 1023, format, args);
		va_end(args);
		return msg;
	}

	std::wstring cls_name(HWND hwnd)
	{
		std::wstring cls(256, L'\0');
		auto len = ::GetClassNameW(hwnd, cls.data(), 250);
		cls.resize(len);
		return std::move(cls);
	}

	std::wstring hwnd_text(HWND hwnd)
	{
		std::wstring text(256, L'\0');
		auto len = ::GetWindowTextW(hwnd, text.data(), 250);
		text.resize(len);
		return std::move(text);
	}

	BOOL IsMouseWheelPresent()
	{
		return (GetSystemMetrics(SM_MOUSEWHEELPRESENT) != 0);
	}

	struct point_t : public POINT
	{
		point_t()
		{
			x = 0;
			y = 0;
		}

		point_t(const POINT pt)
		{
			x = pt.x;
			y = pt.y;
		}

		point_t(long x, long y)
		{
			this->x = x;
			this->y = y;
		}

		point_t(LPARAM lparam)
		{
			x = LOWORD(lparam);
			y = HIWORD(lparam);
		}


		point_t &to_client(HWND hwnd)
		{
			::ScreenToClient(hwnd, this);
			return *this;
		}

		point_t to_client(HWND hwnd) const
		{
			point_t pt = *this;
			::ScreenToClient(hwnd, &pt);
			return pt;
		}

		point_t &to_screen(HWND hwnd)
		{
			::ClientToScreen(hwnd, this);
			return *this;
		}

		point_t to_screen(HWND hwnd) const
		{
			point_t pt = *this;
			::ClientToScreen(hwnd, &pt);
			return pt;
		}

		auto window() const
		{
			return ::WindowFromPoint(*this);
		}

		bool in_rect(const RECT &rc) const
		{
			return x >= rc.left && x <= rc.right && y >= rc.top && y <= rc.bottom;
			//return PtInRect(&rc, *this);
		}

		bool in_rect_x(const RECT &rc) const
		{
			return x >= rc.left && x <= rc.right;
		}

		bool in_rect_y(const RECT &rc) const
		{
			return y >= rc.top && y <= rc.bottom;
		}

		bool operator==(const point_t &rh) const
		{
			return x == rh.x && y == rh.y;
		}

		operator LPARAM() const
		{
			return MAKELPARAM(x, y);
		}

		static point_t pos()
		{
			point_t pt;
			::GetCursorPos(&pt);
			return pt;
		}
};

	struct rect_t : public RECT
	{
		rect_t()
		{
			left = 0;
			top = 0;
			right = 0;
			bottom = 0;
		}

		rect_t(const RECT other)
		{
			left = other.left;
			top = other.top;
			right = other.right;
			bottom = other.bottom;
		}

		rect_t(const point_t &pt)
		{
			left = pt.x;
			top = pt.y;
		}

		rect_t(long left, long top, long right, long bottom)
		{
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}

		rect_t(HWND hWnd)
		{
			::GetWindowRect(hWnd, this);
		}

		long width() const { return right - left; }
		long height() const { return bottom - top; }

		SIZE size() const { return { width() , height() }; }
		point_t point() const { return { left , top }; }
	};

	enum class menuitem_type : int32_t
	{
		none = -1,
		item,
		menu,
		separator,
		label,
		spacer
	};

	template<typename T>
	struct property_t
	{
		property_t() {}
		property_t(const T &value) : _value{ value } {}
		property_t(T &&value) : _value{ std::move(value) } {}
		operator T() const { return _value; }

	private:
		T _value{};
	};

	class menuitem_t
	{
	public:
		property_t<menuitem_t *> parent;
		property_t<bool> disabled;
		property_t<menuitem_type> type = menuitem_type::none;

	public:
		menuitem_t() {}
		virtual ~menuitem_t() {}
	};


	class menuitem_separator_t : public menuitem_t
	{
	public:
		menuitem_separator_t(menuitem_t *parent = nullptr)
		{
			type = menuitem_type::separator;
			parent = parent;
		}
	};

	class menuitem_item_t : public menuitem_t
	{
	public:
		uint32_t id = 0;
		uint32_t image = 0;
		property_t<std::wstring> title;

	public:
		menuitem_item_t() {}
		menuitem_item_t(std::wstring title, menuitem_t *parent = nullptr)
		{
			type = menuitem_type::item;
			parent = parent;
			title = std::move(title);
		}
	};

	class menuitem_menu_t : public menuitem_item_t
	{
		std::vector<menuitem_t> _items;
	public:
		menuitem_menu_t(std::wstring title, menuitem_t *parent = nullptr)
			: menuitem_item_t(std::move(title), parent)
		{
			type = menuitem_type::menu;
		}

		void add(menuitem_t item)
		{
			_items.push_back(std::move(item));
			_items.back().parent = this;
		}

		void add(const std::vector<menuitem_t> &items)
		{
			_items.insert(_items.end(), items.begin(), items.end());
		}

		menuitem_t *get(size_t index)
		{
			return index < _items.size() ? &_items[index] : nullptr;
		}

		size_t count() const { return _items.size(); }
	};


	class menuitem_core_t : public menuitem_t
	{
	public:
		rect_t rc{};
		bool sel = false;
		int layout = 0;
		bool layout_multiple = false;
	};

	struct MenuItem
	{
		MenuItem *parent = nullptr;
		UINT id = 0;
		UINT type = 0;
		string title;
		wchar_t glyph = 0;
		rect_t rc{};
		bool sel = false;
		bool enabled = true;
		int layout = 0;
		bool layout_multiple = false;
		HBITMAP image = nullptr;
		std::vector<MenuItem> items;
	};

	// check that lpszString is a valid string, and not an ATOM in disguise
	bool is_atom(const wchar_t *lpszString)
	{
		return (((DWORD_PTR)lpszString & 0xffff0000) == 0);
		//wsprintf(ach, _T("%08X (Atom)"), lpszString);
		//MB(ach);
	}


	/*
	static bool IsPopupMenu(HWND hWnd)
	{
		if(::GetClassLongPtrW(hWnd, GCW_ATOM) == 32768)
			return true;
		wchar_t pszClass[MAX_PATH + 1]{};
		if(auto length = ::GetClassNameW(hWnd, pszClass, MAX_PATH); length == 6) //#32768
		{
			if(0 == ::memcmp(pszClass, L"#32768", 6 * sizeof(wchar_t)))
				return DLL::User32<BOOL>("IsTopLevelWindow", hWnd);
		}
		return false;
	}
	*/

	//      Helper function to fill a rectangle with a solid color.
	void FillRectClr(HDC hdc, const rect_t &prc, COLORREF clr)
	{
		SetDCBrushColor(hdc, clr);
		FillRect(hdc, &prc, (HBRUSH)GetStockObject(DC_BRUSH));
	}

	/*
	class MenuRoot
	{
		MenuItem *result = nullptr;
		HWND _hwndOwner = nullptr;
	};
	*/

	std::recursive_mutex g_proc_mutex{};

	auto __hFont = ::CreateFontW(-16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Segoe Fluent Icons");

	class Menu
	{
	public:
		inline static uint16_t atom = 0;
		inline static constexpr auto WC_DIJA = L"dija";

	public:
		static bool is_cls(const HWND hwnd)
		{
			auto ret = ::GetClassWord(hwnd, GCW_ATOM);
			return ret == (atom != 0 ? atom : (uint16_t)::RegisterWindowMessageW(WC_DIJA));
		}

	private:
		std::vector<Menu *> *levels = nullptr;
		Menu *_root = nullptr;
		Menu *_parent = nullptr;
		HWND _handle = nullptr;
		const HWND _hwndOwner = nullptr; // Our owner window
		bool draw_enter = true;
		size_t level = size_t(-1);

	public:
		bool done = false;
		size_t _seletedindex = size_t(-1);

		std::vector<MenuItem> *_items = nullptr;
		bool fKeyboardUsed = false;     // Has the keyboard been used?
		rect_t rc{};
		HDC _hdc = nullptr;
		MenuItem *result = nullptr;            // Which color should be returned?
		MenuItem *__seleteditem = nullptr;
		MenuItem *selected_subitem = nullptr;

		Menu(std::vector<Menu *> *levels, std::vector<MenuItem> *items,
			 Menu *parent = nullptr, HWND hwndOwner = nullptr) :
			levels(levels),
			_items(items),
			_parent(parent),
			_hwndOwner(parent ? parent->_root->_hwndOwner : hwndOwner),
			_root(parent ? parent->_root : this)
		{
		}

		operator HWND() const { return _handle; }
		const HWND handle() const { return _handle; }
		const HWND owner() const { return _root->_hwndOwner; }
		const Menu *root() const { return _root; }
		Menu *parent() const { return _parent; }
		bool is_root() const { return _root == this; }

		/*~Menu()
		{
			if(_hwndPopup)
			{
				::DestroyWindow(_hwndPopup);
			}
		}
		*/
		/*
			bool IsRootPopupMenu(PPOPUPMENU ppopupmenu)
			{
				return (ppopupmenu == ppopupmenu->ppopupmenuRoot);
			}
		*/

		//  Stash away our state.
		LRESULT OnCreate(HWND hwnd, [[maybe_unused]] CREATESTRUCT *cs)
		{
			levels->push_back(this);
			level = levels->size();
			_handle = hwnd;
			::SetPropW(_handle, L"dija", this);
			::SetWindowLongPtrW(_handle, GWLP_USERDATA, (LPARAM)this);
			rc = _handle;
			_hdc = ::GetWindowDC(_handle);

			//if(_parent)
			//	SetWindowText(_root->_hwndOwner, to_format(L"%d %d", rc.left, _parent->rc.right).c_str());

			return 0;
		}

		LRESULT OnDestroy(HWND /*hwnd*/)
		{
			if(_hdc)
				::ReleaseDC(_handle, _hdc);

			if(_parent)
			{
				for(auto &item : *_items)
					item.sel = false;

				::RemovePropW(_handle, L"dija");
				levels->pop_back();
				delete this;
			}
			return 0;
		}

		MenuItem *get(size_t index) const
		{
			return index < _items->size() ? &_items->at(index) : nullptr;
		}

		__declspec(property(get = get_back)) Menu *back;
		Menu *get_back() const
		{
			return levels->empty() ? nullptr : levels->back();
		}

		void destroy_current_isback()
		{
			if(back == this) ::DestroyWindow(_handle);
		}

		bool destroy(bool all = false)
		{
			auto b = back;
			if(b)
			{
				if(!all && !_parent)
					return false;
				::DestroyWindow(b->handle());
				return true;
			}
			return false;
		}

		void draw(MenuItem *item)
		{
			if(!item || !_hdc)
				return;

			if(item->type == 2)
			{
				auto rect = item->rc;
				rect.left += 4;
				rect.right -= 4;
				rect.top += 4;
				rect.bottom -= 4;
				FillRectClr(_hdc, rect, 0x808080);
				return;
			}

			::SetBkMode(_hdc, TRANSPARENT);
			::SetTextColor(_hdc, 0xffffff);

			auto rect = item->rc;
			FillRectClr(_hdc, rect, item->sel ? 0x404040 : 0x2b2b2b);

			auto rectIMG = rect;
			if(item->title.empty())
			{
				rectIMG.left += 4;
				rectIMG.right -= 4;
				rectIMG.top += 4;
				rectIMG.bottom -= 4;
				if(item->glyph)
					SetTextColor(_hdc, item->enabled ? 0xffffff : 0x808080);
				else if(item->image)
				{
				
					DC memDC(::CreateCompatibleDC(_hdc), 1);
					if(memDC)
					{
						if(memDC.select_bitmap(item->image))
						{
							
							//dc.draw_image(rcim.point(), image->size, memDC, 255);
							::BitBlt(_hdc, rectIMG.left, rectIMG.top, rectIMG.right, rectIMG.bottom, memDC, 0, 0, SRCCOPY);
							memDC.reset_bitmap();
						}
					}
					//FillRectClr(_hdc, rectIMG, item->clr);
				}
			}
			else
			{
				if(item->image)
				{
					DC memDC(::CreateCompatibleDC(_hdc), 1);
					if(memDC)
					{
						if(memDC.select_bitmap(item->image))
						{
							BITMAP bitmap = {};
							GetObjectW(item->image, sizeof(bitmap), &bitmap);

							rectIMG.left += 4;
							rectIMG.right = rectIMG.left + bitmap.bmWidth;
							rectIMG.top += 4;
							rectIMG.bottom = rectIMG.top + bitmap.bmHeight;

							SIZE sz = { rectIMG.right - rectIMG.left, rectIMG.bottom - rectIMG.top };
							if(bitmap.bmBitsPixel == 32)
								::GdiAlphaBlend(_hdc, rectIMG.left, rectIMG.top, sz.cx, sz.cy,
												memDC, 0, 0, sz.cx, sz.cy,
												{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
							else
								::BitBlt(_hdc, rectIMG.left, rectIMG.top, sz.cx, sz.cy, memDC, 0, 0, SRCCOPY);

							memDC.reset_bitmap();
						}
					}
					//FillRectClr(_hdc, rectIMG, item->clr);
				}
				SetTextColor(_hdc, item->enabled ? 0xffffff : 0x808080);
			}

			if(item->title.empty())
			{
				if(item->glyph)
				{
					auto hFontOriginal = SelectObject(_hdc, __hFont);
					DrawText(_hdc,
							 &item->glyph, 1,
							 &rectIMG,
							 DT_CENTER | DT_VCENTER | DT_INTERNAL | DT_SINGLELINE | DT_NOCLIP);
					SelectObject(_hdc, hFontOriginal);
				}
			}
			else
			{
				auto rectT = rect;
				rectT.left += BORDERPADDING + CXMARGINMENU + CXIMAGE + CXPADDING;

				DrawText(_hdc,
						 item->title.c_str(), (int)item->title.length(),
						 &rectT,
						 DT_LEFT | DT_EDITCONTROL | DT_EXPANDTABS/* | DT_HIDEPREFIX */| DT_INTERNAL | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);

				// draw popup arrow
				if(item->items.size() > 0)
				{
					auto hFontOriginal = SelectObject(_hdc, __hFont);
					rect = { rect.right - 20, rect.top + 10, rect.right - 10, rect.bottom - 10 };
					DrawText(_hdc,
							 L"\uE00F", 1,
							 &rectT,
							 DT_RIGHT | DT_EDITCONTROL | DT_INTERNAL | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);

					SelectObject(_hdc, hFontOriginal);

					//FillRectClr(_hdc, rect, 0x00ff00);
				}
			}
		}

		BOOL invalidate(MenuItem *item, bool select = false)
		{
			__seleteditem = item;
			if(!item)
				return FALSE;
			item->sel = select;
			draw(item);
			return 1;
		}

		BOOL invalidate(MenuItem *item, bool select, bool erase)
		{
			__seleteditem = item;
			if(!item)
				return FALSE;
			item->sel = select;
			return ::InvalidateRect(_handle, &item->rc, erase);
		}

		//  Draw the background, color bars, and appropriate highlighting for the selected color.
		LRESULT OnPaint()
		{
			// std::lock_guard _{ g_proc_mutex };
			PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint(_handle, &ps);
			if(hdc)
			{
				if(draw_enter)
				{
					auto r = ps.rcPaint;
					FillRectClr(_hdc, r, 0x808080);

					r.left++;
					r.top++;
					r.right--;
					r.bottom--;

					FillRectClr(hdc, r, 0x2b2b2b);

					for(auto &item : *_items)
						draw(&item);
					draw_enter = false;
				}
				/*else
				{
					if(fKeyboardUsed)
					{
						for(auto &item : *_items)
							draw(&item);
					}
					else
					{
						draw(__seleteditem);
					}
					__seleteditem = nullptr;
				}*/
			}
			::EndPaint(_handle, &ps);

			return 0;
		}

		// Change the selection to the specified item.
		void ChangeSel(size_t seletedindex, int key = 0)
		{
			// If the selection changed, then repaint the items that need repainting.
			if(_seletedindex != seletedindex)
			{
				invalidate(get(_seletedindex), false);

				auto item = get(seletedindex);
				if(item && item->type == 2)
				{
					if(key == VK_UP)
						seletedindex--;
					else if(key == VK_DOWN)
						seletedindex++;
				}

				selected_subitem = nullptr;
				_seletedindex = seletedindex;
				invalidate(get(_seletedindex), true);
			}
		}

		void OnMouseLeave()
		{
			/* while(!__hwnd.empty() && __hwnd.back() != _hwndPopup)
				 ::DestroyWindow(__hwnd.back());
			 */
		}

		// Track the mouse to see if it is over any of our colors.
		void OnMouseMove(const point_t &point)
		{
			static point_t ptLast;
			const point_t pt = point;

			//SetWindowText(_root->_hwndOwner, to_format(L"%zd", level).c_str());

			// we need to ignore extra mouse events: example when this happens is when
			// the mouse is on the menu and we open a submenu from keyboard - Windows
			// then sends us a dummy mouse move event, we (correctly) determine that it
			// happens in the parent menu and so immediately close the just opened submenu!

			auto ptCur0 = pt.to_client(_handle);// = pt.pos();
			auto ptCur = pt.to_screen(_handle);// = GetCursorPos();

			if(ptCur == ptLast)
				return;

			if((levels->size() - level) > 1)
			{
				while(levels->size() > (level + 1))
					::DestroyWindow(back->handle());
				::SendMessageW(back->handle(), MN_SELECTITEM, (WPARAM)-1, 0);
			}

			auto lpt = ptLast;
			ptLast = ptCur;

			auto seletedindex = _seletedindex;
			_seletedindex = size_t(-1);

			_root->result = nullptr;

			for(size_t i = 0; i < _items->size(); i++)
			{
				auto item = get(i);

				if(pt.in_rect(item->rc))
				{
					if(item->type == 2)
						break;

					_seletedindex = i;
					item->sel = true;

					if(seletedindex != _seletedindex)
					{
						invalidate(item, true);
						auto item2 = get(seletedindex);
						if(item2)
						{
							item2->sel = 0;
							invalidate(item2);
						}
						// Cancel pending show timer if any
						::KillTimer(_handle, IDSYS_MNSHOW);
					}

					if(item && item->items.size() > 0)
					{
						if(selected_subitem != item)
						{
							selected_subitem = item;
							::SetTimer(_handle, IDSYS_MNSHOW, UI_TIMERTOOPENHIERARCHY, 0);
						}
					}

					if(seletedindex != _seletedindex)
						break;
				}
				else if(item->sel)
				{
					if(selected_subitem)
					{
						if(point.in_rect_y(item->rc) && ptCur.in_rect_x(rc))
						{
							// left
							if(ptCur.x > (rc.left + (rc.width() / 2)) && ptCur.x <= rc.right)
								// if(ptCur.x >= rc.right - 20 && ptCur.x <= rc.right)
							{
								auto b = _root->levels->back();
								if(b->parent() != this)
								{
								}
								_seletedindex = seletedindex;
								return;
							}
							else
							{
								// right
								auto b = _root->levels->back();
								if(b->parent() == this)
								{
									if(rc.left > b->rc.right)
									{
										_seletedindex = seletedindex;
										return;
									}
								}
							}
						}
					}

					selected_subitem = nullptr;
					invalidate(item);

					// close top menus except this
					auto b = back;
					while(b != this)
					{
						::DestroyWindow(b->handle());
						b = back;
					}
					break;
				}
			}
		}

		void OnMouseDown([[maybe_unused]] UINT button, [[maybe_unused]] const point_t &pt)
		{
			/* auto pt = GetCursorPos();
			 auto hx = WindowFromPoint(pt);
			 bool in = false;
			 for(auto &h : __hwnd)
			 {
				 if(h == hx)
				 {
					 MB(0)
					 in = true;
					 break;
				 }
			 }
			 if(!in)
				 beep;
			 if(!in) dismiss()
			 */
		}

		// When the button comes up, we are done.
		void OnMouseUp(uint32_t button, const point_t &pt)
		{
			[[maybe_unused]] auto seleteditem = _seletedindex;
			// First track to the final location, in case the user moves the mouse
			// REALLY FAST and immediately lets go.
			OnMouseMove(pt);
			if(_seletedindex < _items->size())
			{
				// Set the result to the current selection.
				_root->result = get(_seletedindex);
				// And tell the message loop that we're done.
			}
			else
			{
				auto pts = pt;
				pts.to_screen(_handle);

				if(!pts.in_rect(rc))
				{
					_root->done = true;
					return;
				}
			}

			if(_root->result && _root->result->items.empty()&& _root->result->enabled)
			{
				if(button == WM_LBUTTONUP)
					_root->done = true;
			}
		}

		//      If the ESC key is pressed, then abandon the fake menu.
		//      If the Enter key is pressed, then accept the current selection.
		//      If an arrow key is pressed, the move the selection.
		void OnKeyDown(WPARAM vk)
		{
			fKeyboardUsed = true;
			selected_subitem = nullptr;

			switch(vk)
			{
				case VK_ESCAPE:
				{
					destroy_current_isback();
					break;
				}
				case VK_RETURN:
				{	// Accept current selection
					auto item = get(_seletedindex);
					if(item && item->items.size() > 0)
					{
						ShowSubmenu(item, true);
						return;
					}
					_root->result = item;
					_root->done = true;
					break;
				}
				case VK_TAB:
				{
					// Abandoned
					_root->done = true;
					break;
				}
				case VK_UP:
				{
					// Decrement selection
					auto condition = _seletedindex - 1 < _items->size();
					ChangeSel((condition ? _seletedindex : _items->size()) - 1, VK_UP);
					break;
				}
				case VK_DOWN:
				{
					// Increment selection
					auto condition = _seletedindex + 1 < _items->size();
					ChangeSel(condition ? _seletedindex + 1 : 0, VK_DOWN);
					break;
				}
				case VK_RIGHT:
				{
					auto item = get(_seletedindex);
					if(item)
					{
						if(item->items.size() > 0)
							ShowSubmenu(item, true);
						else
						{
							if(item->layout_multiple)
							{
								auto condition = _seletedindex + 1 < _items->size();
								auto index = condition ? _seletedindex + 1 : 0;
								item = get(index);
								if(item && item->layout_multiple)
									ChangeSel(index, VK_DOWN);
							}
						}
					}
					break;
				}
				case VK_LEFT:
				{
					if(destroy(false))
						break;

					auto item = get(_seletedindex);
					if(item && item->layout_multiple)
					{
						auto condition = _seletedindex - 1 < _items->size();
						auto index = (condition ? _seletedindex : _items->size()) - 1;
						item = get(index);
						if(item && item->layout_multiple)
							ChangeSel(index, VK_UP);

						//ChangeSel((condition ? _seletedindex : _items->size()) - 1, VK_UP);
					}
					break;
				}
				case VK_HOME:
				{
					ChangeSel(0, VK_HOME);
					break;
				}
				case VK_END:
				{
					ChangeSel(_items->size() - 1, VK_END);
					break;
				}
			}
		}

		void OnTimer(WPARAM wParam, LPARAM/* lParam*/)
		{
			if(wParam == IDSYS_MNSHOW)
			{
				::KillTimer(_handle, IDSYS_MNSHOW);
				ShowSubmenu(selected_subitem, false);
			}
		}

		// Clean up the hwnds we created.
		void dismiss()
		{
			_root->done = true;
			if(levels)
			{
				for(auto l : *levels)
					::DestroyWindow(l->_handle);
				levels->clear();
			}
		}

		SIZE build_layout()
		{
			MenuItem *prev = nullptr;

			SIZE size = { 0, 0 };
			//long max_item_width = 0;

			auto hdc = GetDC(nullptr);

			for(size_t i = 0ull; i < _items->size(); i++)
			{
				auto item = &_items->at(i);

				auto next = i + 1 >= _items->size() ? nullptr : &_items->at(i + 1);
				auto r = &item->rc;

				rect_t rectT;
				if(item->title.empty())
				{
					rectT.right = CXPADDING + CXIMAGE + CXPADDING;
				}
				else
				{
					DrawText(hdc, item->title.c_str(), (int)item->title.length(), &rectT, DT_CALCRECT);
					rectT.left = 0;
					rectT.right = CXIMAGE + CXPADDING + rectT.width() + CXPADDING + 20;
				}

				if(prev == nullptr)
				{
					if(item->layout == 1)
					{
						r->left = BORDERPADDING + CXMARGINMENU;
						r->right = r->left + rectT.width();
						r->top = BORDERPADDING;
						r->bottom = r->top + CXPADDING + CYIMAGE + CXPADDING + CYMARGINMENU;
						item->layout_multiple = true;
					}
					else
					{
						r->left = BORDERPADDING;
						r->right = rectT.width();
						r->top = BORDERPADDING;
						r->bottom = r->top + CYMENU;
					}
				}
				else
				{
					if(item->layout == 1)
					{
						r->left = prev->rc.right + CXMARGINMENU;
						if(item->type == 2)
							r->right = r->left + 7;
						else
							r->right = r->left + rectT.width();

						r->top = prev->rc.top;
						r->bottom = prev->rc.bottom;
						item->layout_multiple = true;
					}
					else
					{
						if(next && next->layout == 1)
							item->layout_multiple = true;

						r->left = BORDERPADDING;
						r->right = r->left + rectT.width();
						r->top = prev->rc.bottom + CYMARGINMENU;

						if(item->type == 2)
							r->bottom = r->top + 7;
						else
							r->bottom = r->top + CYMENU;
					}
				}

				if(!item->items.empty())
					r->right += CXPOPUP;

				size.cx = std::max<long>(r->right, size.cx);
				size.cy = std::max<long>(r->bottom, size.cy);

				prev = item;
			}

			for(size_t i = 0; i < _items->size(); i++)
			{
				auto item = &_items->at(i);
				//auto next = i + 1 >= _items->size() ? nullptr : &_items->at(i + 1);
				auto r = &item->rc;

				if(!item->layout_multiple)
				{
					r->right = size.cx;
				}
			}

			ReleaseDC(nullptr, hdc);

			size.cx += BORDERPADDING;
			size.cy += BORDERPADDING;

			return size;
		}

		void ShowSubmenu(MenuItem *item, bool keyboard)
		{
			selected_subitem = nullptr;
			if(item)
			{
				selected_subitem = item;
				auto menu = new Menu(levels, &item->items, this);
				if(menu->Popup(rc.right + POPUPMARGIN, rc.top + item->rc.top - POPUPMARGIN, keyboard))
					menu->show();
			}
		}

		Menu &show()
		{
			if(_handle)
				::ShowWindow(_handle, SW_SHOWNOACTIVATE);
			return *this;
		}

		//      Find a place to put the window so it won't go off the screen
		//      or straddle two monitors.
		//
		//      x, y = location of mouse click (preferred upper-left corner)
		//      cx, cy = size of window being created
		//
		//      We use the same logic that real menus use.
		//
		//      -   If (x, y) is too high or too far left, then slide onto screen.
		//      -   Use (x, y) if all fits on the monitor.
		//      -   If too low, then slide up.
		//      -   If too far right, then flip left.
		//

		point_t best_location(HWND hwnd, int x, int y, const SIZE &size)
		{
			point_t pt = { x, y };
			// First get the dimensions of the monitor that contains (x, y).

			HMONITOR hmon = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);

			// If (x, y) is not on any monitor, then use the monitor that the owner
			// window is on.
			if(hmon == NULL)
			{
				hmon = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			}

			MONITORINFO minf{};
			minf.cbSize = sizeof(minf);
			::GetMonitorInfoW(hmon, &minf);
			
			rect_t rcMonitor = minf.rcMonitor;

			// Now slide things around until they fit.

			if(_parent)
			{
				auto rcp = &_parent->rc;
				int margin = 10;

				//if(rcp->right == x-1)
				//	beep;

				if((rcp->right + POPUPMARGIN + size.cx + margin) >= rcMonitor.right)
					pt.x = rcp->left - (POPUPMARGIN + 1 + size.cx);

				/*
				// If too far left, then slide right.
				if(pt.x < rcMonitor.left)
					pt.x = _parent->rc.left;

				// If too far right, then flip left.
				if(pt.x >= rcMonitor.right - size.cx)
					//pt.x -= cx;
					pt.x = _parent->rc.right - size.cx - 10;

				*/
				pt.y -= 9;

				// If too high, then slide down.
				if(pt.y < rcMonitor.top)
					pt.y = rcMonitor.top - 10;

				if(pt.y > (rcMonitor.bottom - (size.cy + margin)))
				{
					pt.y -= size.cy;
					if(auto p = _parent->selected_subitem; p)
						pt.y += p->rc.height() + 18;
				}
			}
			else
			{
				// If too high, then slide down.
				if(pt.y < rcMonitor.top)
					pt.y = rcMonitor.top;

				if(pt.y > (rcMonitor.bottom - size.cy))
					pt.y -= size.cy;

				// If too far left, then slide right.
				if(pt.x < rcMonitor.left)
					pt.x = rcMonitor.left;

				// If too far right, then flip left.
				if(pt.x >= rcMonitor.right - size.cx)
					//pt.x -= cx;
					pt.x = rcMonitor.right - size.cx - 10;
			}

			return pt;
		}

		//
		//  Popup
		//
		//      Display a fake menu to allow the user to select the color.
		//
		//      Return the color index the user selected, or -1 if no color
		//      was selected.
		//
		bool Popup(int x, int y, bool keyboard = false)
		{
			auto valid = _items && !_items->empty();

			// Set up the style and extended style we want to use.
			DWORD dwStyle = WS_POPUP | DS_NOIDLEMSG /*| WS_BORDER*/;
			DWORD dwExStyle = WS_EX_COMPOSITED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE /*| WS_EX_CONTROLPARENT*/;

			dwExStyle |= /*WS_EX_NOREDIRECTIONBITMAP| */WS_EX_LAYERED;

			if(_parent == nullptr)
			{
				x += 1;
				y += 1;
			}

			SIZE size = { 0, 0, };
			point_t pt = { x, y };

			if(valid)
			{
				fKeyboardUsed = keyboard;
				if(fKeyboardUsed)
				{
					_seletedindex = 0;
					_items->at(_seletedindex).sel = true;
				}

				// Setup popup window dimensions
				// Now find a proper home for the window that won't go off the screen or straddle two monitors.
				size = build_layout();

				/*
				rect_t rr = { 0,0, size.cx , size.cy };
				AdjustWindowRectEx(&rr, dwStyle, FALSE, dwExStyle);
				size.cx = rr.right;
				size.cy = rr.bottom;
				*/

				pt = best_location(owner(), x, y, size);
			}

			//rc = { pt.x, pt.y, pt.x + size.cx, pt.y + size.cy };

			// Create the menu window.
			_handle = ::CreateWindowExW(dwExStyle, WC_DIJA, nullptr, dwStyle, pt.x, pt.y, size.cx, size.cy, owner(), nullptr, CurrentModule, this);
			if(_handle)
			{
				::PostMessageW(_handle, WM_SETCURSOR, 0, 0);
				//MARGINS margins = { -1 };
				//::DwmExtendFrameIntoClientArea(_hwndPopup, &margins);
				if(valid)
				{
					//SetWindowText(_hwndPopup, std::to_wstring(_level).c_str());
					::SetLayeredWindowAttributes(_handle, 0x000000, 255, ULW_ALPHA);
					// Show the window but don't activate it!
					::ShowWindow(_handle, SW_SHOWNOACTIVATE);
					::UpdateWindow(_handle);
					return true;
				}
				::DestroyWindow(_handle);
				_handle = nullptr;
			}

			if(_parent)
				delete this;

			return false;
		}

		static LRESULT __stdcall WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			auto _this = reinterpret_cast<Menu *>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
			//auto _this = reinterpret_cast<Menu *>(::GetPropW(hwnd, L"this"));
			if(uMsg == WM_CREATE || uMsg == WM_NCCREATE)
			{
				auto lret = ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
				if(!_this)
				{
					auto cs = reinterpret_cast<CREATESTRUCT *>(lParam);
					_this = reinterpret_cast<Menu *>(cs->lpCreateParams);
					_this->OnCreate(hwnd, cs);
				}
				return lret;
			}

			if(!_this || _this->_handle != hwnd)
				return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);

			switch(uMsg)
			{
				case WM_NCPAINT:
					return 0;
				case WM_PAINT:
					return _this->OnPaint();
				case WM_SYSKEYDOWN:
				case WM_KEYDOWN:
					_this->OnKeyDown(wParam);
					break;
				case WM_SYSKEYUP:
				case WM_KEYUP:
					_this->fKeyboardUsed = false;
					break;
				case WM_MOUSEACTIVATE:
					return MA_NOACTIVATE;
				case WM_MOUSEMOVE:
				{
					TRACKMOUSEEVENT me = { sizeof(TRACKMOUSEEVENT) , TME_LEAVE, hwnd };
					::TrackMouseEvent(&me);
					_this->OnMouseMove(lParam);
					break;
				}
				case WM_MOUSELEAVE:
				case WM_NCMOUSELEAVE:
					_this->OnMouseLeave();
					break;
				case WM_RBUTTONDOWN:
				case WM_LBUTTONDOWN:
					_this->OnMouseDown(uMsg, lParam);
					break;
				case WM_RBUTTONUP:
				case WM_LBUTTONUP:
					_this->OnMouseUp(uMsg, lParam);
					break;
				case WM_TIMER:
					_this->OnTimer(wParam, lParam);
					break;
				case MN_SELECTITEM:
					_this->ChangeSel(wParam);
					break;
				case WM_ERASEBKGND:
					return 0;
					// Do not activate when somebody clicks the window.
				case WM_CAPTURECHANGED:
					break;
				case WM_THEMECHANGED:
				case WM_SETTINGCHANGE:
					// UpdateVisuals();
					break;
				case WM_SETCURSOR:
				{
					::SetCursor(::LoadCursorW(nullptr, IDC_ARROW));
					return TRUE;
					break;
				}
				case WM_KILLFOCUS:
					break;
					//case WM_DESTROY:
				case WM_NCDESTROY:
					::KillTimer(hwnd, IDSYS_MNSHOW);
					_this->OnDestroy(hwnd);
					break;
			}
			return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}

		static MenuItem *Popup(HWND hwndOwner, int x, int y, std::vector<MenuItem> *items)
		{
			if(!hwndOwner || !items || items->empty())
				return nullptr;

			// Early check:  We must be on same thread as the owner so we can see
			// its mouse and keyboard messages when we set capture to it.
			/* if(GetCurrentThreadId() != GetWindowThreadProcessId(hwndOwner, NULL))
			{
				// Error: Menu must be on same thread as parent window.
				return -1;
			}*/

			if(atom == 0)
			{
				WNDCLASSEXW wc = { sizeof(wc) };
				if(!::GetClassInfoExW(CurrentModule, WC_DIJA, &wc))
				{
					wc.cbSize = sizeof(wc);
					wc.lpfnWndProc = WndProc;
					wc.hInstance = CurrentModule;
					wc.hCursor = ::LoadCursorW(NULL, IDC_ARROW);
					wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
					wc.style = CS_DBLCLKS | CS_SAVEBITS;
					wc.lpszClassName = WC_DIJA;
					atom = ::RegisterClassExW(&wc);
				}

				if(atom == 0)
					return nullptr;

				/*
					auto err = ::GetLastError();
					if(!atom && !(err == ERROR_CLASS_ALREADY_EXISTS || err == ERROR_ALREADY_EXISTS)) {
					//throw only if not successful and not already registered
					}
				*/
			}

			std::vector<Menu *> level;

			Menu menu(&level, items, nullptr, hwndOwner);
			//std::unique_ptr<Menu> menu(new Menu(items, nullptr, hwndOwner));

			if(!menu.Popup(x, y, true))
				return nullptr;

			menu.show();

			// auto hwnd = menu.hwndPopupCurrent;
			// We want to receive all mouse messages, but since only the active
			// window can capture the mouse, we have to set the capture to our
			// owner window, and then steal the mouse messages out from under it.
			//::SetCapture(hwndOwner);

			// Go into a message loop that filters all the messages it receives
			// and route the interesting ones to the color picker window.
			MSG msg{};

			while(!menu.done)
			{
				if(level.empty())
					break;

				// If our owner stopped being the active window (e.g. the user
				// Alt+Tab'd to another window in the meantime), then stop.
				auto hwndActive = ::GetActiveWindow();
				if(hwndActive != hwndOwner && !::IsChild(hwndActive, hwndOwner))
					break;

			//	if(auto hCapture = ::GetCapture(); hCapture != hwndOwner)
			//		break;

				auto ret = ::GetMessageW(&msg, nullptr, 0, 0);
				if(ret == 0 || ret == -1)
					break;

			//	if(msg.message == WM_CAPTURECHANGED)
			//		break;

				if(level.empty())
					break;
				// At this point, we get to snoop at all input messages before
				// they get dispatched.  This allows us to route all input to our
				// popup window even if really belongs to somebody else.

				// All mouse messages are remunged and directed at our popup
				// menu. If the mouse message arrives as client coordinates, then
				// we have to convert it from the client coordinates of the original
				// target to the client coordinates of the new target.

				switch(msg.message)
				{
					// These mouse messages arrive in client coordinates, so in
					// addition to stealing the message, we also need to convert the coordinates.
					case WM_MOUSEMOVE:
					case WM_NCMOUSEMOVE:
					case WM_LBUTTONDOWN:
					case WM_RBUTTONDOWN:
					case WM_MBUTTONDOWN:
					case WM_LBUTTONUP:
					case WM_RBUTTONUP:
					case WM_MBUTTONUP:
					case WM_LBUTTONDBLCLK:
					case WM_RBUTTONDBLCLK:
					case WM_MBUTTONDBLCLK:
					case WM_NCLBUTTONDOWN:
					case WM_NCLBUTTONUP:
					case WM_NCLBUTTONDBLCLK:
					case WM_NCRBUTTONDOWN:
					case WM_NCRBUTTONUP:
					case WM_NCRBUTTONDBLCLK:
					case WM_NCMBUTTONDOWN:
					case WM_NCMBUTTONUP:
					case WM_NCMBUTTONDBLCLK:
					{
						if(!is_cls(msg.hwnd))
						{
							auto pt = point_t::pos();
							auto hwnd = pt.window();
							if(!is_cls(hwnd))
							{
								if(msg.message == WM_MOUSEMOVE || msg.message == WM_NCMOUSEMOVE)
									continue;

								menu.dismiss();
								break;
							}

							pt = msg.lParam;
							::MapWindowPoints(msg.hwnd, hwnd, &pt, 1);
							msg.lParam = pt;
							msg.hwnd = hwnd;
						}
						break;
					}
					// These mouse messages arrive in screen coordinates, so we just need to steal the message.
					case WM_MOUSELEAVE:
					case WM_NCMOUSELEAVE:
						//msg.hwnd = menu.back();
						break;
						// We need to steal all keyboard messages, too.
					case WM_KEYDOWN:
					case WM_KEYUP:
					case WM_CHAR:
					case WM_DEADCHAR:
					case WM_SYSKEYDOWN:
					case WM_SYSKEYUP:
					case WM_SYSCHAR:
					case WM_SYSDEADCHAR:
						msg.hwnd = menu.back->handle();
						break;
				}

				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}

			// Clean up the capture we created.
			//::ReleaseCapture();

			//PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOYIELD | PM_NOREMOVE);

			// Clean up the hwnds we created.
			menu.dismiss();

			return menu.result;
		}
	};

	HINSTANCE g_hInstance = NULL;
	COLORREF g_clrBackground;   // The selected color

	// This is the array of predefined colors we put into the color picker.
	const COLORREF c_rgclrPredef[] =
	{
		RGB(0x00, 0x00, 0x00),          // 0 = black
		RGB(0x80, 0x00, 0x00),          // 1 = maroon
		RGB(0x00, 0x80, 0x00),          // 2 = green
		RGB(0x80, 0x80, 0x00),          // 3 = olive
		RGB(0x00, 0x00, 0x80),          // 4 = navy
		RGB(0x80, 0x00, 0x80),          // 5 = purple
		RGB(0x00, 0x80, 0x80),          // 6 = teal
		RGB(0x80, 0x80, 0x80),          // 7 = gray
		RGB(0xC0, 0xC0, 0xC0),          // 8 = silver
		RGB(0xFF, 0x00, 0x00),          // 9 = red
		RGB(0x00, 0xFF, 0x00),          // A = lime
		RGB(0xFF, 0xFF, 0x00),          // B = yellow
		RGB(0x00, 0x00, 0xFF),          // C = blue
		RGB(0xFF, 0x00, 0xFF),          // D = fuchsia
		RGB(0x00, 0xFF, 0xFF),          // E = cyan
		RGB(0xFF, 0xFF, 0xFF),          // F = white
		RGB(0x80, 0x00, 0x00),          // 1 = maroon
		RGB(0x00, 0x80, 0x00),          // 2 = green
		0xff00ff,
		0xdd00dd,
		0xbb00bb,
		0x00ff00,
		0x00dd00,
		0x00bb00
	};

	int iii = 0;
	void build_system_menuitems(HWND hwnd, HMENU hMenu, std::vector<MenuItem> *items, bool isroot = false)
	{
		::SendMessageW(hwnd, WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(hMenu), MAKELPARAM(0, iii++));
		::SendMessageW(hwnd, WM_ENTERIDLE, MSGF_MENU, reinterpret_cast<LPARAM>(hMenu));

		auto itmes_count = ::GetMenuItemCount(hMenu);
		items->reserve(itmes_count);

		bool sep = 0;
		for(int i = 0; i < itmes_count; i++)
		{
			MENUITEMINFOW mii{ sizeof(MENUITEMINFOW) };

			mii.fMask = MenuItemInfo::FMASK;
			string title;
			mii.dwTypeData = title.buffer((1024));
			mii.cch = 1024;

			if(::GetMenuItemInfoW(hMenu, i, true, &mii))
			{
				MenuItem item;
				item.id = mii.wID;

				if(mii.fType & MFT_SEPARATOR)
				{
					if(!items->empty() && items->back().type == 2)
						continue;
					item.type = 2;
				}
				else
				{
					item.type = mii.hSubMenu != nullptr;
					item.enabled = (mii.fState & MFS_DISABLED) == 0;
					item.image = MenuItemInfo::FindImage(&mii);
					
					if(mii.cch > 0)
					{
						item.title = title.release(mii.cch).move();

						if(mii.hSubMenu)
						{
							item.type = 1;
							build_system_menuitems(hwnd, mii.hSubMenu, &item.items);
						}

						if(isroot)
						{
							if(item.title.equals(L"Cu&t"))
								item.glyph = L'\uE8C6';
							else if(item.title.equals(L"&Copy"))
								item.glyph = L'\uE8C8';
							else if(item.title.equals(L"Rena&me"))
								item.glyph = L'\uE8AC';
							else if(item.title.equals(L"&Delete"))
								item.glyph = L'\uE74D';
							else if(item.title.equals(L"&Paste"))
								item.glyph = L'\uE77F';

							if(item.glyph)
							{
								item.title = L"";
								item.layout = 1;
								items->insert(items->begin(), std::move(item));

								if(sep == 0)
								{
									MenuItem item0;
									item0.id = 0;
									item0.type = 2;
									sep = 1;
									items->insert(items->begin() + 1, std::move(item0));
								}
								continue;
							}
						}
					}
					else
					{
						MEASUREITEMSTRUCT mi = {};
						mi.CtlType = ODT_MENU;
						mi.itemID = mii.wID;
						mi.itemData = mii.dwItemData;
						::SendMessageW(hwnd, WM_MEASUREITEM, 0, reinterpret_cast<LPARAM>(&mi));
						//MBF(L"%d %d", mi.itemWidth, mi.itemHeight);
					}
				}
				items->push_back(std::move(item));
			}
		}
		::SendMessageW(hwnd, WM_UNINITMENUPOPUP, reinterpret_cast<WPARAM>(hMenu), 0);
	}

	// Display the color-picker pseudo-menu so the user can change the color.
	uint32_t OnContextMenu(HWND hwnd, int x, int y, HMENU hMenu = nullptr)
	{
		// If the coordinates are (-1, -1), then the user used the keyboard -
		// we'll pretend the user clicked at client (0, 0).
		if(x == -1 && y == -1)
		{
			POINT pt{};
			ClientToScreen(hwnd, &pt);
			x = pt.x;
			y = pt.y;
		}

		UINT id = 0;

		std::vector<MenuItem> items;

		/*
		for(int i = 0; i < 7; i++)
		{
			MenuItem item;
			item.id = id++;

			if(i == 0)
				item.glyph = L'\uE16B';
			else if(i == 1)
				item.glyph = L'\uE16F';
			else if(i == 2)
				item.glyph = L'\uE16D';
			else if(i == 3)
				item.glyph = L'\uE13E';
			else if(i == 4)
				item.glyph = L'\uE107';
			else if(i == 5)
				item.type = 2;
			else if(i == 6)
				item.glyph = L'\uE72D';

			//item.title = L"item id = " + std::to_wstring(item.id);
			item.clr = c_rgclrPredef[item.id];
			item.layout = 1;
			items.push_back(std::move(item));
		}

		for(int i = 7; i < 13; i++)
		{
			MenuItem item;
			item.id = id++;

			if(i == 7)
				item.type = 2;
			else
			{
				if(i == 10)
				{
					item.layout = 1;
					item.glyph = L'\uE115';
				}
				item.clr = c_rgclrPredef[item.id];
				item.title = L"item " + std::to_wstring(item.id);
			}

			items.push_back(std::move(item));
		}

		// 0-30, 30-60
		for(int i = 0; i < 6; i++)
		{
			MenuItem item;
			item.id = id++;
			if(i == 3)
				item.type = 2;
			else {
				item.title = L"item " + std::to_wstring(item.id);
				item.clr = c_rgclrPredef[item.id];
			}
			items[8].items.push_back(std::move(item));
		}

		for(int i = 0; i < 6; i++)
		{
			MenuItem item;
			item.id = id++;
			item.layout = i == 3 ? 0 : 1;
			item.clr = c_rgclrPredef[item.id];
			items[8].items[0].items.push_back(std::move(item));
		}
		*/
		iii = 0;
		
		::SendMessageW(hwnd, WM_ENTERMENULOOP, TRUE, 0);
		::SendMessageW(hwnd, WM_INITMENU, reinterpret_cast<WPARAM>(hMenu), 0);
		build_system_menuitems(hwnd, hMenu, &items, true);
		::SendMessageW(hwnd, WM_EXITMENULOOP, TRUE, 0);

		auto result = Menu::Popup(hwnd, x, y, &items);
		// If the user picked a color, then change to that color.
		return result ? result->id : 0;
	}
}
#endif

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

			if(!RegistryConfig::Register(path, REG_REGISTER | REG_CONTEXTMENU | REG_ICONOVERLAY))
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
