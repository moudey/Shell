#pragma once

//#include "Icon.h"
//#include "DC.h"
//#include "Bitmap.h"

#include <wincodec.h>
#include <d2d1.h>

#pragma warning(disable:4458)
//#pragma warning(disable:4189)//local variable is initialized but not referenced

namespace Nilesoft
{
	namespace Drawing
	{
		using namespace Text;
		using namespace IO;

		struct ImageData
		{
			uint32_t id;
			uint32_t width = 0;
			uint32_t height = 0;
			HBITMAP bitmap = nullptr;
			uint8_t *pixels = nullptr;
			uintptr_t pixels_hash = 0;

			~ImageData() { clear(); }
			ImageData(uint32_t _id = 0, HBITMAP hbitmap = nullptr) : id(_id), bitmap(hbitmap){ }
			
			ImageData(ImageData &&img) noexcept { *this = std::move(img); };

			ImageData(ImageData const &imgdata) = delete;
			ImageData &operator=(const ImageData &imgdata) = delete;

			uint32_t size() const
			{
				return width * height * 4;
			}

			bool is_measure()
			{
				return (width >= 1 && width <= 32) && (height >= 1 && height <= 32);
			}

			//min (1*1* 4) and max=(32*32*4)
			bool is_valid() const
			{
				return (size() >= 4 && size() < 4096);
			}

			void clear()
			{
				delete[] std::exchange(this->pixels, nullptr);
				if(bitmap) ::DeleteObject(std::exchange(bitmap, nullptr));
				id = 0;
				width = 0;
				height = 0;
			}

			ImageData &operator=(ImageData &&rhs) noexcept
			{
				if(this != &rhs)
				{
					if(pixels || bitmap) clear();

					id = rhs.id;
					width = rhs.width;
					height = rhs.height;
					pixels = rhs.pixels;
					bitmap = rhs.bitmap;

					rhs.bitmap = nullptr;
					rhs.pixels = nullptr;
				}
				return *this;
			}

			bool equals(const ImageData *other)
			{
				if(size() == other->size())
					return equals(pixels, other->pixels, size());
				return false;
			}


			uintptr_t hash()
			{
				pixels_hash = hash(pixels, size()); 
				return pixels_hash;
			}

			static uintptr_t hash(const uint8_t *buf, long size)
			{
				const auto offset_basis = 5381llU;
				//const auto prime = 33llu;

				uintptr_t hash = offset_basis;
				for(long i = 0; i < size; i++)
					hash = hash * 33 + int(buf[i]);
				return hash == offset_basis ? 0 : hash;
			}

			template<typename T>
			uintptr_t copy(const T *src, long size)
			{
				if(pixels) delete[] pixels;
				pixels = new uint8_t[size]{};
				pixels_hash = copy(pixels, (const uint8_t *)src, size);
				return pixels_hash;
			}

			static uintptr_t copy(uint8_t *dst, const uint8_t *src, long size)
			{
				const auto offset_basis = 5381llU;
				//const auto prime = 33llu;

				uintptr_t hash = offset_basis;
				for(long i = 0; i < size; i++)
				{
					dst[i] = src[i];
					hash = hash * 33 + int(src[i]);
				}
				return hash == offset_basis ? 0 : hash;
			}

			static bool equals(const uint8_t *buf1, const uint8_t *buf2, long size)
			{
				for(long i = 0; i < size; i++)
				{
					if(buf1[i] != buf2[i]) return false;
				}
				return true;
			}
		};

		class Image : public Bitmap
		{
		public:

			/*static HBITMAP FromBytes(ImageData* imgdata)
			{
				if(!imgdata->bitmap)
				{
					// initialize return value
					if(imgdata->pixels)
					{
						uint8_t* pixels = nullptr;
						BITMAPINFOHEADER bi = { sizeof(bi), long(imgdata->width), -long(imgdata->height), 1, 32 };
						// create a DIB section that can hold the image
						auto hbitmap = ::CreateDIBSection(nullptr, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pixels, nullptr, 0);
						if(hbitmap && pixels)
						{
							imgdata->bitmap = hbitmap;
							//imgdata->copy(pixels, imgdata->pixels, imgdata->size());
							::memcpy(pixels, imgdata->pixels, imgdata->size());
						}
					}
				}
				return imgdata->bitmap;
			}

			// Gets the "bits" from the bitmap, andcopies them into a buffer that's pointed to by hbitmap.
			static bool ToBytes(ImageData* imgdata)
			{
				DIBSECTION dib{};
				if(::GetObjectW(imgdata->bitmap, sizeof(dib), &dib) &&
				   dib.dsBm.bmBits)
				{
					imgdata->width = dib.dsBm.bmWidth;
					imgdata->height = dib.dsBm.bmHeight;
					imgdata->copy(dib.dsBm.bmBits, dib.dsBm.bmWidth * dib.dsBm.bmHeight * 4);
					return true;
				}
				return false;
			}*/

