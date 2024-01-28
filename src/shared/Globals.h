#pragma once
//CWM_GETISHELLBROWSER
// Undefined windows message sent by CreateViewObject
#define WM_GETISHELLBROWSER WM_USER + 7

#define except __except(EXCEPTION_EXECUTE_HANDLER)
#define __EXCEPT __except(EXCEPTION_EXECUTE_HANDLER) { }

#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(_r) (((ULONG_PTR)(_r) >> 16) == 0)
#endif

namespace Nilesoft
{
	static const GUID IID_Shell						= { 0xbae3934b, 0x8a6a, 0x4bfb, { 0x81, 0xbd, 0x3f, 0xc5, 0x99, 0xa1, 0xba, 0xf0 } };
	static const GUID IID_ContextMenu				= { 0xbae3934b, 0x8a6a, 0x4bfb, { 0x81, 0xbd, 0x3f, 0xc5, 0x99, 0xa1, 0xba, 0xf1 } };
	static const GUID IID_IconOverlay				= { 0xbae3934b, 0x8a6a, 0x4bfb, { 0x81, 0xbd, 0x3f, 0xc5, 0x99, 0xa1, 0xba, 0xf2 } };
	static const GUID IID_FolderExtensions			= { 0xbae3934b, 0x8a6a, 0x4bfb, { 0x81, 0xbd, 0x3f, 0xc5, 0x99, 0xa1, 0xba, 0xf3 } };

	// Windows.UI.FileExplorer.ContextMenu {86ca1aa0-34aa-4e8b-a509-50c905bae2a2}
	static const GUID IID_FileExplorerContextMenu	= { 0x86ca1aa0, 0x34aa, 0x4e8b, { 0xa5, 0x09, 0x50, 0xc9, 0x05, 0xba, 0xe2, 0xa2 } };
	static const GUID IID_FileExplorerCommandBar	= { 0xd93ed569, 0x3b3e, 0x4bff, { 0x83, 0x55, 0x3c, 0x44, 0xf6, 0xa5, 0x2b, 0xb5 } };
	//static const GUID IID_ExplorerHostCreator		= { 0xab0b37ec, 0x56f6, 0x4a0e, { 0xa8, 0xfd, 0x7a, 0x8b, 0xf7, 0xc2, 0xda, 0x96 } };

#define L(x)      L ## x

// GUIDs for COM-objects
	constexpr auto CLS_Shell  = L"{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF0}";
#define CLS_ContextMenu			L"{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF1}"
#define CLS_IconOverlay			L"{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF2}"
#define CLS_FolderExtensions	L"{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}"
#define CLS_FileExplorerContextMenu	"{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}"

	constexpr auto app_namea = "Shell";
	constexpr auto app_name = L"Shell";
	
	constexpr auto app_full_name = L"Nilesoft Shell";
	constexpr auto str_trim = L" \t\r\n\v\f\"'";

	// Macro to get a random integer with a specified range 
#define GetRandom(min, max) ((rand()%(int)(((max) + 1)-(min)))+(min))
/*
	class T
	{
	public:
		void *operator new(size_t n)
		{
			void *p = ::operator new(n);
			heap_track().insert(p);
			return p;
		}

		void operator delete(void *p)
		{
			heap_track().erase(p);
			::operator delete(p);
		}

	private:

		// a function to avoid static initialization order fiasco
		static std::set<void *> &heap_track()
		{
			static std::set<void *> s_;
			return s_;
		}

	public:
		static bool is_heap(void *p)
		{
			return heap_track().find(p) != heap_track().end();
		}
	};

	//Then you can do stuff like this:
	T* x = new X;
	if(T::is_heap(x))
	{
		delete x;
	}
*/

//#pragma warning(suppress:C26495)
//#pragma warning(suppress:C26451)

/*
#define OK(hr)		(((HRESULT)(hr)) == S_OK)
#define IsOK(hr)		(((HRESULT)(hr)) == S_OK)
#define IsFalse(hr)		(((HRESULT)(hr)) != S_OK)
#define SOK(hr)			(((HRESULT)(hr)) == S_OK)
#define SFALSE(hr)		(((HRESULT)(hr)) != S_OK)

#define IS_FLAG_SET(val, flag) (((val) & (flag)) == (flag))
#define IS_FLAG_CLEAR(val, flag) (((val) & (flag)) == 0)

#define EQUALS(X,T)		((X) == (T))
#define EQUALS2(X,T1,T2)	(((X) == (T1)) || ((X) == (T2)))
*/
#define MAX_FMT_BUF		1024

