#pragma once

namespace Nilesoft
{
	namespace Drawing
	{
		// IWICImagingFactory
		class WIC
		{
		private:
			// Get the WIC factory from the singleton wrapper class
			inline static IWICImagingFactory* _factory = nullptr;
			inline static const uint32_t _width = ::GetSystemMetrics(SM_CXSMICON);
			inline static const uint32_t _height = ::GetSystemMetrics(SM_CYSMICON);

		public:

			WIC() { init(); }
			~WIC() { release(); }

			static void release()
			{
				if(_factory) 
					_factory->Release();
				_factory = nullptr;
			}

			static bool init()
			{
				auto hr = S_OK;
				if(!_factory)
				{
					if(::IsWindows8OrGreater())
					{
						hr = ::CoCreateInstance
						(
							CLSID_WICImagingFactory2,
							nullptr,
							CLSCTX_INPROC_SERVER,
							__uuidof(IWICImagingFactory2),
							(void**)&_factory
						);
					}
					else
					{
						hr = ::CoCreateInstance
						(
							CLSID_WICImagingFactory1,
							nullptr,
							CLSCTX_INPROC_SERVER,
							__uuidof(IWICImagingFactory),
							(void**)&_factory
						);
					}
				}
				return(hr == S_OK && _factory);
			}

			static IWICBitmapSource* To32bppPBGRA(IWICBitmapSource* bitmap, uint32_t width = 0, uint32_t height = 0)
			{
				if(!(init() && bitmap)) return nullptr;

				uint32_t w = 0, h = 0;

				// Retrieve the image dimensions
				if(S_OK == bitmap->GetSize(&w, &h))
				{
					if(width > 0 && height > 0)
					{
						if(w != width || h != height)
						{
							w = width;
							h = height;

							IWICBitmapScaler *scaler{};
							// Create a WIC image scaler to scale the image to the requested size
							if(S_OK ==_factory->CreateBitmapScaler(&scaler))
							{
								auto hr = scaler->Initialize
								(
									bitmap,
									w, h,
									WICBitmapInterpolationModeHighQualityCubic
								);

								if(S_OK == hr)
								{

									bitmap->Release();
									bitmap = scaler;
								}
								else
								{
									scaler->Release();
								}
							}
						}
					}

					auto pixelFormat = GUID_WICPixelFormat32bppPBGRA;
					if(S_OK == bitmap->GetPixelFormat(&pixelFormat))
					{
						if(pixelFormat != GUID_WICPixelFormat32bppBGRA ||
						   pixelFormat != GUID_WICPixelFormat32bppPBGRA)
						{
							IWICFormatConverter* formatConverter = nullptr;
							// Convert the format of the image frame to 32bppPBGR
							if(S_OK == _factory->CreateFormatConverter(&formatConverter))
							{
								BOOL canConvert = FALSE;
								auto hr = formatConverter->CanConvert(pixelFormat,
																	  GUID_WICPixelFormat32bppPBGRA,
																	  &canConvert);
								if(S_OK == hr && canConvert)
								{
									hr = formatConverter->Initialize
									(
										bitmap,
										GUID_WICPixelFormat32bppPBGRA,
										WICBitmapDitherTypeNone,
										nullptr,
										0.f,
										WICBitmapPaletteTypeCustom//WICBitmapPaletteTypeMedianCut
									);

									if(S_OK == hr)
									{
										bitmap->Release();
										bitmap = formatConverter;
									}
									else
									{
										formatConverter->Release();
									}
								}
								else
								{
									formatConverter->Release();
								}
							}
						}
					}
				}
				return bitmap;
			}