		public:
			static HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
			{
				HDC hdcMem, hdcMem2;
				HBITMAP hbmMask;
				BITMAP bm;

				// Create monochrome (1 bit) mask bitmap.
				GetObject(hbmColour, sizeof(BITMAP), &bm);
				hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

				// Get some HDCs that are compatible with the display driver
				hdcMem = CreateCompatibleDC(0);
				hdcMem2 = CreateCompatibleDC(0);

				SelectObject(hdcMem, hbmColour);
				SelectObject(hdcMem2, hbmMask);

				// Set the background colour of the colour image to the colour
				// you want to be transparent.
				SetBkColor(hdcMem, crTransparent);

				// Copy the bits from the colour image to the B+W mask... everything
				// with the background colour ends up white while everythig else ends up
				// black...Just what we wanted.
				BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

				// Take our new mask and use it to turn the transparent colour in our
				// original colour image to black so the transparency effect will
				// work right.
				BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

				// Clean up.
				DeleteDC(hdcMem);
				DeleteDC(hdcMem2);

				return hbmMask;
			}

			/*static void DrawTransparent(HDC hdc, int x, int y, HBITMAP hBitmap, COLORREF crColour)
			{
				COLORREF crOldBack = SetBkColor(hdc, RGB(255, 255, 255));
				COLORREF crOldText = SetTextColor(hdc, RGB(0, 0, 0));
				HDC dcImage, dcTrans;

				// Create two memory dcs for the image and the mask
				dcImage = CreateCompatibleDC(hdc);
				dcTrans = CreateCompatibleDC(hdc);

				// Select the image into the appropriate dc
				HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(dcImage, hBitmap);

				// Create the mask bitmap
				BITMAP bitmap;
				GetObject(hBitmap, sizeof(BITMAP), &bitmap);
				HBITMAP bitmapTrans = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);

				// Select the mask bitmap into the appropriate dc
				HBITMAP pOldBitmapTrans = (HBITMAP)SelectObject(dcTrans, bitmapTrans);

				// Build mask based on transparent colour
				SetBkColor(dcImage, crColour);
				BitBlt(dcTrans, 0, 0, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCCOPY);

				// Do the work - True Mask method - cool if not actual display
				BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCINVERT);
				BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, dcTrans, 0, 0, SRCAND);
				BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCINVERT);

				// Restore settings
				SelectObject(dcImage, pOldBitmapImage);
				SelectObject(dcTrans, pOldBitmapTrans);
				SetBkColor(hdc, crOldBack);
				SetTextColor(hdc, crOldText);
			}*/

