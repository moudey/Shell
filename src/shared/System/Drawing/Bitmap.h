#pragma once

namespace Nilesoft
{
	namespace Drawing
	{
#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) | ((Color << 16) & 0xff0000)

		class Bitmap
		{
		protected:

			HBITMAP handle = nullptr;
/*public void ApplyGamma(double r, double g, double B)/>
        {

            int A,R,G,B;
            Color pixelColor;

            int [] redGamma = new int[256];
            int [] greenGamma = new int[256];
            int [] blueGamma = new int[256];

            for(int i = 0; i<256; i++)
            {
                redGamma[i] = (byte)Math.Min(255,(int)((255*Math.Pow(i/255,1/r))+0.5));
                greenGamma[i] = (byte)Math.Min(255,(int)((255*Math.Pow(i/255,1/g))+0.5));
                blueGamma[i] = (byte)Math.Min(255,(int)((255*Math.Pow(i/255,1/B)/>)+0.5));
            }

            for (int y =0; y < currentBitmap.Height;y++)
            {
                for(int x =0; x <currentBitmap.Width;x++)
                {
                    pixelColor = currentBitmap.GetPixel(x,y);
                    A =  pixelColor.A;
                    R = redGamma[pixelColor.R];
                    G = greenGamma[pixelColor.G];
                    B = blueGamma[pixelColor.B];
                    currentBitmap.SetPixel(x,y,Color.FromArgb((int)A,(int)R,(int)G,(int)B)/>);
                }
            }
            picCanvas.Image = currentBitmap;
        }
*/ 
			/*
Gamma-correct sub pixel rendering
// convert the vertex colour to linear space
	vec4 v_colour_linear = pow4(v_colour, inv_gamma);

	// convert the background colour to linear space
	vec4 v_background_colour_linear = pow4(v_background_colour, inv_gamma);

	// blend
	float r = tex_col.r * v_colour_linear.r + (1.0 - tex_col.r) * v_background_colour_linear.r;
	float g = tex_col.g * v_colour_linear.g + (1.0 - tex_col.g) * v_background_colour_linear.g;
	float b = tex_col.b * v_colour_linear.b + (1.0 - tex_col.b) * v_background_colour_linear.b;

	// gamma encode the result
	gl_FragColor = pow4(vec4(r, g, b, tex_col.a), gamma);
			*/ 
			//preMultiply  
			//alpha andpre - multiply with RGB
			static void premultiply_alpha(uint8_t *pixel, long width, long height, COLORREF color)
			{
				// Calculate alpha
				uint8_t r = GetRValue(color);
				uint8_t g = GetGValue(color);
				uint8_t b = GetBValue(color);
				uint8_t a = 0;

				/*for(int LoopY = 0; LoopY < height; LoopY++)
				{
					for(int LoopX = 0; LoopX < width; LoopX++)
					{
						a = *pixel; // Move alpha and pre-multiply with RGB
						*pixel++ = (b * a) >> 8;
						*pixel++ = (g * a) >> 8;
						*pixel++ = (r * a) >> 8;
						*pixel++ = a; // Set Alpha
					}
				}*/

				for(long i = 0; i < width * height; ++i)
				{
					//a = opacity - uint8_t(((abs(pixel[0] - b) + abs(pixel[1] - g) + abs(pixel[2] - r)) / 3));
					a = 255 - pixel[0];
					// Set colour
					pixel[0] = (r * a) / 255;
					pixel[1] = (g * a) / 255;
					pixel[2] = (b * a) / 255;
					pixel[3] = a;
					pixel += 4;
				}
			}

		public:
			long width{};
			long hieght{};
			uint8_t *data{};

			Bitmap()  noexcept = default;
			Bitmap(const Bitmap &bitmap) noexcept = default;
			Bitmap(Bitmap && bitmap) noexcept = default;
			Bitmap(HBITMAP handle) : handle(handle) { }
			~Bitmap() { destroy(); }

			HBITMAP get() const { return handle; }
			
			bool is_valid() const { return (handle != nullptr); }

			void destroy() 
			{
				handle = Bitmap::Destroy(handle); 
			}

			HBITMAP release() { return std::exchange(handle, nullptr); }
			
