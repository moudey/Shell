#pragma once
//#include "Icon.h"
//#include "DC.h"
//#include "Bitmap.h"

namespace Nilesoft
{
	namespace Drawing
	{
	/*
		struct Brush0
		{
			uint32_t _color = CLR_INVALID;

			HBRUSH _hbrush = nullptr;

			Brush0() {}

			Brush0(uint32_t color) : _color{ color }
			{
				create();
			}

			~Brush0()
			{
				if(_hbrush) ::DeleteObject(_hbrush);
			}

			HBRUSH create()
			{
				_hbrush = ::CreateSolidBrush(_color);
				return _hbrush;
			}

			operator HBRUSH() { return _hbrush; }
		};
		*/
		class Brush
		{
			bool _delete = true;

		public:
			HBRUSH Handle = nullptr;

			Brush()
			{
			}

			Brush(HBRUSH hbrush, bool delete_)
			{
				Release();
				Handle = hbrush;
				_delete = delete_;
			}

			Brush(COLORREF clr)
			{
				Release();
				Handle = ::CreateSolidBrush(clr);
			}

			~Brush()
			{
				Release();
			}

			void Release()
			{
				if(Handle && _delete) 
					::DeleteObject(Handle);
				
				Handle = nullptr;
				_delete = true;
			}

			operator HBRUSH() const { return Handle; }
			//operator COLORREF() const { return ToColor(); }

			Brush& From(COLORREF clr)
			{
				Release();
				Handle = ::CreateSolidBrush(clr);
				return *this;
			}

			Brush& From(HBRUSH hbrush, bool delete_)
			{
				Release();
				Handle = hbrush;
				_delete = delete_;
				return *this;
			}

			COLORREF ToColor() const
			{
				return ToColor(Handle);
			}

			static COLORREF ToColor(HBRUSH hbrush)
			{
				if(hbrush)
				{
					auto sizeof_LOGBRUSH = (int)sizeof(LOGBRUSH);
					LOGBRUSH lb { };
					if(sizeof_LOGBRUSH == ::GetObjectW(hbrush, sizeof_LOGBRUSH, &lb))
						return lb.lbColor;
				}
				return 0x000000;
			}
		};
	}
}