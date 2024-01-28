#pragma once

namespace Nilesoft
{
	namespace Drawing
	{
		/*
HDC tempHdc         = CreateCompatibleDC(hdc);
// create a bitmap of a size you need, let's say it 100x100
int width = 100;
int height = 100;
HBITMAP canvas = CreateCompatibleBitmap(hdc, width, height);
// select new bitmap into context, don't forget to save old bitmap handle
HBITMAP oldBmp = SelectObject(tepmHdc, canvas);

BLENDFUNCTION blend = {AC_SRC_OVER, 0, 127, AC_SRC_ALPHA};

SetDCPenColor(tempHdc, RGB(255,255,0));
SetDCBrushColor(tempHdc, RGB(255,255,0));
Rectangle(tempHdc, dim.left, dim.top, dim.right, dim.bottom);

bool res = AlphaBlend(hdc, dim.left, dim.top, dim.right, dim.bottom, tempHdc, dim.left, dim.top, dim.right, dim.bottom, blend);
// reset the old bitmap
SelectObect(tempHdc, oldBmp);
// canvas is no longer needed and should be deleted to avoid GDI leaks
DeleteObject(canvas);

return res;

bool paintRect(HDC hdc, RECT dim, COLORREF penCol, COLORREF brushCol, unsigned int opacity)
{
		HDC tempHdc         = CreateCompatibleDC(hdc);
		BLENDFUNCTION blend = {AC_SRC_OVER, 0, 127, 0};

		HBITMAP hbitmap;       // bitmap handle
		BITMAPINFO bmi;        // bitmap header
		// zero the memory for the bitmap info
		ZeroMemory(&bmi, sizeof(BITMAPINFO));

		// setup bitmap info
		// set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas.
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = dim.right-dim.left;
		bmi.bmiHeader.biHeight = dim.bottom-dim.top;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = (dim.right-dim.left) * (dim.bottom-dim.top) * 4;

		// create our DIB section and select the bitmap into the dc
		hbitmap = CreateDIBSection(tempHdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0x0);
		SelectObject(tempHdc, hbitmap);

		SetDCPenColor(tempHdc, RGB(0,0,255));
		SetDCBrushColor(tempHdc, RGB(0,0,255));
		FillRect(tempHdc, &dim, CreateSolidBrush(RGB(0,0,255)));

		return bool(AlphaBlend(hdc, dim.left, dim.top, dim.right, dim.bottom, tempHdc, dim.left, dim.top, dim.right, dim.bottom, blend));
}
		*/
		/*
//Converting from 24bit to 32bit is fairly trivial with two points and casting. Something like this should do:
uint8_t* pArgb;
uint8_t* pRgb;

GetDIBits(BufferDC, hImage, 0, BufferInfo.bmiHeader.biHeight, pRgb, &BufferInfo, DIB_RGB_COLORS);

pArgb = (uint8_t*)malloc(4 * BufferInfo.bmiHeader.biWidth * BufferInfo.bmiHeader.biHeight);
for (int i = 0; i < BufferInfo.bmiHeader.biWidth * BufferInfo.bmiHeader.biHeight; ++i) {
	*(int*)pArgb = 0xff000000 | (*(int*)(pRgb) >> 8);
	pArgb += 4;
	pRgb += 3;
}
		*/