			static IWICBitmapSource* ToWICBitmap(IWICBitmapDecoder* decoder, uint32_t width = 0, uint32_t height = 0)
			{
				IWICBitmapSource* bitmap = nullptr;
				if(decoder)
				{
					uint32_t frameCount = 0;
					// Get the number of frames in this image
					if(S_OK == decoder->GetFrameCount(&frameCount) && frameCount > 0)
					{
						if(S_OK == decoder->GetFrame(0, reinterpret_cast<IWICBitmapFrameDecode**>(&bitmap)))
						{
							bitmap = To32bppPBGRA(bitmap, width, height);
						}
					}
					decoder->Release();
				}
				return bitmap;
			}

			static HBITMAP ToHBITMAP(IWICBitmapSource* bitmap, uint8_t** lpbits = nullptr)
			{
				uint32_t width{}, height{};
				if(bitmap && S_OK == bitmap->GetSize(&width, &height))
				{
					// Calculate the number of bytes in 1 scanline
					const uint32_t stride = width * 4;
					// Calculate the total size of the image
					const uint32_t pixels_size = stride * height;
					BITMAPINFOHEADER bi = { sizeof(bi), long(width), -long(height), 1, 32 };
					uint8_t* bits = nullptr;
					auto result = ::CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, reinterpret_cast<void**>(&bits), nullptr, 0);
					if(result)
					{
						if(S_OK == bitmap->CopyPixels(nullptr, stride, pixels_size, bits))
						{
							if(lpbits) *lpbits = bits;
							return result;
						}
						::DeleteObject(result);
					}
				}
				return nullptr;
			}

			static IWICBitmapSource *ToWICBitmap(HBITMAP hbitmap, const SIZE &size = { -1, -1 })
			{
				if(init())
				{
					IWICBitmapSource *bitmap = nullptr;
					auto hr = _factory->CreateBitmapFromHBITMAP(hbitmap, nullptr, WICBitmapUsePremultipliedAlpha, reinterpret_cast<IWICBitmap **>(&bitmap));
					if(hr == S_OK)
						return To32bppPBGRA(bitmap, size.cx, size.cy);
				}
				return nullptr;
			}

			static HBITMAP ToBitmap32(HBITMAP hbitmap, const SIZE &size = { -1, -1 }, uint8_t **lpbits = nullptr)
			{
				return ToHBITMAP(ToWICBitmap(hbitmap, size), lpbits);
			}

			static HBITMAP LoadPNG(const wchar_t *path)
			{
				HBITMAP result = nullptr;
				if(init() && path)
				{
					IWICBitmapDecoder *decoder = nullptr;
					auto hr = _factory->CreateDecoderFromFilename(path, 
																  ::IsWindows8OrGreater() ? &CLSID_WICPngDecoder2 : &CLSID_WICPngDecoder1, 
																  GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);

					if(S_OK == hr && decoder)
					{
						auto bitmap = ToWICBitmap(decoder);
						if(bitmap)
						{
							result = ToHBITMAP(bitmap);
							bitmap->Release();
						}
						decoder->Release();
					}
				}
				return result;
			}
		
			static ID2D1Bitmap* ToD2D1Bitmap(IWICBitmapSource* wicbitmap, ID2D1RenderTarget* render)
			{
				if(wicbitmap && render)
				{
					ID2D1Bitmap* bitmap = nullptr;
					auto hr = render->CreateBitmapFromWicBitmap(wicbitmap, &bitmap);
					wicbitmap->Release();
					if(hr == S_OK)
						return bitmap;
				}
				return nullptr;
			}
			