			/*static HBITMAP ReplaceColor(HBITMAP hBmp, HDC hBmpDC, COLORREF cOldColor, COLORREF cNewColor, COLORREF cTolerance = 0x101010)
			{
				HBITMAP RetBmp = NULL;
				if(hBmp)
				{
					HDC BufferDC = CreateCompatibleDC(NULL);    // DC for Source Bitmap
					if(BufferDC)
					{
						HBITMAP hTmpBitmap = (HBITMAP)NULL;
						if(hBmpDC)
							if(hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
							{
								hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
								SelectObject(hBmpDC, hTmpBitmap);
							}

						HGDIOBJ PreviousBufferObject = SelectObject(BufferDC, hBmp);
						// here BufferDC contains the bitmap

						HDC DirectDC = CreateCompatibleDC(NULL); // DC for working
						if(DirectDC)
						{
							// Get bitmap size
							BITMAP bm;
							GetObject(hBmp, sizeof(bm), &bm);

							// create a BITMAPINFO with minimal initilisation 
							// for the CreateDIBSection
							BITMAPINFO RGB32BitsBITMAPINFO;
							ZeroMemory(&RGB32BitsBITMAPINFO, sizeof(BITMAPINFO));
							RGB32BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
							RGB32BitsBITMAPINFO.bmiHeader.biWidth = bm.bmWidth;
							RGB32BitsBITMAPINFO.bmiHeader.biHeight = bm.bmHeight;
							RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
							RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;

							// pointer used for direct Bitmap pixels access
							UINT *ptPixels;

							HBITMAP DirectBitmap = CreateDIBSection(DirectDC,
																	(BITMAPINFO *)&RGB32BitsBITMAPINFO,
																	DIB_RGB_COLORS,
																	(void **)&ptPixels,
																	NULL, 0);
							if(DirectBitmap)
							{
								// here DirectBitmap!=NULL so ptPixels!=NULL no need to test
								HGDIOBJ PreviousObject = SelectObject(DirectDC, DirectBitmap);
								BitBlt(DirectDC, 0, 0,
									   bm.bmWidth, bm.bmHeight,
									   BufferDC, 0, 0, SRCCOPY);

								// here the DirectDC contains the bitmap

								// Convert COLORREF to RGB (Invert RED and BLUE)
								cOldColor = (cOldColor & 0xff00) | ((cOldColor >> 16) & 0xff) | ((cOldColor << 16) & 0xff0000);
								cNewColor = (cNewColor & 0xff00) | ((cNewColor >> 16) & 0xff) | ((cNewColor << 16) & 0xff0000);

								// Keep on hand highest and lowest values for the "transparent" pixels
								BYTE lr = GetRValue(cOldColor);
								BYTE lg = GetGValue(cOldColor);
								BYTE lb = GetBValue(cOldColor);
								BYTE hr = min(0xff, lr + GetRValue(cTolerance));
								BYTE hg = min(0xff, lg + GetGValue(cTolerance));
								BYTE hb = min(0xff, lb + GetBValue(cTolerance));

								COLORREF cToleranceColor = RGB(hr, hg, hb);

								// After all the inits we can do the job : Replace Color
								for(int i = ((bm.bmWidth * bm.bmHeight) - 1); i >= 0; i--)
								{
									if(ptPixels[i] >= cOldColor && GetRValue(ptPixels[i]) <= hr && GetGValue(ptPixels[i]) <= hg && GetBValue(ptPixels[i]) <= hb)
										ptPixels[i] = cNewColor;
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
			}*/

			/*static double GetBitmapLuminance(HBITMAP hBmp, HDC hBmpDC, HRGN hRgn = NULL)
			{
				double luminanceY = 0;
				double nbPixels = 0;
				COLORREF crPix;

				HDC hMemDC = (!hBmpDC) ? CreateCompatibleDC(NULL) : hBmpDC;
				SelectObject(hMemDC, hBmp);

				BITMAP bm;
				GetObject(hBmp, sizeof(bm), &bm);
				long width = bm.bmWidth;
				long height = bm.bmHeight;

				for(int xx = 0; xx < width; xx++)
					for(int yy = 0; yy < height; yy++)
					{
						if(hRgn && !PtInRegion(hRgn, xx, yy)) continue;
						crPix = GetPixel(hMemDC, xx, yy);
						luminanceY += ((double)0.299 * GetRValue(crPix) +
									   (double)0.587 * GetGValue(crPix) +
									   (double)0.114 * GetBValue(crPix)) / 255; // see https://en.wikipedia.org/wiki/HSL_and_HSV#Lightness
						nbPixels++;
					}


				if(!hBmpDC) DeleteDC(hMemDC);
				return (nbPixels > 0) ? luminanceY / nbPixels : 0;

			}*/

