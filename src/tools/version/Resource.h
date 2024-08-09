#pragma once

#define VERSION_MAJOR			$MAJOR$
#define VERSION_MINOR			$MINOR$
#define VERSION_BUILD			$BUILD$
#define VERSION_REV				$MAINT$
#define VERSION_YEAR			$YEAR$

#define IDI_LOGO				1100;
#define IDI_IMAGES 				1101;

namespace Nilesoft
{

#define STRINGIFY(x)			L#x
#define TOSTRING(x)				STRINGIFY(x)

#ifdef _WIN64
#define APP_PROCESS				L"64-bit"
#else
#define APP_PROCESS				L"32-bit"
#endif

//#define APP_IS_CANARY			1
//#define APP_CANARY				L"Canary"

#define APP_COMPANY				L"Nilesoft"
#define APP_COMPANYLTD			APP_COMPANY L" Ltd"
#define APP_NAMEA				"Shell"
#define APP_NAME				L"Shell"
#define APP_FULLNAME			APP_COMPANY L" " APP_NAME
//#define APP_FULLNAME_VERSION	APP_FULLNAME L" (debug)"
#define APP_FILENAME_TITLE		L"shell"

#define	APP_PRODUCT_VERSION		L"$MAJOR$.$MINOR$"
#define	APP_VERSION				L"$MAJOR$.$MINOR$.$BUILD$"
//#define APP_VERSION_FULL		L"$MAJOR$.$MINOR$ build $BUILD$"

constexpr auto APP_EMAIL		= L"support@nilesoft.org";
constexpr auto APP_WEBSITE		= L"www.nilesoft.org";
constexpr auto APP_WEBSITELINK	= L"https://nilesoft.org";

#ifdef _WINDLL
	#define APP_FILENAME		APP_FILENAME_TITLE L".dll"
	#define TRT_MANIFEST		2
	#define TFILETYPE			VFT_DLL

	#define IDD_INPUTBOX			103
	#define IDC_INPUTBOX_PROMPT		1000
	#define IDC_INPUTBOX_EDIT		1001
	
#else
	#define APP_FILENAME		APP_FILENAME_TITLE L".exe"
	#define TRT_MANIFEST		1
	#define TFILETYPE			VFT_APP
#endif

#ifdef _DEBUG
	#define VS_DEBUG			VS_FF_DEBUG
#else
	#define VS_DEBUG			0x0L
#endif


#define IDR_CSHELL	            	101
#define IDD_DIALOG1             	102
#define IDI_RPMICON             	105
#define IDI_SHELLFONT 				106


#define IDD_CMDLINE             	1000
#define IDC_CMDLINE_TEXT			1001

#define IDS_ADMIN_PRIVILEGES		2000
#define IDS_WINDOWS_COMPATIBILITY	2001
#define IDS_REGISTER_SUCCESS		2002
#define IDS_UNREGISTER_SUCCESS		2003
#define IDS_REGISTER_NOT_SUCCESS	2004
#define IDS_UNREGISTER_NOT_SUCCESS	2005
#define IDS_RESTART_EXPLORERQ		2006
#define IDS_RESTART_EXPLORER		2007
#define IDS_SELECT_FOLDER			2008
}