#pragma once

#include <d2d1.h>

//#include "Icon.h"
//#include "DC.h"
//#include "Bitmap.h"

//COLORREF LightenColor(const UINT iScale, const COLORREF clrColor) noexcept
//{
//	const auto nScale = static_cast<int>(iScale);
//	const auto R = MulDiv(255 - GetRValue(clrColor), nScale, 255) + GetRValue(clrColor);
//	const auto G = MulDiv(255 - GetGValue((clrColor & 0xFFFF)), nScale, 255) + GetGValue((clrColor & 0xFFFF));
//	const auto B = MulDiv(255 - GetBValue(clrColor), nScale, 255) + GetBValue(clrColor);
//
//	/*
//	const auto nScale = gsl::narrow_cast<int>(iScale);
//	const auto R = Dcx::dcxMulDiv32(255 - GetRValue(clrColor), nScale, 255) + GetRValue(clrColor);
//	const auto G = Dcx::dcxMulDiv32(255 - GetGValue((clrColor & 0xFFFF)), nScale, 255) + GetGValue((clrColor & 0xFFFF));
//	const auto B = Dcx::dcxMulDiv32(255 - GetBValue(clrColor), nScale, 255) + GetBValue(clrColor);
//	*/
//	return RGB(R, G, B);
//}
//
//
//COLORREF DarkenColor(const UINT iScale, const COLORREF clrColor) noexcept
//{
//	const auto nScale = static_cast<int>(iScale);
//	const auto R = MulDiv(GetRValue(clrColor), (255 - nScale), 255);
//	const auto G = MulDiv(GetGValue((clrColor & 0xFFFF)), (255 - nScale), 255);
//	const auto B = MulDiv(GetBValue(clrColor), (255 - nScale), 255);
//
//	/*const auto nScale = gsl::narrow_cast<int>(iScale);
//	const auto R = Dcx::dcxMulDiv32(GetRValue(clrColor), (255 - nScale), 255);
//	const auto G = Dcx::dcxMulDiv32(GetGValue((clrColor & 0xFFFF)), (255 - nScale), 255);
//	const auto B = Dcx::dcxMulDiv32(GetBValue(clrColor), (255 - nScale), 255);
//	*/
//	return RGB(R, G, B);
//}

namespace Nilesoft
{
	namespace Drawing
	{
/*
#define PALETTERGB(r,g,b)   (0x02000000 | RGB(r,g,b))
#define PALETTEINDEX(i)     ((COLORREF)(0x01000000 | (DWORD)(WORD)(i)))
*/


/*
static uint32_t ToAbgr(Color color)
{
	return ((uint32_t)color.A << 24)
		| ((uint32_t)color.B << 16)
		| ((uint32_t)color.G << 8)
		| color.R;
}*/

		struct RGB
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
		};

		struct HSL
		{
			uint8_t H;
			uint8_t S;
			uint8_t L;
		};

		struct RGBA
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t a;

			void copy(const RGBA *clr)
			{
				b = clr->b;
				g = clr->g;
				r = clr->r;
				a = clr->a;
			}

			void pre(RGBA *clr)
			{
				b = uint8_t((clr->r * a) / 255);
				g = uint8_t((clr->g * a) / 255);
				r = uint8_t((clr->b * a) / 255);
			}

			void prem()
			{
				auto b2 = b;
				b = r;
				r = b2;
			}

			void toBGRA(RGBA *clr)
			{
				b = clr->r;
				g = clr->g;
				r = clr->b;
				a = clr->a;
			}

			uint32_t toBGRA(uint32_t rgba)
			{
				auto clr = (RGBA *)&rgba;
				uint32_t res = 0;

				clr = (RGBA *)&rgba;
				auto bgra = (RGBA *)&res;

				bgra->b = clr->r;
				bgra->g = clr->g;
				bgra->r = clr->b;
				bgra->a = clr->a;


				return res;
			}