			HBITMAP reset(HBITMAP hbitmap = nullptr)
			{
				destroy();
				return handle = hbitmap;
			}

			Bitmap& reset(Bitmap &&bitmap)
			{
				*this = std::move(bitmap);
			}

			operator HBITMAP() const { return handle; }
			explicit operator bool(void) const { return is_valid(); }

			Bitmap &operator=(const Bitmap& bitmap)
			{
				handle = reset(bitmap.handle);
				return(*this);
			}

			Bitmap &operator=(Bitmap &&bitmap) noexcept
			{
				*this = std::exchange(bitmap.handle, nullptr);
				return(*this);
			}

			Bitmap &operator=(HBITMAP hbitmap)
			{
				handle = reset(hbitmap);
				return(*this);
			}

			bool info(BITMAP &bitmap)
			{
				return Info(handle, bitmap);
			}

			HBITMAP trim(HBITMAP hSrc)
			{
				BITMAP bmp{};

				if(Info(hSrc, bmp))
				{
					auto w = bmp.bmWidth;
					auto h = bmp.bmHeight;
					auto b = (uint8_t *)bmp.bmBits;

					int bottom = 0;
					int left = w;
					int right = 0;
					int top = h;

					for(int y = 0; y < h; y++)
					{
						for(int x = 0; x < w; x++)
						{
							auto a = (b + (x * 4))[3];
							if(a > 0)
							{
								if(x < left) left = x;
								if(x >= right) right = x + 1;
								if(y < top) top = y;
								if(y >= bottom) bottom = y + 1;
							}
						}
						b += (w * 4);
					}

					if(left < right && top < bottom)
					{
						SIZE trim = { right - left, bottom - top };
						if(auto hbitmap = DC::CreateDIBSection(trim.cx, trim.cy); hbitmap)
						{
							DC dcmem(::CreateCompatibleDC(nullptr), 1);
							dcmem.select_bitmap(hbitmap);
							dcmem.bitblt({ 0, 0, trim.cx, trim.cy }, hSrc, left, top);
							dcmem.restore_bitmap();
							return hbitmap;
						}
					}
				}
				return nullptr;
			}

			static bool Info(HBITMAP hbitmap, BITMAP &bitmap)
			{
				return sizeof(BITMAP) == ::GetObjectW(hbitmap, sizeof(BITMAP), &bitmap);
			}