			static ID2D1Bitmap *ToD2D1Bitmap(HBITMAP hbitmap, ID2D1RenderTarget *render)
			{
				if(hbitmap && render && init())
				{
					BITMAP bitmap{};
					GetObject(hbitmap, sizeof BITMAP, &bitmap);
					auto options = WICBitmapUsePremultipliedAlpha;
					if(bitmap.bmBitsPixel < 32)
					{
						options = WICBitmapIgnoreAlpha;
					}

					IWICBitmapSource *wicbitmap = nullptr;
					auto hr = _factory->CreateBitmapFromHBITMAP(hbitmap, nullptr, options, reinterpret_cast<IWICBitmap **>(&wicbitmap));
					if(hr == S_OK)
					{
						IWICBitmapSource *bitmapsrc = wicbitmap;
						if(bitmap.bmBitsPixel < 32)
						{
							bitmapsrc = To32bppPBGRA(wicbitmap, bitmap.bmWidth, bitmap.bmHeight);
							if(wicbitmap != bitmapsrc)
								wicbitmap->Release();
						}

						ID2D1Bitmap *d2DBitmap = nullptr;
						hr = render->CreateBitmapFromWicBitmap(bitmapsrc, &d2DBitmap);
						bitmapsrc->Release();

						if(hr == S_OK)
							return d2DBitmap;
					}
				}
				return nullptr;
			}

			static ID2D1Bitmap* ToD2D1Bitmap(HBITMAP hbitmap, ID2D1RenderTarget* render, const SIZE& size)
			{
				if(hbitmap && render && init())
				{
					BITMAP bitmap{};
					GetObject(hbitmap, sizeof BITMAP, &bitmap);
					auto options = WICBitmapUsePremultipliedAlpha;
					if(bitmap.bmBitsPixel < 32)
					{
						options = WICBitmapIgnoreAlpha;
					}
					
					IWICBitmapSource* wicbitmap = nullptr;
					auto hr = _factory->CreateBitmapFromHBITMAP(hbitmap, nullptr, options, reinterpret_cast<IWICBitmap**>(&wicbitmap));
					if(hr == S_OK)
					{
						IWICBitmapSource* bitmapsrc = wicbitmap;
						if(bitmap.bmBitsPixel < 32)
						{
							bitmapsrc = To32bppPBGRA(wicbitmap, size.cx, size.cy);
							if(wicbitmap != bitmapsrc)
								wicbitmap->Release();
						}
						
						ID2D1Bitmap* d2DBitmap = nullptr;
						hr = render->CreateBitmapFromWicBitmap(bitmapsrc, &d2DBitmap);
						bitmapsrc->Release();

						if(hr == S_OK)
							return d2DBitmap;
					}
				}
				return nullptr;
			}

			
			static IWICBitmapSource *ToWICBitmap0(HBITMAP hbitmap, const SIZE &size = { -1, -1 })
			{
				if(init() && hbitmap)
				{
					IWICBitmapSource *bitmap = nullptr;
					auto hr = _factory->CreateBitmapFromHBITMAP(hbitmap, nullptr, WICBitmapUsePremultipliedAlpha, reinterpret_cast<IWICBitmap **>(&bitmap));
					if(hr == S_OK)
						return To32bppPBGRA(bitmap, size.cx, size.cy);
				}
				return nullptr;
			}

