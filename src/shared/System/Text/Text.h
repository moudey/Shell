#pragma once

namespace Nilesoft
{
	namespace Text
	{
		class Clipboard
		{
		public:
			static bool Set(const string &str)
			{
				bool result = false;
				// Get Text Length
				if(str.length() == 0)
					return result;
				// Open the clipboard, and empty it.
				if(::OpenClipboard(nullptr))
				{
					if(::EmptyClipboard())
					{
						auto len = str.length() + 1;
						// Get Size Of Text In Bytes
						auto sizeInBytes = (SIZE_T)len * sizeof(wchar_t);
						// Allocate a global memory object for the text.
						auto hglbCopy = ::GlobalAlloc(GMEM_MOVEABLE, sizeInBytes);
						if(hglbCopy != nullptr)
						{
							// Lock the handle and copy the text to the buffer. 
							auto lpstrCopy = ::GlobalLock(hglbCopy);
							if(lpstrCopy) {
								::memcpy(lpstrCopy, str.c_str(), sizeInBytes);
								// lpstrCopy[textLength] = (TCHAR)0;  // NULL Terminate String For Clipboard
								::GlobalUnlock(hglbCopy);
							}
							// Place the handle on the clipboard. 
							result = ::SetClipboardData(CF_UNICODETEXT, hglbCopy) != nullptr;
						}
					}
					// Close the clipboard.
					::CloseClipboard();
				}
				return result;
			}

			static inline void Empty()
			{
				// Open the clipboard, and empty it.
				if(::OpenClipboard(nullptr))
				{
					::EmptyClipboard();
					// Close the clipboard.
					::CloseClipboard();
				}
			}

			static bool IsFormatAvailable(uint32_t format) 
			{
				return ::IsClipboardFormatAvailable(format);
			}
		};
	}
}