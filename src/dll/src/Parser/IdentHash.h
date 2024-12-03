#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		constexpr auto CONFIG_SHELL = 0x105AC57DU;
		constexpr auto CONFIG_VER = 0x0B88B652U;
		constexpr auto CONFIG_VERSION = 0x73006C4BU;
		constexpr auto CONFIG_IMAGES = 0x04B8121BU;
		constexpr auto CONFIG_SET = 0x0B88A991U;
		constexpr auto CONFIG_SETTINGS = 0x1304DC16U;
		constexpr auto CONFIG_CONST = 0x0F3D3B4CU;
		constexpr auto CONFIG_VAR = 0x0B88B5CEU;
		constexpr auto CONFIG_VARIABLES = 0x6BD9E13EU;
		constexpr auto CONFIG_IMPORT = 0x04C06F80U;

		constexpr auto CONFIG_ITEM = 0x7C989E34U;
		constexpr auto CONFIG_POPUP = 0x10288299U;
		constexpr auto CONFIG_SEP = 0x0B88A98DU;
		constexpr auto CONFIG_SEPARATOR = 0x14015AB6U;
		constexpr auto CONFIG_STYLES = 0x1C979249U;
		constexpr auto CONFIG_STYLE = 0x1061AF16U;
		constexpr auto CONFIG_STYLE_LAYOUT = 0x0AEBB323U;

		constexpr auto CONFIG_MENU = 0x7C9A911AU;
		constexpr auto CONFIG_NEW = 0x0B88944FU;
		constexpr auto CONFIG_MODIFY = 0x0E32936DU;
		constexpr auto CONFIG_UPDATE = 0x20EDD0A8U;
		constexpr auto CONFIG_CHANGE = 0xF65EC1CBU;

		//
		// Menu types
		constexpr auto MENU_TYPE_CHANGE = 0xF65EC1CBU;
		constexpr auto MENU_TYPE_MENU = 0x7C9A911AU;
		constexpr auto MENU_TYPE_ITEM = 0x7C989E34U;
		constexpr auto MENU_TYPE_SEP = 0x0B88A98DU;
		constexpr auto MENU_TYPE_SEPARATOR = 0x14015AB6U;
		constexpr auto MENU_TYPE_VAR = 0x0B88B5CEU;

		// Static prop
		constexpr auto MENU_PARENT = 0x143C538FU;
		constexpr auto MENU_SUB = 0x0B88AB8FU;
		constexpr auto MENU_TARGET = 0x1D90FD6CU;
		constexpr auto MENU_WHERE = 0x10A32840U;
		constexpr auto MENU_CONDITION = 0x733EFBACU;
		//
		// Menu properties
		constexpr auto MENU_ID = 0x00597832U;

		constexpr auto MENU_SEP = 0x0B88A98DU;
		constexpr auto MENU_SEPARATOR = 0x14015AB6U;

		//constexpr auto MENU_INDEX = 0x0FA9159DU;
		constexpr auto MENU_POS = 0x0B889E17U;
		constexpr auto MENU_POSITION = 0x4CEF7ABAU;

		constexpr auto MENU_SEL = 0x0B88A989U;
		constexpr auto MENU_MODE = 0x7C9ABA4AU;

		constexpr auto MENU_CLSID = 0x0F3BAA14U;

		// menu visibility
		constexpr auto MENU_VIS = 0x0B88B6D7U;
		constexpr auto MENU_VISIBILITY = 0x7A0F4BADU;

		constexpr auto MENU_TYPE = 0x7C9EBD07U;
		constexpr auto MENU_FIND = 0x7C96CB66U;
		constexpr auto MENU_IN = 0x0059783CU;
		constexpr auto MENU_CHECKED = 0xC279666CU;
		constexpr auto MENU_TEXT = 0x7C9E690AU;
		constexpr auto MENU_TITLE = 0x106DAA27U;

		constexpr auto MENU_RUN = 0x0B88A75AU;
		constexpr auto MENU_CMD = 0x0B886679U;
		constexpr auto MENU_COMMAND = 0xD3639944U;
		constexpr auto MENU_ACTION = 0xF1644D03U;
		constexpr auto MENU_ACTIONS = 0x1DEDEDD6U;
		constexpr auto MENU_COMMANDS = 0x3FD6C237U;
		constexpr auto MENU_CMDS = 0x7C95360CU;

		constexpr auto MENU_CMD_CONSOLE = 0xD3793758U;
		constexpr auto MENU_CMD_LINE = 0x7C9A15ADU;
		constexpr auto MENU_CMD_PROMPT = 0x156E7147U;
		constexpr auto MENU_CMD_EXPLORER = 0xD41BAE96U;
		constexpr auto MENU_CMD_SHELL = 0x105AC57DU;
		constexpr auto MENU_CMD_OPEN = 0x7C9BD777U;
		constexpr auto MENU_CMD_POWERSHELL = 0xBFDE26EAU;
		constexpr auto MENU_CMD_PS = 0x00597928U;
		constexpr auto MENU_CMD_PWSH = 0x7C9C8367U;

		constexpr auto MENU_CMDLINE = 0xCE160121U;
		constexpr auto MENU_OPEN = 0x7C9BD777U;

		constexpr auto MENU_ARG = 0x0B885E9FU;
		constexpr auto MENU_ARGUMENT = 0x40DA0E88U;

		constexpr auto MENU_ARGS = 0x7C9432F2U;
		constexpr auto MENU_ARGUMENTS = 0x5C1BDFFBU;

		constexpr auto MENU_VERB = 0x7C9F80F4U;
		constexpr auto MENU_WINDOW = 0x251EFE5DU;
		constexpr auto MENU_WAIT = 0x7C9FFB3AU;
		constexpr auto MENU_IMAGE = 0x0FA87CA8U;
		constexpr auto MENU_ICON = 0x7C98572EU;
		constexpr auto MENU_KEYS = 0x7C9979C1U;
		constexpr auto MENU_DIR = 0x0B886A44U;
		constexpr auto MENU_DIRECTORY = 0x940211FAU;
		constexpr auto MENU_TIP = 0x0B88AE52U;
		constexpr auto MENU_SEPARATED = 0x1401595EU;
		constexpr auto MENU_INVOKE = 0x04D5D151U;

		//constexpr auto MENU_HELP = 0x7C97D2EEU;
		//constexpr auto MENU_INFO = 0x7C9884D1U;
		//constexpr auto MENU_HINT = 0x7C97E438U;

		//Privileges
		constexpr auto MENU_ADMIN = 0x0F12FC8EU;
		//constexpr auto MENU_RUNAS = 0x104FF2CEU;

		constexpr auto MENU_COLUMN = 0xF6E39413U;
		constexpr auto MENU_COL = 0x0B8866C3U;

		constexpr auto MENU_EXPANDED = 0xD353D90EU;

		// constants
		constexpr auto IDENT_NO = 0x005978E2U;
		constexpr auto IDENT_OK = 0x005978FFU;
		constexpr auto IDENT_YES = 0x0B88C316U;

		//
		// Functions
		constexpr auto IDENT_IMPORT = 0x04C06F80U;
		constexpr auto IDENT_NATIVE = 0x0F932D0CU;
		constexpr auto IDENT_BASIC = 0x0F238967U;
		constexpr auto IDENT_MODIFICATION = 0x01884D9BU;
		constexpr auto IDENT_STATIC = 0x1C8A8BADU;
		constexpr auto IDENT_DYNAMIC = 0x37BD22AAU;
		constexpr auto IDENT_ID = 0x00597832U;
		constexpr auto IDENT_IF = 0x00597834U;
		constexpr auto IDENT_ELSE = 0x7C964C6EU;
		constexpr auto IDENT_FOR = 0x0B88738CU;
		constexpr auto IDENT_FOREACH = 0xBAA8659DU;
		constexpr auto IDENT_BREAK = 0x0F2C9F4AU;
		constexpr auto IDENT_CONTINUE = 0x42AEFB8AU;
		constexpr auto IDENT_ITEM = 0x7C989E34U;
		constexpr auto IDENT_WHERE = 0x10A32840U;
		constexpr auto IDENT_DRAW = 0x7C95D753U;
		constexpr auto IDENT_RECT = 0x7C9D4D93U;
		constexpr auto IDENT_CIRCLE = 0xF679FE97U;
		constexpr auto IDENT_SOLID = 0x105EB980U;
		constexpr auto IDENT_LINE = 0x7C9A15ADU;
		constexpr auto IDENT_GLYPH = 0x0F842689U;
		constexpr auto IDENT_SYMBOL = 0x1CEB4EFBU;
		
		constexpr auto IDENT_EVAL = 0x7C9674ADU;
		constexpr auto IDENT_VIS = MENU_VIS;
		constexpr auto IDENT_VISIBILITY = MENU_VISIBILITY;

		constexpr auto IDENT_CMD_INVOKED = 0x9F8FFBD5U;
		constexpr auto IDENT_INVOKE = 0x04D5D151U;
		constexpr auto IDENT_CANCEL = 0xF5E7082BU;

		constexpr auto IDENT_OWNER = 0x101AC290U;
		constexpr auto IDENT_PROCESS = 0xC336EFE4U;
		constexpr auto IDENT_MODULE = 0x0E32C72BU;
		constexpr auto IDENT_INSTANCE = 0x8AE521DAU;
		constexpr auto IDENT_WINDOW = 0x251EFE5DU;
		constexpr auto IDENT_WND = 0x0B88BBAEU;
		constexpr auto IDENT_IS_DESKTOP = 0xE40E965AU;
		constexpr auto IDENT_IS_TASKBAR = 0xAA5D7188U;
		constexpr auto IDENT_IS_EXPLORER = 0x01C5A551U;
		constexpr auto IDENT_IS_STARTED = 0x8873A357U;
		constexpr auto IDENT_IS_NAV = 0x0523A9A5U;
		constexpr auto IDENT_IS_TREE = 0xA99C6F10U;
		constexpr auto IDENT_IS_SIDE = 0xA99BBC45U;
		constexpr auto IDENT_IS_EDIT = 0xA993FA66U;
		constexpr auto IDENT_IS_START = 0xDD19426EU;
		constexpr auto IDENT_IS_TITLEBAR = 0x60467157U;
		constexpr auto IDENT_IS_ICONTEXTMENU = 0xA5329703U;
		constexpr auto IDENT_IS_CONTEXTMENUHANDLER = 0x49FA7698U;
		
		constexpr auto IDENT_IS_PRIMARY_MONITOR = 0x9E5789CBU;
		constexpr auto IDENT_MONITOR = 0xD53A30CDU;
		constexpr auto IDENT_IS_PRIMARY = 0x9D6852E4U;

	//	constexpr auto IDENT_I = 0x0002B60EU;
		constexpr auto IDENT_IMAGE = 0x0FA87CA8U;
		constexpr auto IDENT_IMG = 0x0B888002U;
		constexpr auto IDENT_ICON = 0x7C98572EU;
		constexpr auto IDENT_ICO = 0x0B887EC0U;
		constexpr auto IDENT_SVG = 0x0B88ABB5U;
		constexpr auto IDENT_SVGF = 0x7C9E22BBU;
		constexpr auto IDENT_RES = 0x0B88A54FU;
		constexpr auto IDENT_FONT = 0x7C96E4FCU;
		constexpr auto IDENT_SCALE = 0x1057F68DU;

		constexpr auto IDENT_BOOL = 0x7C94B391U;
		constexpr auto IDENT_GREATER = 0x0DB4FD4FU;
		constexpr auto IDENT_LESS = 0x7C9A055CU;
		constexpr auto IDENT_EQUAL = 0x0F62A07DU;
		constexpr auto IDENT_NOTEQUAL = 0x767723AEU;
		constexpr auto IDENT_NOT = 0x0B889596U;
		constexpr auto IDENT_OR = 0x00597906U;
		constexpr auto IDENT_AND = 0x0B885E18U;
		//
		constexpr auto IDENT_SHL = 0x0B88A9ECU;
		constexpr auto IDENT_SHR = 0x0B88A9F2U;
		//
		//constexpr auto IDENT_ASTERISK = 0xAB08A2EBU;
		//constexpr auto IDENT_QUESTION = 0xE71B785DU;
		//constexpr auto IDENT_ERROR = 0x0F6321EFU;
		constexpr auto IDENT_INFO = 0x7C9884D1U;
		constexpr auto IDENT_PRIMARY = 0xC2CFC789U;
		constexpr auto IDENT_SUCCESS = 0xB04BF9FEU;
		constexpr auto IDENT_WARNING = 0xB6A3487BU;
		constexpr auto IDENT_DANGER = 0xF83C41D6U;
		// 
		constexpr auto IDENT_SELECT = 0x1B80E3C5U;
		constexpr auto IDENT_SEL = 0x0B88A989U;
		constexpr auto IDENT_APP = 0x0B885E66U;
		constexpr auto IDENT_IO = 0x0059783DU;
		constexpr auto IDENT_PATH = 0x7C9C25F2U;
		constexpr auto IDENT_STR = 0x0B88AB7EU;
		constexpr auto IDENT_WIN = 0x0B88BB13U;
		constexpr auto IDENT_SYS = 0x0B88AC24U;
		constexpr auto IDENT_LEVEL = 0x0FDABC3DU;
		constexpr auto IDENT_ISUWP = 0x0FAC1E1DU;
		constexpr auto IDENT_CLSID = 0x0F3BAA14U;

		constexpr auto IDENT_SET = 0x0B88A991U;
		constexpr auto IDENT_GET = 0x0B887685U;
		constexpr auto IDENT_DARK = 0x7C959127U;
		//constexpr auto IDENT_CH = 0x00597770U;
		constexpr auto IDENT_CHAR = 0x7C952063U;

		constexpr auto IDENT_WIDTH = 0x10A3B0A5U;
		constexpr auto IDENT_HEIGHT = 0x01D688DEU;
		constexpr auto IDENT_OPACITY = 0x70951BFEU;
		constexpr auto IDENT_RADIUS = 0x18DF096DU;
		constexpr auto IDENT_OFFSET = 0x123B4B4CU;
		constexpr auto IDENT_LAUNCH = 0x0AE97B00U;
		constexpr auto IDENT_RUN = 0x0B88A75AU;
		constexpr auto IDENT_SHELL = 0x105AC57DU;
		constexpr auto IDENT_EXEC = 0x7C967DAAU;
		constexpr auto IDENT_PRINT = 0x102A0912U;
		constexpr auto IDENT_QUOTE = 0x103DE0F3U;
		constexpr auto IDENT_TOHEX = 0x1070C08DU;
		constexpr auto IDENT_VAR = 0x0B88B5CEU;
		constexpr auto IDENT_VARIABLES = 0x6BD9E13EU;
		constexpr auto IDENT_EXPAND = 0xFC329E65U;
		constexpr auto IDENT_WORKDIR = 0xD749C647U;
		constexpr auto IDENT_CURDIR = 0xF753288EU;
		constexpr auto IDENT_CMD = 0x0B886679U;		// Constant
		constexpr auto IDENT_COMMAND = 0xD3639944U;
		constexpr auto IDENT_SEND = 0x7C9DDB4FU;
		constexpr auto IDENT_POST = 0x7C9C616BU;
		constexpr auto IDENT_HANDLE = 0x0190D871U;
		constexpr auto IDENT_NIL = 0x0B8894C8U;
		constexpr auto IDENT_NULL = 0x7C9B6140U;	// Constant
		constexpr auto IDENT_DEFAULT = 0x0885548AU;
		constexpr auto IDENT_THIS = 0x7C9E73DDU;
		constexpr auto IDENT_CHEVRON = 0xC283F0BAU;
		constexpr auto IDENT_CHECKMARK = 0x467116CEU;
		constexpr auto IDENT_CHECKED = 0xC279666CU;
		constexpr auto IDENT_BULLET = 0xF4FAD72DU;
		constexpr auto IDENT_RADIO = 0x1044D1B4U;
		constexpr auto IDENT_GROUP = 0x0F8746F2U;
		constexpr auto IDENT_SORT = 0x7C9E066DU;
		constexpr auto IDENT_ALIGN = 0x0F174E50U;
		constexpr auto IDENT_OVERLAY = 0x7EE4B5C7U;
		constexpr auto IDENT_POS = 0x0B889E17U;
		constexpr auto IDENT_VERB = 0x7C9F80F4U;
		//constexpr auto IDENT_SELF = 0x7C9DDB0FU;

		// app namespace
		constexpr auto IDENT_EXE = 0x0B887067U;
		constexpr auto IDENT_CFG = 0x0B886595U;
		constexpr auto IDENT_DLL = 0x0B886AA1U;
		constexpr auto IDENT_RELOAD = 0x192BE55CU;
		constexpr auto IDENT_UNLOAD = 0x20CE3BC8U;
		constexpr auto IDENT_ABOUT = 0x0F11EDE0U;

		constexpr auto IDENT_VER = 0x0B88B652U;
		constexpr auto IDENT_VERSION = 0x73006C4BU;

		constexpr auto IDENT_IS_OR_EARLIER = 0x0B67FBA4U;
		constexpr auto IDENT_IS_OR_GREATER = 0xCC130FEAU;

		constexpr auto IDENT_ISOREARLIER = 0x6FBF68A6U;
		constexpr auto IDENT_ISORGREATER = 0x306A7CECU;
		constexpr auto IDENT_IS7ORGREATER = 0xF499D723U;
		constexpr auto IDENT_IS8ORGREATER = 0xF8A46844U;
		constexpr auto IDENT_IS81ORGREATER = 0xABD1C6B5U;
		constexpr auto IDENT_IS10ORGREATER = 0x023E40CDU;
		constexpr auto IDENT_S11ORGREATER = 0x63F55B65U;
		constexpr auto IDENT_IS7OREARLIER = 0x33EEC2DDU;
		constexpr auto IDENT_IS8OREARLIER = 0x37F953FEU;
		constexpr auto IDENT_IS81OREARLIER = 0xEB26B26FU;
		constexpr auto IDENT_IS10OREARLIER = 0x41932C87U;
		constexpr auto IDENT_IS11OREARLIER = 0x459DBDA8U;

		constexpr auto IDENT_IS64 = 0x7C9893ABU;
		constexpr auto IDENT_IS11 = 0x7C989303U;
		constexpr auto IDENT_IS10 = 0x7C989302U;
		constexpr auto IDENT_IS81 = 0x7C9893EAU;
		constexpr auto IDENT_IS8 = 0x0B888099U;
		constexpr auto IDENT_IS7 = 0x0B888098U;


		constexpr auto IDENT_ISBG = 0x7C98998AU;
		constexpr auto IDENT_ISBACK = 0x052517B2U;
		constexpr auto IDENT_BACK = 0x7C947676U;
		constexpr auto IDENT_BACKGROUND = 0x677E1785U;

		constexpr auto IDENT_PADDING = 0x9AC8CC5CU;
		constexpr auto IDENT_PADDING_LEFT = 0x4E2E28B4U;
		constexpr auto IDENT_PADDING_TOP = 0x30EA439CU;
		constexpr auto IDENT_PADDING_RIGHT = 0x146206E7U;
		constexpr auto IDENT_PADDING_BOTTOM = 0x7BC4407EU;
		constexpr auto IDENT_PADDING_Y = 0x702D59A2U;
		constexpr auto IDENT_PADDING_X = 0x702D59A1U;
	
		constexpr auto IDENT_MARGIN = 0x0D3CE183U;
		constexpr auto IDENT_MARGIN_LEFT = 0xC0708E3BU;
		constexpr auto IDENT_MARGIN_TOP = 0x3C229443U;
		constexpr auto IDENT_MARGIN_RIGHT = 0xCEF11D4EU;
		constexpr auto IDENT_MARGIN_BOTTOM = 0x883623C5U;
		constexpr auto IDENT_MARGIN_Y = 0x4FFB5489U;
		constexpr auto IDENT_MARGIN_X = 0x4FFB5488U;

		constexpr auto IDENT_GAP = 0x0B8875FDU;
		constexpr auto IDENT_COUNT = 0x0F3D586EU;
		constexpr auto IDENT_INDEX = 0x0FA9159DU;
		constexpr auto IDENT_I = 0x0002B60EU;

		constexpr auto IDENT_SIZE = 0x7C9DEDE0U;
		constexpr auto IDENT_LEN = 0x0B888BC4U;
		constexpr auto IDENT_LENGTH = 0x0B2DEAC7U;

		constexpr auto IDENT_SHORT = 0x105AF0D5U;
		constexpr auto IDENT_FULL = 0x7C96FE38U;
		constexpr auto IDENT_RAW = 0x0B88A4CFU;
		constexpr auto IDENT_ROOT = 0x7C9D79A9U;
		constexpr auto IDENT_PARENT = 0x143C538FU;
		constexpr auto IDENT_LOCATION = 0x7604D91EU;
		constexpr auto IDENT_DIR = 0x0B886A44U;
		constexpr auto IDENT_DIRECTORY = 0x940211FAU;
		constexpr auto IDENT_FILE = 0x7C96CB25U;

		constexpr auto IDENT_INDEXOF = 0x9E44FF92U;

		constexpr auto IDENT_EXT = 0x0B887076U;
		constexpr auto IDENT_TITLE = 0x106DAA27U;
		constexpr auto IDENT_NAME = 0x7C9B0C46U;

		constexpr auto IDENT_SUBMENU = 0xB03F5CA4U;
		constexpr auto IDENT_POPUP = 0x10288299U;
		constexpr auto IDENT_MENU = 0x7C9A911AU;
		constexpr auto IDENT_PARALLEL = 0x1455E3F2U;

		constexpr auto IDENT_LOC = 0x0B888D03U;
		constexpr auto IDENT_LANG = 0x7C99F3A7U;
		constexpr auto IDENT_COUNTRY = 0xD3F53B19U;

		constexpr auto IDENT_THEME = 0x106CDE18U;
		constexpr auto IDENT_THEME_AUTO = 0x7C94415EU;
		constexpr auto IDENT_THEME_HIGHCONTRAST = 0xA855C1B3U;
		constexpr auto IDENT_THEME_BLACK = 0x0F294442U;
		constexpr auto IDENT_THEME_WHITE = 0x10A33986U;
		constexpr auto IDENT_THEME_SYSTEM = 0x1CEEE48AU;
		constexpr auto IDENT_THEME_CLASSIC = 0xCB8E8F87U;
		constexpr auto IDENT_THEME_DARK = 0x7C959127U;
		constexpr auto IDENT_THEME_LIGHT = 0x0FDCAE5DU;
		constexpr auto IDENT_THEME_MODERN = 0x0E3283EAU;
		constexpr auto IDENT_THEME_MODERN_LIGHT = 0xBAFC0D61U;
		constexpr auto IDENT_THEME_MODERN_DARK = 0x7A031FABU;
		constexpr auto IDENT_THEME_CUSTOM = 0xF753F9C0U;
		constexpr auto IDENT_DPI = 0x0B886B22U;
		constexpr auto IDENT_ISDARK = 0x05263263U;
		constexpr auto IDENT_ISLIGHT = 0xAA817719U;
		constexpr auto IDENT_ISHIGHCONTRAST = 0x5121DEEFU;
		constexpr auto IDENT_GRADIENT = 0xBB3045F3U;
		constexpr auto IDENT_GRADIENT_LINEAR = 0x0B7641E0U;
		constexpr auto IDENT_GRADIENT_RADIAL = 0x18DF06D2U;
		constexpr auto IDENT_GRADIENT_STOP = 0x7C9E1B4BU;


		constexpr auto IDENT_SHOWDELAY = 0xBE22D115U;
		constexpr auto IDENT_SCREENSHOT = 0x9A37F083U;
		constexpr auto IDENT_PRIORITY = 0x1CF615A7U;

		constexpr auto IDENT_BORDER = 0xF4916C63U;
		constexpr auto IDENT_TIP = 0x0B88AE52U;
		constexpr auto IDENT_FRAME = 0x0F74F1D0U;
		constexpr auto IDENT_SHADOW = 0x1BB1226BU;
		constexpr auto IDENT_LAYOUT = 0x0AEBB323U;
		constexpr auto IDENT_RTL = 0x0B88A737U;

		constexpr auto IDENT_TRANSPARENCY = 0x39451EBFU;