			static HBITMAP Destroy(HBITMAP hbitmap)
			{
				if(hbitmap) ::DeleteObject(hbitmap);
				return nullptr;
			}

/*
			/////////////////////////////////////////////////////////////////////
			static HBITMAP GetBitmapByPos(HBITMAP hBMSrc, int nPos)
			{
				HDC hDCSrc, hDCDst;
				BITMAP bm{};
				HBITMAP hBMDst = NULL;

				if((hDCSrc = CreateCompatibleDC(NULL)) != NULL)
				{
					if((hDCDst = CreateCompatibleDC(NULL)) != NULL)
					{
						SelectObject(hDCSrc, hBMSrc);
						::GetObjectW(hBMSrc, sizeof(bm), &bm);
						hBMDst = ::CreateBitmap(bm.bmHeight, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);
						if(hBMDst)
						{
							::GetObjectW(hBMDst, sizeof(bm), &bm);
							auto hOldBmp = ::SelectObject(hDCDst, hBMDst);
							BitBlt(hDCDst, 0, 0, bm.bmHeight, bm.bmHeight, hDCSrc, nPos * bm.bmHeight, 0, SRCCOPY);
							SelectObject(hDCDst, hOldBmp);
						}
						DeleteDC(hDCDst);
					}
					DeleteDC(hDCSrc);
				}
				return hBMDst;
			}

			static HBITMAP GetBitmapByPos4Menu(HBITMAP hBMSrc, int nPos)
			{
				HDC hDCSrc, hDCDst;
				BITMAP bm;
				HBITMAP hBMDst = NULL;
				int cx, cy;

				cx = GetSystemMetrics(SM_CXMENUCHECK);
				cy = GetSystemMetrics(SM_CYMENUCHECK);
				if((hDCSrc = CreateCompatibleDC(NULL)) != NULL)
				{
					if((hDCDst = ::CreateCompatibleDC(NULL)) != NULL)
					{
						SelectObject(hDCSrc, hBMSrc);
						::GetObjectW(hBMSrc, sizeof(bm), &bm);
						hBMDst = CreateBitmap(cx, cy, bm.bmPlanes, bm.bmBitsPixel, NULL);
						if(hBMDst)
						{
							auto hOldBmp = ::SelectObject(hDCDst, hBMDst);
							StretchBlt(hDCDst, 0, 0, cx, cy, hDCSrc, nPos * bm.bmHeight, 0, bm.bmHeight, bm.bmHeight, SRCCOPY);
							SelectObject(hDCDst, hOldBmp);
							::GetObject(hBMDst, sizeof(bm), &bm);
						}
						DeleteDC(hDCDst);
					}
					DeleteDC(hDCSrc);
				}
				return hBMDst;
			}

//-------------------------------------------------------------------------------
// ReplaceColor
//
// Author    : Dimitri Rochette drochette@coldcat.fr
// Specials Thanks to Joe Woodbury for his comments and code corrections
//
// Includes  : Only <windows.h>
/*			HBITMAP hBmp2 = LoadBitmap(g_hinstance, MAKEINTRESOURCE(IDB_SAMPLEBITMAP));
			HBITMAP hBmp = ReplaceColor(hBmp2, 0xff0000, 0x00ff00); // replace blue by green
			DeleteObject(hBmp2);

			// Use your modified Bitmap here

			DeleteObject(hBmp);*/
//
// hBmp         : Source Bitmap
// cOldColor : Color to replace in hBmp
// cNewColor : Color used for replacement
// hBmpDC    : DC of hBmp ( default NULL ) could be NULL if hBmp is not selected
//
// Retcode   : HBITMAP of the modified bitmap or NULL for errors
//
//-------------------------------------------------------------------------------
			static HBITMAP ReplaceColor(HBITMAP hBmp, COLORREF cOldColor, COLORREF cNewColor, HDC hBmpDC)
			{
				HBITMAP RetBmp = nullptr;
				if(hBmp)
				{
					HDC BufferDC = CreateCompatibleDC(nullptr);    // DC for Source Bitmap
					if(BufferDC)
					{
						HBITMAP hTmpBitmap = (HBITMAP)nullptr;
						if(hBmpDC)
							if(hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
							{
								hTmpBitmap = CreateBitmap(1, 1, 1, 1, nullptr);
								SelectObject(hBmpDC, hTmpBitmap);
							}

						HGDIOBJ PreviousBufferObject = SelectObject(BufferDC, hBmp);
						// here BufferDC contains the bitmap

						HDC DirectDC = CreateCompatibleDC(nullptr); // DC for working
						if(DirectDC)
						{
							// Get bitmap size
							BITMAP bm{};
							::GetObject(hBmp, sizeof(bm), &bm);

							// create a BITMAPINFO with minimal initilisation 
							// for the CreateDIBSection
							BITMAPINFO RGB32BitsBITMAPINFO {};
							RGB32BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
							RGB32BitsBITMAPINFO.bmiHeader.biWidth = bm.bmWidth;
							RGB32BitsBITMAPINFO.bmiHeader.biHeight = bm.bmHeight;
							RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
							RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;

							// pointer used for direct Bitmap pixels access
							uint32_t *ptPixels = nullptr;
							HBITMAP DirectBitmap = ::CreateDIBSection(DirectDC,
								(BITMAPINFO *)& RGB32BitsBITMAPINFO,
																	DIB_RGB_COLORS,
																	reinterpret_cast<void **>(&ptPixels),
																	nullptr, 0);
							if(DirectBitmap)
							{
								// here DirectBitmap!=NULL so ptPixels!=NULL no need to test
								auto PreviousObject = SelectObject(DirectDC, DirectBitmap);
								BitBlt(DirectDC, 0, 0,
									   bm.bmWidth, bm.bmHeight,
									   BufferDC, 0, 0, SRCCOPY);

						   // here the DirectDC contains the bitmap

						// Convert COLORREF to RGB (Invert RED and BLUE)
								cOldColor = COLORREF2RGB(cOldColor);
								cNewColor = COLORREF2RGB(cNewColor);

								// After all the inits we can do the job : Replace Color
								for(int i = ((bm.bmWidth * bm.bmHeight) - 1); i >= 0; i--)
								{
									if(ptPixels[i] == cOldColor) ptPixels[i] = cNewColor;
								}
								// little clean up
								// Don't delete the result of SelectObject because it's 
								// our modified bitmap (DirectBitmap)
								SelectObject(DirectDC, PreviousObject);

							 // finish
								RetBmp = DirectBitmap;
							}
							// clean up
							DeleteDC(DirectDC);
						}
						if(hTmpBitmap)
						{
							SelectObject(hBmpDC, hBmp);
							DeleteObject(hTmpBitmap);
						}
						SelectObject(BufferDC, PreviousBufferObject);
						// BufferDC is now useless
						DeleteDC(BufferDC);
					}
				}
				return RetBmp;
			}

			/*
inline BOOL ReplaceDIBColor(HBITMAP* pDIB, COLORREF oldColor,COLORREF newColor)
{
_ASSERTE(pDIB!=NULL);
if (pDIB==NULL) return FALSE;
if (oldColor==newColor) return TRUE;//nothing to do
BOOL bRet=FALSE;

//get color information
DIBSECTION ds;
if (!::GetObject(*pDIB, sizeof(DIBSECTION), &ds)) return FALSE;
uint32_t nColors = ds.dsBmih.biClrUsed ? ds.dsBmih.biClrUsed : 1<8 || nColors>256) return FALSE;//8bpp is max

HDC hDC=::CreateCompatibleDC(NULL);
if (!hDC) return FALSE;
HBITMAP hbmpOld=(HBITMAP)::SelectObject(hDC, *pDIB);
if (hbmpOld!=NULL)
{ //replace color table entries
RGBQUAD clrtbl[256*sizeof(RGBQUAD)];
if (::GetDIBColorTable(hDC, 0, nColors, clrtbl))
{
uint32_t i;
for (i=0; i<nColors; i++)
{
if (oldColor==RGB(clrtbl[i].rgbRed, clrtbl[i].rgbGreen, clrtbl[i].rgbBlue))
{
clrtbl[i].rgbRed=GetRValue(newColor);
clrtbl[i].rgbGreen=GetGValue(newColor);
clrtbl[i].rgbBlue=GetBValue(newColor);
bRet=TRUE;
}
}
if (bRet)//set new table
if (!::SetDIBColorTable(hDC, 0, nColors, clrtbl)) bRet=FALSE;
}
*pDIB=(HBITMAP)::SelectObject(hDC, hbmpOld);
}
::DeleteDC(hDC);
return bRet;
}			*/

/*
1

I have been searching in Google and Stack Overflow, and I cannot find a working example.

I need to convert a HBitmap to a Managed .NET bitmap, but the following code does not preserve the alpha channel.

	private static Bitmap GetBitmapFromHBitmap(IntPtr nativeHBitmap)
	{
		Bitmap bmp = Bitmap.FromHbitmap(nativeHBitmap);
		return bmp;
	}

I found this answer in SO, but it does not work for me, the example preserves the transparency, however it flips my image 180º in Y axis and also rotate it 180º. I don't know why.

This other example seems to work, but it is C++

Someone has this working in C#, and very important, without memory leaks?

Thanks in advance.

EDIT: Regarding the comment from @Hans Passant, I use the following code to get the HBitmap (it's a shell call to get the thumbnail or icon from the OS (only Vista and Win7).

	private static IntPtr GetHBitmap(string fileName, int width, int height, ThumbnailOptions options)
	{
		IShellItem nativeShellItem;
		Guid shellItem2Guid = new Guid(IShellItem2Guid);
		int retCode = SHCreateItemFromParsingName(fileName, IntPtr.Zero, ref shellItem2Guid, out nativeShellItem);

		NativeSize nativeSize = new NativeSize();
		nativeSize.Width = width;
		nativeSize.Height = height;

		IntPtr hBitmap;
		HResult hr = ((IShellItemImageFactory)nativeShellItem).GetImage(nativeSize, options, out hBitmap);

		Marshal.ReleaseComObject(nativeShellItem);

		if (hr == HResult.Ok) return hBitmap;

		throw Marshal.GetExceptionForHR((int) hr);
	}

			*/
		};

	}
}