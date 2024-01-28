#pragma once
//#include "Icon.h"
//#include "DC.h"
//#include "Bitmap.h"

namespace Nilesoft
{
	namespace Drawing
	{

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
			operator COLORREF() const { return ToColor(); }

			/*Brush& operator =(HBRUSH hbr) const 
			{ 
				Release();
				Handle = hbr;
				_delete = true;
				return *this; 
			}*/

			Brush& From(HBRUSH hbrush, bool delete_)
			{
				Release();
				Handle = hbrush;
				_delete = delete_;
				return *this;
			}

			Brush& From(COLORREF clr)
			{
				Release();
				Handle = ::CreateSolidBrush(clr);
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
					LOGBRUSH lb { };
					if(::GetObjectW(hbrush, sizeof(LOGBRUSH), &lb))
						return lb.lbColor;
				}
				return 0x000000;
			}
		};
	}
}