			uint32_t MakeArgb(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
			{
				return(uint32_t)(((uint32_t)(red << 16 | green << 8 | blue << 0 | alpha << 24))) & UINT32_MAX;
			}
		};
		/*
		WORD rgb_color(DWORD abgr_color)
{
    // registry keeps the colors in ABGR format, we want RGB
    auto r = (abgr_color & 0xFF);
    auto g = (abgr_color & 0xFF00) >> 8;
    auto b = (abgr_color & 0xFF0000) >> 16;
    return (r << 16) | (g << 8) | b;
}
		* */
		struct Color
		{
		private:
			struct BGRA
			{
				uint8_t b{};
				uint8_t g{};
				uint8_t r{};
				uint8_t a{};
			};

			static long hlsrgb_value(long n1, long n2, short hue)
			{
				long val;

				while(hue >= 256)
					hue -= 256;

				while(hue < 0)
					hue += 256;

				if(hue < (256 / 6))
					val = n1 + (n2 - n1) * (long)hue / (256 / 6);
				else if(hue < 256 / 2)
					val = n2;
				else if(hue < 2 * 256 / 3)
					val = n1 + (long)(n2 - n1) * (2 * 256 / 3 - hue) / (256 / 6);
				else
					val = n1;

				val >>= 8;
				if(val > 255) val = 255;
				if(val < 0) val = 0;
				return(val);
			}

			/*static uint32_t length(uint32_t value)
			{
				uint32_t ret = 0;
				while(value != 0)
				{
					++ret;
					value /= 10;
				}
				return ret;
			}
			*/

			static uint32_t to_uint(const BGRA *value)
			{
				return *(uint32_t *)value;
			}

			static BGRA to_abgr(uint32_t value)
			{
				return *reinterpret_cast<BGRA *>(&value);
			}

			BGRA _value;
			bool _empty = true;

		public:

			Color()
			{
			}

			Color(uint32_t value)
				: _value(to_abgr(value)), _empty(value == 0)
			{
				/*if(_value.a == 0)
				{
					// 0xFFFFFF
					//	if(length(value) <= 8)
					//		_value.a = 0xFF;
				}*/
			}

			Color(uint32_t value, uint8_t a)
				: _value(to_abgr(value)), _empty(value == 0 && a == 0)
			{
				_value.a = a;
			}

			Color(uint8_t b, uint8_t g, uint8_t r, uint8_t a)
				: _value{ b, g, r, a}, _empty(false)
			{
			}

			Color(const Color& other) 
				: _value(other._value), _empty(other._empty)
			{
			}

			~Color() 
			{
				_value = {};
				_empty = true;
			}
			
			operator D2D_COLOR_F() const { return to_d2d(); }
			operator COLORREF() const { return to_uint(&_value); }
			operator uint32_t() const { return to_uint(&_value); }
			explicit operator bool() const { return !empty(); }
			
			bool operator ==(const Color &rhs) const { return equals(rhs); }
			bool operator ==(uint32_t rhs) { return equals(rhs);}
			bool operator ==(nullptr_t) { return empty(); }

			bool empty() const { return _empty; }

			__declspec(property(get = get_a, put = set_a)) uint8_t a;

			// Transparent

			Color from(uint32_t color)
			{
				_empty = false;
				auto old = _value;
				_value = to_abgr(color);
				return to_uint(&old);
			}

			Color from(uint32_t color, uint8_t opacity)
			{
				auto old = from(color);
				this->opacity(opacity);
				return old;
			}

			Color from(uint8_t b, uint8_t g, uint8_t r)
			{
				_empty = false;
				auto old = _value;
				_value.b = b;
				_value.g = g;
				_value.r = r;
				return to_uint(&old);
			}

			Color from(uint8_t b, uint8_t g, uint8_t r, uint8_t a)
			{
				_empty = false;
				auto old = _value;
				_value = { b, g, r, a };
				return to_uint(&old);
			}

			Color from(const Color& rgb)
			{
				_empty = false;
				auto old = _value;
				_value.b = rgb.b();
				_value.g = rgb.g();
				_value.r = rgb.r();
				return to_uint(&old);
			}

