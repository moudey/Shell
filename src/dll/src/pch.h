// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

// add headers that you want to pre-compile here
//#include "framework.h"
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
//#pragma once

#ifndef PCH_H
#define PCH_H

#include "Include/targetver.h"
#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <aclapi.h>
#include <shlobj.h>
#include <initguid.h>
#include <wincodec.h> // WIC
#include <psapi.h>

#include <uxtheme.h>
#include <vssym32.h>

#include <thread>
#include <mutex>
//#include <future>
#include <functional>
#include <filesystem>
//#include <vector>
//#include <deque>
#include <map>
#include <unordered_map>

#include <d2d1.h>
#include <dwrite.h>
#include <windowsx.h>
//#include <Shlwapi.h>

#define __debug0

#ifdef __debug
#define catch(...)  catch (...)
#endif


/*
template<typename TKey, typename TValue>
using IDictionary = std::map<TKey, TValue>;

template<typename TKey, typename TValue>
using Dictionary = std::unordered_map<TKey, TValue>;
*/
/*
#define delete_ptr(ptr) {delete ptr; ptr = nullptr;}
#define destory(ptr) {delete ptr; ptr = nullptr;}
#define destory_array(ptr) {delete[] ptr; ptr = nullptr;}
*/
/*
#define DLLEXPORT			extern "C" __declspec(dllexport)
#define _DllExport			__declspec( dllexport )
#define STD_EXPORT(TYPE)	extern "C" DLLEXPORT TYPE __stdcall
#define CDE_EXPORT(TYPE)	extern "C" DLLEXPORT TYPE __cdecl
#define DLL_EXPORT(TYPE)	extern "C" DLLEXPORT TYPE __cdecl

*/
#define maximum_path 32767
#include <System.h>

using namespace ::Nilesoft;
using namespace ::Nilesoft::Collections;
using namespace ::Nilesoft::Text;
using namespace ::Nilesoft::IO;
using namespace ::Nilesoft::Drawing;
using namespace ::Nilesoft::Windows::Forms;

#include <Resource.h>
// TODO: reference additional headers your program requires here
#include <Globals.h>


using namespace Nilesoft;

#include "Parser\IdentHash.h"
#include <FileSystemObjects.h>
#include "Include\Selections.h"
#include "Parser\Token.h"
#include "Parser\Ident.h"

#include "Include\Keyboard.h"
#include "Expression\Scope.h"
#include "Expression\Context.h"
#include "Expression\ExpressionType.h"
#include "Expression\Expression.h"
#include "Expression\LiteralExpression.h"
#include "Expression\OperatorExpression.h"
#include "Expression\ArrayExpression.h"
#include "Expression\StatementExpression.h"
#include "Expression\IdentExpression.h"


#include "Include\Styles.h"
#include "Include\Menu.h"
#include "Include\MenuItem.h"
#include "Include\Cache.h"
#include "Parser\Lexer.h"
#include "Parser\Parser.h"
#include "Include\Hooker.h"
#include "Include\Initializer.h"
// Global variable definition
// explicit declaration
#include "Include\FindPattern.h"


//default 4
#define TABSIZE		1

//using namespace Nilesoft::Registry;


//#define ISOLATION_AWARE_ENABLED 1
/*
#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
*/

#endif //PCH_H