		/*
f the svg path only consists of M L Q Z ZM functions your method looks like this:

private GraphicsPath svgMLQZToGraphicsPath(string svgString)
{
	GraphicsPath graphicsPath = new GraphicsPath();
	float[] x = new float[4];
	float[] y = new float[4];
	string prev = "";
	string[] splits = svgString.Split(' ');
	for (int s = 0; s < splits.Length; s++)
	{
		if (splits[s].Substring(0, 1) == "M")
		{
			x[0] = float.Parse(splits[s].Substring(1).Replace('.', ','));
			y[0] = float.Parse(splits[s + 1].Replace('.', ','));
			s++;
			prev = "M";
			graphicsPath.StartFigure();
		}
		else if (splits[s].Substring(0, 1) == "L")
		{
			x[1] = float.Parse(splits[s].Substring(1).Replace('.', ','));
			y[1] = float.Parse(splits[s + 1].Replace('.', ','));
			graphicsPath.AddLine(new PointF(x[0], y[0]), new PointF(x[1], y[1]));
			x[0] = x[1]; // x[1] = new float();
			y[0] = y[1]; //y[1] = new float();
			s++;
			prev = "L";
		}
		else if (splits[s].Substring(0, 1) == "Q")
		{
			x[1] = x[0] + (2 / 3) * (float.Parse(splits[s].Substring(1).Replace('.', ',')) - x[0]);
			y[1] = y[0] + (2 / 3) * (float.Parse(splits[s + 1].Replace('.', ',')) - y[0]);
			x[3] = float.Parse(splits[s + 2].Replace('.', ','));
			y[3] = float.Parse(splits[s + 3].Replace('.', ','));
			x[2] = x[3] + (2 / 3) * (float.Parse(splits[s].Substring(1).Replace('.', ',')) - y[3]);
			y[2] = y[3] + (2 / 3) * (float.Parse(splits[s + 1].Replace('.', ',')) - y[3]);
			graphicsPath.AddBezier(new PointF(x[0], y[0]), new PointF(x[1], y[1]), new PointF(x[2], y[2]), new PointF(x[3], y[3]));
			x[0] = x[3];
			y[0] = y[3];
			s = s + 3;
			prev = "Q";
		}
		else if (splits[s].Substring(0, 1) == "Z")
		{
			graphicsPath.CloseFigure();
			if (splits[s].Length >= 2 && splits[s].Substring(0, 2) == "ZM")
			{
				x[0] = float.Parse(splits[s].Substring(2).Replace('.', ','));
				y[0] = float.Parse(splits[s + 1].Replace('.', ','));
				s++;
				graphicsPath.StartFigure();
				prev = "M";
			}
		}
		else
		{
			string ok = @"^[a-zA-Z]*$";
			if (!Regex.IsMatch(splits[s + 1].Substring(0, 1), ok))
			{
				string replace = prev + splits[s + 1];
				splits[s + 1] = replace;
			}
		}
	}
	return graphicsPath;
}

		*/ 
		/*void grayscale(HBITMAP hbitmap)
		{
			BITMAP bm;
			GetObject(hbitmap, sizeof(bm), &bm);
			if(bm.bmBitsPixel < 24)
			{
				DebugBreak();
				return;
			}

			HDC hdc = GetDC(HWND_DESKTOP);
			DWORD size = ((bm.bmWidth * bm.bmBitsPixel + 31) / 32) * 4 * bm.bmHeight;

			BITMAPINFO bmi
			{ sizeof(BITMAPINFOHEADER), bm.bmWidth, bm.bmHeight, 1, bm.bmBitsPixel, BI_RGB, size };

			int stride = bm.bmWidth + (bm.bmWidth * bm.bmBitsPixel / 8) % 4;
			BYTE* bits = new BYTE[size];
			GetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
			for(int y = 0; y < bm.bmHeight; y++) {
				for(int x = 0; x < stride; x++) {
					int i = (x + y * stride) * bm.bmBitsPixel / 8;
					BYTE gray = BYTE(0.1 * bits[i + 0] + 0.6 * bits[i + 1] + 0.3 * bits[i + 2]);
					bits[i + 0] = bits[i + 1] = bits[i + 2] = gray;
				}
			}

			SetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
			ReleaseDC(HWND_DESKTOP, hdc);
			delete[]bits;
		}
		*/

		class BufferedPaint
		{
			HPAINTBUFFER _hBufferedPaint{};
			HDC _hdcPaint{};
			HDC _hdcTarget{};
			RECT _rect{};

		public:
			BufferedPaint()
			{
			}

			BufferedPaint(HDC hdcTarget, const RECT *rcTarget)
			{
				_hdcTarget = hdcTarget;
				if(rcTarget)
					_rect = *rcTarget;
			}

			~BufferedPaint()
			{
				end();
			}

			HDC begin(HDC hdcTarget, const RECT *rcTarget, BP_BUFFERFORMAT dwFormat = BPBF_COMPOSITED, DWORD paintParamsFlags = BPPF_ERASE | BPPF_NOCLIP, uint8_t opacity = 255)
			{
				_hdcTarget = hdcTarget;
				if(rcTarget)
					_rect = *rcTarget;
				return begin(dwFormat, paintParamsFlags, opacity);
			}

			HDC begin(uint8_t opacity = 255)
			{
				return begin(BPBF_COMPOSITED, BPPF_ERASE | BPPF_NOCLIP, opacity);
			}

			HDC begin(BP_BUFFERFORMAT dwFormat, DWORD paintParamsFlags, uint8_t opacity)
			{
				BLENDFUNCTION bf = { AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA };
				BP_PAINTPARAMS params = { sizeof(BP_PAINTPARAMS) };
				params.dwFlags = paintParamsFlags;
				params.pBlendFunction = &bf;
				_hBufferedPaint = ::BeginBufferedPaint(_hdcTarget, &_rect, dwFormat, &params, &_hdcPaint);
				if(!_hBufferedPaint)
					return nullptr;
				return _hdcPaint;
			}

			bool clear() {
				if(_hBufferedPaint)
					return S_OK == ::BufferedPaintClear(_hBufferedPaint, &_rect);
				return false;
			}

			HDC hdc() const { return _hdcPaint; }
			HDC targetHDC() const { return _hdcTarget; }
			const RECT *rect() const { return &_rect; }
			operator HDC() const { return _hdcPaint; }