			/*static HRGN BitmapToRegion(HBITMAP hBmp, HDC hBmpDC = NULL, COLORREF cTransparentColor = 0, COLORREF cTolerance = 0x101010)
			{
				HRGN hRgn = NULL;

				if(hBmp)
				{
					// Create a memory DC inside which we will scan the bitmap content
					HDC hMemDC = (!hBmpDC) ? CreateCompatibleDC(NULL) : hBmpDC;
					if(hMemDC)
					{
						// Get bitmap size
						BITMAP bm;
						GetObject(hBmp, sizeof(bm), &bm);

						// Create a 32 bits depth bitmap and select it into the memory DC 
						BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
								sizeof(BITMAPINFOHEADER),	// biSize 
								bm.bmWidth,					// biWidth; 
								bm.bmHeight,				// biHeight; 
								1,							// biPlanes;
								32,							// biBitCount 
								BI_RGB,						// biCompression; 
								0,							// biSizeImage; 
								0,							// biXPelsPerMeter;
								0,							// biYPelsPerMeter; 
								0,							// biClrUsed; 
								0							// biClrImportant; 
						};
						VOID *pbits32;
						HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
						if(hbm32)
						{
							HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

							// Create a DC just to copy the bitmap into the memory DC
							HDC hDC = CreateCompatibleDC(hMemDC);
							if(hDC)
							{
								// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
								BITMAP bm32;
								GetObject(hbm32, sizeof(bm32), &bm32);
								while(bm32.bmWidthBytes % 4)
									bm32.bmWidthBytes++;

								// Copy the bitmap into the memory DC
								HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
								BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

								// For better performances, we will use the ExtCreateRegion() function to create the
								// region. This function take a RGNDATA structure on entry. We will add rectangles by
								// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
								DWORD maxRects = ALLOC_UNIT;
								HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
								RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
								pData->rdh.dwSize = sizeof(RGNDATAHEADER);
								pData->rdh.iType = RDH_RECTANGLES;
								pData->rdh.nCount = pData->rdh.nRgnSize = 0;
								SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

								// Keep on hand highest and lowest values for the "transparent" pixels
								BYTE lr = GetRValue(cTransparentColor);
								BYTE lg = GetGValue(cTransparentColor);
								BYTE lb = GetBValue(cTransparentColor);
								BYTE hr = min(0xff, lr + GetRValue(cTolerance));
								BYTE hg = min(0xff, lg + GetGValue(cTolerance));
								BYTE hb = min(0xff, lb + GetBValue(cTolerance));

								COLORREF cToleranceColor = RGB(hr, hg, hb);

								// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
								BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
								for(int y = 0; y < bm.bmHeight; y++)
								{
									// Scan each bitmap pixel from left to right
									for(int x = 0; x < bm.bmWidth; x++)
									{
										// Search for a continuous range of "non transparent pixels"
										int x0 = x;
										LONG *p = (LONG *)p32 + x;
										while(x < bm.bmWidth)
										{
											if(*p >= cTransparentColor && *p <= cToleranceColor) break;

											BYTE b = (BYTE)(((DWORD)(*p)) >> 16);// cannot use GetRValue(*p) because it give blue value ( (*p) is RGB and not COLORREF type);
											if(b >= lr && b <= hr)
											{
												b = (BYTE)(((DWORD)(*p)) >> 8);// here GetGValue(*p) may be used
												if(b >= lg && b <= hg)
												{
													b = ((BYTE)(*p));// cannot use GetBValue(*p); because it give red value!
													if(b >= lb && b <= hb)
														// This pixel is "transparent"
														break;
												}

											}
											p++;
											x++;
										}

										if(x > x0)
										{
											// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
											if(pData->rdh.nCount >= maxRects)
											{
												GlobalUnlock(hData);
												maxRects += ALLOC_UNIT;
												hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
												pData = (RGNDATA *)GlobalLock(hData);
											}
											RECT *pr = (RECT *)&pData->Buffer;
											SetRect(&pr[pData->rdh.nCount], x0, y, x, y + 1);
											if(x0 < pData->rdh.rcBound.left)
												pData->rdh.rcBound.left = x0;
											if(y < pData->rdh.rcBound.top)
												pData->rdh.rcBound.top = y;
											if(x > pData->rdh.rcBound.right)
												pData->rdh.rcBound.right = x;
											if(y + 1 > pData->rdh.rcBound.bottom)
												pData->rdh.rcBound.bottom = y + 1;
											pData->rdh.nCount++;

											// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
											// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
											if(pData->rdh.nCount == 2000)
											{
												HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
												if(hRgn)
												{
													CombineRgn(hRgn, hRgn, h, RGN_OR);
													DeleteObject(h);
												}
												else
													hRgn = h;
												pData->rdh.nCount = 0;
												SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
											}
										}
									}

									// Go to next row (remember, the bitmap is inverted vertically)
									p32 -= bm32.bmWidthBytes;
								}

								// Create or extend the region with the remaining rectangles
								HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
								if(hRgn)
								{
									CombineRgn(hRgn, hRgn, h, RGN_OR);
									DeleteObject(h);
								}
								else
									hRgn = h;

								// Clean up
								SelectObject(hDC, holdBmp);
								DeleteDC(hDC);
							}

							DeleteObject(SelectObject(hMemDC, holdBmp));
						}

						if(!hBmpDC) DeleteDC(hMemDC);
					}
				}

				return hRgn;
			}*/

			static HBITMAP Copy(HDC hdc, HBITMAP hBmp)
			{
				HDC memInDC = ::CreateCompatibleDC(hdc);
				HDC memOutDC = ::CreateCompatibleDC(hdc);

				BITMAP bm;
				::GetObject(hBmp, sizeof(BITMAP), &bm);
				HBITMAP hOutputBmp = ::CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);