/*
		constexpr auto IDENT_TRANSPARENCY_ACRYLIC = 0x1DD2756CU;
		constexpr auto IDENT_TRANSPARENCY_MICA = 0x7C9AA09FU;
		constexpr auto IDENT_TRANSPARENCY_TABBED = 0x1D882207U;
		constexpr auto IDENT_TRANSPARENCY_BLUR = 0x7C94A79AU;
		constexpr auto IDENT_TRANSPARENCY_TRANSPARENT = 0x6E576D97U;
*/
		constexpr auto IDENT_EFFECT = 0xFAE77932U;
		constexpr auto IDENT_EFFECT_TRANSPARENT = 0x6E576D97U;
		constexpr auto IDENT_EFFECT_BLUR = 0x7C94A79AU;
		constexpr auto IDENT_EFFECT_ACRYLIC = 0x1DD2756CU;
		constexpr auto IDENT_EFFECT_MICA = 0x7C9AA09FU;
		constexpr auto IDENT_EFFECT_TABBED = 0x1D882207U;
		constexpr auto IDENT_TINTCOLOR = 0xCADAC563U;

		constexpr auto IDENT_USED = 0x7C9F2E76U;

		constexpr auto IDENT_EXCLUDE = 0x81A152CFU;
		constexpr auto IDENT_EXCLUDED = 0xB5CBAD13U;
		constexpr auto IDENT_BLOCK = 0x0F297FD0U;
		constexpr auto IDENT_TEXT = 0x7C9E690AU;
		constexpr auto IDENT_ARROW = 0x0F1ABFF0U;
		constexpr auto IDENT_PREFIX = 0x1568D6D3U;

		constexpr auto IDENT_DISPLAY = 0x12CD479BU;
		constexpr auto IDENT_VIEW = 0x7C9F9060U;
		constexpr auto IDENT_VIEW_AUTO = 0x7C94415EU;
		constexpr auto IDENT_VIEW_COMPACT = 0xD3653D0CU;
		constexpr auto IDENT_VIEW_SMALL = 0x105D725EU;
		constexpr auto IDENT_VIEW_MEDIUM = 0x0D7DA046U;
		constexpr auto IDENT_VIEW_LARGE = 0x0FD879F0U;
		constexpr auto IDENT_VIEW_WIDE = 0x7CA01C8EU;
		
		constexpr auto IDENT_TYPES = 0x10765E5AU;
		constexpr auto IDENT_DIRECTORIES = 0x9CCE41E2U;
		constexpr auto IDENT_DIRS = 0x7C95B337U;//directories
		constexpr auto IDENT_FILES = 0x0F703038U;
		constexpr auto IDENT_PATHS = 0x1020E4A5U;
		constexpr auto IDENT_SHORTS = 0x1BB90BE8U;
		constexpr auto IDENT_ROOTS = 0x104CAF3CU;
		constexpr auto IDENT_DRIVERS = 0x2719EEE4U;
		constexpr auto IDENT_NAMESPACES = 0x208384E5U;
		constexpr auto IDENT_NAMES = 0x0FFC9579U;
		constexpr auto IDENT_EXTS = 0x7C967FA9U;
		constexpr auto IDENT_TITLES = 0x1E22EF7AU;
		constexpr auto IDENT_COMBINE = 0xD35DB322U;
		constexpr auto IDENT_JOIN = 0x7C9915D5U;
		constexpr auto IDENT_SPLIT = 0x105F45F1U;
		constexpr auto IDENT_SEP = 0x0B88A98DU;
		constexpr auto IDENT_SEPARATOR = 0x14015AB6U;
		constexpr auto IDENT_ISABSOLUTE = 0x42FE2EA0U;
		constexpr auto IDENT_ISRELATIVE = 0xC8FEC49DU;
		constexpr auto IDENT_ISFILE = 0x05276C61U;
		constexpr auto IDENT_ISDIR = 0x0FABD400U;
		constexpr auto IDENT_ISDIRECTORY = 0x1114D8B6U;
		constexpr auto IDENT_ISDRIVE = 0xA9F5ACBBU;
		constexpr auto IDENT_ISROOT = 0x052E1AE5U;
		constexpr auto IDENT_ISNAMESPACE = 0xBC1E84EEU;
		constexpr auto IDENT_ISCLSID = 0xA9E072D0U;
		constexpr auto IDENT_ISEXE = 0x0FABDA23U;
		constexpr auto IDENT_REMOVEEXTENSION = 0x4E267810U;
		constexpr auto IDENT_GETKNOWNFOLDER = 0x3A65268EU;
		constexpr auto IDENT_CURRENTDIRECTORY = 0xE57336DDU;
		constexpr auto IDENT_ABSOLUTE = 0x8CC74F64U;
		constexpr auto IDENT_RELATIVE = 0x12C7E561U;
		constexpr auto IDENT_WSL = 0x0B88BC5BU;
		constexpr auto IDENT_ATTRIBUTES = 0xD772C3CCU;
		constexpr auto IDENT_ATTRIBUTE = 0x160B3C39U;
		constexpr auto IDENT_IO_ATTRIB = 0xF298130BU;
		constexpr auto IDENT_IO_ATTRIBUTE_NORMAL = 0x108F79AEU;
		constexpr auto IDENT_IO_ATTRIBUTE_ARCHIVE = 0x3FB709C7U;
		constexpr auto IDENT_IO_ATTRIBUTE_COMPRESSED = 0x9E8BFF7AU;
		constexpr auto IDENT_IO_ATTRIBUTE_DEVICE = 0xF8890F15U;
		constexpr auto IDENT_IO_ATTRIBUTE_DIRECTORY = 0x940211FAU;
		constexpr auto IDENT_IO_ATTRIBUTE_ENCRYPTED = 0x42FD2953U;
		constexpr auto IDENT_IO_ATTRIBUTE_HIDDEN = 0x021C1FD1U;
		constexpr auto IDENT_IO_ATTRIBUTE_READONLY = 0xF952C623U;
		constexpr auto IDENT_IO_ATTRIBUTE_SPARSEFILE = 0xDCDDDF93U;
		constexpr auto IDENT_IO_ATTRIBUTE_SYSTEM = 0x1CEEE48AU;
		constexpr auto IDENT_IO_ATTRIBUTE_TEMPORARY = 0xC45F2F68U;
		constexpr auto IDENT_IO_ATTRIBUTE_VIRTUAL = 0x7C55CC6CU;
		constexpr auto IDENT_IO_ATTRIBUTE_OFFLINE = 0x59A0EEC8U;
		constexpr auto IDENT_IO_ATTRIBUTE_INVALID = 0x9F88534CU;

		constexpr auto IDENT_IO_DT = 0x0059779DU;
		constexpr auto IDENT_IO_DATETIME = 0x0636F3F2U;
		constexpr auto IDENT_IO_DATETIME_CREATED = 0xD9CC0A3DU;
		constexpr auto IDENT_IO_DATETIME_MODIFIED = 0x6524EC06U;
		constexpr auto IDENT_IO_DATETIME_ACCESSED = 0xB3BDD940U;

		constexpr auto IDENT_META = 0x7C9A91CCU;

		constexpr auto IDENT_FONT_WEIGHT = 0x24D3EA4DU;
		constexpr auto IDENT_FONT_ITALIC = 0x0536D35BU;

		// Path
		constexpr auto IDENT_REMOVE_ARGS = 0x5E446F9FU;
		constexpr auto IDENT_ARGS = 0x7C9432F2U;
		constexpr auto IDENT_COMPACT = 0xD3653D0CU;
		constexpr auto IDENT_QUOTE_SPACES = 0x4B9E5951U;
		constexpr auto IDENT_REMOVE_EXTENSION = 0x73D70DAFU;
		constexpr auto IDENT_RENAME_EXTENSION = 0x9F5679D9U;
		constexpr auto IDENT_ADD_EXTENSION = 0xC02963CAU;

		//file attributes
		constexpr auto IDENT_READONLY = 0xF952C623U;
		constexpr auto IDENT_TOFILE = 0x1E87C948U;
		constexpr auto IDENT_LNK = 0x0B888CEAU;
		constexpr auto IDENT_LNK_TARGET = 0x1D90FD6CU;
		constexpr auto IDENT_LNKTARGET = 0xDD2F0D11U;
		constexpr auto IDENT_BOX = 0x0B88628EU;
		constexpr auto IDENT_CHANGE = 0xF65EC1CBU;
		constexpr auto IDENT_EMPTY = 0x0F605C34U;
		constexpr auto IDENT_EXISTS = 0xFC2F1525U;
		constexpr auto IDENT_DELETE = 0xF8838478U;
		constexpr auto IDENT_MAKE = 0x7C9A7FA3U;
		constexpr auto IDENT_CREATE = 0xF715B2B9U;
		constexpr auto IDENT_COPY = 0x7C954020U;
		constexpr auto IDENT_MOVE = 0x7C9ABC9CU;
		constexpr auto IDENT_RENAME = 0x192CC41DU;
		constexpr auto IDENT_WRITE = 0x10A8B550U;
		constexpr auto IDENT_READ = 0x7C9D4D41U;
		constexpr auto IDENT_APPEND = 0xF24D48DDU;
		//
		constexpr auto IDENT_USERS = 0x1084FF77U;
		constexpr auto IDENT_MAJOR = 0x0FEA717EU;
		constexpr auto IDENT_MINOR = 0x0FEEE58AU;
		constexpr auto IDENT_BUILD = 0x0F2E56D5U;
		constexpr auto IDENT_BEEP = 0x7C9487C1U;

		// folder paths
		constexpr auto IDENT_SYSTEM = 0x1CEEE48AU;
		constexpr auto IDENT_BIN = 0x0B8861BEU;
		constexpr auto IDENT_BIN32 = 0x0F27D003U;
		constexpr auto IDENT_BIN64 = 0x0F27D068U;
		constexpr auto IDENT_WOW = 0x0B88BBE2U;
		constexpr auto IDENT_PROG = 0x7C9C6D9DU;
		constexpr auto IDENT_PROG32 = 0x156E4FA2U;
		constexpr auto IDENT_PROGRAMDATA = 0xD2BFBEB7U;
		constexpr auto IDENT_USER = 0x7C9F2E84U;
		constexpr auto IDENT_PROFILE = 0xC338A516U;
		constexpr auto IDENT_APPDATA = 0x3BF5A340U;
		constexpr auto IDENT_LOCALAPPDATA = 0x69B9868BU;
		constexpr auto IDENT_TEMP = 0x7C9E679BU;
		constexpr auto IDENT_HOME = 0x7C97FD8EU;
		constexpr auto IDENT_DESKTOP = 0x09760AFFU;
		constexpr auto IDENT_DOWNLOADS = 0xB40E8DF0U;
		constexpr auto IDENT_PERSONAL = 0x49863D29U;
		constexpr auto IDENT_DOCUMENTS = 0xC0E2C157U;
		constexpr auto IDENT_SENDTO = 0x1B81FA72U;
		constexpr auto IDENT_STARTMENU = 0x91057CE8U;
		constexpr auto IDENT_FAVORITES = 0x67325138U;
		constexpr auto IDENT_PICTURES = 0x5A874314U;
		constexpr auto IDENT_VIDEOS = 0x22C45E2FU;
		constexpr auto IDENT_MUSIC = 0x0FF58E86U;
		constexpr auto IDENT_CONTACTS = 0x42AA69A4U;
		constexpr auto IDENT_LIBRARIES = 0x866F7922U;
		constexpr auto IDENT_DOCUMENTSLIBRARY = 0x102BD54CU;
		constexpr auto IDENT_TEMPLATES = 0xC41FE294U;
		constexpr auto IDENT_QUICKLAUNCH = 0xF64BA4FDU;
		constexpr auto IDENT_PACKAGE = 0x9ABA6791U;
		constexpr auto IDENT_UWP = 0x0B88B461U;
		constexpr auto IDENT_APPX = 0x7C942B9EU;
		constexpr auto IDENT_FAMILY = 0xFCE61027U;
		//
		// datetime
		constexpr auto IDENT_DATETIME = 0x0636F3F2U;
		//
		//  date
		constexpr auto IDENT_DATE = 0x7C959163U;
		constexpr auto IDENT_DATE_YY = 0x00597A57U;
		constexpr auto IDENT_DATE_Y = 0x0002B61EU;
		constexpr auto IDENT_DATE_YEAR = 0x7CA123F6U;
		constexpr auto IDENT_DATE_M = 0x0002B612U;
		constexpr auto IDENT_DATE_MONTH = 0x0FF2306BU;
		constexpr auto IDENT_DATE_DW = 0x005977A0U;
		constexpr auto IDENT_DATE_DAYOFWEEK = 0x64566CE4U;
		constexpr auto IDENT_DATE_D = 0x0002B609U;
		constexpr auto IDENT_DATE_DAY = 0x0B886943U;
		//
		// Time
		constexpr auto IDENT_TIME = 0x7C9E7894U;
		constexpr auto IDENT_TIME_H = 0x0002B60DU;
		constexpr auto IDENT_TIME_HOUR = 0x7C97FEA3U;
		constexpr auto IDENT_TIME_MIN = 0x0B889089U;
		constexpr auto IDENT_TIME_MINUTE = 0x0DCBB0F7U;
		constexpr auto IDENT_TIME_S = 0x0002B618U;
		constexpr auto IDENT_TIME_SECOND = 0x1B7C2041U;
		constexpr auto IDENT_TIME_MS = 0x005978C5U;
		constexpr auto IDENT_TIME_MILLISECONDS = 0x2A7DA08BU;
		constexpr auto IDENT_TIME_PM = 0x00597922U;

		// string
		constexpr auto IDENT_GUID = 0x7C978A2EU;
		constexpr auto IDENT_SUB = 0x0B88AB8FU;
		constexpr auto IDENT_LEFT = 0x7C9A03B0U;
		constexpr auto IDENT_RIGHT = 0x10494163U;
		constexpr auto IDENT_FORMAT = 0xFDE634AEU;
		constexpr auto IDENT_TAG = 0x0B88AD41U;
		constexpr auto IDENT_EQ = 0x005977BBU;
		constexpr auto IDENT_EQUALS = 0xFBB6B090U;
		constexpr auto IDENT_START = 0x106149D3U;
		constexpr auto IDENT_END = 0x0B886F1CU;
		//constexpr auto IDENT_ENDSWITH = 0xB6DA51ABU;
		//constexpr auto IDENT_STARTSWITH = 0xB22B76C2U;
		//constexpr auto IDENT_STARTS = 0x1C8A84A6U;
		//constexpr auto IDENT_ENDS = 0x7C96530FU;
		//constexpr auto IDENT_WITH = 0x7CA01EA1U;
		constexpr auto IDENT_FIND = 0x7C96CB66U;
		constexpr auto IDENT_FINDLAST = 0x3876F33AU;
		constexpr auto IDENT_CONTAINS = 0x42AA8264U;
		constexpr auto IDENT_TRIM = 0x7C9E9E61U;
		constexpr auto IDENT_TRIMSTART = 0x89180CAFU;
		constexpr auto IDENT_TRIMEND = 0xF6B8CAF8U;
		constexpr auto IDENT_UPPER = 0x10838771U;
		constexpr auto IDENT_LOWER = 0x0FE03C4EU;
		constexpr auto IDENT_CAPITALIZE = 0xEE09978BU;
		constexpr auto IDENT_REMOVE = 0x192C7473U;
		constexpr auto IDENT_REPLACE = 0x3EEF4E01U;
		constexpr auto IDENT_AT = 0x0059773AU;
		//constexpr auto IDENT_PADDING = 0x9AC8CC5CU;
		constexpr auto IDENT_PADLEFT = 0x9ACD1D65U;
		constexpr auto IDENT_PADRIGHT = 0xF4DF91B8U;
		//
		// msg
		constexpr auto IDENT_MSG = 0x0B8891CCU;

		// msg icons
		constexpr auto IDENT_MSG_ERROR = 0x0F6321EFU;
		constexpr auto IDENT_MSG_QUESTION = 0xE71B785DU;
		constexpr auto IDENT_MSG_WARNING = 0xB6A3487BU;
		constexpr auto IDENT_MSG_INFO = 0x7C9884D1U;
		// msg buttons
		constexpr auto IDENT_MSG_OK = 0x005978FFU;
		constexpr auto IDENT_MSG_OKCANCEL = 0x06DDFFA5U;
		constexpr auto IDENT_MSG_YESNOCANCEL = 0xB31F4CD9U;
		constexpr auto IDENT_MSG_YESNO = 0x10C5EF33U;
		//msg result
		constexpr auto IDENT_MSG_IDCANCEL = 0x627B5918U;
		constexpr auto IDENT_MSG_IDOK = 0x7C985B6CU;
		constexpr auto IDENT_MSG_IDYES = 0x0FA3F323U;
		constexpr auto IDENT_MSG_IDNO = 0x7C985B4FU;
		// msg options
		constexpr auto IDENT_MSG_TOPMOST = 0xF03827DBU;
		constexpr auto IDENT_MSG_APPLMODAL = 0x236B3A1FU;
		constexpr auto IDENT_MSG_TASKMODAL = 0xD565B1A5U;
		constexpr auto IDENT_MSG_RIGHT = 0x10494163U;
		constexpr auto IDENT_MSG_RTLREADING = 0xB0A784D1U;
		constexpr auto IDENT_MSG_SETFOREGROUND = 0x7D794A4CU;
		constexpr auto IDENT_MSG_DEFBUTTON1 = 0x724D67A1U;
		constexpr auto IDENT_MSG_DEFBUTTON2 = 0x724D67A2U;
		constexpr auto IDENT_MSG_DEFBUTTON3 = 0x724D67A3U;
		
		/*
		MB_RIGHT
		MB_RTLREADING
		MB_SETFOREGROUND
		MB_TOPMOST
		MB_DEFBUTTON1
		MB_DEFBUTTON2
		MB_DEFBUTTON3
		*/ 
		//
		// reg
		constexpr auto IDENT_REG = 0x0B88A543U;
		// reg type
		constexpr auto IDENT_REG_NONE = 0x7C9B47F5U;
		constexpr auto IDENT_REG_SZ = 0x00597992U;
		constexpr auto IDENT_REG_EXPAND_SZ = 0x3DCEE3B1U;
		constexpr auto IDENT_REG_MULTI_SZ = 0x4674279CU;
		constexpr auto IDENT_REG_EXPAND = 0xFC329E65U;
		constexpr auto IDENT_REG_MULTI = 0x0FF57230U;
		constexpr auto IDENT_REG_BINARY = 0xF4229CCAU;
		constexpr auto IDENT_REG_DWORD = 0x0F53BAE5U;
		constexpr auto IDENT_REG_QWORD = 0x103EF972U;
		constexpr auto IDENT_VALUES = 0x22383FF5U;

		constexpr auto IDENT_INI = 0x0B888025U;
		constexpr auto IDENT_INPUT = 0x0FA94AB5U;
		constexpr auto IDENT_INPUT_RESULT = 0x192FD704U;

		/////////////////////
		// Constants
		constexpr auto IDENT_ZERO = 0x00000000U;
		constexpr auto IDENT_FALSE = 0x0F6BCEF0U;
		constexpr auto IDENT_TRUE = 0x7C9E9FE5U;
		constexpr auto IDENT_DISABLE = 0x12C4E4B9U;
		constexpr auto IDENT_ENABLE = 0xFB7573ACU;
		constexpr auto IDENT_DISABLED = 0x6B617C3DU;
		constexpr auto IDENT_ENABLED = 0x6A23E990U;

		// cmd window visible
		constexpr auto IDENT_SHOW = 0x7C9DE846U;
		constexpr auto IDENT_VISIBLE = 0x7C618D53U;
		constexpr auto IDENT_NORMAL = 0x108F79AEU;
		constexpr auto IDENT_HIDDEN = 0x021C1FD1U;
		constexpr auto IDENT_WINDOW_HIDDEN = 0x021C1FD1U;
		constexpr auto IDENT_MINIMIZED = 0x827CD46BU;
		constexpr auto IDENT_MAXIMIZED = 0x22348CEDU;
		constexpr auto IDENT_EXTENDED = 0xDCF0DD96U;

		// image
		constexpr auto IDENT_INHERIT = 0x9E8D4758U;
		constexpr auto IDENT_STAR = 0x0002B5CFU;
		//
		constexpr auto IDENT_HASH = 0x7C97C2C9U;

		// menu property index
		constexpr auto IDENT_AUTO = 0x7C94415E;
		constexpr auto IDENT_TOP = 0x0B88AF18U;
		constexpr auto IDENT_BOTTOM = 0xF492CA7AU;
		constexpr auto IDENT_MIDDLE = 0x0DC5EBD4U;
		// menu property sep
		constexpr auto IDENT_NONE = 0x7C9B47F5U;
		constexpr auto IDENT_AFTER = 0x0F143297U;
		constexpr auto IDENT_BEFORE = 0xF3D61338U;
		constexpr auto IDENT_BOTH = 0x7C94B432U;
		//
		// menu property type
		constexpr auto IDENT_TYPE = 0x7C9EBD07U;
		constexpr auto IDENT_TYPE_UNKNOWN = 0x3A834E55U;
		constexpr auto IDENT_TYPE_BACK = 0x7C947676;
		constexpr auto IDENT_TYPE_ROOT = 0x7C9D79A9U;
		constexpr auto IDENT_TYPE_DRIVE = 0x0F50E3FFU;
		constexpr auto IDENT_TYPE_DIR = 0x0B886A44U;
		constexpr auto IDENT_TYPE_DIRECTORY = 0x940211FAU;
		constexpr auto IDENT_TYPE_FILE = 0x7C96CB25U;
		constexpr auto IDENT_TYPE_FIXED = 0x0F706335U;
		constexpr auto IDENT_TYPE_VHD = 0x0B88B6A7U;
		constexpr auto IDENT_TYPE_DVD = 0x0B886BE3U;
		constexpr auto IDENT_TYPE_USB = 0x0B88B3CFU;
		constexpr auto IDENT_TYPE_REMOVABLE = 0xD9868142U;
		constexpr auto IDENT_TYPE_NAMESPACE = 0x3F0BBE32U;
		constexpr auto IDENT_TYPE_REMOTE = 0x192C7431U;
		constexpr auto IDENT_TYPE_DESKTOP = 0x09760AFFU;
		constexpr auto IDENT_TYPE_COMPUTER = 0x40181CF4U;
		constexpr auto IDENT_TYPE_RECYCLEBIN = 0xA6013D65U;
		constexpr auto IDENT_TYPE_TASKBAR = 0xCFC4E62DU;
		constexpr auto IDENT_TYPE_START = 0x106149D3U;
		constexpr auto IDENT_TYPE_EDIT = 0x7C96292BU;
		constexpr auto IDENT_TYPE_TITLEBAR = 0x329C7A9CU;
		//
		// menu property vis
		constexpr auto IDENT_VIS_ENABLE = 0xFB7573ACU;
		constexpr auto IDENT_VIS_ENABLED = 0x6A23E990U;
		constexpr auto IDENT_VIS_DISABLED = 0x6B617C3DU;

		constexpr auto IDENT_VIS_VISIBLE = 0x7C618D53U;
		constexpr auto IDENT_VIS_DISABLE = 0x12C4E4B9U;
		constexpr auto IDENT_VIS_STATIC = 0x1C8A8BADU;
		constexpr auto IDENT_VIS_LABEL = 0x0FD835A5U;
		constexpr auto IDENT_VIS_HIDDEN = 0x021C1FD1U;
		constexpr auto IDENT_VIS_EXT = 0x0B887076U;
		//constexpr auto IDENT_VIS_EXTEND = 0xFC34E0EDU;
		constexpr auto IDENT_VIS_EXTENDED = 0xDCF0DD96U;
		constexpr auto IDENT_VIS_REMOVE = 0x192C7473U;;
		constexpr auto IDENT_VIS_EXTENDED_HIDDEN = 0x021C1FD1U;
		constexpr auto IDENT_VIS_EXTENDED_DISABLE = 0x12C4E4B9U;

		constexpr auto IDENT_VIS_EXTENDEDHIDDEN = 0xBD629F81U;
		constexpr auto IDENT_VIS_EXTENDEDDISABLE = 0x36DB5A69U;

		constexpr auto IDENT_MODIFY = 0x0E32936DU;
		constexpr auto IDENT_DUPLICATE = 0x1DDAB4A0U;

		//
		// menu property mode
		constexpr auto IDENT_MODE = 0x7C9ABA4AU;
		constexpr auto IDENT_MODE_NONE = 0x7C9B47F5U;
		constexpr auto IDENT_MODE_SINGLE = 0x1BCA6827U;
		constexpr auto IDENT_MODE_UNIQUE = 0x20CCA1BCU;
		constexpr auto IDENT_MODE_MULTIPLE = 0x46746EF1U;
		constexpr auto IDENT_MODE_MULTI = 0x0FF57230U;
		constexpr auto IDENT_MODE_MULTI_SINGLE = 0x2C410B31U;
		constexpr auto IDENT_MODE_MULTI_UNIQUE = 0x314344C6U;

		constexpr auto IDENT_PRIVILEGES_NONE = 0x7C9B47F5U;
		constexpr auto IDENT_PRIVILEGES_DEFAULT = 0x0885548AU;
		constexpr auto IDENT_PRIVILEGES_USER = 0x7C9F2E84U;
		constexpr auto IDENT_PRIVILEGES_SYSTEM = 0x1CEEE48AU;
		constexpr auto IDENT_PRIVILEGES_TRUSTEDINSTALLER = 0x0D21885EU;
		constexpr auto IDENT_PRIVILEGES_FULL = 0x7C96FE38U;

		constexpr auto IDENT_PRIVILEGES__NONE = 0x6628276EU;
		constexpr auto IDENT_PRIVILEGES__DEFAULT = 0x911C2D63U;
		constexpr auto IDENT_PRIVILEGES__USER = 0x662C0DFDU;
		constexpr auto IDENT_PRIVILEGES__SYSTEM = 0x9D318643U;
		constexpr auto IDENT_PRIVILEGES__TRUSTEDINSTALLER = 0xF5E06557U;
		constexpr auto IDENT_ID_REMOVE = 0x192C7473U;
		constexpr auto IDENT_ID_ACCOUNT = 0x1CBDB112U;
		
		constexpr auto IDENT_ID_SEND_FEEDBACK = 0xF4ABBE33U;
		constexpr auto IDENT_CLIPBOARD = 0x4912A9B5U;
		constexpr auto IDENT_IS_EMPTY = 0xDC1854CFU;
		constexpr auto IDENT_ID_COPY_TO_CLIPBOARD = 0x49C3DC71U;

		constexpr auto IDENT_COMMAND_RESTORE_ALL_WINDOWS = 0xDFA7F50BU;
		constexpr auto IDENT_COMMAND_MINIMIZE_ALL_WINDOWS = 0x5A0A4929U;
		constexpr auto IDENT_COMMAND_TOGGLE_DESKTOP = 0x059EF140U;
		constexpr auto IDENT_COMMAND_RUN = 0x0B88A75AU;
		constexpr auto IDENT_COMMAND_FIND = 0x7C96CB66U;
		constexpr auto IDENT_COMMAND_SWITCHER = 0x62F0FEAEU;
		constexpr auto IDENT_COMMAND_SEARCH = 0x1B7B12DBU;
		constexpr auto IDENT_COMMAND_CASCADE_WINDOWS = 0x49EFFBF3U;
		constexpr auto IDENT_COMMAND_REFRESH = 0x3E3DB654U;
		constexpr auto IDENT_COMMAND_SHOW_WINDOWS_SIDE_BY_SIDE = 0x30178D12U;
		constexpr auto IDENT_COMMAND_SHOW_WINDOWS_STACKED = 0x06B0E4EEU;
		constexpr auto IDENT_COMMAND_COPY_TO_CLIPBOARD = 0x49C3DC71U;
		constexpr auto IDENT_COMMAND_COPY = 0x7C954020U;
		constexpr auto IDENT_COMMAND_TOGGLEEXT = 0x1BD84D58U;
		constexpr auto IDENT_COMMAND_TOGGLEHIDDEN = 0xDD2A6973U;
		constexpr auto IDENT_COMMAND_SLEEP = 0x105CF61EU;
		constexpr auto IDENT_COMMAND_RESTART_EXPLORER = 0x846E0BBAU;

		constexpr auto IDENT_COMMAND_NAVIGATE = 0x45B4B5F4U;
		constexpr auto IDENT_COMMAND_GOBACK = 0x0032646CU;
		constexpr auto IDENT_COMMAND_GOFORWARD = 0xF7ABF890U;
		constexpr auto IDENT_COMMAND_GOHOME = 0x0035EB84U;

		constexpr auto IDENT_RANDOM = 0x18E46F26U;

		constexpr auto IDENT_REGEX = 0x10470F80U;
		constexpr auto IDENT_REGEX_MATCH = 0x0FEA9A72U;
		constexpr auto IDENT_REGEX_MATCHES = 0xB4FB0C6AU;

		constexpr auto IDENT_KEYS = 0x7C9979C1U;
		constexpr auto IDENT_KEY = 0x0B88878EU;
		constexpr auto IDENT_KEY_SEND = 0x7C9DDB4FU;
		constexpr auto IDENT_KEY_NONE = 0x7C9B47F5U;
		constexpr auto IDENT_KEY_ENTER = 0x0F60F7A3U;

		constexpr auto IDENT_KEY_UP = 0x005979CAU;
		constexpr auto IDENT_KEY_DOWN = 0x7C95CD5DU;

		constexpr auto IDENT_KEY_SHIFT = 0x105AD5C3U;
		constexpr auto IDENT_KEY_RSHIFT = 0x1A26F215U;
		constexpr auto IDENT_KEY_LSHIFT = 0x0C27FE4FU;

		constexpr auto IDENT_KEY_LEFT = 0x7C9A03B0U;
		constexpr auto IDENT_KEY_RIGHT = 0x10494163U;

		constexpr auto IDENT_KEY_WIN = 0x0B88BB13U;
		constexpr auto IDENT_KEY_RWIN = 0x7C9D9AE5U;

		constexpr auto IDENT_KEY_ALT = 0x0B885DE6U;
		constexpr auto IDENT_KEY_LALT = 0x7C99F372U;
		constexpr auto IDENT_KEY_RALT = 0x7C9D3DB8U;

		constexpr auto IDENT_KEY_LBUTTON = 0x6A71978DU;
		constexpr auto IDENT_KEY_RBUTTON = 0x384F0413U;
		constexpr auto IDENT_KEY_MBUTTON = 0xB76BD44EU;

		//constexpr auto IDENT_KEY_MENU = 0x7C9A911AU;
		//constexpr auto IDENT_KEY_RMENU = 0x104B6B2CU;
		//constexpr auto IDENT_KEY_LMENU = 0x0FDED826U;

		constexpr auto IDENT_KEY_BACK = 0x7C947676U;
		constexpr auto IDENT_KEY_CAPITAL = 0xB2EFC7A3U;
		constexpr auto IDENT_KEY_CONTROL = 0xD379D0E6U;
		constexpr auto IDENT_KEY_DELETE = 0xF8838478U;
		constexpr auto IDENT_KEY_END = 0x0B886F1CU;
		constexpr auto IDENT_KEY_F1 = 0x0059779CU;
		constexpr auto IDENT_KEY_F10 = 0x0B886B4CU;
		constexpr auto IDENT_KEY_F11 = 0x0B886B4DU;
		constexpr auto IDENT_KEY_F12 = 0x0B886B4EU;
		constexpr auto IDENT_KEY_F2 = 0x0059779DU;
		constexpr auto IDENT_KEY_F3 = 0x0059779EU;
		constexpr auto IDENT_KEY_F4 = 0x0059779FU;
		constexpr auto IDENT_KEY_F5 = 0x005977A0U;
		constexpr auto IDENT_KEY_F6 = 0x005977A1U;
		constexpr auto IDENT_KEY_F7 = 0x005977A2U;
		constexpr auto IDENT_KEY_F8 = 0x005977A3U;
		constexpr auto IDENT_KEY_F9 = 0x005977A4U;
		constexpr auto IDENT_KEY_HELP = 0x7C97D2EEU;
		constexpr auto IDENT_KEY_HOME = 0x7C97FD8EU;
		constexpr auto IDENT_KEY_INSERT = 0x04D4029AU;
		constexpr auto IDENT_KEY_LCONTROL = 0xF7334C72U;
		
		constexpr auto IDENT_KEY_LWIN = 0x7C9A509FU;
		constexpr auto IDENT_KEY_NEXT = 0x7C9B1EC4U;
		constexpr auto IDENT_KEY_PAUSE = 0x1020EA43U;
		constexpr auto IDENT_KEY_PRIOR = 0x102A0931U;
		constexpr auto IDENT_KEY_RCONTROL = 0x80BE49B8U;
		constexpr auto IDENT_KEY_RETURN = 0x19306425U;
		constexpr auto IDENT_KEY_APPS = 0x7C942B99U;
		constexpr auto IDENT_KEY_CAPSLOCK = 0x119988D5U;
		constexpr auto IDENT_KEY_CANCEL = 0xF5E7082BU;
		constexpr auto IDENT_KEY_EXECUTE = 0x81C09678U;
		constexpr auto IDENT_KEY_PAGEDOWN = 0xFAF1331AU;
		constexpr auto IDENT_KEY_PAGEUP = 0x14364C47U;
		constexpr auto IDENT_KEY_PLAY = 0x7C9C525BU;
		constexpr auto IDENT_KEY_PRINT = 0x102A0912U;
		constexpr auto IDENT_KEY_PRINTSCREEN = 0x50E60712U;
		constexpr auto IDENT_KEY_SNAPSHOT = 0x9B3BFC35U;
		constexpr auto IDENT_KEY_SPACE = 0x105F1651U;
		constexpr auto IDENT_KEY_TAB = 0x0B88AD3CU;
		constexpr auto IDENT_KEY_ESCAPE = 0xFBD10336U;
		constexpr auto IDENT_KEY_CTRL = 0x7C95559AU;
		constexpr auto IDENT_KEY_RCTRL = 0x10462FACU;
		constexpr auto IDENT_KEY_LCTRL = 0x0FD99CA6U;
		//
		// COLOR
		constexpr auto IDENT_COLOR = 0x0F3D3244U;
		constexpr auto IDENT_COLOR1 = 0xF6E37AF5U;
		constexpr auto IDENT_COLOR2 = 0xF6E37AF6U;
		constexpr auto IDENT_COLOR3 = 0xF6E37AF7U;
		constexpr auto IDENT_COLOR_DEFAULT = 0x0885548AU;
		constexpr auto IDENT_COLOR_ALICEBLUE = 0x41F60F4BU;
		constexpr auto IDENT_COLOR_ANTIQUEWHITE = 0x3A2D20FDU;
		constexpr auto IDENT_COLOR_AQUA = 0x7C94306DU;
		constexpr auto IDENT_COLOR_AQUAMARINE = 0x52E1F409U;
		constexpr auto IDENT_COLOR_AZURE = 0x0F1F300CU;
		constexpr auto IDENT_COLOR_BEIGE = 0x0F259021U;
		constexpr auto IDENT_COLOR_BISQUE = 0xF4259F0EU;
		constexpr auto IDENT_COLOR_BLACK = 0x0F294442U;
		constexpr auto IDENT_COLOR_BLANCHEDALMOND = 0x25A17751U;
		constexpr auto IDENT_COLOR_BLUE = 0x7C94A78DU;
		constexpr auto IDENT_COLOR_BLUEVIOLET = 0x59F4DB60U;
		constexpr auto IDENT_COLOR_BROWN = 0x0F2CCCADU;
		constexpr auto IDENT_COLOR_BURLYWOOD = 0xD00306ACU;
		constexpr auto IDENT_COLOR_CADETBLUE = 0x88F15CAEU;
		constexpr auto IDENT_COLOR_CHARTREUSE = 0xFB91543BU;
		constexpr auto IDENT_COLOR_CHOCOLATE = 0x487F4C37U;
		constexpr auto IDENT_COLOR_CORAL = 0x0F3D49F6U;
		constexpr auto IDENT_COLOR_CORNFLOWERBLUE = 0x68196CEEU;
		constexpr auto IDENT_COLOR_CORNSILK = 0x4B9C706AU;
		constexpr auto IDENT_COLOR_CRIMSON = 0xDA1AFDE0U;
		constexpr auto IDENT_COLOR_CYAN = 0x7C9568B0U;
		constexpr auto IDENT_COLOR_DARKBLUE = 0x01EF64AFU;
		constexpr auto IDENT_COLOR_DARKCYAN = 0x01F025D2U;
		constexpr auto IDENT_COLOR_DARKGOLDENROD = 0xC6D90285U;
		constexpr auto IDENT_COLOR_DARKGRAY = 0x01F2399AU;
		constexpr auto IDENT_COLOR_DARKGREEN = 0x40397BB8U;
		constexpr auto IDENT_COLOR_DARKKHAKI = 0x407C51AFU;
		constexpr auto IDENT_COLOR_DARKMAGENTA = 0xDAE143E4U;
		constexpr auto IDENT_COLOR_DARKOLIVEGREEN = 0x092C7A97U;
		constexpr auto IDENT_COLOR_DARKORANGE = 0x5A102C03U;
		constexpr auto IDENT_COLOR_DARKORCHID = 0x5A112B80U;
		constexpr auto IDENT_COLOR_DARKRED = 0x000F4622U;
		constexpr auto IDENT_COLOR_DARKSALMON = 0x623732F1U;
		constexpr auto IDENT_COLOR_DARKSEAGREEN = 0xE6A4E091U;
		constexpr auto IDENT_COLOR_DARKSLATEBLUE = 0x4E741068U;
		constexpr auto IDENT_COLOR_DARKSLATEGRAY = 0x4E76E553U;
		constexpr auto IDENT_COLOR_DARKTURQUOISE = 0x28082838U;
		constexpr auto IDENT_COLOR_DARKVIOLET = 0x69C9107AU;
		constexpr auto IDENT_COLOR_DEEPPINK = 0x17E761B5U;
		constexpr auto IDENT_COLOR_DEEPSKYBLUE = 0x84780222U;
		constexpr auto IDENT_COLOR_DIMGRAY = 0x125BDEB2U;
		constexpr auto IDENT_COLOR_DODGERBLUE = 0x8208B222U;
		constexpr auto IDENT_COLOR_FIREBRICK = 0x7CE7A736U;
		constexpr auto IDENT_COLOR_FLORALWHITE = 0xA97767C6U;
		constexpr auto IDENT_COLOR_FORESTGREEN = 0x8EDA0A29U;
		constexpr auto IDENT_COLOR_FUCHSIA = 0xC799DC48U;
		constexpr auto IDENT_COLOR_GAINSBORO = 0xF0B2B209U;
		constexpr auto IDENT_COLOR_GHOSTWHITE = 0x44AB668BU;
		constexpr auto IDENT_COLOR_GOLD = 0x7C97710BU;
		constexpr auto IDENT_COLOR_GOLDENROD = 0xAAF0C023U;
		constexpr auto IDENT_COLOR_GRAY = 0x7C977C78U;
		constexpr auto IDENT_COLOR_GREEN = 0x0F871A56U;
		constexpr auto IDENT_COLOR_GREENYELLOW = 0xC0A3F4F2U;
		constexpr auto IDENT_COLOR_HONEYDEW = 0xDEF14DE8U;
		constexpr auto IDENT_COLOR_HOTPINK = 0x54C73BC2U;
		constexpr auto IDENT_COLOR_INDIANRED = 0x4B374F13U;
		constexpr auto IDENT_COLOR_INDIGO = 0x04CBD87FU;
		constexpr auto IDENT_COLOR_IVORY = 0x0FADA91EU;
		constexpr auto IDENT_COLOR_KHAKI = 0x0FC9F04DU;
		constexpr auto IDENT_COLOR_LAVENDER = 0x6CEC8BB6U;
		constexpr auto IDENT_COLOR_LAVENDERBLUSH = 0x4D30E8B4U;
		constexpr auto IDENT_COLOR_LAWNGREEN = 0x6BFFAD68U;
		constexpr auto IDENT_COLOR_LEMONCHIFFON = 0x1AA3AB7DU;
		constexpr auto IDENT_COLOR_LIGHTBLUE = 0xF14E2CE5U;
		constexpr auto IDENT_COLOR_LIGHTCORAL = 0x1B277A4EU;
		constexpr auto IDENT_COLOR_LIGHTCYAN = 0xF14EEE08U;
		constexpr auto IDENT_COLOR_LIGHTGOLDENRODYELLOW = 0x269C7ED7U;
		constexpr auto IDENT_COLOR_LIGHTGRAY = 0xF15101D0U;
		constexpr auto IDENT_COLOR_LIGHTGREEN = 0x1B714AAEU;
		constexpr auto IDENT_COLOR_LIGHTPINK = 0xF155CC8FU;
		constexpr auto IDENT_COLOR_LIGHTSALMON = 0xA468E0A7U;
		constexpr auto IDENT_COLOR_LIGHTSEAGREEN = 0x7BF8D3C7U;
		constexpr auto IDENT_COLOR_LIGHTSKYBLUE = 0x49BDB6BCU;
		constexpr auto IDENT_COLOR_LIGHTSLATEGRAY = 0x8E493F49U;
		constexpr auto IDENT_COLOR_LIGHTSTEELBLUE = 0x01BF4E82U;
		constexpr auto IDENT_COLOR_LIGHTYELLOW = 0xB2B03239U;
		constexpr auto IDENT_COLOR_LIME = 0x7C9A158CU;
		constexpr auto IDENT_COLOR_LIMEGREEN = 0xB749873DU;
		constexpr auto IDENT_COLOR_LINEN = 0x0FDCCBBBU;
		constexpr auto IDENT_COLOR_MAGENTA = 0xB4110202U;
		constexpr auto IDENT_COLOR_MAROON = 0x0D3D0451U;
		constexpr auto IDENT_COLOR_MEDIUMAQUAMARINE = 0x5393448AU;
		constexpr auto IDENT_COLOR_MEDIUMBLUE = 0xD877EB4EU;
		constexpr auto IDENT_COLOR_MEDIUMORCHID = 0xF4D5D5DFU;
		constexpr auto IDENT_COLOR_MEDIUMPURPLE = 0xF769A41EU;
		constexpr auto IDENT_COLOR_MEDIUMSEAGREEN = 0x453D9EB0U;
		constexpr auto IDENT_COLOR_MEDIUMSLATEBLUE = 0x80249267U;
		constexpr auto IDENT_COLOR_MEDIUMSPRINGGREEN = 0x814643CAU;
		constexpr auto IDENT_COLOR_MEDIUMTURQUOISE = 0x59B8AA37U;
		constexpr auto IDENT_COLOR_MEDIUMVIOLETRED = 0x3BE46A94U;
		constexpr auto IDENT_COLOR_MIDNIGHTBLUE = 0x5F9313A1U;
		constexpr auto IDENT_COLOR_MINTCREAM = 0x9B7533E5U;
		constexpr auto IDENT_COLOR_MISTYROSE = 0x1DE6AB94U;
		constexpr auto IDENT_COLOR_MOCCASIN = 0x62609D92U;
		constexpr auto IDENT_COLOR_NAVAJOWHITE = 0xE2BC6625U;
		constexpr auto IDENT_COLOR_NAVY = 0x7C9B0D83U;
		constexpr auto IDENT_COLOR_OLDLACE = 0x677B8E19U;
		constexpr auto IDENT_COLOR_OLIVE = 0x1014A744U;
		constexpr auto IDENT_COLOR_OLIVEDRAB = 0xCD1770FDU;
		constexpr auto IDENT_COLOR_ORANGE = 0x13119E61U;
		constexpr auto IDENT_COLOR_ORANGERED = 0xDC4C011CU;
		constexpr auto IDENT_COLOR_ORCHID = 0x13129DDEU;
		constexpr auto IDENT_COLOR_PALEGOLDENROD = 0x46E1CE45U;
		constexpr auto IDENT_COLOR_PALEGREEN = 0xDA326778U;
		constexpr auto IDENT_COLOR_PALETURQUOISE = 0xA810F3F8U;
		constexpr auto IDENT_COLOR_PALEVIOLETRED = 0x8A3CB455U;
		constexpr auto IDENT_COLOR_PAPAYAWHIP = 0xC670DD19U;
		constexpr auto IDENT_COLOR_PEACHPUFF = 0x37E01157U;
		constexpr auto IDENT_COLOR_PERU = 0x7C9C36C1U;
		constexpr auto IDENT_COLOR_PINK = 0x7C9C4737U;
		constexpr auto IDENT_COLOR_PLUM = 0x7C9C54E3U;
		constexpr auto IDENT_COLOR_POWDERBLUE = 0x547B961EU;
		constexpr auto IDENT_COLOR_PURPLE = 0x15A66C1DU;
		constexpr auto IDENT_COLOR_RED = 0x0B88A540U;
		constexpr auto IDENT_COLOR_ROSYBROWN = 0xF7E975FAU;
		constexpr auto IDENT_COLOR_ROYALBLUE = 0x8E773554U;
		constexpr auto IDENT_COLOR_SADDLEBROWN = 0x92BBF35AU;
		constexpr auto IDENT_COLOR_SALMON = 0x1B38A54FU;
		constexpr auto IDENT_COLOR_SANDYBROWN = 0xE10B172CU;
		constexpr auto IDENT_COLOR_SEAGREEN = 0xE5CC626FU;
		constexpr auto IDENT_COLOR_SEASHELL = 0xE6A00D96U;
		constexpr auto IDENT_COLOR_SIENNA = 0x1BC596C3U;
		constexpr auto IDENT_COLOR_SILVER = 0x1BC98E5AU;
		constexpr auto IDENT_COLOR_SKYBLUE = 0x9A861064U;
		constexpr auto IDENT_COLOR_SLATEBLUE = 0x328BCE06U;
		constexpr auto IDENT_COLOR_SLATEGRAY = 0x328EA2F1U;
		constexpr auto IDENT_COLOR_SNOW = 0x7C9E01CCU;
		constexpr auto IDENT_COLOR_SPRINGGREEN = 0x6A6AE329U;
		constexpr auto IDENT_COLOR_STEELBLUE = 0xA604B22AU;
		constexpr auto IDENT_COLOR_TAN = 0x0B88AD48U;
		constexpr auto IDENT_COLOR_TEAL = 0x7C9E660BU;
		constexpr auto IDENT_COLOR_THISTLE = 0xDF68BE82U;
		constexpr auto IDENT_COLOR_TOMATO = 0x1E8B7EF9U;
		constexpr auto IDENT_COLOR_TRANSPARENT = 0x6E576D97U;
		constexpr auto IDENT_COLOR_TURQUOISE = 0x0C1FE5D6U;
		constexpr auto IDENT_COLOR_VIOLET = 0x22CA82D8U;
		constexpr auto IDENT_COLOR_WHEAT = 0x10A3261EU;
		constexpr auto IDENT_COLOR_WHITE = 0x10A33986U;
		constexpr auto IDENT_COLOR_WHITESMOKE = 0x2580CAE5U;
		constexpr auto IDENT_COLOR_YELLOW = 0x297FF6E1U;
		constexpr auto IDENT_COLOR_YELLOWGREEN = 0xDA4A85B2U;
		// color accent
		constexpr auto IDENT_COLOR_ACCENT = 0xF15AE973U;
		constexpr auto IDENT_COLOR_ACCENT_LIGHT1 = 0x48DD6B1BU;
		constexpr auto IDENT_COLOR_ACCENT_LIGHT2 = 0x48DD6B1CU;
		constexpr auto IDENT_COLOR_ACCENT_LIGHT3 = 0x48DD6B1DU;
		constexpr auto IDENT_COLOR_ACCENT_DARK1 = 0x4771BD85U;
		constexpr auto IDENT_COLOR_ACCENT_DARK2 = 0x4771BD86U;
		constexpr auto IDENT_COLOR_ACCENT_DARK3 = 0x4771BD87U;

		// color functions
		constexpr auto IDENT_COLOR_RGB = 0x0B88A580U;
		constexpr auto IDENT_COLOR_RGBA = 0x7C9D55E1U;
		constexpr auto IDENT_COLOR_RANDOM = 0x18E46F26U;
		constexpr auto IDENT_COLOR_INVERT = 0x04D5A7BDU;
		constexpr auto IDENT_COLOR_LIGHT = 0x0FDCAE5DU; //deprcate
		constexpr auto IDENT_COLOR_DARK = 0x7C959127U; //deprcate
		constexpr auto IDENT_COLOR_ADJUST = 0xF1711D50U;
		constexpr auto IDENT_COLOR_LIGHTEN = 0x79C1C710U;
		constexpr auto IDENT_COLOR_DARKEN = 0xF83E845AU;

		//
		// FONTS
		constexpr auto IDENT_FONT_LOADED = 0x0BDBB02EU;
		constexpr auto IDENT_FONT_SYSTEM = 0x1CEEE48AU;
		constexpr auto IDENT_FONT_EXISTS = 0xFC2F1525U;

		constexpr auto IDENT_FONT_SEGOE_FLUENT_ICONS = 0xA970D1C0U;
		constexpr auto IDENT_FONT_SEGOE_MDL2 = 0x7EF88207U;
		constexpr auto IDENT_FONT_SEGOE_UI_SYMBOL = 0x6470616CU;
		constexpr auto IDENT_FONT_SEGOE_UI = 0x8B4866B6U;
		constexpr auto IDENT_FONT_SEGOE_UI_EMOJI = 0x76645EEAU;
		constexpr auto IDENT_FONT_SEGOE_UI_HISTORIC = 0x5ADB4EFBU;
		constexpr auto IDENT_FONT_WEBDINGS = 0x98EDCC38U;
		constexpr auto IDENT_FONT_WINGDINGS = 0x0A0BDDEFU;
		constexpr auto IDENT_FONT_WINGDINGS2 = 0x4B879C01U;
		constexpr auto IDENT_FONT_WINGDINGS3 = 0x4B879C02U;

		// icons id
		constexpr auto IDENT_ICONS_SEGOE = 0x10592938U;
		constexpr auto IDENT_ICON_FLUENT = 0xFDB16FF3U;
		constexpr auto IDENT_ICON_MDL = 0x0B888FE2U;

		// ID
		constexpr auto IDENT_ID_CUT = 0x0B886791U;
		constexpr auto IDENT_ID_COPY = 0x7C954020U;
		constexpr auto IDENT_ID_PASTE = 0x1020E1E2U;
		constexpr auto IDENT_ID_CREATE_SHORTCUT = 0xAFE6E994U;
		constexpr auto IDENT_ID_DELETE = 0xF8838478U;
		constexpr auto IDENT_ID_RENAME = 0x192CC41DU;
		constexpr auto IDENT_ID_PASTE_SHORTCUT = 0xBBA836DDU;
		constexpr auto IDENT_ID_COPY_AS_PATH = 0xD7D9D35FU;
		constexpr auto IDENT_ID_EDIT = 0x7C96292BU;
		constexpr auto IDENT_ID_EMPTY_RECYCLE_BIN = 0x0B255032U;
		constexpr auto IDENT_ID_GROUP_BY = 0xDD0FDAECU;
		constexpr auto IDENT_ID_REFRESH = 0x3E3DB654U;
		constexpr auto IDENT_ID_CUSTOMIZE_THIS_FOLDER = 0x6A27805AU;
		constexpr auto IDENT_ID_INCLUDE_IN_LIBRARY = 0x74D87B53U;
		constexpr auto IDENT_ID_RESTORE_DEFAULT_LIBRARIES = 0x3D27EDA9U;
		constexpr auto IDENT_ID_MAP_AS_DRIVE = 0xB5768DCFU;
		constexpr auto IDENT_ID_NEW = 0x0B88944FU;
		constexpr auto IDENT_ID_FILE_EXPLORER_OPTIONS = 0x621B1060U;
		constexpr auto IDENT_ID_OPEN_AUTOPLAY = 0x9FF2D5E5U;
		constexpr auto IDENT_ID_OPEN_COMMAND_PROMPT = 0x44015056U;
		constexpr auto IDENT_ID_OPEN_COMMAND_WINDOW_HERE = 0xD142FCEFU;
		constexpr auto IDENT_ID_OPEN_FILE_LOCATION = 0x9387CC8EU;
		constexpr auto IDENT_ID_OPEN_FOLDER_LOCATION = 0x13A9C9CAU;
		constexpr auto IDENT_ID_OPEN_IN_NEW_PROCESS = 0xDC7DAA14U;
		constexpr auto IDENT_ID_OPEN_NEW_TAB = 0x4487F6B6U;
		constexpr auto IDENT_ID_OPEN_NEW_WINDOW = 0x5A088D97U;
		constexpr auto IDENT_ID_OPEN_IN_NEW_WINDOW = 0x165F328DU;
		constexpr auto IDENT_ID_OPEN_IN_NEW_TAB = 0x91A9CF6CU;
		constexpr auto IDENT_ID_SEARCH = 0x1B7B12DBU;
		constexpr auto IDENT_ID_OPEN_POWERSHELL_WINDOW_HERE = 0xFE8745F5U;
		constexpr auto IDENT_ID_OPEN_WITH = 0x372A40D2U;
		constexpr auto IDENT_ID_PIN_TO_START = 0xC12B6D3BU;
		constexpr auto IDENT_ID_PIN_TO_TASKBAR = 0xDBA58395U;
		constexpr auto IDENT_ID_ARRANGE_BY = 0x0553551FU;
		constexpr auto IDENT_ID_VIEW = 0x7C9F9060U;
		constexpr auto IDENT_ID_RUN_AS_ADMINISTRATOR = 0x681DDF6DU;
		constexpr auto IDENT_ID_RUN_AS_ANOTHER_USER = 0x23DA51BBU;
		constexpr auto IDENT_ID_RUN_AS_DIFFERENT_USER = 0x02C88681U;
		constexpr auto IDENT_ID_SEND_TO = 0x8BC0F071U;
		constexpr auto IDENT_ID_SORT_BY = 0xA365AC87U;
		constexpr auto IDENT_ID_UNPIN_FROM_START = 0x138825AFU;
		constexpr auto IDENT_ID_UNPIN_FROM_TASKBAR = 0x38122909U;
		constexpr auto IDENT_ID_ADD_TO_PLAYLIST = 0xF1EE6BA1U;
		constexpr auto IDENT_ID_PROPERTIES = 0xEBA6EB32U;
		constexpr auto IDENT_ID_COPY_HERE = 0x3D743023U;
		constexpr auto IDENT_ID_MOVE_HERE = 0x6986DA1FU;
		constexpr auto IDENT_ID_CREATE_SHORTCUTS = 0xACC41C87U;
		constexpr auto IDENT_ID_CREATE_SHORTCUTS_HERE = 0xD98FE4EAU;
		constexpr auto IDENT_ID_CANCEL = 0xF5E7082BU;
		constexpr auto IDENT_ID_COPY_PATH = 0x3D78826CU;
		constexpr auto IDENT_ID_COPY_TO = 0xD3A06FA2U;
		constexpr auto IDENT_ID_MOVE_TO = 0xD5C86A9EU;
		constexpr auto IDENT_ID_PRINT = 0x102A0912U;
		constexpr auto IDENT_ID_CONFIGURE = 0x77E25F67U;
		constexpr auto IDENT_ID_INSTALL = 0x9F5C469CU;
		constexpr auto IDENT_ID_OPEN = 0x7C9BD777U;
		constexpr auto IDENT_ID_CAST_TO_DEVICE = 0xAEF4E421U;
		constexpr auto IDENT_ID_PLAY = 0x7C9C525BU;
		constexpr auto IDENT_ID_MAP_NETWORK_DRIVE = 0x8CC807E5U;
		constexpr auto IDENT_ID_DISCONNECT_NETWORK_DRIVE = 0x1C80F871U;
		constexpr auto IDENT_ID_ADD_A_NETWORK_LOCATION = 0x6B08640FU;
		constexpr auto IDENT_ID_RESTORE = 0x3F2A3809U;
		constexpr auto IDENT_ID_EJECT = 0x0F5E8610U;
		constexpr auto IDENT_ID_UNDO = 0x7C9F191BU;
		constexpr auto IDENT_ID_REDO = 0x7C9D4DAFU;
		constexpr auto IDENT_ID_ERASE_THIS_DISC = 0xFDB7AA8EU;
		constexpr auto IDENT_ID_AUTOPLAY = 0x45B10334U;
		constexpr auto IDENT_ID_MOUNT = 0x0FF24D78U;
		constexpr auto IDENT_ID_TROUBLESHOOT_COMPATIBILITY = 0x6D848FA8U;
		constexpr auto IDENT_ID_OPEN_WINDOWS_POWERSHELL = 0x09469325U;
		constexpr auto IDENT_ID_FORMAT = 0xFDE634AEU;
		constexpr auto IDENT_ID_DISCONNECT = 0x82223B0FU;
		constexpr auto IDENT_ID_EXTRACT_ALL = 0x85CDEB78U;
		constexpr auto IDENT_ID_EXTRACT_TO = 0xA6F6BC02U;
		constexpr auto IDENT_ID_MANAGE = 0x0D3A962EU;
		constexpr auto IDENT_ID_ROTATE_LEFT = 0x308CE4DEU;
		constexpr auto IDENT_ID_ROTATE_RIGHT = 0x42984851U;
		constexpr auto IDENT_ID_PREVIEW = 0xC28C7547U;
		constexpr auto IDENT_ID_PIN_TO_QUICK_ACCESS = 0x7A22B65BU;
		constexpr auto IDENT_ID_PIN_CURRENT_FOLDER_TO_QUICK_ACCESS = 0x44C3A8B8U;
		constexpr auto IDENT_ID_UNPIN_FROM_QUICK_ACCESS = 0x6BD4E44FU;
		constexpr auto IDENT_ID_REMOVE_FROM_QUICK_ACCESS = 0x99015293U;
		constexpr auto IDENT_ID_REMOVE_FROM_RECENT = 0xC9412FC6U;
		constexpr auto IDENT_ID_DEVICE_MANAGER = 0xA799128FU;
		constexpr auto IDENT_ID_ADJUST_DATE_TIME = 0xB63C21EBU;
		constexpr auto IDENT_ID_CUSTOMIZE_NOTIFICATION_ICONS = 0xF3DF5869U;
		constexpr auto IDENT_ID_CORTANA = 0xD3C1EA6DU;
		constexpr auto IDENT_ID_NEWS_AND_INTERESTS = 0x73107C74U;
		constexpr auto IDENT_ID_SHOW_CORTANA_BUTTON = 0x76303688U;
		constexpr auto IDENT_ID_SHOW_TASK_VIEW_BUTTON = 0x53DBDF8DU;
		constexpr auto IDENT_ID_SHOW_PEOPLE_ON_THE_TASKBAR = 0x6895DE6DU;
		constexpr auto IDENT_ID_SHOW_PEN_BUTTON = 0x690E2483U;
		constexpr auto IDENT_ID_SHOW_TOUCH_KEYBOARD_BUTTON = 0xF4E93B13U;
		constexpr auto IDENT_ID_SHOW_TOUCHPAD_BUTTON = 0xF9A1EDF8U;
		constexpr auto IDENT_ID_CASCADE_WINDOWS = 0x49EFFBF3U;
		constexpr auto IDENT_ID_SHOW_WINDOWS_STACKED = 0x06B0E4EEU;
		constexpr auto IDENT_ID_SHOW_WINDOWS_SIDE_BY_SIDE = 0x30178D12U;
		constexpr auto IDENT_ID_SHOW_THE_DESKTOP = 0x0890419FU;
		constexpr auto IDENT_ID_SHOW_DESKTOP_ICONS = 0xDF0C579AU;
		constexpr auto IDENT_ID_TASK_MANAGER = 0xED07EDD2U;
		constexpr auto IDENT_ID_CONTROL_PANEL = 0x2A2443B5U;
		constexpr auto IDENT_ID_DESKTOP = 0x09760AFFU;
		constexpr auto IDENT_ID_COMMAND_PROMPT = 0x7E25FCE5U;
		constexpr auto IDENT_ID_LOCK_THE_TASKBAR = 0x70E7EE55U;
		constexpr auto IDENT_ID_LOCK_ALL_TASKBARS = 0x1D929820U;
		constexpr auto IDENT_ID_TASKBAR_SETTINGS = 0xD202AA3DU;
		constexpr auto IDENT_ID_EXIT_EXPLORER = 0xC9F4490FU;
		constexpr auto IDENT_ID_BURN_DISC_IMAGE = 0x33E9E560U;
		constexpr auto IDENT_ID_RUN = 0x0B88A75AU;
		constexpr auto IDENT_ID_OPTIONS = 0x71F05091U;
		constexpr auto IDENT_ID_RESTORE_PREVIOUS_VERSIONS = 0xA4DA1D7DU;
		constexpr auto IDENT_ID_SET_AS_DESKTOP_WALLPAPER = 0xF8DD0AC4U;
		constexpr auto IDENT_ID_SET_AS_DESKTOP_BACKGROUND = 0x108AAD5CU;
		constexpr auto IDENT_ID_NEXT_DESKTOP_BACKGROUND = 0x9F8BC71CU;
		constexpr auto IDENT_ID_TURN_ON_BITLOCKER = 0xA1387848U;
		constexpr auto IDENT_ID_TURN_OFF_BITLOCKER = 0xE581E266U;
		constexpr auto IDENT_ID_SHOW_THIS_PC = 0xF223960FU;
		constexpr auto IDENT_ID_SHOW_NETWORK = 0x1E10330FU;
		constexpr auto IDENT_ID_SHOW_LIBRARIES = 0x34B84182U;
		constexpr auto IDENT_ID_SHOW_ALL_FOLDERS = 0x1305778CU;
		constexpr auto IDENT_ID_EXPAND_TO_CURRENT_FOLDER = 0xE7BD2EC4U;
		constexpr auto IDENT_ID_COMPRESSED = 0x9E8BFF7AU;
		constexpr auto IDENT_ID_CLEANUP = 0xCBCCFF0DU;
		constexpr auto IDENT_ID_NEW_FOLDER = 0x40CB10EAU;
		constexpr auto IDENT_ID_NEW_ITEM = 0x7B3F443DU;
		constexpr auto IDENT_ID_OPEN_AS_PORTABLE = 0x8044C402U;
		constexpr auto IDENT_ID_IMPORT_PICTURES_AND_VIDEOS = 0x7FC81329U;
		constexpr auto IDENT_ID_SHOW_FILE_EXTENSIONS = 0x880DD7D4U;
		constexpr auto IDENT_ID_SHOW_HIDDEN_FILES = 0x08F6A8C3U;
		constexpr auto IDENT_ID_MAKE_AVAILABLE_OFFLINE = 0x7F393D25U;
		constexpr auto IDENT_ID_MAKE_AVAILABLE_ONLINE = 0x2B38A6E7U;
		constexpr auto IDENT_ID_REMOVE_PROPERTIES = 0xFCAF359FU;
		constexpr auto IDENT_ID_EXPAND_ALL_GROUPS = 0x9BB31DBCU;
		constexpr auto IDENT_ID_COLLAPSE_ALL_GROUPS = 0x0EF1D8EFU;
		constexpr auto IDENT_ID_FOLDER_OPTIONS = 0x6348884CU;
		constexpr auto IDENT_ID_MORE_OPTIONS = 0xDFE006C3U;
		constexpr auto IDENT_ID_SHIELD = 0x1BB5893EU;
		constexpr auto IDENT_ID_COPY_TO_FOLDER = 0x137690DDU;
		constexpr auto IDENT_ID_MOVE_TO_FOLDER = 0x1E6ED859U;
		constexpr auto IDENT_ID_GO_TO = 0x0F855D9DU;
		constexpr auto IDENT_ID_SELECT = 0x1B80E3C5U;
		constexpr auto IDENT_ID_SELECT_ALL = 0xF55D013DU;
		constexpr auto IDENT_ID_SELECT_NONE = 0xA1045734U;
		constexpr auto IDENT_ID_INVERT_SELECTION = 0x38FB5FA2U;
		constexpr auto IDENT_ID_RECONVERSION = 0xC82B4322U;
		constexpr auto IDENT_ID_INSERT_UNICODE_CONTROL_CHARACTER = 0x25C0B08CU;
		constexpr auto IDENT_ID_SETTINGS = 0x1304DC16U;
		constexpr auto IDENT_ID_STORE = 0x10618552U;
		constexpr auto IDENT_ID_WINDOWS_POWERSHELL = 0xE7FCEF34U;
		constexpr auto IDENT_ID_WINDOWS_TERMINAL = 0x9A79E12BU;

		constexpr auto IDENT_ID_TASKBAR = 0xCFC4E62DU;
		constexpr auto IDENT_ID_WINDOWS = 0xC8FECA70U;
		constexpr auto IDENT_ID_MINIMIZE_ALL_WINDOWS = 0x5A0A4929U;
		constexpr auto IDENT_ID_RESTORE_ALL_WINDOWS = 0xDFA7F50BU;
		constexpr auto IDENT_ID_CLOSE_ALL_WINDOWS = 0x1F4D625DU;
		constexpr auto IDENT_ID_FILE_EXPLORER = 0x8E8878B5U;
		constexpr auto IDENT_ID_SHOW_OPEN_WINDOWS = 0xD930F761U;

		constexpr auto IDENT_ID_MOVE = 0x7C9ABC9CU;
		constexpr auto IDENT_ID_SIZE = 0x7C9DEDE0U;
		constexpr auto IDENT_ID_MINIMIZE = 0xAE9EEF27U;
		constexpr auto IDENT_ID_MAXIMIZE = 0x5E209F69U;

		constexpr auto IDENT_ID_HIDE = 0x7C97E2DFU;
		constexpr auto IDENT_ID_SHOW = 0x7C9DE846U;
		constexpr auto IDENT_ID_TERMINAL = 0xFA1DB361U;
		constexpr auto IDENT_ID_POWER_OPTIONS = 0xF06182FDU;
		constexpr auto IDENT_ID_DISPLAY_SETTINGS = 0xBB92876BU;
		constexpr auto IDENT_ID_PERSONALIZE = 0x45516751U;
		constexpr auto IDENT_ID_ADD_TO_WINDOWS_MEDIA_PLAYER_LIST = 0x5D0DD300U;
		constexpr auto IDENT_ID_PLAY_WITH_WINDOWS_MEDIA_PLAYER = 0x6B344D8BU;
		constexpr auto IDENT_ID_GIVE_ACCESS_TO = 0x5840D7A3U;
		constexpr auto IDENT_ID_SHARE = 0x105AB538U;
		constexpr auto IDENT_ID_MERGE = 0x0FECC3F5U;
		constexpr auto IDENT_ID_EXPAND = 0xFC329E65U;
		constexpr auto IDENT_ID_COLLAPSE = 0x3D6F8B78U;
		constexpr auto IDENT_ID_SHARE_WITH = 0x17EEDF33U;
		constexpr auto IDENT_ID_EXTRA_LARGE_ICONS = 0xB961A88EU;
		constexpr auto IDENT_ID_LARGE_ICONS = 0xE8667EEBU;
		constexpr auto IDENT_ID_MEDIUM_ICONS = 0xCEE08DC1U;
		constexpr auto IDENT_ID_SMALL_ICONS = 0xBA4B91D9U;
		constexpr auto IDENT_ID_LIST = 0x7C9A1661U;
		constexpr auto IDENT_ID_DETAILS = 0x0982788BU;
		constexpr auto IDENT_ID_TILES = 0x106D8746U;
		constexpr auto IDENT_ID_CONTENT = 0xD3799980U;
		constexpr auto IDENT_ID_AUTO_ARRANGE_ICONS = 0x2E1B1238U;
		constexpr auto IDENT_ID_ALIGN_ICONS_TO_GRID = 0xE95AE0D2U;
		constexpr auto IDENT_ID_EXPAND_GROUP = 0x8F0D70B1U;
		constexpr auto IDENT_ID_COLLAPSE_GROUP = 0x2E1AA004U;
		constexpr auto IDENT_ID_RUN_WITH_POWERSHELL = 0x7B5A9A39U;

		constexpr auto IDENT_ID_ALL_CONTROL_PANEL_ITEMS = 0x3401622EU;
		constexpr auto IDENT_ID_FOLDERS = 0xBA3B5934U;
		constexpr auto IDENT_ID_FOLDER = 0xFDE2C4A1U;
		constexpr auto IDENT_ID_ADMINISTRATOR = 0xAF9BC4E6U;
		constexpr auto IDENT_ID_PROGRAMS = 0x2A642270U;
		constexpr auto IDENT_ID_SHORTCUT = 0xEE2B7161U;
		constexpr auto IDENT_ID_PROGRAMS_AND_FEATURES = 0xE8F26740U;
		constexpr auto IDENT_ID_ADD_TO_FAVORITES = 0xB5AB4B62U;
		constexpr auto IDENT_ID_REMOVE_FROM_FAVORITES = 0x8F212FD8U;
		constexpr auto IDENT_ID_SIGN_OUT = 0x2828B62DU;
		constexpr auto IDENT_ID_SHUT_DOWN = 0x84A5CC60U;
		constexpr auto IDENT_ID_RESTART = 0x3F29FC8AU;
		constexpr auto IDENT_ID_START_MENU = 0xB0BBFF27U;
		constexpr auto IDENT_ID_APPLICATION = 0x5B0F86F9U;
		constexpr auto IDENT_ID_SYSTEM = 0x1CEEE48AU;
		constexpr auto IDENT_ID_POWER = 0x10289E52U;
		constexpr auto IDENT_ID_NONE = 0x7C9B47F5U;
		constexpr auto IDENT_ID_EMPTY = 0x0F605C34U;
		constexpr auto IDENT_ID_MORE = 0x7C9ABC18U;
		constexpr auto IDENT_ID_RECYCLE_BIN = 0x662726E4U;
		constexpr auto IDENT_ID_SECURITY = 0xEB741DBDU;
		constexpr auto IDENT_ID_THIS_PC = 0xDF6865AFU;
		constexpr auto IDENT_ID_NETWORK = 0x0B5502AFU;
		constexpr auto IDENT_ID_DOCUMENTS = 0xC0E2C157U;
		constexpr auto IDENT_ID_SYSTEM_FOLDER = 0x092370C5U;
		constexpr auto IDENT_ID_DEFAULT = 0x0885548AU;
		constexpr auto IDENT_ID_FILE_IN_USE = 0x9EEF0587U;
		constexpr auto IDENT_ID_FOLDER_IN_USE = 0xD5CBFB83U;
		constexpr auto IDENT_ID_FILE_ACCESS_DENIED = 0x2866F2DEU;
		constexpr auto IDENT_ID_FOLDER_ACCESS_DENIED = 0x214BD55AU;
		constexpr auto IDENT_ID_GENERAL = 0xF006F223U;
		constexpr auto IDENT_ID_PICTURES = 0x5A874314U;
		constexpr auto IDENT_ID_VIDEOS = 0x22C45E2FU;
		constexpr auto IDENT_ID_MUSIC = 0x0FF58E86U;
		constexpr auto IDENT_ID_MEDIA = 0x0FEC88A5U;
		constexpr auto IDENT_ID_FAVORITES = 0x67325138U;
		constexpr auto IDENT_ID_DOWNLOADS = 0xB40E8DF0U;
		constexpr auto IDENT_ID_PERSONAL_FOLDER = 0xA3EB0084U;
		constexpr auto IDENT_ID_CLOSE = 0x0F3B9A5BU;
		constexpr auto IDENT_ID_PERMANENTLY_DELETE = 0xBF4EC426U;

		constexpr auto IDENT_ID_PIN_TO_START_MENU = 0x06EC838FU;
		constexpr auto IDENT_ID_UNPIN_FROM_START_MENU = 0x7692A483U;

		//SEGOE MDL2 ASSETS
		constexpr auto IDENT_SEGOEMDL2 = 0x7EF88207U;

		constexpr auto IDENT_EXT_PNG = 0x7C7833F8U;
		constexpr auto IDENT_EXT_JPG = 0x7C781AB4U;
		constexpr auto IDENT_EXT_JPEG = 0x0B7B7159U;
		constexpr auto IDENT_EXT_BMP = 0x7C77F852U;
		constexpr auto IDENT_EXT_GIF = 0x7C780D09U;
		constexpr auto IDENT_EXT_TTIF = 0x0B80FEAAU;
		constexpr auto IDENT_EXT_WEBP = 0x0B826321U;
		constexpr auto IDENT_EXT_SVG = 0x7C7841C3U;
		constexpr auto IDENT_EXT_ICO = 0x7C7814CEU;
		constexpr auto IDENT_EXT_EXE = 0x7C780675U;
		constexpr auto IDENT_EXT_DLL = 0x7C7800AFU;
		//
		constexpr auto IDENT_EXT_TXT = 0x7C784653U;
		constexpr auto IDENT_EXT_INI = 0x7C781633U;
		constexpr auto IDENT_EXT_INF = 0x7C781630U;
		constexpr auto IDENT_EXT_H = 0x0059709BU;
		constexpr auto IDENT_EXT_C = 0x00597096U;
		constexpr auto IDENT_EXT_CPP = 0x7C77FCF6U;
		constexpr auto IDENT_EXT_CS = 0x0B8783C9U;
		constexpr auto IDENT_EXT_HTML = 0x0B7A6AA8U;
		constexpr auto IDENT_EXT_CSHTML = 0xC801439EU;
		constexpr auto IDENT_EXT_JS = 0x0B8784B0U;
		constexpr auto IDENT_EXT_XML = 0x7C7855E4U;
		constexpr auto IDENT_EXT_RAZOR = 0x7B7281A1U;
		constexpr auto IDENT_EXT_JSON = 0x0B7B7F6DU;
		constexpr auto IDENT_EXT_CSS = 0x7C77FD5CU;
		constexpr auto IDENT_EXT_BAT = 0x7C77F6CAU;
		constexpr auto IDENT_EXT_CMD = 0x7C77FC87U;
		constexpr auto IDENT_EXT_PS1 = 0x7C783467U;
		//
		constexpr auto IDENT_EXT_ZIP = 0x7C785DE6U;
		constexpr auto IDENT_EXT_CAB = 0x7C77FAF9U;
		constexpr auto IDENT_EXT_7Z = 0x0B877E24U;
		constexpr auto IDENT_EXT_RAR = 0x7C783AD8U;
		constexpr auto IDENT_EXT_TAR = 0x7C78435AU;
		constexpr auto IDENT_EXT_ISO = 0x7C7816DEU;
		constexpr auto IDENT_EXT_IMG = 0x7C781610U;
		constexpr auto IDENT_EXT_WIM = 0x7C785120U;
		//
		constexpr auto IDENT_EXT_MKV = 0x7C7826E1U;
		constexpr auto IDENT_EXT_AVI = 0x7C77F533U;
		constexpr auto IDENT_EXT_MP4 = 0x7C782744U;
		constexpr auto IDENT_EXT_WMV = 0x7C7851ADU;
		constexpr auto IDENT_EXT_MPG = 0x7C782777U;
		constexpr auto IDENT_EXT_FLV = 0x7C78093BU;
		constexpr auto IDENT_EXT_WMA = 0x7C785198U;
		constexpr auto IDENT_EXT_MP3 = 0x7C782743U;
		constexpr auto IDENT_EXT_WAV = 0x7C785021U;

		constexpr auto IDENT_IS_BITMAP = 0x57DD23DDU;

		constexpr auto IDENT_ALL = 0x0B885DDEU;
		constexpr auto IDENT_LIST = 0x7C9A1661U;
		constexpr auto IDENT_REFRESH = 0x3E3DB654U;

		constexpr auto IDENT_TOINT = 0x1070C5F3U;
		constexpr auto IDENT_TODOUBLE = 0xDB666E03U;
		constexpr auto IDENT_TOUINT = 0x1E900348U;
		constexpr auto IDENT_TOFLOAT = 0xEF82A41EU;


		constexpr auto IDENT_DECODE = 0xF87EBD89U;
		constexpr auto IDENT_ENCODE = 0xFB76C2B3U;
		constexpr auto IDENT_URL = 0x0B88B3B8U;

		// End constants


	}
}