			bool set_alpha(uint8_t opacity = 255, const RECT *rect = nullptr)
			{
				if(_hBufferedPaint)
					return S_OK == ::BufferedPaintSetAlpha(_hBufferedPaint, rect, opacity);
				return false;
			}

			void end(bool updateTarget = true)
			{
				if(_hBufferedPaint)
				{
					::EndBufferedPaint(_hBufferedPaint, updateTarget);
					_hBufferedPaint = nullptr;
				}
			}

			bool is_valid() const
			{
				return _hBufferedPaint && _hdcPaint;
			}

			static bool init() {
				return S_OK == ::BufferedPaintInit();
			}

			static bool uninit() {
				return S_OK == ::BufferedPaintUnInit();
			}
		};

		class DC
		{
			HWND _hWnd = nullptr;
			int _release = 0;
			HDC	_handle = nullptr;
			HBITMAP _hbitmap = nullptr;
			HFONT _hfont= nullptr;
			HBRUSH _hbrush = nullptr;
			HPEN _hpen = nullptr;
			COLORREF _text_color = CLR_INVALID;
			COLORREF _bkcolor = CLR_INVALID;
			int _bkcolor_mode = TRANSPARENT;
			int _savedDC = 0;

		public:
			inline static constexpr long DPI = 96;

		public:
			DC() {}
			DC(HDC hdc) : _handle{ hdc } {}
			DC(HDC hdc, int release) : _handle{ hdc }, _release{ release } {}
			DC(HDC hdc, HWND hWnd) : _handle{ hdc }, _hWnd{ hWnd }, _release{ 2 } {}
			DC(HWND hWnd) : _handle{ ::GetWindowDC(hWnd)}, _hWnd{hWnd}, _release{2} {}

			~DC() { release(); }

			operator HDC() const { return _handle; }
			explicit operator bool() const { return _handle != nullptr; }

			HDC release(int release = -1)
			{
				if(_handle)
				{
					if(release > 0)
						_release = release;

					if(_release == 1)
						::DeleteDC(_handle);
					else if(_release == 2)
						::ReleaseDC(_hWnd, _handle);
					_handle = nullptr;
				}
				return _handle;
			}

			DC& create_compatible(HDC hdc = nullptr)
			{
				_handle = ::CreateCompatibleDC(hdc);
				_release = 1;
				return *this;
			}

			HDC CreateCompatibleDC() const
			{
				return ::CreateCompatibleDC(_handle);
			}

			DC &GetWindowDC(HWND hWnd, bool save = false)
			{
				auto release();
				_handle = ::GetWindowDC(hWnd);
				_hWnd = hWnd; 
				_release = save ? 0 : 2;
				return *this;
			}
		
			DC &GetDC(HWND hWnd, bool save = false)
			{
				auto release();
				_handle = ::GetDC(hWnd);
				_hWnd = hWnd;
				_release = save ? 0 : 2;
				return *this;
			}

			int save() { _savedDC = ::SaveDC(_handle); return _savedDC; }
			bool restore(int savedDC = 0) { return ::RestoreDC(_handle, savedDC == 0 ? _savedDC : savedDC); }

			template<typename T>
			T select(T obj)
			{
				return static_cast<T>(::SelectObject(_handle, obj));
			}

			auto select_stock(int obj)
			{
				return ::SelectObject(_handle, ::GetStockObject(obj));
			}
			
			int set_back_mode(bool transparent = true)
			{
				_bkcolor_mode = ::SetBkMode(_handle, transparent ? TRANSPARENT : OPAQUE);
				return _bkcolor_mode;
			}
			
			auto get_text() const { return ::GetTextColor(_handle); }
			auto get_back() const { return ::GetBkColor(_handle); }

			auto set_text(COLORREF color) 
			{
				_text_color = ::SetTextColor(_handle, color);
				return  _text_color;
			}

			auto set_back(COLORREF color)
			{
				_bkcolor = ::SetBkColor(_handle, color);
				return _bkcolor;
			}

			auto SetBrushOrg(int x=0, int y=0, Point *point = nullptr) {
				return ::SetBrushOrgEx(_handle, x, y, point);
			}

			HBITMAP select_bitmap(HBITMAP hbitmap)
			{
				_hbitmap = select(hbitmap);
				return _hbitmap;
			}

			HBITMAP restore_bitmap()
			{
				return select(_hbitmap);
			}

			auto set_brush(HBRUSH brush)
			{
				_hbrush = select(brush);
				return _hbrush;
			}

			auto set_brush(COLORREF color)
			{
				return ::SetDCBrushColor(_handle, color);
			}

			auto set_pen(COLORREF color)
			{
				return ::SetDCPenColor(_handle, color);
			}

			auto set_pen(HPEN pen)
			{
				_hpen = select(pen);
				return _hpen;
			}

			auto set_font(HFONT hfont) 
			{
				_hfont = select(hfont);
				return _hfont;
			}

			auto get()const { return _handle; }