				::SelectObject(memInDC, hBmp);
				::SelectObject(memOutDC, hOutputBmp);

				::BitBlt(memOutDC, 0, 0, bm.bmWidth, bm.bmHeight, memInDC, 0, 0, SRCCOPY);

				::DeleteDC(memInDC);
				::DeleteDC(memOutDC);

				return hOutputBmp;
			}

			//https://github.com/noelmartinon/popup/blob/master/Picture.hpp
			static HBITMAP ResizeBmp(HBITMAP hBmpSrc, SIZE newSize)
			{
				// current size
				BITMAP bmpInfo;
				GetObject(hBmpSrc, sizeof(BITMAP), &bmpInfo);
				SIZE oldSize;
				oldSize.cx = bmpInfo.bmWidth;
				oldSize.cy = bmpInfo.bmHeight;

				// select src in DC
				HDC hdc = GetDC(NULL);
				HDC hDCSrc = CreateCompatibleDC(hdc);
				HBITMAP hOldBmpSrc = (HBITMAP)SelectObject(hDCSrc, hBmpSrc);

				// create bitmap dest and select it in DC
				HDC hDCDst = CreateCompatibleDC(hdc);
				HBITMAP hBmpDst = CreateCompatibleBitmap(hdc, newSize.cx, newSize.cy);
				HBITMAP hOldBmpDst = (HBITMAP)SelectObject(hDCDst, hBmpDst);

				// resize
				StretchBlt(hDCDst, 0, 0, newSize.cx, newSize.cy, hDCSrc, 0, 0, oldSize.cx, oldSize.cy, SRCCOPY);

				// free resources
				SelectObject(hDCSrc, hOldBmpSrc);
				SelectObject(hDCDst, hOldBmpDst);
				DeleteDC(hDCSrc);
				DeleteDC(hDCDst);
				ReleaseDC(NULL, hdc);

				return hBmpDst;
			}

			inline bool InvertBitmap(HDC hdc, HBITMAP hBmp)
			{
				// The functor exists to perform the operation this way.
				// return ManipulateDIBits(hdc, hBmp, Invert());
				// However, this ROP3 method in BitBlt is implemented in hardware:
				BITMAP bm;
				::GetObject(hBmp, sizeof(BITMAP), &bm);

				HDC memDC = ::CreateCompatibleDC(hdc);
				::SelectObject(memDC, hBmp);
				::BitBlt(memDC, 0, 0, bm.bmWidth, bm.bmHeight, 0, 0, 0, DSTINVERT);
				::DeleteDC(memDC);
				return true;
			}

			static HBITMAP FromBMP(const wchar_t *path, int resId = 0)
			{
				HBITMAP hbitmap = nullptr;
				if(path)
				{
					auto_handle h;
					uint32_t fuLoad = LR_SHARED | LR_LOADFROMFILE;

					if(resId >= 0)
					{
						h = ::LoadLibraryW(path);
						fuLoad = 0;
						path = nullptr;
					}

					return (HBITMAP)::LoadImageW(h.get<HINSTANCE>(), path,
						IMAGE_BITMAP,
						::GetSystemMetrics(SM_CXSMICON),
						::GetSystemMetrics(SM_CYSMICON),
						LR_CREATEDIBSECTION | fuLoad
					);
				}
				return hbitmap;
			}
			