			/*
			static IWICBitmapSource* ToWICBitmap(HICON hIcon, const SIZE& size = { -1, -1 })
			{
				IWICBitmapSource* bitmap = nullptr;
				if(hIcon)
				{
					if(init())
					{
						if(S_OK == _factory->CreateBitmapFromHICON(hIcon, reinterpret_cast<IWICBitmap**>(&bitmap)))
						{
							bitmap = To32bppPBGRA(bitmap, size);
						}
					}
					::DestroyIcon(hIcon);
				}
				return bitmap;
			}

			// Loads a PNG image from the specified stream (using Windows Imaging Component).
			static IWICBitmapSource* ToWICBitmap(int id, const wchar_t* type = L"PNG")
			{
				if(!init())
					return nullptr;

				// find the resource
				auto hrsrc = ::FindResourceW(nullptr, MAKEINTRESOURCEW(id), type);
				if(hrsrc)
				{
					// load the resource
					auto dwResourceSize = ::SizeofResource(nullptr, hrsrc);
					auto hglbImage = ::LoadResource(nullptr, hrsrc);
					if(hglbImage)
					{
						// lock the resource, getting a pointer to its data
						auto pvSourceResourceData = ::LockResource(hglbImage);
						if(pvSourceResourceData)
						{
							// allocate memory to hold the resource data
							auto hgblResourceData = ::GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
							if(hgblResourceData)
							{
								// get a pointer to the allocated memory
								auto pvResourceData = ::GlobalLock(hgblResourceData);
								if(pvResourceData)
								{
									// copy the data from the resource to the new memory block
									::CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
									::GlobalUnlock(hgblResourceData);
									// Creates a stream object initialized with the data from an executable resource.
									IStream* ipImageStream = nullptr;
									// create a stream on the HGLOBAL containing the data
									if(S_OK == ::CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipImageStream))
									{
										IWICBitmapDecoder* decoder = nullptr;
										auto hr = _factory->CreateDecoderFromStream(ipImageStream, &CLSID_WICPngDecoder, WICDecodeMetadataCacheOnDemand, &decoder);
										if(S_OK == hr && decoder)
										{
											return ToWICBitmap(decoder);
										}
									}
								}
								// couldn't create stream; free the memory
								::GlobalFree(hgblResourceData);
							}
						}
					}
				}
				return nullptr;
			}

			static IWICBitmapSource* ToWICBitmap(const wchar_t* path, const SIZE& size = { -1, -1 })
			{
				if(init() && path)
				{
					IWICBitmapDecoder* decoder = nullptr;
					auto hr = _factory->CreateDecoderFromFilename(path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
					if(S_OK == hr && decoder)
					{
						return ToWICBitmap(decoder, size);
					}
				}
				return nullptr;
			}

			static HBITMAP ToHBITMAP(const wchar_t* path, const SIZE& size = { -1, -1 })
			{
				HBITMAP result = nullptr;
				if(init() && path)
				{
					IWICBitmapDecoder* decoder = nullptr;
					auto hr = _factory->CreateDecoderFromFilename(path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
					if(S_OK == hr && decoder)
					{
						auto bitmap = ToWICBitmap(decoder, size);
						if(bitmap)
						{
							result = ToHBITMAP(bitmap);
							bitmap->Release();
						}
						decoder->Release();
					}
				}
				return result;
			}
*/
		
			static HBITMAP ToHBITMAP(HICON hIcon, int size)
			{
				HBITMAP result = nullptr;
				if(hIcon)
				{
					if(init())
					{
						IWICBitmapSource* bitmap = nullptr;
						if(S_OK == _factory->CreateBitmapFromHICON(hIcon, reinterpret_cast<IWICBitmap**>(&bitmap)))
						{
							bitmap = To32bppPBGRA(bitmap, size, size);
							result = ToHBITMAP(bitmap);
							bitmap->Release();
						}
					}
					::DestroyIcon(hIcon);
				}
				return result;
			}