			template<typename T>
			auto get(int type) const
			{
				return static_cast<T>(::GetCurrentObject(_handle, type));
			}

			auto get_font() { return get<HFONT>(OBJ_FONT); }
			auto get_brush() { return get<HBRUSH>(OBJ_BRUSH); }
			auto get_pen() { return get<HPEN>(OBJ_PEN); }
			auto get_pixel(int x, int y) const { return ::GetPixel(_handle, x, y); }
			auto set_pixel(int x, int y, uint32_t color) const { return ::SetPixel(_handle, x, y, color); }
			auto restore_bruach()
			{
				_hbrush = set_brush(_hbrush);
				return _hbrush;
				//return select(get_stock<HBRUSH>(DC_BRUSH));
			}

			/*template<typename T>
			T set_stock(int i)
			{
				select(static_cast<T>(::GetStockObject(i)));
			}*/

			auto draw_line(int x, int y) const
			{
				return ::LineTo(_handle,x, y);
			}

			auto move(int x, int y, Point *point = nullptr) const
			{
				return ::MoveToEx(_handle, x, y, point);
			}

			auto draw_rect(const Rect & rect, int radius) const
			{ 
				return ::RoundRect(_handle, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
			}

			auto draw_rect(const Rect &rect) const
			{
				return ::Rectangle(_handle, rect.left, rect.top, rect.right, rect.bottom);
			}

			auto draw_fillrect(const Rect & rect, uint32_t fill_color, uint32_t pen = CLR_INVALID)
			{
				if(pen == CLR_INVALID)
					select_stock(NULL_PEN);
				else
				{
					select_stock(DC_PEN);
					set_pen(pen);
				}

				select_stock(DC_BRUSH);
				set_brush(fill_color);
				return ::Rectangle(_handle, rect.left, rect.top, rect.right, rect.bottom);
			}

			auto draw_fill_rounded_rect(const Rect &rect, int r, uint32_t fill_color, uint32_t pen = CLR_INVALID)
			{
				if(pen == CLR_INVALID)
					select_stock(NULL_PEN);
				else
				{
					select_stock(DC_PEN);
					set_pen(pen);
				}

				select_stock(DC_BRUSH);
				set_brush(fill_color);
				return ::RoundRect(_handle, rect.left, rect.top, rect.right, rect.bottom, r, r);
			}

			auto fill_rect(Rect* rect, int stock_brush) const
			{
				return ::FillRect(_handle, rect, static_cast<HBRUSH>(::GetStockObject(stock_brush)));
			}

			auto fill_rect(const Rect& rect, HBRUSH brush) const
			{
				return ::FillRect(_handle, &rect, brush);
			}

			auto fill_rect(HBRUSH brush) const
			{
				return fill_rect({ 0, 0, -1, -1 }, brush);
			}

			int stretchblt(int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop = SRCCOPY)
			{
				return ::StretchBlt(_handle, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, rop);
			}

			bool bitblt(const Rect &rect, HDC hdc, int x = 0, int y = 0, DWORD rop = SRCCOPY) const {
				return ::BitBlt(_handle, rect.left, rect.top, rect.right, rect.bottom, hdc, x, y, rop);
			}

			bool bitblt(const Rect &rect, HBITMAP hbitmap, int x = 0, int y = 0, DWORD rop = SRCCOPY) const
			{
				DC dcmem(CreateCompatibleDC(), 1);
				dcmem.select_bitmap(hbitmap);
				auto ret = bitblt(rect, dcmem, x, y, rop);
				dcmem.reset_bitmap();
				return ret;
			}

			int draw_image(const POINT &pt_dst, const SIZE &size_dst, 
							HDC hdc, 
							const POINT &pt_src, const SIZE &size_src, uint8_t opacity = 255) const
			{
				return ::GdiAlphaBlend(_handle, pt_dst.x, pt_dst.y, size_dst.cx, size_dst.cy,
									   hdc, pt_src.x, pt_src.y, size_src.cx, size_src.cy,
									   { AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA });
			}

			bool draw_image(const POINT &pt, const SIZE &size, HDC hdc, uint8_t opacity = 255) const
			{
				return draw_image(pt, size, hdc, { 0, 0 }, size, opacity);
			}

			bool draw_image9(const Rect& dst, HDC hdc, const Rect& src, uint8_t opacity = 255) const
			{
				return draw_image({ dst.left, dst.top }, { dst.right, dst.bottom },
								  hdc, { src.left, src.top }, { src.right, src.bottom }, opacity);
			}
			
			int draw_image(const POINT &pt_dst, const SIZE &size_dst, HBITMAP hbitmap,
							const POINT &pt_src, const SIZE &size_src, uint8_t opacity = 255, bool delete_hbitmap = false) const
			{
				int ret = FALSE;
				if(hbitmap)
				{
					DC memDC(::CreateCompatibleDC(_handle), 1);
					if(memDC)
					{
						if(memDC.select_bitmap(hbitmap))
						{
							ret = draw_image({ pt_dst.x, pt_dst.y }, { size_dst.cx, size_dst.cy }, memDC,
											 { pt_src.x, pt_src.y }, { size_src.cx, size_src.cy }, opacity);
							memDC.restore_bitmap();
						}
					}

					if(delete_hbitmap)
						::DeleteObject(hbitmap);
				}
				return ret;
			}

			bool draw_image(const POINT &pt, const SIZE &size, HBITMAP hbitmap,
							 uint8_t opacity = 255, bool delete_hbitmap = false) const
			{
				return draw_image({ pt.x, pt.y }, { size.cx, size.cy }, hbitmap,
								  { 0, 0 }, { size.cx, size.cy }, opacity, delete_hbitmap);
			}

			/*
			bool draw_image9(const Rect &dst, HBITMAP hbitmap, const Rect &src, uint8_t opacity = 255, bool delete_hbitmap = false) const
			{
				return draw_image({ dst.left, dst.top }, { dst.right, dst.bottom }, hbitmap,
								  { src.left, src.top }, { src.right, src.bottom }, opacity, delete_hbitmap);
			}

			bool draw_image9(const Rect &rect, HBITMAP hbitmap, uint8_t opacity = 255, bool delete_hbitmap = false) const
			{
				return draw_image9(rect, hbitmap, { 0, 0, rect.width(), rect.height() }, opacity, delete_hbitmap);
			}

			bool draw_image(const Rect &rect, HDC hdc, uint8_t opacity = 255) const
			{
				Rect src = { 0, 0, rect.width(), rect.height()};
				return draw_image(rect, hdc, src, opacity);
			}


			bool draw_image0(const Rect &dst, HBITMAP hbitmap, const Rect &src, uint8_t opacity = 255) const
			{
				bool ret = false;
				if(hbitmap)
				{
					if(auto memDC = ::CreateCompatibleDC(_handle); memDC)
					{
						auto prev_bitmap = ::SelectObject(memDC, hbitmap);
						ret = draw_image(dst, memDC, src, opacity);
						::SelectObject(memDC, prev_bitmap);
						::DeleteDC(memDC);
					}
				}
				return ret;
			}

			bool draw_image0(const Rect &rect, HBITMAP hbitmap, uint8_t opacity = 255) const
			{
				return draw_image0(rect, hbitmap, { 0, 0, rect.width(), rect.height() }, opacity);
			}*/

			/*HBITMAP to_hbitmap(const Rect &rect) const
			{
				auto hbitmap = this->createbitmap(rect.width(), rect.height());
				if(hbitmap)
				{
					DC dcmem(CreateCompatibleDC(), 1);
					dcmem.select_bitmap(hbitmap);
					dcmem.draw_image({ 0, 0, rect.width(), rect.height() }, _handle, rect.left, rect.top);
					return dcmem.reset_bitmap();
				}
				return nullptr;
			}*/

			/*HBITMAP CreateDIBSection(const Rect &rect) const
			{
				BITMAPINFOHEADER bi = { sizeof(bi), rect.width(), -rect.height(), 1, 32, BI_RGB };
				DWORD *bits = nullptr;
				auto hbitmap = ::CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS,
											 reinterpret_cast<void **>(&bits), nullptr, 0);
				if(hbitmap)
				{
					DC dcmem(CreateCompatibleDC(), 1);
					dcmem.select_bitmap(hbitmap);
					dcmem.draw_image({ 0, 0, rect.width(), rect.height() }, _handle, rect.left, rect.top);
					return dcmem.reset_bitmap();
				}
				return nullptr;
			}*/

			/*void draw_img(HBITMAP hbitmap, const Rect &rect, uint8_t opacity) const
			{
				if(hbitmap)
				{
					DC memDC(::CreateCompatibleDC(_handle), 1);
					if(memDC)
					{
						if(memDC.select_bitmap(hbitmap))
						{
							draw_image({ rect.left, rect.top, rect.width(), rect.height() }, memDC, { 0, 0, rect.width(), rect.height() }, opacity * 255 / 100);
							memDC.reset_bitmap();
						}
					}
					::DeleteObject(hbitmap);
				}
			}
			*/
			
			auto draw_text(const wchar_t *text, int length, Rect& rect, uint32_t format)
			{
				return ::DrawTextW(_handle, text, length, &rect, format);
			}

			auto draw_glyph(wchar_t glyph, const Rect& rect, uint32_t format = DT_CENTER)
			{
				return ::DrawTextW(_handle, &glyph, 1, (RECT*)&rect, format | DT_VCENTER | DT_SINGLELINE);
			}

			auto draw_glyph(wchar_t glyph, const Rect& rect, uint32_t color, uint32_t format)
			{
				set_text(color);
				return draw_glyph(glyph, rect, format);
			}

			auto text_extent(const wchar_t *text, int length, SIZE *sz)
			{
				return ::GetTextExtentPoint32W(_handle, text, length, sz);
			}

			auto measure(const wchar_t *text, int length, Rect* rect, uint32_t format = DT_NOCLIP)
			{
				return ::DrawTextW(_handle, text, length, rect, DT_CALCRECT | format );
			}

			auto exclude_clip_rect(const Rect& rect) const
			{
				return ::ExcludeClipRect(_handle, rect.left, rect.top, rect.right, rect.bottom);
			}

			//reset
			auto reset_font() { return select(_hfont); }
			auto reset_brush() { return select(_hbrush); }
			auto reset_pen() { return select(_hpen); }
			auto reset_text() { return set_text(_text_color); }
			auto reset_bkcolor() { return set_back(_bkcolor); }
			HBITMAP reset_bitmap() { return select_bitmap(_hbitmap); }

			auto add_bruch() {
				return select_stock(DC_BRUSH);
			}

			auto remove_bruch() {
				return select_stock(NULL_BRUSH);
			}

			auto add_pen() {
				return select(::GetStockObject(DC_PEN));
			}

			auto remove_pen() {
				return select_stock(NULL_PEN);
			}

			HBITMAP createbitmap(int w, int h) const {
				return ::CreateCompatibleBitmap(_handle, w, h);
			}

			int dpiX() const {
				return GetDeviceCaps(LOGPIXELSX);
			}

			int dpiY() const {
				return GetDeviceCaps(LOGPIXELSY);
			}

			int GetDeviceCaps(int index) const {
				return ::GetDeviceCaps(_handle, index);
			}

			int GetDeviceCapsY() const {
				return GetDeviceCaps(LOGPIXELSY);
			}

			int GetDeviceCapsX() const {
				return GetDeviceCaps(LOGPIXELSX);
			}

			HBITMAP toHBITMAP(int cx, int cy) const
			{
				return toHBITMAP(_handle, cx, cy);
			}

			static HBITMAP toHBITMAP(HDC hdc, int cx, int cy)
			{
				HBITMAP hbitmap{};
				if(hdc)
				{
					hbitmap = ::CreateCompatibleBitmap(hdc, cx, cy);
					if(hbitmap)
					{
						auto hdcmem = ::CreateCompatibleDC(hdc);
						if(hdcmem)
						{
							auto old = ::SelectObject(hdcmem, hbitmap);
							::BitBlt(hdcmem, 0, 0, cx, cy, hdc, 0, 0, SRCCOPY);
							::SelectObject(hdcmem, old);
							::DeleteDC(hdcmem);
						}
					}
				}
				return hbitmap;
			}

			static HBITMAP toHBITMAP(HWND hWnd)
			{
				Rect rc = hWnd;
				DC dc = hWnd;
				return toHBITMAP(dc, rc.width(), rc.height());
			}

			static HBITMAP CreateDIBSection(long width, long height, uint8_t **lpbits = nullptr) {
				uint8_t *bits = nullptr;
				BITMAPINFOHEADER bi = { sizeof(bi),width, height, 1, 32 };
				auto hbitmap = ::CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS,
												  reinterpret_cast<void **>(&bits), nullptr, 0);
				if(lpbits) *lpbits = bits;
				return hbitmap;
			}

