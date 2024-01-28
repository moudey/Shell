#pragma once
#include <dwmapi.h>

namespace Nilesoft
{
	namespace Drawing
	{
		struct Point : POINT
		{
			Point() { x = {}; y = {}; }

			Point(const POINT &pt)
			{
				x = pt.x;
				y = pt.y;
			}

			Point(long x, long y)
			{
				this->x = x;
				this->y = y;
			}

			operator POINT() const { return { x, y }; }
			operator POINT*() { return this; }

			bool in(const RECT &rect) const
			{
				return !(x < rect.left || x > rect.right || y < rect.top || y > rect.bottom);
			}

			bool InRect(const RECT &rect) const
			{ 
				return !(x < rect.left || x > rect.right || y < rect.top || y > rect.bottom);
			}

			Point &GetCursorPos()
			{
				::GetCursorPos(this);
				return *this;
			}

			Point &ScreenToClient(HWND hWnd)
			{
				::ScreenToClient(hWnd, this);
				return *this;
			}

			Point &ClientToScreen(HWND hWnd)
			{
				::ClientToScreen(hWnd, this);
				return *this;
			}

			static Point CursorPos()
			{
				return Point().GetCursorPos();
			}
		};

		// rectangle
		struct Rect : RECT
		{
			Rect()
			{
				left = {};
				top = {};
				right = {};
				bottom = {};
			}

			Rect(const RECT &rc)
			{
				left = rc.left;
				top = rc.top;
				right = rc.right;
				bottom = rc.bottom;
			}

			Rect(const POINT &pt)
			{
				left = pt.x;
				top = pt.y;
			}

			Rect(long left, long top, long right, long bottom)
			{
				this->left = left;
				this->top = top;
				this->right = right;
				this->bottom = bottom;
			}

			Rect(HWND hWnd)
			{
				GetWindowRect(hWnd);
			}

			operator RECT() const { return { left, top, right, bottom }; }
			operator const RECT*() const { return this; }
			operator const RECT *() { return this; }
			operator RECT*() { return this; }
			operator Point() const { return { left, top }; }

			long width() const { return right - left; }
			long height() const { return bottom - top; }
			bool empty() const { return (top > bottom || left > right); }

			bool contains(const POINT &pt) const
			{
				if(pt.x < left || pt.x > right || pt.y < top || pt.y > bottom)
					return false;
				return true;
			}

			bool contains(long x, long y) const
			{
				if(x < left || x > right || y < top || y > bottom)
					return false;
				return true;
			}

			/*bool contains(const Rect &rect) const
			{
				return (rect + *this == *this);
			}

			Rect intersect(const Rect &rhs) const
			{
				return {
					std::max(left, rhs.left),
					std::max(top, rhs.top),
					std::min(right, rhs.right),
					std::min(bottom, rhs.bottom)
				};
			}
			inline rectangle intersect (
				const rectangle& a,
				const rectangle& b
			) { return a.intersect(b); }
			*/

			/*inline const Rect centered_rect(
				long x,
				long y,
				unsigned long width,
				unsigned long height
			)
			{
				Rect result;
				result.set_left(x - static_cast<long>(width) / 2);
				result.set_top(y - static_cast<long>(height) / 2);
				result.set_right(result.left() + width - 1);
				result.set_bottom(result.top() + height - 1);
				return result;
			}

			inline unsigned long area(const Rect &a) {
				return a.area();
			}

			inline POINT center(const Rect &rect)
			{
				POINT temp(rect.left + rect.right + 1,
						   rect.top + rect.bottom + 1);
				if(temp.x < 0) temp.x -= 1;
				if(temp.y < 0) temp.y -= 1;
				return temp / 2;
			}
			*/
			bool operator== (const Rect &rect) const
			{
				return (left == rect.left) && (top == rect.top) && (right == rect.right) && (bottom == rect.bottom);
			}

			bool operator!= (const Rect &rect) const
			{
				return !(*this == rect);
			}

			inline bool operator< (const Rect &b) const
			{
				if(left < b.left) return true;
				else if(left > b.left) return false;
				else if(top < b.top) return true;
				else if(top > b.top) return false;
				else if(right < b.right) return true;
				else if(right > b.right) return false;
				else if(bottom < b.bottom) return true;
				else if(bottom > b.bottom) return false;
				else                    return false;
			}

			Point topLeft() const { return { left, top }; }
			Point topRight() const { return { left + width(), top }; }
			Point bottomLeft() const { return { left, top - height() }; }
			Point bottomRight() const { return { left + width(), top - height() }; }

			bool HasCursorPos() const{
				Point pt;
				return contains(pt.GetCursorPos()); 
			}
			
			Rect &GetWindowRect(HWND hWnd)
			{
				::GetWindowRect(hWnd, this);
				return *this;
			}

			Rect &GetClientRect(HWND hWnd)
			{
				::GetClientRect(hWnd, this);
				return *this;
			}