	inline int MBFormat(uint32_t type, const wchar_t *format, ...)
	{
		int ret = 0;
		wchar_t msg[MAX_FMT_BUF] = { 0 };
		va_list args;
		va_start(args, format);
		::vswprintf_s(msg, MAX_FMT_BUF, format, args);
		va_end(args);
		::MessageBoxW(nullptr, msg, app_full_name, type);
		return ret;
	}

	inline int MBFormat(const wchar_t *format, ...)
	{
		int ret = 0;
		wchar_t msg[MAX_FMT_BUF] = { 0 };
		va_list args;
		va_start(args, format);
		::vswprintf_s(msg, MAX_FMT_BUF, format, args);
		va_end(args);
		ret = ::MessageBoxW(nullptr, msg, app_full_name, MB_OK | MB_ICONASTERISK);
		return ret;
	}

#define msgboxf MBFormat 
#define msgbox(T, M) ::MessageBoxW(nullptr, M, T, MB_OK | MB_ICONASTERISK)

#define MBF MBFormat 
#define MB(X) ::MessageBoxW(nullptr, X, app_full_name, MB_OK | MB_ICONASTERISK)

#define BEEP(X) ::MessageBeep(X)
#define beep ::MessageBeep(0)

	inline void Debug(LPCWSTR format, ...)
	{
		if(!format) return;
#ifdef _DEBUG
		wchar_t msg[MAX_FMT_BUF] = {};
		va_list args;
		va_start(args, format);
		::vswprintf_s(msg, MAX_FMT_BUF, format, args);
		va_end(args);
		::MessageBoxW(nullptr, msg, app_full_name, MB_ICONERROR);
#else
		(format);
#endif
	}

	inline void Debug(LPCSTR format, ...)
	{
		if(!format) return;
#ifdef _DEBUG
		char msg[MAX_FMT_BUF] = { 0 };
		va_list args;
		va_start(args, format);
		::vsprintf_s(msg, MAX_FMT_BUF, format, args);
		va_end(args);
		::MessageBoxA(nullptr, msg, app_namea, MB_ICONERROR);
#else
		(format);
#endif
	}

	inline void DebugA(LPCSTR text, LPCSTR title = app_namea)
	{
#ifdef _DEBUG
		::MessageBoxA(nullptr, text, title, MB_ICONERROR);
#else
		(text);
		(title);
#endif
	}

	// Calculate the powers
	inline double Pow(double n, double p)
	{
		double result = 1.0;
		for(int j = 0; j < p; j++)
		{
			result *= n;
		}
		return result;
	}

	namespace Shell
	{
		template <class T> class FlagDescriptor
		{
		public:
			struct FLAGS
			{
				T value;
				const char *name;
			};

			static std::string ToBitString(T value, const FLAGS *flags)
			{
				std::string desc;

				size_t index = 0;
				while(flags[index].name)
				{
					const T &flag = flags[index].value;
					const char *name = flags[index].name;

					//if flag is set
					if((value & flag) == flag)
					{
						if(!desc.empty())
							desc.append("|");
						desc.append(name);
					}

					//next flag
					index++;
				}
				return desc;
			}

			static std::string ToValueString(T value, const FLAGS *flags)
			{
				std::string desc;

				size_t index = 0;
				while(flags[index].name)
				{
					const T &flag = flags[index].value;
					const char *name = flags[index].name;

					//if flag is set
					if(value == flag)
					{
						if(!desc.empty())
							desc.append(",");
						desc.append(name);
					}

					//next flag
					index++;
				}
				return desc;
			}
		};
	}
}
