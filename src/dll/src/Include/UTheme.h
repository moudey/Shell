#pragma once
#include "Include\Theme.h"

namespace Nilesoft
{
	namespace Shell
	{
/*
//Windows 10 build 10586 (also known as the November Update or version 1511)
//Windows 10 >= (v1507, b10240), (v1511, b10586) Taskbar contextmenu darkmode

ctober 2018 Update (version 1809)
HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize
AppsUseLightTheme==1 && SystemUsesLightTheme==1; > explorer_menu_light | taskbar_menu_light
AppsUseLightTheme==0 && SystemUsesLightTheme==0; > explorer_menu_dark | taskbar_menu_dark

AppsUseLightTheme==0 && SystemUsesLightTheme==1; > explorer_menu_dark | taskbar_menu_light
AppsUseLightTheme==1 && SystemUsesLightTheme==0; > explorer_menu_light | taskbar_menu_dark

//windows 10 build 10162 1607

			//Windows 10 build 10586 (also known as the November Update or version 1511)
			//Windows 10 >= (v1507, b10240), (v1511, b10586) Taskbar contextmenu darkmode
			//
			//Windows 10 build 10586 (also known as the November Update or version 1511)
			//Windows 10 >= (v1507, b10240), (v1511, b10586) Taskbar contextmenu darkmode
*/
		class UxTheme : public Theme
		{
			Version* m_version = nullptr;
			HTHEME _handle;

		public:
			int transparent = 0;
			SIZE image_size{};

		public:

			UxTheme(HTHEME handle =nullptr) 
				: _handle{ handle }, m_version{ &Version::Instance() }
			{
			}

			~UxTheme() { close(); }

			operator HTHEME() const { return _handle; }
			explicit operator bool() const { return _handle != nullptr; }

			bool open(HWND hWnd, const wchar_t *classList)
			{
				close();
				_handle = ::OpenThemeData(hWnd, classList);
				return _handle != nullptr;
			}

			void close()
			{
				if(_handle)
					::CloseThemeData(_handle);
				_handle = nullptr;
			}

			bool get_text_color(int partId, int stateId, COLORREF &color)
			{
				if(_handle)
				{
					if(S_OK == ::GetThemeColor(_handle, partId, stateId, TMT_TEXTCOLOR, &color))
						return true;
				}
				return false;
			}

			bool get_background_color(int partId, int stateId, COLORREF &color)
			{
				if(_handle)
				{
					if(S_OK == ::GetThemeColor(_handle, partId, stateId, TMT_FILLCOLOR, &color))
						return true;
				}
				return false;
			}

			HBITMAP get_bitmap(int partId, int stateId)
			{
				if(_handle)
				{
					HBITMAP result = nullptr;
					if(S_OK == ::GetThemeBitmap(_handle, partId, stateId, TMT_DIBDATA, GBF_DIRECT, &result))
					{
						return result;
					}
				}

				return nullptr;
			}

			bool draw_text(HDC hdc, int partId, int stateId, const wchar_t *text, int cchText, uint32_t dwTextFlags, uint32_t dwTextFlags2, const RECT *pRect) const
			{
				return S_OK == ::DrawThemeText(_handle, hdc, partId, stateId, text, cchText, dwTextFlags, dwTextFlags2, pRect);
			}

			bool draw_background(HDC hdc, int partId, int stateId, const RECT *pRect, const RECT *pClipRect = nullptr) const
			{
				return S_OK == ::DrawThemeBackground(_handle, hdc, partId, stateId, pRect, pClipRect);
			}

			/*bool draw_background(HDC hdc, int partId, int stateId, const RECT *pRect, const RECT *pClipRect = nullptr) const
			{
				return S_OK == ::DrawThemeIcon(_handle, hdc, partId, stateId, pRect, pClipRect);
			}*/

			bool is_part_defined(int partId, int stateId) const
			{
				return IsThemePartDefined(_handle, partId, stateId);
			}

			bool get_text_extent(HDC hdc, int partId, int stateId, const wchar_t *text, int cch, uint32_t dwTextFlags, const RECT *pBoundingRect, RECT *pExtentRect) const
			{
				return S_OK == ::GetThemeTextExtent(_handle, hdc, partId, stateId, text, cch, dwTextFlags, pBoundingRect, pExtentRect);
			}