			Point point(long value = 0) const { return { left + value, top + value }; }
			SIZE size(long value = 0) const { return { width() + value, height() + value }; }

		};

        class Font
        {
            /*
            auto hdc = ::CreateCompatibleDC(nullptr);
            int DpiY = ::GetDeviceCaps(hdc, LOGPIXELSY);
            int nDPIX = ::GetDeviceCaps(hdc, LOGPIXELSX);
            ::DeleteDC(hdc);

            Get the font height from the specified size.  size is in point units and height in logical
            units (pixels when using MM_TEXT) so we need to make the conversion using the number of
            pixels per logical inch along the screen height.

            auto dpi = GetDpiForSystem();
            int pixelsY = (int)std::ceil(DpiY * ImageSize.cy / 72); // 1 point = 1/72 inch.;
            int font_height = dpi / nDPIY * height;
            int font_width = dpi / nDPIX * width;

            auto lfHeight = -MulDiv(ImageSize.cy, ::GetDeviceCaps(dc, LOGPIXELSY),72 dpi);

            The lfHeight represents the font cell height (line spacing) which includes the internal
            leading; we specify a negative size value (in pixels) for the height so the font mapper
            provides the closest match for the character height rather than the cell height (MSDN).

            */
            HFONT _handle = nullptr;
            bool save = false;
        public:
            int Quality = ANTIALIASED_QUALITY;
            int CharSet = DEFAULT_CHARSET;
            int Size = 16;
            string Name = nullptr;

            Font() = default;

			Font(HFONT handle) :_handle{ handle }
			{
			}

			Font(const LOGFONTW& lf) : Font(lf.lfFaceName, lf.lfHeight, lf.lfQuality, lf.lfCharSet)
			{
			}

            Font(const wchar_t *faceName, int size = 16, int quality = ANTIALIASED_QUALITY, int charSet = DEFAULT_CHARSET)
                : Name{ faceName }, Size{ size }, Quality{ quality }, CharSet{ charSet }, _handle{nullptr}
            {
				create();
            }

            ~Font() { reset(); }

            HFONT get() const { return _handle; }

			Font& create(bool force_create = true)
			{
				//reset();
				if(!force_create)
				{
					if(!Installed(Name, CharSet))
						return *this;
				}
				
				int width = Size;

				if(Size > 0)
				{
					Size = -Size;
					width = 0;
				}

				_handle = ::CreateFontW(Size, width, 0, 0, FW_NORMAL, 0, 0, 0,
										CharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										Quality, DEFAULT_PITCH, Name);
				return *this;
			}

            void reset()
            {
                if(_handle)
                {
                    ::DeleteObject(_handle);
                    _handle = nullptr;
                    Quality = ANTIALIASED_QUALITY;
                    CharSet = DEFAULT_CHARSET;
                    Size = 16;
                    Name = nullptr;
                }
            }

            HFONT release()
            {
                return std::exchange(_handle, nullptr);
            }

            operator HFONT(void) const { return _handle; }
            explicit operator bool(void) const { return _handle != nullptr; }

            uint32_t hash() const { return Name.hash() + Size; }

			static int load(const wchar_t *path)
			{
				return ::AddFontResourceExW(path, FR_PRIVATE, nullptr);
			}

			static bool unload(const wchar_t* path)
			{
				int i = 0;
				while(::RemoveFontResourceExW(path, FR_PRIVATE, nullptr))
				{
					i++;
				}
				return i > 0;
			}

             //Get the font height from the specified size.
            static int Height(int size, HDC dc = nullptr)
            {
                int height = size;
                HDC hdc = dc == nullptr ? ::CreateCompatibleDC(nullptr) : dc;
                if(hdc)
                {
                    height = (int)std::ceil(::GetDeviceCaps(hdc, LOGPIXELSY) * size / 96);
                    if(!dc) ::DeleteDC(hdc);
                }
                return height;
            }

            static HFONT Create(const wchar_t *faceName, int size, int quality = CLEARTYPE_QUALITY, int charSet = DEFAULT_CHARSET)
            {
                return Font(faceName, size, quality, charSet).create().release();
            }

            static bool Installed(const wchar_t *faceName, int charSet = DEFAULT_CHARSET, HDC dc = nullptr)
            {
                bool exists = false;
                if(faceName)
                {
                    HDC hdc = dc == nullptr ? ::CreateCompatibleDC(nullptr) : nullptr;
                    if(hdc)
                    {
                        LOGFONTW lf = { };
                        lf.lfCharSet = uint8_t(charSet);
                        string::Copy(lf.lfFaceName, faceName);
                        //::lstrcpyW(lf.lfFaceName, faceName);
                        ::EnumFontFamiliesExW(hdc, &lf, [](const LOGFONTW *, const TEXTMETRICW *, DWORD, LPARAM lParam)  { 
							*(bool *)lParam = true; return 0; 
						}, (LPARAM)&exists, 0);
                        if(!dc) ::DeleteDC(hdc);
                    }
                }
                return exists;
            }
		};
	}
}