			static HBRUSH stock_brush(int i) { return get_stock<HBRUSH>(i); }

			template<typename T>
			static T get_stock(int i)
			{
				return static_cast<T>(::GetStockObject(i));
			}
		};


/*
		class DC
		{
		private:
			HWND hWnd = nullptr;
			bool create = false;
			bool _delete = true;
			bool _save = false;
		public:
			HDC	Handle;

		public:
			DC() noexcept : Handle(nullptr), hWnd(nullptr) { }
			DC(HWND hwnd) : Handle(nullptr), hWnd(hwnd) { }
			DC(HDC hdc, bool delete_) : Handle(hdc), hWnd(nullptr), _delete(delete_) { }
			~DC() { Release(); }

			HDC Create() { return Handle = ::GetDC(hWnd); }

			HDC Create(const wchar_t* lpszDriver, const wchar_t* lpszDevice,
					   const wchar_t* lpszOutput, const DEVMODEW *lpInitData)
			{
				create = true;
				return Handle = ::CreateDCW(lpszDriver, lpszDevice, lpszOutput, lpInitData);
			}

			HDC CreateCompatible(HDC hdc = nullptr)
			{
				create = true;
				return Handle = ::CreateCompatibleDC(hdc);
			}

			template<typename T>
			T Select(T h)
			{
				return (T)::SelectObject(Handle, h);
			}

			void Release()
			{
				if(Handle && _delete)
				{
					if(create)
						::DeleteDC(Handle);
					else
						::ReleaseDC(hWnd, Handle);
					Handle = nullptr;
				}
				hWnd = nullptr;
			}

			operator HDC() const { return Handle; }

			int BackgroundMode()
			{
				return ::GetBkMode(Handle);
			}

			int BackgroundMode(int mode)
			{
				return ::SetBkMode(Handle, mode);
			}

			COLORREF BackgroundColor()
			{
				return ::GetBkColor(Handle);
			}

			COLORREF BackgroundColor(COLORREF color)
			{
				return ::SetBkColor(Handle, color);
			}

			COLORREF TextColor()
			{
				return ::GetTextColor(Handle);
			}

			COLORREF TextColor(COLORREF color)
			{
				return ::SetTextColor(Handle, color);
			}

			COLORREF TextColor(COLORREF color, int bkMode)
			{
				::SetBkMode(Handle, bkMode);
				return ::SetTextColor(Handle, color);
			}

			int DrawString(const wchar_t* text, int length, LPRECT rect, uint32_t format, COLORREF color, int bkMode = TRANSPARENT) const
			{
				::SetBkMode(Handle, bkMode);
				::SetTextColor(Handle, color);
				return ::DrawTextW(Handle, text, length, rect, format);
			}

			int DrawString(const wchar_t* text, int length, LPRECT rect, uint32_t format) const
			{
				return ::DrawTextW(Handle, text, length, rect, format);
			}

			BOOL DrawImage(int x, int y, int width, int height, HBITMAP hbitmap, bool alphaBlend = false) const
			{
				return DrawImage(x, y, width, height, hbitmap, 0, 0, alphaBlend);
			}

			BOOL DrawImage(int x, int y, int width, int height, HBITMAP hbitmap, int xSrc, int ySrc, bool alphaBlend) const
			{
				BOOL ret = FALSE;
				DC dc;
				dc.CreateCompatible(Handle);
				auto oldBitmap = dc.Select(hbitmap);
				if(alphaBlend)
					ret = DrawImage(x, y, width, height, dc, xSrc, ySrc, 255);
				else
					ret = DrawImage(x, y, width, height, dc);
				dc.Select(oldBitmap);
				return ret;
			}

			BOOL DrawImage(int x, int y, int width, int height, HDC hdcSrc) const
			{
				return DrawImage(x, y, width, height, hdcSrc, 0, 0, SRCCOPY);
			}

			BOOL DrawImage(int x, int y, int width, int height, HDC hdcSrc, int xSrc, int ySrc, DWORD rop) const
			{
				return ::BitBlt(Handle, x, y, width, height, hdcSrc, xSrc, ySrc, rop);
			}

			// Draw image with alpha blend
			BOOL DrawImage(int x, int y, int width, int height, HDC hdcSrc, int xSrc, int ySrc, int alpha = 255) const
			{
				if(alpha < 0) alpha = 0;
				else if(alpha > 255) alpha = 255;
				BLENDFUNCTION bf { AC_SRC_OVER, 0, (byte)alpha, AC_SRC_ALPHA };
				return ::GdiAlphaBlend(Handle, x, y, width, height, hdcSrc, xSrc, ySrc, width, height, bf);
			}

			 // Alpha blend bitmap into destination.  Alpha is 0 (nothing) to 255 (max).
			void AlphaBlt(HDC destdc, RECT *rdest, HDC srcdc, RECT *rsrc, BYTE alpha)
			{
				if(alpha < 0) alpha = 0;
				if(alpha > 255) alpha = 255;
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = alpha;
				bf.AlphaFormat = AC_SRC_ALPHA;
				::GdiAlphaBlend(destdc, rdest->left, rdest->top, rdest->right, rdest->bottom,
						   srcdc, rsrc->left, rsrc->top, rsrc->right, rsrc->bottom, bf);
			}

			//int FillRectangle(const Brush& br, const RECT *rect)
			//{ return ::FillRect(Handle, rect, br); }

			int FillRectangle(const HBRUSH hbr, const RECT *rect)
			{
				return ::FillRect(Handle, rect, hbr);
			}

			int FillRectangle(COLORREF color, const RECT *rect)
			{
				return ::FillRect(Handle, rect, Brush(color));
			}

			int FillRectangle(const HBRUSH hbr, int x, int y, int width, int hegth)
			{
				RECT rect { x, y, width , hegth };
				return ::FillRect(Handle, &rect, hbr);
			}

			int FillRectangle(COLORREF color, int x, int y, int width, int hegth)
			{
				RECT rect { x, y, width , hegth };
				return ::FillRect(Handle, &rect, Brush(color));
			}

			COLORREF Pixel(int x, int y)
			{
				return ::GetPixel(Handle, x, y);
			}

			COLORREF DrawPixel(COLORREF color, int x, int y)
			{
				return ::SetPixel(Handle, x, y, color);
			}

			//void DrawLine(COLORREF color, int x1, int y1, int x2, int y2)
			//{
			//	for(int i = x1; i < x2; i++)
			//		::SetPixel(Handle, i, y1, color);
			//
			//	for(int i = y1; i < y2; i++)
			//		::SetPixel(Handle, x1, i, color);
			//}

			void DrawLine(COLORREF color, int x, int y, int w, int h)
			{
				for(int i = x; i < x + w; i++)
				{
					for(int p = 0; p < h; p++)
					{
						::SetPixel(Handle, i, y + p, color);
					}
				}
			}

			int MeasureString(const wchar_t* str, int length, SIZE *lpsize)
			{
				if(!lpsize)
				{
					SIZE size { };
					::GetTextExtentPoint32W(Handle, str, length, &size);
					return size.cx;
				}

				::GetTextExtentPoint32W(Handle, str, length, lpsize);
				return lpsize->cx;
			}

			int ExcludeClipRect(int left, int top,  int right,  int bottom)
			{
				return ::ExcludeClipRect(Handle, left, top, right, bottom);
			}

			int ExcludeClipRect(const RECT *rect)
			{
				return ::ExcludeClipRect(Handle, rect->left, rect->top, rect->right, rect->bottom);
			}

			COLORREF SetPixel(int x, int y, COLORREF color)
			{
				return ::SetPixel(Handle, x, y, color);
			}

			void SetXRangePixel(int x, int y, COLORREF color, int count)
			{
				for(int i = 0; i < count; i++)
					::SetPixel(Handle, x + i, y, color);
			}

			void FillSolidRect(int x, int y, int cx, int cy, COLORREF clr)
			{
				RECT rect = { x, y, x + cx, y + cy };
				COLORREF clrOld = ::SetBkColor(Handle, clr);
				::ExtTextOutW(Handle, 0, 0, ETO_OPAQUE, &rect, nullptr, 0, nullptr);
				::SetBkColor(Handle, clrOld);
			}

			void FillSolidRect(RECT *lprc, COLORREF clr)
			{
				COLORREF clrOld = ::SetBkColor(Handle, clr);
				::ExtTextOutW(Handle, 0, 0, ETO_OPAQUE, lprc, nullptr, 0, nullptr);
				::SetBkColor(Handle, clrOld);
			}

			int FillSolidRect(int x, int y, int cx, int cy, HBRUSH hBrush)
			{
				RECT rect = { x, y, x + cx, y + cy };
				return ::FillRect(Handle, &rect, hBrush);
			}

			void DrawFrame(RECT *lprc, int nSize, COLORREF clr)
			{
				FillSolidRect(lprc->left, lprc->top, lprc->right - lprc->left - nSize, nSize, clr);
				FillSolidRect(lprc->left, lprc->top, nSize, lprc->bottom - lprc->top - nSize, clr);
				FillSolidRect(lprc->right, lprc->top, -nSize, lprc->bottom - lprc->top, clr);
				FillSolidRect(lprc->left, lprc->bottom, lprc->right - lprc->left, -nSize, clr);
			}

			void DrawFrame(RECT *lprc, int nSize, HBRUSH hBrush)
			{
				FillSolidRect(lprc->left, lprc->top, lprc->right - lprc->left - nSize, nSize, hBrush);
				FillSolidRect(lprc->left, lprc->top, nSize, lprc->bottom - lprc->top - nSize, hBrush);
				FillSolidRect(lprc->right - nSize, lprc->top, nSize, lprc->bottom - lprc->top, hBrush);
				FillSolidRect(lprc->left, lprc->bottom - nSize, lprc->right - lprc->left, nSize, hBrush);
			}

			bool GetParentDCClipBox(HWND pwnd, HDC hdc, RECT *lprc)
			{
				if(::GetClipBox(hdc, lprc) == NULLREGION)
					return false;

				if((::GetClassLongPtrW(pwnd, GCL_STYLE) & CS_PARENTDC) == 0)
					return true;

				RECT rc{};
				::GetClientRect(pwnd, &rc);

				return ::IntersectRect(lprc, lprc, &rc);
			}
		};*/

		class Pen
		{
			HPEN _handle = nullptr;

		public:
			Pen() {}
			Pen(HPEN hpen) : _handle{ hpen } {}

			~Pen() { release(); }

			operator HPEN() const { return _handle; }
			explicit operator bool() const { return _handle != nullptr; }

			Pen& release()
			{
				if(_handle)
					::DeleteObject(_handle);
				_handle = nullptr;
				return *this;
			}

			Pen& create(int style, int w, uint32_t color)
			{
				release();
				_handle = ::CreatePen(style, w, color);
				return *this;
			}

			Pen& create_ext(int w, uint32_t color, uint32_t style = BS_SOLID, int iPenStyle = PS_GEOMETRIC | PS_ENDCAP_SQUARE)
			{
				release();
				LOGBRUSH lb{ style, color, 0 };
				_handle = ::ExtCreatePen(iPenStyle, w, &lb, 0, nullptr);
				return *this;
			}

			auto get()const { return _handle; }
		};
	}
}