			static HBITMAP Load(const wchar_t* path, int size = 0)
			{
				HBITMAP hbitmap = nullptr;
				// Get the WIC factory from the singleton wrapper class
				if(!(init() && path))
					return hbitmap;

				IWICBitmapDecoder* decoder;
				auto hr = _factory->CreateDecoderFromFilename
				(
					path,
					nullptr,
					GENERIC_READ,
					WICDecodeMetadataCacheOnDemand,
					&decoder
				);

				if(hr == S_OK)
				{
					uint32_t width = size, height = size;
					if(size > 0)
					{
						/*if(size <= (int)_width)
						{
							width = _width;
							height = _height;
						}*/
					}
					//Logger::Info(L"%d %d", width, height);
					auto bitmap = ToWICBitmap(decoder, width, height);
					hbitmap = ToHBITMAP(bitmap);
					bitmap->Release();
				}

				return hbitmap;
			}
/*
static HBITMAP LoadPNG(const wchar_t *path)
{
	HBITMAP result = nullptr;
	IWICImagingFactory *wic_factory = NULL;
   auto hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>(&wic_factory));
   if(hr == S_OK)
   {
	   IWICBitmapDecoder *decoder = nullptr;
	   auto hr = wic_factory->CreateDecoderFromFilename(path, &CLSID_WICPngDecoder, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
	   if(S_OK == hr && decoder)
	   {
		   uint32_t frameCount = 0;
		   // Get the number of frames in this image
		   if(S_OK == decoder->GetFrameCount(&frameCount) && frameCount > 0)
		   {
			   IWICBitmapSource *bitmap = nullptr;
			   if(S_OK == decoder->GetFrame(0, reinterpret_cast<IWICBitmapFrameDecode **>(&bitmap)))
			   {
				   auto pixelFormat = GUID_WICPixelFormat32bppPBGRA;
				   if(S_OK == bitmap->GetPixelFormat(&pixelFormat))
				   {
					   if(pixelFormat != GUID_WICPixelFormat32bppBGRA ||
						  pixelFormat != GUID_WICPixelFormat32bppPBGRA)
					   {
						   IWICFormatConverter *formatConverter = nullptr;
						   // Convert the format of the image frame to 32bppPBGR
						   if(S_OK == wic_factory->CreateFormatConverter(&formatConverter))
						   {
							   BOOL canConvert = FALSE;
							   auto hr = formatConverter->CanConvert(pixelFormat,
																	 GUID_WICPixelFormat32bppPBGRA,
																	 &canConvert);
							   if(S_OK == hr && canConvert)
							   {
								   hr = formatConverter->Initialize
								   (
									   bitmap,
									   GUID_WICPixelFormat32bppPBGRA,
									   WICBitmapDitherTypeNone,
									   nullptr,
									   0.f,
									   WICBitmapPaletteTypeCustom//WICBitmapPaletteTypeMedianCut
								   );

								   if(S_OK == hr)
								   {
									   bitmap->Release();
									   bitmap = formatConverter;
								   }
								   else
								   {
									   formatConverter->Release();
								   }
							   }
							   else
							   {
								   formatConverter->Release();
							   }
						   }
					   }

					   uint32_t width {}, height {};
					   if(bitmap && S_OK == bitmap->GetSize(&width, &height))
					   {
						   // Calculate the number of bytes in 1 scanline
						   const uint32_t stride = width * 4;
						   // Calculate the total size of the image
						   const uint32_t pixels_size = stride * height;
						   BITMAPINFOHEADER bi = { sizeof(bi), long(width), -long(height), 1, 32 };
						   uint32_t *bits = nullptr;
						   result = ::CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS,
													   reinterpret_cast<void **>(&bits), nullptr, 0);
						   if(result)
						   {
							   if(S_OK == bitmap->CopyPixels(nullptr, stride, pixels_size, reinterpret_cast<uint8_t *>(bits)))
							   {
								   return result;
							   }
							   ::DeleteObject(result);
						   }
					   }
				   }
				   bitmap->Release();
			   }
		   }
		   decoder->Release();
	   }
	   wic_factory->Release();
   }
	return nullptr;
}
			HRESULT LoadBitmapFromFile(const wchar_t* filename, ID2D1HwndRenderTarget* target, ID2D1Bitmap** pBitmap)
			{
				HRESULT hr = S_FALSE;
				IWICImagingFactory* wic_factory = NULL;
				IWICBitmapDecoder* decoder = NULL;
				IWICBitmapFrameDecode* frame = NULL;
				IWICFormatConverter* converter = NULL;

				hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&wic_factory));
				if FAILED(hr) goto clenaup;

				hr = wic_factory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
				if FAILED(hr) goto clenaup;

				hr = decoder->GetFrame(0, &frame);
				if FAILED(hr) goto clenaup;

				hr = wic_factory->CreateFormatConverter(&converter);
				if FAILED(hr) goto clenaup;

				hr = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
				if FAILED(hr) goto clenaup;

				hr = target->CreateBitmapFromWicBitmap(converter, 0, pBitmap);
				if FAILED(hr) goto clenaup;

			clenaup:
				safe_release(decoder);
				safe_release(converter);
				safe_release(frame);
				safe_release(wic_factory);
				return hr;
			}

			HBITMAP wic_to_hbitmap(IWICBitmapSource* bitmap, const SIZE& size = { 16, 16 })
			{
				uint32_t width = size.cx, height = size.cx;

				if(!_factory)  init();
				// Retrieve the image dimensions
				if(SUCCEEDED(bitmap->GetSize(&width, &height)))
				{
					if(width > size.cx || height > size.cx)
					{
						width = size.cx;
						height = size.cy;

						IWICBitmapScaler* scaler = nullptr;
						// Create a WIC image scaler to scale the image to the requested size
						if(SUCCEEDED(_factory->CreateBitmapScaler(&scaler)))
						{
							auto hr = scaler->Initialize
							(
								bitmap,
								width, height,
								WICBitmapInterpolationModeFant
							);

							if(SUCCEEDED(hr))
							{

								bitmap->Release();
								bitmap = scaler;
							}
							else
							{
								scaler->Release();
							}
						}
					}

					auto pixelFormat = GUID_WICPixelFormat32bppPBGRA;
					if(SUCCEEDED(bitmap->GetPixelFormat(&pixelFormat)))
					{
						if(pixelFormat != GUID_WICPixelFormat32bppBGRA &&
						   pixelFormat != GUID_WICPixelFormat32bppPBGRA)
						{
							IWICFormatConverter* formatConverter = nullptr;
							// Convert the format of the image frame to 32bppPBGR
							if(SUCCEEDED(_factory->CreateFormatConverter(&formatConverter)))
							{
								BOOL canConvert = FALSE;
								auto hr = formatConverter->CanConvert(pixelFormat,
																	  GUID_WICPixelFormat32bppPBGRA,
																	  &canConvert);
								if(SUCCEEDED(hr) && canConvert)
								{
									hr = formatConverter->Initialize
									(
										bitmap,
										GUID_WICPixelFormat32bppPBGRA,
										WICBitmapDitherTypeNone,
										nullptr,
										0.f,
										WICBitmapPaletteTypeCustom//WICBitmapPaletteTypeMedianCut
									);

									if(SUCCEEDED(hr))
									{
										bitmap->Release();
										bitmap = formatConverter;
									}
									else
									{
										formatConverter->Release();
									}
								}
								else
								{
									formatConverter->Release();
								}
							}
						}
					}
				}

				// Calculate the number of bytes in 1 scanline
				const uint32_t stride = width * 4;
				// Calculate the total size of the image
				const uint32_t pixels_size = stride * height;

				BITMAPINFOHEADER bi = { sizeof(bi), long(width), -long(height), 1, 32 };
				uint32_t* bits = nullptr;
				auto hbitmap = ::CreateDIBSection(nullptr, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
				if(hbitmap)
				{
					if(FAILED(bitmap->CopyPixels(nullptr, stride, pixels_size, (BYTE*)bits)))
					{
						::DeleteObject(hbitmap);
						hbitmap = nullptr;
					}
				}

				bitmap->Release();

				return hbitmap;
			}

			HBITMAP Load(const wchar_t* path, COLORREF color = -1)
			{
				if(_factory)
				{
					IWICBitmapDecoder* decoder = nullptr;
					auto hr = _factory->CreateDecoderFromFilename(path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
					if(SUCCEEDED(hr) && decoder)
					{
						return Decode(decoder, color);
					}
				}
				return nullptr;
			}

			HBITMAP Decode(IWICBitmapDecoder* decoder, COLORREF color = -1)
			{
				HBITMAP hbitmap = nullptr;
				uint32_t frameCount = 0;
				// Get the number of frames in this image
				if(SUCCEEDED(decoder->GetFrameCount(&frameCount)) && (frameCount > 0))
				{
					IWICBitmapSource* bitmap = nullptr;
					if(SUCCEEDED(decoder->GetFrame(0, (IWICBitmapFrameDecode**)&bitmap)))
					{
						uint32_t width = 0, height = 0;
						// Retrieve the image dimensions
						if(SUCCEEDED(bitmap->GetSize(&width, &height)))
						{
							auto pixelFormat = GUID_WICPixelFormat32bppPBGRA;
							if(SUCCEEDED(bitmap->GetPixelFormat(&pixelFormat)))
							{
								auto dstPixelFormat = pixelFormat;
								if(pixelFormat == GUID_WICPixelFormat32bppBGRA)
								{
									dstPixelFormat = GUID_WICPixelFormat32bppPBGRA;
								}

								if(dstPixelFormat != pixelFormat)
								{
									IWICFormatConverter* formatConverter = 0;
									// Convert the format of the image frame to 32bppPBGR
									if(SUCCEEDED(_factory->CreateFormatConverter(&formatConverter)))
									{
										BOOL canConvert = FALSE;
										auto hr = formatConverter->CanConvert(pixelFormat, dstPixelFormat, &canConvert);
										if(SUCCEEDED(hr) && canConvert)
										{
											hr = formatConverter->Initialize
											(
												bitmap,
												dstPixelFormat,
												WICBitmapDitherTypeNone,
												nullptr,
												0.f,
												WICBitmapPaletteTypeMedianCut
											);

											if(SUCCEEDED(hr))
											{
												bitmap->Release();
												bitmap = formatConverter;
											}
											else
											{
												formatConverter->Release();
											}
										}
									}
								}
							}

							// Calculate the number of bytes in 1 scanline
							const uint32_t stride = width * 4;
							// Calculate the total size of the image
							const uint32_t pixels_size = stride * height;

							BITMAPINFOHEADER bi = { sizeof(bi), long(width), -long(height), 1, 32 };
							uint32_t* bits = nullptr;
							hbitmap = ::CreateDIBSection(nullptr, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
							if(hbitmap)
							{
								if(SUCCEEDED(bitmap->CopyPixels(nullptr, stride, pixels_size, (BYTE*)bits)))
								{
									if(color != -1)
									{
										auto r = GetRValue(color);
										auto g = GetGValue(color);
										auto b = GetBValue(color);

										auto pxiels = (uint8_t*)bits;
										for(uint32_t x = 0; x < (width * height); x++)
										{
											auto a = pxiels[3];
											pxiels[0] = (b * a) / 255;
											pxiels[1] = (g * a) / 255;
											pxiels[2] = (r * a) / 255;
											pxiels += 4;
										}
									}
								}
								else
								{
									::DeleteObject(hbitmap);
									hbitmap = nullptr;
								}
							}
						}
						bitmap->Release();
					}
				}

				decoder->Release();

				return hbitmap;
			}

			HBITMAP FromFile(const wchar_t* path)
			{
				HBITMAP hbitmap = nullptr;
				// Get the WIC factory from the singleton wrapper class
				if(!_factory)
					return hbitmap;

				IComPtr<IWICBitmapDecoder> decoder;
				auto hr = _factory->CreateDecoderFromFilename
				(
					path,
					nullptr,
					GENERIC_READ,
					WICDecodeMetadataCacheOnDemand,
					decoder
				);

				if(SUCCEEDED(hr))
				{
					uint32_t frameCount = 0;
					// Get the number of frames in this image
					if(SUCCEEDED(decoder->GetFrameCount(&frameCount)) && (frameCount > 0))
					{
						IWICBitmapSource* bitmap = nullptr;
						if(SUCCEEDED(decoder->GetFrame(0, (IWICBitmapFrameDecode**)&bitmap)))
						{
							hbitmap = wic_to_hbitmap(bitmap);
						}
					}
				}

				return hbitmap;
			}*/
		};
	}
}