			static std::tuple<COLORREF, COLORREF> GetColor(bool dark, HWND hWnd = nullptr)
			{
				COLORREF clr_menu = dark ? 0x000000 : 0xFFFFFF;
				COLORREF clr_menu_text = dark ? 0xFFFFFF : 0x000000;

				if(IsHighContrast())
				{
					clr_menu_text = ::GetSysColor(COLOR_MENUTEXT);
					clr_menu = ::GetSysColor(COLOR_MENU);
				}
				else
				{
					if(hWnd == nullptr)
						hWnd = ::GetDesktopWindow();

					UxTheme theme;

					//IsThemePartDefined
					if(dark)
					{
						if(!theme.open(hWnd, L"ImmersiveStartDark::Menu"))
						{
							if(!theme.open(hWnd, L"DarkMode_ImmersiveStart::Menu"))
							{
								if(!theme.open(hWnd, L"DarkMode::Menu"))
								{
								}
							}
						}
					}
					else
					{
						if(!theme.open(hWnd, L"ImmersiveStart::Menu"))
						{
							theme.open(hWnd, VSCLASS_MENU);
						}
					}

					if(theme)
					{
						theme.get_text_color(MENU_POPUPITEM, MPI_NORMAL, clr_menu_text);

						if(!theme.get_background_color(MENU_POPUPBACKGROUND, MPI_NORMAL, clr_menu))
						{
							auto bmp = theme.get_bitmap(MENU_POPUPITEM, MPI_NORMAL);
							if(bmp)
							{
								if(auto dc = ::CreateCompatibleDC(nullptr))
								{
									auto old = ::SelectObject(dc, bmp);
									clr_menu = ::GetPixel(dc, 0, 0);
									::SelectObject(dc, old);
									::DeleteDC(dc);
								}
							}
						}
					}
				}
				return std::make_tuple(clr_menu_text, clr_menu);
			}

/*
HRESULT CMenuMetrics::Initialize()
{
    HRESULT hr = E_FAIL;
    hTheme = OpenThemeData(hwndTheme, VSCLASS_MENU);
    if (hTheme)
    {
        GetThemePartSize(hTheme, NULL, MENU_POPUPCHECK, 0, NULL, TS_TRUE, &sizePopupCheck);
        GetThemeInt(hTheme, MENU_POPUPITEM, 0, TMT_BORDERSIZE, &iPopupBorderSize);
        GetThemeMargins(hTheme, NULL, MENU_POPUPCHECK, 0, TMT_CONTENTMARGINS, NULL, &marPopupCheck);
        hr = S_OK;
    }
    return hr;
}

An HTHEME is requested from the parent window which maps to the menu class.  
If it succeeds we then ask for the metrics we will need to properly measure and layout our menu items.
Modifying Owner-Draw code – Measurement

There are several “Get” functions that return information on a part/state:

	GetThemePartSize() will give you the dimensions of a part/state pair.
	GetThemeMargins() will give you the spacing around a part/state pair.
	GetThemeInt(…, TMT_BORDERSIZE, …) will give you the size of the border around a part/state pair
	GetThemeTextExtent() will give you the dimensions of the text you specify using the correct font for the part/state pair.  
	The parameters are similar to the DrawText API, including a parameter that accepts DT_* flags.

This information will enable you to make the appropriate measurement and layout calculations.  
In the test app CMenuMetrics caches these metrics and provides some helper functions, like ToMeasureSize() which applies the specified margins to the tight bounding box you calculated for the menu item.
Modifying Owner-Draw code – Drawing
The first thing you need to do is convert the DRAWITEMSTRUCT’s itemState field into the correct state id (POPUPITEMSTATES for popup menus.)  
For example, ODS_HOTLIGHT gets translated to MPI_HOT and ODS_INACTIVE can get translated to MPI_DISABLED.  See CMenuMetrics::ToItemStateId() for details.
The next thing you do is layout the items according to the metrics you calculated during WM_MEASUREITEM and draw the menu in layers using DrawThemeBackground(), starting from the bottom layer:
	MENU_POPUPBACKGROUND (if the background contains transparency)
	MENU_POPUPGUTTER (if you want a gutter)
	MENU_POPUPSEPARATOR (if the item is a separator)
	MENU_POPUPITEM
	MENU_POPUPCHECKBACKPGROUND (if you are rendering a checkmark)
	MENU_POPUPCHECK (if you are rendering a checkmark)
	DrawThemeText(…, MENU_POPUPITEM, …)

CVistaOwnerDraw::_DrawMenuItem() demonstrates this process.

Special considerations
Allowing the test app to switch between owner-draw and non-owner draw menus presented an interesting issue: USER does not issue new WM_MEASUREITEM messages when the MFT_OWNERDRAW bit is toggled so it continues to use the old metrics.  This may be old news (it appears to have always worked this way) but it was a surprise to me.
Fortunately there is a simple workaround: make sure fMask has MIIM_BITMAP set when you call SetMenuItemInfo() and this will cause new WM_MEASUREITEM messages to be sent.  The ResetMenuMetrics() helper function will clear out all the menu items of the specified hmenu.  A more efficient method would be to set this bit when flipping the MFT_OWNERDRAW bit but I wanted to call this out clearly in the sample code.  The MakeOwnerDraw() helper function is used by the test app to change between owner-draw and non-owner-draw.
Important details not covered in article
The sample code presents a simplified version of menu rendering in order to explain the basic concepts.  It does not use every part and state (no submenu rendering), it does not cover the menu bar, and it will not necessarily line up to the pixel with the system’s menu rendering.  If you are in the business of custom rendered menus then I don’t believe any of these simplifications will be an issue for you.
At this point someone may say “Why didn’t you make an API that would render portions of the menu to make all this easier?”  The answer to that is a familiar one: time and resources.  Vista was a big undertaking and we had plenty to do in order to get it wrapped up and out the door.
			*/ 
		};
	}
}