			static HICON FromResource(const wchar_t *path, int id, int size)
			{
				HICON hIcon = nullptr;
				if(path)
				{
					auto cx = ::GetSystemMetrics(SM_CXSMICON);
					if(id < 0)
					{
						bool freelib = false;;
						auto hModule = GetModuleHandleW(path);
						if(!hModule)
						{
							hModule = ::LoadLibraryExW(path, nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
							freelib = true;
						}

						if(hModule)
						{
							if(size > cx) size = 32;
							id *= -1;
							//::MessageBox(0, std::to_wstring(id).c_str(), L"0000000", 0);
							hIcon = (HICON)::LoadImageW(hModule,
														MAKEINTRESOURCEW(id),
														IMAGE_ICON,
														size,
														size,
														LR_DEFAULTCOLOR);
							if(freelib)
								::FreeLibrary(hModule);
						}
					}
					else
					{
						if(size <= cx)
							::ExtractIconExW(path, id, nullptr, &hIcon, 1);
						else
							::ExtractIconExW(path, id, &hIcon, nullptr, 1);
					}
				}
				return hIcon;
			}

			static HBITMAP FromHICON(const wchar_t* path, int resId, int size)
			{
				return WIC::ToHBITMAP(FromResource(path, resId, size), size);
			}

			/*static HBITMAP FromHICON(HICON hIcon, int size)
			{
				return WIC::ToHBITMAP(hIcon, size);
			}*/

			enum IMG_EXT
			{
				IMG_EXT_EXE = 0x7C780675U,
				IMG_EXT_DLL = 0x7C7800AFU,
				IMG_EXT_MUI = 0x7C78281EU,
				IMG_EXT_SCR = 0x7C783F5BU,
				IMG_EXT_CPL = 0x7C77FCF2U,
				IMG_EXT_OCX = 0x7C782E5DU,
				IMG_EXT_SYS = 0x7C784232U,
				IMG_EXT_AX = 0x0B87838CU,
				IMG_EXT_ICO = 0x7C7814CEU,
				IMG_EXT_PNG = 0x7C7833F8U,
				IMG_EXT_BMP = 0x7C77F852U,
				IMG_EXT_GIF = 0x7C780D09U,
				IMG_EXT_JPG = 0x7C781AB4U,
				IMG_EXT_CUR = 0x7C77FD9DU,
				IMG_EXT_LNK = 0x7C7822F8U
			};

			static HBITMAP FromFileExtension(const wchar_t *extension, int size)
			{
				return WIC::ToHBITMAP(Icon::FromFileTypeIcon(extension, size), size);
			}

			/*static HBITMAP FromAssociated(const wchar_t *path, int size)
			{
				return WIC::ToHBITMAP(Icon::FromAssociated(path, size), size);
			}*/

			//image.icon(path[, index])    image.icon('.exe') 
			//image.res('', icon|png, id)
			//image.inheart | image.parent
			//image.cmd

			static HBITMAP FromIcon(const wchar_t *path, int resId, int size)
			{
				if(!path || *path == 0)
					return nullptr;

				string full = Path::Full(path).move();

				if(full[0] == '.')
					return FromFileExtension(full, size);

				return FromHICON(full, resId, size);
			}

			/*HBITMAP FromResourceById(const wchar_t *path, int resId, ImageData *imagdata)
			{
				if(!path || *path == 0)
					return nullptr;
				string full = Path::Full(path).move();
				return this->FromHICON(Icon::FromResourceById(full, MAKEINTRESOURCEW(resId)), imagdata);
			}*/

			/*std::optional<std::wstring> SearchPathExecutable(const std::wstring &name) 
			{
				auto size = SearchPathW(nullptr, name.c_str(), L".exe", 0, nullptr, nullptr);
				if(!size) {
					return std::nullopt;
				}

				std::vector<WCHAR> buffer(static_cast<size_t>(size) + 1);
				WCHAR *filename{};
				if(!SearchPathW(nullptr, name.c_str(), L".exe", size + 1, buffer.data(), &filename)) {
					return std::nullopt;
				}
				return buffer.data();
			}*/

			static HBITMAP From(const wchar_t* path, int size)
			{
				if(!path || *path == 0)
					return nullptr;

				if(path[0] == '.')
					return FromFileExtension(path, size);

				//string full0 = Path::Full(path).move();
				Path _path(path);

				int resId = 0;
				if(!_path._extension.empty())
				{
					auto p = _path._extension.last_index_of(',', false);
					if(p != _path._extension.npos)
					{
						resId = _path._extension.substr(p + 1).to_int<int>();
						_path._extension.trim().remove(p);
					}

					string full = Environment::Expand(Path::Combine(_path._dir, _path._title + _path._extension).trim()).move();

					/*if(!_path.is_absolute())
					{
						string full2;
						auto len = ::SearchPathW(nullptr, full, _path._extension, MAX_PATH, full2.buffer(MAX_PATH), nullptr);
						full = full2.release(len).move();
					}*/

					Hash hash(_path._extension.c_str());
					if(hash.equals({ IMG_EXT_EXE, IMG_EXT_DLL, IMG_EXT_MUI }))
						return FromHICON(full, resId, size);
					else if(hash.equals(IMG_EXT_LNK))
					{
						string licon;
						int iicon = 0;
						if(Path::GetLinkInfo(full, nullptr, nullptr, &licon, &iicon))
							return FromHICON(licon, iicon, size);
					}

					return WIC::Load(full.c_str(), size);
				}
				return nullptr;
			}
		};
	}
}