			Color &from_COLORREF(uint32_t color, uint8_t a = 0xFF)
			{
				_empty = false;
				_value = to_abgr(color);
				_value.a = a;
				return *this;
			}

			uint8_t b() const { return _value.b; }
			uint8_t g() const { return _value.g; }
			uint8_t r() const { return _value.r; }
			uint8_t get_a() const { return _value.a; }

			uint8_t b(uint8_t b) { return _value.b = b; }
			uint8_t g(uint8_t g) { return _value.g = g; }
			uint8_t r(uint8_t r) { return _value.r = r; }
			uint8_t set_a(uint8_t a) { return _value.a = a; }

			uint8_t opacity(uint8_t value)
			{
				return _value.a = ToByte(value > 100 ? 100 : value);
			}

			Color &opacities(int value = 50)
			{
				//_value.a = static_cast<uint8_t>(std::clamp<int>((value * _value.a) / 100, 0, 255));
				auto ret = (value * _value.a) / 100;
				_value.a = static_cast<uint8_t>(ret > 255 ? 255 : ret);
				return *this;
			}

			bool equals(uint32_t other) const
			{
				return 0 == ::memcmp(&_value, &other, sizeof(BGRA));
			}

			bool equals(const Color& other) const
			{
				return 0 == ::memcmp(&_value, &other._value, sizeof(BGRA));
			}

			uint32_t to_RGB() const
			{
				BGRA rgba = { _value.r, _value.g, _value.b,  0 };
				return to_uint(&rgba);
			}

			uint32_t to_ARGB() const
			{
				BGRA rgba = { _value.r, _value.g, _value.b,  _value.a };
				return to_uint(&rgba);
			}

			uint32_t to_ABGR() const
			{
			//	BGRA rgba = { _value.b, _value.g, _value.r,  _value.a };
				return to_uint(&_value);
			}

			uint32_t to_BGR() const
			{
				BGRA rgba = { _value.b, _value.g, _value.r,  0 };
				return to_uint(&rgba);
			}

			static float tof(uint8_t v)
			{
				return static_cast<float>(v) / 255.0f;
			}

			D2D_COLOR_F to_d2d() const
			{
				return { tof(_value.b), tof(_value.g), tof(_value.r), tof(_value.a) };
			}

			uint32_t invert()
			{
				return 0xFFFFFFu ^ to_uint(&_value);
			}

			//Creates a new light color from the specified color and lightens it by the specified percentage.
			Color& light(int value = 50)
			{
				return adjust(value);
			}

			//Creates a new dark color from the specified color and darkens it by the specified percentage.
			Color &dark(int value = 50)
			{
				return adjust(-value);
			}

			/*
			Color c1 = Color.Red;
			Color c2 = Color.FromArgb(c1.A, (int)(c1.R * 0.8), (int)(c1.G * 0.8), (int)(c1.B * 0.8));
			*/ 
			//luminance 
			//ColorHLSToRGB/ColorRGBToHLS/ColorAdjustLuma "Shlwapi.h"
			//lighten/darken lighter/darker, luminosity adjust brightness  
			Color &adjust(int value)
			{
				auto _adjust =[](int src, int value) -> auto
				{
					//return static_cast<uint8_t>(std::clamp<int>(((value * src) / 100) + src, 0, 255));
					auto ret = ((value * src) / 100) + src;
					return static_cast<uint8_t>(ret > 255 ? 255 : ret);
				};

				_value.r = _adjust(_value.r, value);
				_value.g = _adjust(_value.g, value);
				_value.b = _adjust(_value.b, value);
				return *this;
			}

			bool is_light() const
			{
				return (((5 * _value.g) + (2 * _value.r) + _value.b) > (8 * 128));
			}

			//215+86+43=344
			static bool IsLight(uint32_t clr)
			{
				return Color(clr).is_light();;
			}

			static uint32_t InvertColor(uint32_t rgbaColor)
			{
				//0xFFFFFFFFu
				return 0xFFFFFF00u ^ rgbaColor; // Assumes alpha is in the rightmost byte, change as needed
			}

			static bool IsColorDark(uint32_t color)
			{
				return (GetRValue(color) * 2 + GetGValue(color) * 5 + GetBValue(color)) <= 1024;
			}

			static uint32_t Invert(uint32_t color)
			{
				return 0xFFFFFFu ^ color; 
				//return RGB(~GetRValue(crf), ~GetGValue(crf), ~GetBValue(crf));
			}

			/*
			static inline RGBA MakeRGBA(uint32_t8_t R, uint32_t8_t G, uint32_t8_t B, uint32_t8_t A)
			{
				return RGB(R, G, B) | (A << 24);
			}
			/*
			static inline RGBA MakeOpaque(RGBA base)
			{
				return base | 0xff000000;
			}

			static inline uint32_t8_t GetAValue(RGBA rgba)
			{
				return (rgba >> 24) & 0xff;
			}*/

			static uint32_t Swap(uint32_t argb)
			{
				auto rgba = reinterpret_cast<BGRA *>(&argb);
				auto r = rgba->r;
				auto b = rgba->b;

				rgba->r = b;
				rgba->b = r;

				return argb;
			}

			/*static uint32_t ToBGR(uint32_t rgb)
			{
				return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
			}

			static uint32_t ToRGB(uint32_t rgb)
			{
				return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
			}

			static uint32_t FromBGR(uint32_t rgb)
			{
				return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
			}*/

			static uint32_t HSLTORGB(short h, short s, short l)
			{
				auto pj_uscale_by = [](uint16_t x, uint16_t p, uint16_t q)
				{
					return (short)((x * p) / q);
				};

				long	m1, m2;
				short	rv, gv, bv;

				if(l <= 128)
					m2 = (long)l * (256 + s);
				else
					m2 = ((long)(l + s) << 8) - (long)l * s;

				m1 = 512 * (long)l - m2;

				if(s == 0)
				{
					rv = l;
					gv = l;
					bv = l;
				}
				else
				{
					s = (short)hlsrgb_value(m1, m2, h - 256 / 3);
					l = (short)hlsrgb_value(m1, m2, h);
					h = (short)hlsrgb_value(m1, m2, h + 256 / 3);
					bv = s;
					gv = l;
					rv = h;
				}

				// scale down to VGA values
				rv += 2;
				bv += 2;
				gv += 2;

				if(rv > 255) rv = 255;
				if(gv > 255) gv = 255;
				if(bv > 255) bv = 255;

				return RGB(rv, gv, bv);
			}

			static void RGBTOHSL(uint32_t rgb, short* h, short* s, short* l)
			{
				auto pj_uscale_by = [](uint16_t x, uint16_t p, uint16_t q)
				{
					long l = (x * p) / q;
					return short((int)l);
				};

				short max, min;
				short rc, gc, bc;
				short hv, lv, sv;

				short r = GetRValue(rgb);
				short g = GetGValue(rgb);
				short b = GetBValue(rgb);

				max = r;
				if(g > max) max = g;
				if(b > max) max = b;
				min = r;
				if(g < min) min = g;
				if(b < min) min = b;
				lv = (max + min) >> 1;

				if(max == min)
				{
					hv = sv = 0;
				}
				else
				{
					if(lv < 128)
						sv = ((long)(max - min) << 8) / (max + min);
					else
						sv = ((long)(max - min) << 8) / (512 - max - min);

					if(sv >= 256)
						sv = 255;
					rc = pj_uscale_by(256, max - r, max - min);
					gc = pj_uscale_by(256, max - g, max - min);
					bc = pj_uscale_by(256, max - b, max - min);

					if(r == max)
						hv = bc - gc;
					else if(g == max)
						hv = 2 * 256 + rc - bc;
					else
						hv = 4 * 256 + gc - rc;

					hv /= 6;
					while(hv < 0) hv += 256;
					while(hv >= 256) hv -= 256;
				}

				*h = hv;
				*l = lv;
				*s = sv;
			}

			COLORREF ToABGR(COLORREF rgb)
			{
				return MakeARGB(255, GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
			}

			COLORREF ToARGB(COLORREF rgb)
			{
				return MakeARGB(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
			}

		/*	COLORREF ToCOLORREF() const
			{
				return RGB(GetRed(), GetGreen(), GetBlue());
			}*/

			enum
			{
				AlphaShift = 24,
				RedShift = 16,
				GreenShift = 8,
				BlueShift = 0
			};

			enum
			{
				AlphaMask = 0xff000000,
				RedMask = 0x00ff0000,
				GreenMask = 0x0000ff00,
				BlueMask = 0x000000ff
			};


			static uint32_t MakeARGB(uint32_t rgb, uint8_t a)
			{
				return rgb | (a << 24);
			}

			static uint32_t MakeARGB(IN BYTE a,
								 IN BYTE r,
								 IN BYTE g,
								 IN BYTE b)
			{
				return (((uint32_t)(b) << BlueShift) |
						((uint32_t)(g) << GreenShift) |
						((uint32_t)(r) << RedShift) |
						((uint32_t)(a) << AlphaShift));
			}

			long MakeArgb(byte alpha, byte red, byte green, byte blue) {
				return(long)(((uint32_t)(red << RedShift |
											  green << GreenShift |
											  blue << BlueShift |
											  alpha << AlphaShift))) & UINT32_MAX;
			}

			/// <summary>
		///       Returns the Hue-Saturation-Lightness (HSL) lightness
		///       for this <see cref='System.Drawing.Color'/> .
		/// </summary>
			float GetBrightness(int R, int G, int B) {
				float r = (float)R / 255.0f;
				float g = (float)G / 255.0f;
				float b = (float)B / 255.0f;

				float max, min;

				max = r; min = r;

				if(g > max) max = g;
				if(b > max) max = b;

				if(g < min) min = g;
				if(b < min) min = b;

				return(max + min) / 2;
			}

			/// <summary>
		///       Returns the Hue-Saturation-Lightness (HSL) hue
		///       value, in degrees, for this <see cref='System.Drawing.Color'/> .  
		///       If R == G == B, the hue is meaningless, and the return value is 0.
		/// </summary>
			float GetHue(int R, int G, int B)
			{
				if(R == G && G == B)
					return 0; // 0 makes as good an UNDEFINED value as any

				float r = (float)R / 255.0f;
				float g = (float)G / 255.0f;
				float b = (float)B / 255.0f;

				float max, min;
				float delta;
				float hue = 0.0f;

				max = r; min = r;

				if(g > max) max = g;
				if(b > max) max = b;

				if(g < min) min = g;
				if(b < min) min = b;

				delta = max - min;

				if(r == max) {
					hue = (g - b) / delta;
				}
				else if(g == max) {
					hue = 2 + (b - r) / delta;
				}
				else if(b == max) {
					hue = 4 + (r - g) / delta;
				}
				hue *= 60;

				if(hue < 0.0f) {
					hue += 360.0f;
				}
				return hue;
			}

			//The lightness of this System.Drawing.Color.The lightness ranges from 0.0 through
				//     1.0, where 0.0 represents black and 1.0 represents white.
			float GetSaturation(int R, int G, int B)
			{
				float r = (float)R / 255.0f;
				float g = (float)G / 255.0f;
				float b = (float)B / 255.0f;

				float max, min;
				float l, s = 0;

				max = r; min = r;

				if(g > max) max = g;
				if(b > max) max = b;

				if(g < min) min = g;
				if(b < min) min = b;

				// if max == min, then there is no color and
				// the saturation is zero.
				//
				if(max != min) {
					l = (max + min) / 2;

					if(l <= .5) {
						s = (max - min) / (max + min);
					}
					else {
						s = (max - min) / (2 - max - min);
					}
				}
				return s;
			}

			static uint8_t ToByte(uint8_t percent) {
				return (percent * 255) / 100;
			}

			static uint8_t ToPercent(uint8_t nbyte) {
				return (nbyte * 100) / 255;
			}
		};
	}
}