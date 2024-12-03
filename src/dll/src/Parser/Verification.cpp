#include <pch.h>
#include "Expression\Constants.h"

namespace Nilesoft
{
	namespace Shell
	{
		ExpressionType Parser::make_error(TokenError tokenError, size_t column)
		{
			l->column = column;
			error(tokenError);
			return ExpressionType::None;
		}

		bool Parser::is_number() const
		{
			if(l->is_digit() || (l->tok == '.' && l->is_digit(1)))
				return true;
			return (l->is({ '-', '+' }) && (l->is_digit(1) || (l->peek == '.' && l->is_digit(2))));
		}

		bool Parser::is_color() const
		{
			return (l->tok == '#' && l->is_xdigit(1));
		}

		TokenError Parser::verify_command(const Ident &id, bool signer)
		{
			if(id.length() > 5) return TokenError::IdentifierMaxLength;

			//auto const not_signer = 0;
			auto const assignable = 1;
			auto const auto_signer = 2;

			auto check = [=](uint32_t len, int signe = 0)
			{
				if(id.length() != len)
					return TokenError::PropertyUnexpected;

				if((signe == auto_signer) || (signe == (int)signer))
					return TokenError::None;

				return signer ? TokenError::AssignUnexpected :
					TokenError::AssignExpected;
			};

			switch(id[0])
			{
				case MENU_CMD:
				case MENU_COMMAND:
				{
					switch(id[1])
					{
						case MENU_CMD_LINE:
						case MENU_CMD_PROMPT:
						case MENU_CMD_SHELL:
						case MENU_CMD_EXPLORER:
						case MENU_CMD_POWERSHELL:
						case MENU_CMD_PS:
						case MENU_CMD_PWSH:
							return check(2, assignable);
						case IDENT_ZERO:
							return check(1, auto_signer);
						default:
							return check(0);
					}
				}
				case MENU_ADMIN:
					return check(1, auto_signer);
				case MENU_WAIT:
				case MENU_INVOKE:
				{
					switch(id[1])
					{
						case IDENT_ZERO:
							return check(1, auto_signer);
						default:
							return check(0);
					}
				}
				case MENU_ARG:
				case MENU_ARGUMENT:
				case MENU_WINDOW:
				case MENU_VERB:
					return check(1, assignable);
				case MENU_ARGS:
				case MENU_ARGUMENTS:
					return check(1, auto_signer);
				case MENU_DIR:
				case MENU_DIRECTORY:
					return check(1, auto_signer);
				default:
					return check(0);
			}
		}

		TokenError Parser::verify(NativeMenu const *menu, const Ident &id, bool signer)
		{
			if(id.length() > 5) return TokenError::IdentifierMaxLength;

			auto const assignable = 1;
			auto const not_signer = 0;
			auto const auto_signer = 2;

			auto type = NativeMenuType::Main;
			if(menu != nullptr)
				type = menu->type;

			auto check = [=](uint32_t len, int signe = 0)
			{
				if(id.length() != len)
					return TokenError::PropertyUnexpected;

				if((signe == auto_signer) || (signe == (int)signer))
					return TokenError::None;

				return signer ? TokenError::AssignUnexpected :
					TokenError::AssignExpected;
			};

			// main menu, menu, item, separator
			switch(id[0])
			{
				case MENU_FIND:
				case MENU_WHERE:
				case MENU_CONDITION:
				case MENU_VIS:
				case MENU_VISIBILITY:
				case MENU_POS:
				case MENU_POSITION:
					return check(1, assignable);
			}

			if(type == NativeMenuType::Main)
			{
				switch(id[0])
				{
					case MENU_ICON:
					case MENU_IMAGE:
					{
						switch(id[1])
						{
							case IDENT_DISABLED:
							case IDENT_ENABLED:
								return check(2, not_signer);
							case IDENT_ZERO:
								return check(1, auto_signer);
							default:
								return check(0);
						}
					}
					case MENU_ADMIN:
						return check(1, auto_signer);
				}
			}

			if(type == NativeMenuType::Menu ||
			   type == NativeMenuType::Item ||
			   type == NativeMenuType::Separator)
			{
				// menu, item, separator
				switch(id[0])
				{
					case MENU_SEL:
					case MENU_MODE:
					case MENU_TYPE:
					case MENU_ID:
						return check(1, assignable);
				}
			}

			if(type == NativeMenuType::Menu ||
			   type == NativeMenuType::Item)
			{
				// menu, menuitem
				switch(id[0])
				{
					case MENU_ICON:
					case MENU_IMAGE:
					{
						switch(id[1])
						{
							case IDENT_ENABLED:
							case IDENT_DISABLED:
							case IDENT_INHERIT:
							case IDENT_PARENT:
							case IDENT_CMD:
							case IDENT_NONE:
							case IDENT_NULL:
							case IDENT_NIL:
								return check(2, not_signer);
							case IDENT_SEL:
							case IDENT_SELECT:
								return check(2, assignable);
							case IDENT_ZERO:
								return check(1, auto_signer);
							default:
								return check(0);
						}
					}
					case MENU_SEP:
					case MENU_SEPARATOR:
					case MENU_DIR:
					case MENU_DIRECTORY:
					case MENU_ADMIN:
					case MENU_COLUMN:
					case MENU_COL:
						return check(1, auto_signer);
					case MENU_TEXT:
					case MENU_TITLE:
					case IDENT_MENU:
					case IDENT_MOVE:
					case MENU_PARENT:
					case MENU_SUB:
					case MENU_TIP:
						return check(1, assignable);
				}
			}
			
			if(type == NativeMenuType::Menu)
			{
				switch(id[0])
				{
					case MENU_EXPANDED:
						return check(1, auto_signer);
				}
				return check(0);
			}

			if(type == NativeMenuType::Item) // menuitem
			{
				switch(id[0])
				{
					case MENU_CMDS:
					case MENU_COMMANDS:
						return check(1);
					case MENU_CMD:
					case MENU_COMMAND:
					{
						switch(id[1])
						{
							case MENU_CMD_LINE:
							case MENU_CMD_PROMPT:
							case MENU_CMD_SHELL:
							case MENU_CMD_EXPLORER:
							case MENU_CMD_POWERSHELL:
							case MENU_CMD_PS:
							case MENU_CMD_PWSH:
								return check(2, assignable);
							case IDENT_ZERO:
								//return check(1, assignable);
								return check(1, auto_signer);
							default:
								return check(0);
						}
					}
					case MENU_ARGS:
					case MENU_ARGUMENTS:

					case MENU_WAIT:
					case MENU_INVOKE:
					case MENU_CHECKED:
						return check(1, auto_signer);

					case MENU_ARG:
					case MENU_ARGUMENT:
					case MENU_VERB:
					case MENU_KEYS:
					case MENU_WINDOW:
						return check(1, assignable);

					default:
						return check(0);
				}
			}
			return check(0);
		}


		TokenError Parser::verify(const Ident &id, bool signer)
		{
			auto length = id.length();

			if(length > 5) return TokenError::IdentifierMaxLength;

			auto const assignable = 1;
			auto const not_signer = 0;
			auto const auto_signer = 2;

			auto check = [=](uint32_t len, int signe = 0)
			{
				if(length != len)
					return TokenError::PropertyUnexpected;

				if((signe == auto_signer) || (signe == (int)signer))
					return TokenError::None;

				return signer ? TokenError::AssignUnexpected :
					TokenError::AssignExpected;
			};

			switch(id[0])
			{
				case MENU_ICON:
				case MENU_IMAGE:
				{
					switch(id[1])
					{
						case IDENT_ENABLED:
						case IDENT_DISABLED:
						case IDENT_INHERIT:
						case IDENT_NONE:
						case IDENT_NULL:
						case IDENT_NIL:
							return check(2, not_signer);
						case IDENT_ZERO:
							return check(1, auto_signer);
						default:
							return check(0);
					}
				}
				case MENU_SEP:
				case MENU_SEPARATOR:
					return check(1, auto_signer);
				
				case MENU_VIS:
				case MENU_VISIBILITY:
				case MENU_POS:
				case MENU_POSITION:
				case MENU_SEL:
				case MENU_MODE:
				case MENU_CONDITION:
				case MENU_WHERE:
				case MENU_FIND:
				case MENU_TEXT:
				case MENU_TITLE:
				case MENU_KEYS:
				case MENU_TIP:

				case IDENT_MOVE:
				case MENU_PARENT:
				case IDENT_MENU:
				case MENU_SUB:
					return check(1, assignable);
				default:
					return check(0);
			}
		}

		ExpressionType Parser::verify_ident(const Ident &id, const size_t argc, bool hasdot)
		{
			if(id[0] == 0) return ExpressionType::None;

			const auto length = id.length();
			/*auto result = [&](uint32_t index, bool condition = false)->ExpressionType
			{
				if(condition && (length == index))
					return ExpressionType::Identifier;
				return this->make_error(argc == 0 ? TokenError::IdentifierUnexpected : TokenError::IdentifierArguments, id.Col(index - 1));
			};*/

			auto result2 = [&](bool condition, std::initializer_list<uint32_t> ids)->ExpressionType
			{
				for(auto i : ids)
				{
					if(i == id.back())
					{
						if(condition)
							return ExpressionType::Identifier;
						break;
					}
				}
				return make_error(argc == 0 ? TokenError::IdentifierUnexpected : TokenError::IdentifierArguments, id.Col(0));
			};

			auto check = [&](bool condition, uint32_t index = 0)->ExpressionType
			{
				if(condition && (length == index + 1)) 
					return ExpressionType::Identifier;
				return make_error(TokenError::IdentifierArguments, id.Col(index));
			};

			auto error_at = [&](uint32_t index = 0)->ExpressionType
			{
				return make_error(TokenError::IdentifierUnexpected, id.Col(index));
			};

			if(hasdot)
			{
				switch(id[0])
				{
					case IDENT_NULL:
					case IDENT_EMPTY:
					case IDENT_LEN:
					case IDENT_LENGTH:
					case IDENT_UPPER:
					case IDENT_LOWER:
					case IDENT_CAPITALIZE:
						return check(argc == 0);
					case IDENT_TRIM:
					case IDENT_TRIMSTART:
					case IDENT_TRIMEND:
						return check(argc <= 1);
					case IDENT_SET:
					case IDENT_GET:
					case IDENT_CHAR:
					case IDENT_AT:
					case IDENT_LEFT:
					case IDENT_RIGHT:
						return check(argc == 1);
					case IDENT_SUB:
					case IDENT_EQ:
					case IDENT_EQUALS:
					case IDENT_NOT:
					case IDENT_START:
					case IDENT_END:
					case IDENT_FIND:
					case IDENT_FINDLAST:
					case IDENT_CONTAINS:
					case IDENT_REMOVE:
					case IDENT_PADLEFT:
					case IDENT_PADRIGHT:
					case IDENT_PADDING:
						return check(argc == 1 || argc == 2);
					case IDENT_REPLACE:
						return check(argc == 2 || argc == 3);
					case IDENT_FORMAT:
						return check(argc > 0);
					default:
						return error_at(0);
				}
			//	return error_at(0);
			}

			switch(id[0])
			{
				case IDENT_EVAL:
					return check(argc == 1, 0);
				case IDENT_SEL:
				{
					switch(id[1])
					{
						case IDENT_GET:
							return check(argc == 1 || argc == 2, 1);
						case IDENT_MODE:
							return check(argc == 0, 1);
						case IDENT_TYPE:
							return check(argc >= 0, 1);
						case IDENT_PATH:
						case IDENT_FULL:
						case IDENT_DIR:
						case IDENT_DIRECTORY:
						{
							switch(id[2])
							{
								case IDENT_LEN:
								case IDENT_LENGTH:
								case IDENT_TITLE:
								case IDENT_NAME:
								case IDENT_QUOTE:
									return check(argc <= 1, 2);
								case IDENT_ZERO:
									return check(argc <= 2, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_SHORT:
						case IDENT_RAW:
						case IDENT_NAME:
						case IDENT_TITLE:
						case IDENT_ITEM:
						{
							switch(id[2])
							{
								case IDENT_LEN:
								case IDENT_LENGTH:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc <= 1, 1);
								default: 
									return error_at(2);
							}
						}
						case IDENT_BACK:
						case IDENT_COUNT:
						case IDENT_LEN:
						case IDENT_LENGTH:
						case IDENT_ROOT:
						case IDENT_READONLY:
						case IDENT_HIDDEN:
							return check(argc == 0, 1);
						case IDENT_WORKDIR:
						case IDENT_CURDIR:
						case IDENT_CURRENTDIRECTORY:
							return check(argc <= 1, 1);
						case IDENT_TOFILE:
							return check(argc <= 3, 1);
						case IDENT_PARENT:
						case IDENT_LOCATION:
						{
							switch(id[2])
							{
								case IDENT_QUOTE:
								case IDENT_RAW:
								case IDENT_NAME:
								case IDENT_LEN:
								case IDENT_LENGTH:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc <= 1, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_INDEX:
						case IDENT_I:
							return check(argc >= 1 || argc <= 2, 1);
						case IDENT_FILE:
						{
							switch(id[2])
							{
								case IDENT_EXT:
								case IDENT_TITLE:
								case IDENT_NAME:
								{
									switch(id[3])
									{
										case IDENT_LEN:
										case IDENT_LENGTH:
											return check(argc == 0, 3);
										case IDENT_ZERO:
											return check(argc == 0, 2);
										default: 
											return error_at(3);
									}
								}
								case IDENT_LEN:
								case IDENT_LENGTH:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc <= 1, 1);
								default: 
									return error_at(2);
							}
						}
						case IDENT_PATHS:
						case IDENT_DIRS:
						case IDENT_DIRECTORIES:
						{
							switch(id[2])
							{
								case IDENT_COUNT:
									return check(argc == 0, 2);
								case IDENT_TITLE:
								case IDENT_NAME:
									return check(argc <= 2, 2);
								case IDENT_QUOTE:
									return check(argc <= 1, 2);
								case IDENT_ZERO:
									return check(argc <= 2, 1);
								default: 
									return error_at(2);
							}
						}
						case IDENT_FILES:
						{
							switch(id[2])
							{
								case IDENT_COUNT:
									return check(argc == 0, 2);
								case IDENT_EXT:
								case IDENT_TITLE:
								case IDENT_NAME:
									return check(argc <= 2, 2);
								case IDENT_QUOTE:
									return check(argc <= 1, 2);
								case IDENT_ZERO:
									return check(argc <= 2, 1);
								default: 
									return error_at(2);
							}
						}
						case IDENT_TYPES:
						case IDENT_SHORTS:
						case IDENT_TITLES:
						case IDENT_NAMES:
						case IDENT_EXTS:
						case IDENT_ROOTS:
						case IDENT_DRIVERS:
						case IDENT_NAMESPACES:
						{
							switch(id[2])
							{
								case IDENT_COUNT:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc <= 2, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_QUOTE:
							return check(argc <= 1, 1);
						case IDENT_LNK:
						case IDENT_LNKTARGET:
						{
							switch(id[2])
							{
								case IDENT_DIR:
								case IDENT_LOCATION:
								case IDENT_LNK_TARGET:
								case IDENT_ICON:
								case IDENT_TYPE:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc == 0, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_META:
							return check(argc <= 2, 1);

						case IDENT_ZERO:
							return check(argc <= 3);
						default: 
							return error_at(1);
					}
				}
				case IDENT_SYS:
				case IDENT_SYSTEM:
				{
					switch(id[1])
					{
						case IDENT_TYPE:
						case IDENT_IS64:
						case IDENT_IS11:
						case IDENT_IS10:
						case IDENT_IS81:
						case IDENT_IS8:
						case IDENT_IS7:
						case IDENT_IS7ORGREATER:
						case IDENT_IS8ORGREATER:
						case IDENT_IS81ORGREATER:
						case IDENT_IS10ORGREATER:
						case IDENT_S11ORGREATER:
						case IDENT_IS7OREARLIER:
						case IDENT_IS8OREARLIER:
						case IDENT_IS81OREARLIER:
						case IDENT_IS10OREARLIER:
						case IDENT_IS11OREARLIER:
						case IDENT_IS_PRIMARY_MONITOR:
						case IDENT_DARK:
						case IDENT_NAME:
						case IDENT_ROOT:
						case IDENT_PATH:
						case IDENT_DIR:
						case IDENT_DIRECTORY:
						case IDENT_BIN:
						case IDENT_BIN32:
						case IDENT_BIN64:
						case IDENT_WOW:
						case IDENT_PROG:
						case IDENT_PROG32:
						case IDENT_TEMP:
						case IDENT_EXTENDED:
						case IDENT_PROGRAMDATA:
						case IDENT_APPDATA:
						case IDENT_USERS:
						case IDENT_TEMPLATES:
						case IDENT_LANG:
						case IDENT_LOC:
						{
							switch(id[2])
							{
								case IDENT_ID:
								case IDENT_NAME:
								case IDENT_COUNTRY:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc == 0, 1);
								default:
									return error_at(2);
							}
							return check(argc == 0, 1);
						}
						case IDENT_ISOREARLIER:
						case IDENT_ISORGREATER:
							return check(argc < 3, 1);
						case IDENT_VAR:
						case IDENT_EXPAND:
							return check(argc == 1, 1);
						case IDENT_VER:
						case IDENT_VERSION:
						{
							switch(id[2])
							{
								case IDENT_MAJOR:
								case IDENT_MINOR:
								case IDENT_BUILD:
								case IDENT_TYPE:
								case IDENT_NAME:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc == 0, 1);
								default: 
									return error_at(2);
							}
						}
						case IDENT_DATETIME:
						{
							switch(id[2])
							{
								case IDENT_TIME_MS:
								case IDENT_TIME_MILLISECONDS:
								case IDENT_TIME_SECOND:
								case IDENT_TIME_MIN:
								case IDENT_TIME_MINUTE:
								case IDENT_TIME_HOUR:
								case IDENT_TIME_PM:
								case IDENT_DATE_DAY:
								case IDENT_DATE_DAYOFWEEK:
								case IDENT_DATE_MONTH:
								case IDENT_DATE_Y:
								case IDENT_DATE_YEAR:
								case IDENT_DATE_YY:
								case IDENT_TIME:
								case IDENT_DATE:
								case IDENT_SHORT:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc <= 1, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_ZERO:
							return check(argc == 0);
						default:
							return error_at(1);
					}
				}
				case IDENT_APP:
				{
					switch(id[1])
					{
						case IDENT_ABOUT:
						case IDENT_CFG:
						case IDENT_DIR:
						case IDENT_DIRECTORY:
						case IDENT_DLL:
						case IDENT_EXE:
						case IDENT_IS64:
						case IDENT_NAME:
						case IDENT_RELOAD:
						case IDENT_UNLOAD:
						case IDENT_VAR:
						case IDENT_USED:
						case IDENT_PROCESS:
							return check(argc == 0, 1);
						case IDENT_VER:
						case IDENT_VERSION:
							switch(id[2])
							{
								case IDENT_MAJOR:
								case IDENT_MINOR:
								case IDENT_BUILD:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc == 0, 1);
								default:
									return error_at(1);
							}
						case IDENT_ZERO:
							return error_at(0);
						default:
							return error_at(1);
					}
				}
				case IDENT_USER:
				{
					switch(id[1])
					{
						case IDENT_HOME:
						case IDENT_APPDATA:
						case IDENT_CONTACTS:
						case IDENT_DESKTOP:
						case IDENT_DIR:
						case IDENT_DIRECTORY:
						case IDENT_DOCUMENTS:
						case IDENT_DOCUMENTSLIBRARY:
						case IDENT_DOWNLOADS:
						case IDENT_FAVORITES:
						case IDENT_LIBRARIES:
						case IDENT_LOCALAPPDATA:
						case IDENT_MUSIC:
						case IDENT_NAME:
						case IDENT_PERSONAL:
						case IDENT_PICTURES:
						case IDENT_PROFILE:
						case IDENT_QUICKLAUNCH:
						case IDENT_SENDTO:
						case IDENT_STARTMENU:
						case IDENT_TEMP:
						case IDENT_TEMPLATES:
						case IDENT_VIDEOS:
							return check(argc == 0, 1);
						case IDENT_EXPAND:
							return check(argc == 1, 1);
						case IDENT_ZERO:
							return check(argc == 0);
						default:
							return error_at(1);
					}
				}
				case IDENT_IO:
				{
					switch(id[1])
					{
						case IDENT_DELETE:
							return check(argc == 1, 1);
						case IDENT_COPY:
							return check(argc == 2 || argc == 3, 1);
						case IDENT_MOVE:
						case IDENT_RENAME:
							return check(argc == 2, 1);
						case IDENT_FILE:
						{
							switch(id[2])
							{
								case IDENT_SIZE:
									return check(argc == 1, 2);
								case IDENT_EXISTS:
									return check(argc >= 1, 2);
								case IDENT_MAKE:
								case IDENT_CREATE:
									return check(argc >= 1 && argc <= 3, 2);
								case IDENT_WRITE:
								case IDENT_APPEND:
									return check(argc == 2, 2);
								case IDENT_READ:
									return check(argc == 1 || argc == 2, 2);
								case IDENT_ZERO:
									return error_at(1);
								default:
									return error_at(2);
							}
						}
						case IDENT_DIR:
						case IDENT_DIRECTORY:
						{
							switch(id[2])
							{
								case IDENT_MAKE:
								case IDENT_CREATE:
									return check(argc >= 0, 2);
								case IDENT_EMPTY:
								case IDENT_EXISTS:
									return check(argc >= 1, 2);
								case IDENT_ZERO:
									return error_at(1);
								default:
									return error_at(2);
							}
						}
						case IDENT_ATTRIBUTES:
							return check(argc == 1 || argc == 2, 1);
						case IDENT_IO_ATTRIB:
						case IDENT_ATTRIBUTE:
						{
							switch(id[2])
							{
								case IDENT_IO_ATTRIBUTE_INVALID:
								case IDENT_IO_ATTRIBUTE_NORMAL:
								case IDENT_IO_ATTRIBUTE_ARCHIVE:
								case IDENT_IO_ATTRIBUTE_COMPRESSED:
								case IDENT_IO_ATTRIBUTE_DEVICE:
								case IDENT_IO_ATTRIBUTE_DIRECTORY:
								case IDENT_IO_ATTRIBUTE_ENCRYPTED:
								case IDENT_IO_ATTRIBUTE_HIDDEN:
								case IDENT_IO_ATTRIBUTE_READONLY:
								case IDENT_IO_ATTRIBUTE_SPARSEFILE:
								case IDENT_IO_ATTRIBUTE_SYSTEM:
								case IDENT_IO_ATTRIBUTE_TEMPORARY:
								case IDENT_IO_ATTRIBUTE_VIRTUAL:
								case IDENT_IO_ATTRIBUTE_OFFLINE:
									return check(argc == 0 || argc == 1, 2);
								case IDENT_ZERO:
									return error_at(1);
								default: 
									return error_at(2);
							}
						}
						case IDENT_IO_DT:
						case IDENT_IO_DATETIME:
						{
							switch(id[2])
							{
								case IDENT_IO_DATETIME_CREATED:
								case IDENT_IO_DATETIME_MODIFIED:
								case IDENT_IO_DATETIME_ACCESSED:
									return check(argc <= 7, 2);
								case IDENT_ZERO:
									return check(argc <= 4, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_META:
							return check(argc >= 1 && argc <= 2, 1);
						case IDENT_ZERO:
							return error_at(0);
						default: 
							return error_at(1);
					}
				}
				case IDENT_PATH:
				{
					switch(id[1])
					{
						case IDENT_ROOT:
						case IDENT_NAME:
						case IDENT_TITLE:
						case IDENT_FULL:
						case IDENT_SHORT:
						case IDENT_PACKAGE:
						case IDENT_ARGS:
						case IDENT_REMOVE_ARGS:
						case IDENT_QUOTE_SPACES:
						case IDENT_REMOVE_EXTENSION:
							return check(argc == 1, 1);
						case IDENT_COMPACT:
							return check(argc == 1 || argc == 2, 1);
						case IDENT_RENAME_EXTENSION:
						case IDENT_ADD_EXTENSION:
							return check(argc == 2, 1);
						case IDENT_EXISTS:
						case IDENT_EMPTY:
							return check(argc >= 1, 1);
						case IDENT_CURDIR:
						case IDENT_CURRENTDIRECTORY:
							return check(argc <= 1, 1);
						case IDENT_SEP:
						case IDENT_SEPARATOR:
							return check(argc == 1 || argc == 2, 1);
						case IDENT_PARENT:
						case IDENT_LOCATION:
						{
							switch(id[2])
							{
								case IDENT_NAME:
									return check(argc == 1, 2);
								case IDENT_ZERO:
									return check(argc == 1, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_LNK:
						case IDENT_LNKTARGET:
						{
							switch(id[2])
							{
								case IDENT_DIR:
								case IDENT_LOCATION:
								case IDENT_LNK_TARGET:
								case IDENT_TYPE:
								case IDENT_ICON:
									return check(argc == 1, 2);
								case IDENT_CREATE:
									return check(argc >= 2 && argc <= 8, 2);
								case IDENT_ZERO:
									return check(argc == 1, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_TYPE:
							return check(argc == 1 || argc == 3, 1);
						case IDENT_FILE:
						{
							switch(id[2])
							{
								case IDENT_NAME:
								case IDENT_TITLE:
								case IDENT_EXT:
									return check(argc == 1, 2);
								case IDENT_BOX:
									return check(argc <= 4, 2);
								case IDENT_ZERO:
									return check(argc == 1, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_DIR:
						case IDENT_DIRECTORY:
						{
							switch(id[2])
							{
								case IDENT_NAME:
								case IDENT_TITLE:
									return check(argc == 2, 2);
								case IDENT_MAKE:
									return check(argc == 0, 2);
								case IDENT_BOX:
									return check(argc <= 4, 2);
								case IDENT_FILES:
									return check(argc <= 2, 2);
								case IDENT_ZERO:
									return check(argc == 1, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_FILES:
						{
							switch(id[2])
							{
								case IDENT_NAME:
								case IDENT_TITLE:
									return check(argc == 2, 2);
								case IDENT_COUNT:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc >= 1 && argc <= 5, 1);
								default:
									return error_at(2);
							}
						}
						case IDENT_JOIN:
						case IDENT_COMBINE:
							return check(argc >= 2, 1);
						case IDENT_ISABSOLUTE:
						case IDENT_ISRELATIVE:
						case IDENT_ISFILE:
						case IDENT_ISDIR:
						case IDENT_ISDIRECTORY:
						case IDENT_ISDRIVE:
						case IDENT_ISROOT:
						case IDENT_ISNAMESPACE:
						case IDENT_ISCLSID:
						case IDENT_ISEXE:
						case IDENT_REMOVEEXTENSION:
						case IDENT_GETKNOWNFOLDER:
						case IDENT_ABSOLUTE:
						case IDENT_RELATIVE:
						case IDENT_WSL:
							return check(argc == 1, 1);
						case IDENT_REFRESH:
							return check(argc <= 1, 1);
						case IDENT_ZERO:
							return error_at(0);
						default:
							return error_at(1);
					}
				}
				case IDENT_STR:
				{
					switch(id[1])
					{
						case IDENT_NULL:
						case IDENT_EMPTY:
						case IDENT_LEN:
						case IDENT_LENGTH:
						case IDENT_UPPER:
						case IDENT_LOWER:
						case IDENT_CAPITALIZE:
						case IDENT_HASH:
							return check(argc == 1, 1);

						case IDENT_DECODE:
						case IDENT_ENCODE:
						{
							switch(id[2])
							{
								case IDENT_URL:
									return check(argc == 1, 2);
								case IDENT_ZERO:
									break;
								default:
									return error_at(2);
							}
							return error_at(1);
						}
						case IDENT_TRIM:
						case IDENT_TRIMSTART:
						case IDENT_TRIMEND:
							return check(argc == 1 || argc == 2, 1);
						case IDENT_RES:
							return check(argc == 1 || argc == 2, 1);
						case IDENT_SET:
						case IDENT_GET:
						case IDENT_CHAR:
						case IDENT_AT:
						case IDENT_LEFT:
						case IDENT_RIGHT:
							return check(argc == 2, 1);
						case IDENT_SUB:
						case IDENT_EQ:
						case IDENT_EQUALS:
						case IDENT_NOT:
						case IDENT_START:
						case IDENT_END:
						case IDENT_FIND:
						case IDENT_FINDLAST:
						case IDENT_CONTAINS:
						case IDENT_REMOVE:
						case IDENT_PADLEFT:
						case IDENT_PADRIGHT:
						case IDENT_PADDING:
						case IDENT_TAG:
							return check(argc == 2 || argc == 3, 1);
						case IDENT_REPLACE:
							return check(argc == 3 || argc == 4, 1);
						case IDENT_FORMAT:
							return check(argc > 0, 1);
						case IDENT_GUID:
							return check(argc <= 1, 1);
						case IDENT_JOIN:
							return check(argc > 1, 1);
						case IDENT_SPLIT:
							return check(argc == 2, 1);
						case IDENT_ZERO:
							return error_at(0);
						default:
							return error_at(1);
					}
				}
				case IDENT_MSG:
				{
					if(length == 1)
						return check(argc >= 1 && argc <= 3);
					else if(length == 2)
					{
						if(id[1] == IDENT_BEEP)
							return check(argc <= 1, 1);

						// CONST variables
						for(auto &t : MSG_FLAGS)
						{
							if(id[1] == std::get<0>(t))
								return check(argc == 0, 1);
						}
						return error_at(1);
					}
					return error_at(0);
				}
				case IDENT_REG:
				{
					switch(id[1])
					{
						// CONST variables
						case IDENT_REG_NONE:
						case IDENT_REG_SZ:
						case IDENT_REG_EXPAND:
						case IDENT_REG_EXPAND_SZ:
						case IDENT_REG_MULTI_SZ:
						case IDENT_REG_MULTI:
						case IDENT_REG_DWORD:
						case IDENT_REG_QWORD:
						case IDENT_REG_BINARY:
							return check(argc == 0, 1);
						case IDENT_GET:
						case IDENT_EXISTS:
							return check(argc >= 1 && argc <= 3 , 1);
						case IDENT_DELETE:
							return check(argc == 1 || argc == 2, 1);
						case IDENT_SET:
							return check(argc >= 1 && argc <= 4, 1); 
						case IDENT_KEYS:
						case IDENT_VALUES:
							return check(argc == 1, 1);
						case IDENT_ZERO:
							return check(argc >= 1 && argc <= 3);
						default:
							return error_at(1);
					}
				}
				case IDENT_INI:
				{
					switch(id[1])
					{
						case IDENT_GET:
						case IDENT_EXISTS:
						case IDENT_ZERO:
							return check(argc == 3, 1);
						case IDENT_SET:
							return check(argc == 4, 1);
						default:
							return error_at(1);
					}
				}
				case IDENT_CLIPBOARD:
				{
					switch(id[1])
					{
						case IDENT_EMPTY:
						case IDENT_IS_EMPTY:
						case IDENT_IS_BITMAP:
						case IDENT_LEN:
						case IDENT_LENGTH:
						case IDENT_GET:
							return check(argc == 0, 1);
						case IDENT_SET:
							return check(argc == 1, 1);
							
						case IDENT_ZERO:
							return check(argc <= 1, 0);
						default:
							return error_at(1);
					}
				}
				case IDENT_INPUT:
				{
					switch(id[1])
					{
						case IDENT_INPUT_RESULT:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return check(argc <= 3, 0);
						default:
							return error_at(1);
					}
				}
				case IDENT_PACKAGE:
				case IDENT_APPX:
				case IDENT_UWP:
				{
					switch(id[1])
					{
						// CONST variables
						case IDENT_ID:
						case IDENT_PATH:
						case IDENT_NAME:
						case IDENT_TITLE:
						case IDENT_FAMILY:
						case IDENT_RUN:
						case IDENT_LAUNCH:
						case IDENT_SHELL:
						case IDENT_VERSION:
						case IDENT_VER:
						case IDENT_EXISTS:
							return check(argc == 1, 1);
						case IDENT_LIST:
							return check(argc <= 1, 1);
						case IDENT_ZERO:
							return check(argc == 1);
						default:
							return error_at(1);
					}
				}
				case IDENT_IMAGE:
				case IDENT_ICON:
				{
					if(id[0] == IDENT_ICON && id[1]==IDENT_BOX)
						return check(argc <= 2, 1);

					if(id[0] == IDENT_ICON && id[1] == IDENT_CHANGE)
						return check(argc > 0 && argc <= 3, 1);

					// icon.copy
					// icon.copy(<color1<, color2<, size>>>)
					switch(id[1])
					{
						case IDENT_AUTO:
						case IDENT_DEFAULT:
							return check(argc >= 0 && argc <= 3, 1); // icon.segoe(code, color, size)
						case IDENT_GLYPH:
							return check(argc >= 1 && argc <= 4, 1); // icon.glyph(code, color, size, font name)
						case IDENT_ICON_FLUENT:
						case IDENT_ICON_MDL:
						case IDENT_ICONS_SEGOE:
							return check(argc >= 1 && argc <= 3, 1); // icon.SEGOE(code, color, size)
						case IDENT_SVG:
						case IDENT_SVGF:
							if(id.length() == 2)
								return check(argc == 1, 1);
							return check(argc <= 1, id.length() - 1);
						case IDENT_RES:		// path, index | id
							return check(argc >= 1 && argc <= 2, 1);
						case IDENT_COLOR1:
						case IDENT_COLOR2:
						case IDENT_COLOR3:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return check(argc >= 1 && argc <= 3); //icon(code, color, size)
						default:
							if(id.length() == 2)
								return check(argc <= 2, id.length() - 1);
							if(auto uid = Initializer::get_muid(id[1]); uid)
								return check(argc <= 3, 1);
							return error_at(1);
					}
				}
				case IDENT_IMG:
				{
					if(id.length() != 2)
						return error_at(1);

					return check(argc <= 2, id.length() - 1);

					/*auto idui = Initializer::get_idmui(id[1]);
					if(idui)
						return check(argc <= 3, 1);
					return error_at(1);*/
				}
				case IDENT_SVG:
				{
					auto l = (int)id.length();
					switch(id[1])
					{
						case IDENT_RAW:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return error_at(0);
						default:
							return check(argc >= 0, --l);
					}
					return error_at(0);
				}
				case IDENT_ID:
				{
					if(length == 1)
						return error_at(0);
					
					if(length == 2 && argc > 0)
						return check(argc == 0, 1);

					if(length > 3)
						return error_at(2);
					
					if(length == 3)
					{
						if(!id.equals({ IDENT_TITLE, IDENT_NAME, IDENT_STR, IDENT_ICON }))
							return error_at(2);
						else if(id.equals(IDENT_ICON) && argc > 4)
							return check(false, 2);
						return ExpressionType::Identifier;
					}

					if(auto uid = Initializer::get_muid(id[1]); uid)
						return check(argc < 3, length - 1);
					
					return error_at(1);
				}
				case IDENT_TITLE:
				{
					if(length == 1)
						return error_at(0);

					if(length == 2 && argc > 0)
						return check(argc == 0, 1);

					if(length > 2)
						return error_at(2);

					if(auto uid = Initializer::get_muid(id[1]); uid)
						return check(argc == 0, length - 1);

					return error_at(1);
				}
				case IDENT_TIP:
				{
					switch(id[1])
					{
						case IDENT_DEFAULT:
						case IDENT_PRIMARY:
						case IDENT_INFO:
						case IDENT_SUCCESS:
						case IDENT_WARNING:
						case IDENT_DANGER:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return check(0);
						default:
							return error_at(1);
					}
				}
				case IDENT_CMD:
				case IDENT_LAUNCH:
				case IDENT_RUN:
				case IDENT_EXEC:
				case IDENT_SHELL:
				{
					if(length == 1)
					{
						if(id[0] == IDENT_CMD)
							return check(argc <= 3);
						return check(argc >= 1 && argc <= 3);
					}
					else if(length == 2)
					{
						if(id[1] == IDENT_CMD_INVOKED)
							return check(argc == 0, 1);

						return result2(argc == 0, {
							IDENT_HIDDEN,
							IDENT_SHOW,
							IDENT_VISIBLE,
							IDENT_NORMAL,
							IDENT_MAXIMIZED,
							IDENT_MINIMIZED });
						//return make_error(1);
					}
					return error_at(0);
				}
				case IDENT_THIS:
				{
					switch(id[1])
					{
						case IDENT_ID:
							return check(argc >= 0, 1);
						case IDENT_TITLE:
						case IDENT_NAME:
						{
							switch(id[2])
							{
							case IDENT_LEN:
							case IDENT_LENGTH:
								return check(argc == 0, 2);
							case IDENT_ZERO:
								return check(argc >= 0, 1);
							default:
								return error_at(2);
							}
						}
						case IDENT_PARENT:
						{
							switch(id[2])
							{
								case IDENT_ID:
								case IDENT_NAME:
								case IDENT_TITLE:
									return check(argc >= 0, 2);
								case IDENT_ZERO:
									return check(argc >= 0, 1);
							}
							return error_at(2);
						}
						case MENU_VERB:
						case IDENT_DISABLED:
						case IDENT_TYPE:
						case IDENT_CHECKED:
						case IDENT_POS:
						case IDENT_COUNT:
						case IDENT_SYS:
						case IDENT_LEVEL:
						case IDENT_ISUWP:
						case IDENT_CLSID:
							return check(argc <= 1, 1);
						case IDENT_ZERO:
							return check(argc == 0, 1);
						default:
							return error_at(1);
					}
				}
				case IDENT_TYPE:
				{
					if(length == 2)
					{
						return result2(argc == 0, {
							IDENT_TYPE_FILE,
							IDENT_TYPE_DIR,
							IDENT_TYPE_DIRECTORY,
							IDENT_TYPE_DRIVE,
							IDENT_TYPE_NAMESPACE,
							IDENT_TYPE_FIXED,
							IDENT_TYPE_DVD,
							IDENT_TYPE_USB,
							IDENT_TYPE_VHD,
							IDENT_TYPE_REMOVABLE,
							IDENT_TYPE_REMOTE,
							IDENT_TYPE_DESKTOP,
							IDENT_TYPE_RECYCLEBIN,
							IDENT_TYPE_COMPUTER,
							IDENT_TYPE_TASKBAR,
							IDENT_TYPE_BACK,
							IDENT_TYPE_UNKNOWN, 
						});
					}
					else if((id[1] == IDENT_TYPE_BACK)  && (length == 3))
					{
						return result2(argc == 0, {
							IDENT_TYPE_DIR,
							IDENT_TYPE_DIRECTORY,
							IDENT_TYPE_DRIVE,
							IDENT_TYPE_NAMESPACE,
							IDENT_TYPE_FIXED,
							IDENT_TYPE_DVD,
							IDENT_TYPE_USB,
							IDENT_TYPE_VHD,
							IDENT_TYPE_REMOVABLE,
							IDENT_TYPE_REMOTE,
							IDENT_TYPE_RECYCLEBIN,
							IDENT_TYPE_COMPUTER});
					}
					return check(argc >= 0, 0);
					//return error_at(0);
				}
				case IDENT_MODE:
				{
					switch(id[1])
					{
						case IDENT_MODE_NONE:
						case IDENT_MODE_SINGLE:
						case IDENT_MODE_MULTIPLE:
						case IDENT_MODE_MULTI:
						case IDENT_MODE_MULTI_UNIQUE:
						case IDENT_MODE_MULTI_SINGLE:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return check(argc == 0);
						default:
							return error_at(1);
					}
				}
				case MENU_VIS:
				case MENU_VISIBILITY:
				{
					switch(id[1])
					{
						case IDENT_NORMAL:
						case IDENT_VIS_ENABLE:
						case IDENT_VIS_VISIBLE:
						case IDENT_VIS_DISABLE:
						case IDENT_VIS_HIDDEN:
						case IDENT_VIS_REMOVE:
						case IDENT_VIS_STATIC:
						case IDENT_VIS_LABEL:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return check(argc == 0);
						default:
							return error_at(1);
					}
				}
				case IDENT_SEP:
				case IDENT_SEPARATOR:
				{
					switch(id[1])
					{
						case IDENT_NONE:
						case IDENT_TOP:
						case IDENT_BEFORE:
						case IDENT_BOTTOM:
						case IDENT_AFTER:
						case IDENT_BOTH:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return check(argc == 0);
						default:
							return error_at(1);
					}
				}
				case IDENT_POS:
				{
					switch(id[1])
					{
						case IDENT_TOP:
						case IDENT_BOTTOM:
						case IDENT_MIDDLE:
						case IDENT_AUTO:
							return check(argc == 0, 1);
						case IDENT_ZERO:
							return error_at(0);
						default:
							return error_at(1);
					}
				}
				case IDENT_KEY:
				case IDENT_KEYS:
				{
					if(length == 1)
					{
						return check(argc >= 0 && argc <= 3);
					}
					else if(length == 2)
					{
						if(id[1] == IDENT_COUNT)
							return check(argc == 0, 1);
						else if(id[1] == IDENT_KEY_SEND)
							return check(argc >= 1, 1);

						for(auto &t : KeyTable)
						{
							if(id[1] == std::get<0>(t))
							{
								return check(argc == 0, 1);
							}
						}
						return error_at(1);
					}
					return error_at(0);
				}
				case IDENT_COLOR:
				{
					if(length == 1)
						return check((argc == 1) || (argc == 3) || (argc == 4), 0);
					else if(length == 2)
					{
						if(id[1] == IDENT_BOX)
							return check(argc <= 2, 1);
						else if(id[1] == IDENT_COLOR_RGB)
							return check(argc == 3, 1);
						else if(id[1] == IDENT_COLOR_RGBA)
							return check(argc == 2 ||argc == 4, 1);
						else if(id[1] == IDENT_COLOR_RANDOM)
							return check(argc <= 2, 1);
						else if(id[1] == IDENT_COLOR_INVERT)
							return check(argc == 1, 1);
						else if(id[1] == IDENT_COLOR_LIGHT || id[1] == IDENT_COLOR_DARK ||
								id[1] == IDENT_COLOR_LIGHTEN || id[1] == IDENT_COLOR_DARKEN ||
								id[1] == IDENT_COLOR_ADJUST || 
								id[1] == IDENT_OPACITY)
							return check(argc == 1 || argc == 2, 1);

						for(auto &t : ColorTable)
						{
							if(id[1] == std::get<0>(t))
								return check(argc == 0, 1);
						}

					//	return error_at(1);
					}
					break;
				}
				case IDENT_FONT:
				{
					switch(id[1])
					{
						case IDENT_ICON:
						case IDENT_FONT_SEGOE_FLUENT_ICONS:
						case IDENT_FONT_SEGOE_MDL2:
						case IDENT_FONT_SEGOE_UI_SYMBOL:
						case IDENT_FONT_SEGOE_UI:
						case IDENT_FONT_SEGOE_UI_EMOJI:
						case IDENT_FONT_SEGOE_UI_HISTORIC:
						case IDENT_FONT_WEBDINGS:
						case IDENT_FONT_WINGDINGS:
						case IDENT_FONT_WINGDINGS2:
						case IDENT_FONT_WINGDINGS3:
						case IDENT_SIZE:
						case IDENT_SCALE:
							return check(argc == 0, 1);
						case IDENT_FONT_SYSTEM:
						case IDENT_FONT_LOADED:
						case IDENT_FONT_EXISTS:
							return check(argc == 1, 1);
					}
					break;
				}
				case IDENT_VIEW:
				{
					switch(id[1])
					{
					case IDENT_AUTO:
					case IDENT_VIEW_COMPACT:
					case IDENT_VIEW_SMALL:
					case IDENT_VIEW_MEDIUM:
					case IDENT_VIEW_LARGE:
					case IDENT_VIEW_WIDE:
						return check(argc == 0, 1);
					}
					return check(argc == 0);
				}
				case IDENT_THEME:
				{
					switch(id[1])
					{
						case IDENT_AUTO:
						case IDENT_THEME_SYSTEM:
						case IDENT_THEME_CLASSIC:
						case IDENT_THEME_LIGHT:
						case IDENT_THEME_DARK:
						case IDENT_THEME_HIGHCONTRAST:
						case IDENT_THEME_BLACK:
						case IDENT_THEME_WHITE:
						case IDENT_THEME_MODERN:
						case IDENT_THEME_CUSTOM:
						case IDENT_ISDARK:
						case IDENT_ISLIGHT:
						case IDENT_ISHIGHCONTRAST:
						case IDENT_DPI:
							return check(argc == 0, 1);
						case IDENT_BACKGROUND:
							if(id.equals(2, { IDENT_OPACITY , IDENT_EFFECT }))
								return check(argc == 0, 2);
							return check(argc == 0, 1);
						case IDENT_ITEM:
						{
							if(id.equals(2, { IDENT_BACK, IDENT_TEXT }))
							{
								if(id.equals(3, { IDENT_NORMAL, IDENT_SELECT }))
								{
									if(id[4] == IDENT_DISABLE)
										return check(argc == 0, 4);
									return check(argc == 0, 3);
								}

								return check(argc == 0, 2);
							}
							return error_at(1);
						}
						case IDENT_MODE:
						{
							if(id[2] == IDENT_ZERO)
								return check(argc == 0, 1);
							if(id[2] == IDENT_SYSTEM)
								return check(argc == 0, 2);
							return error_at(2);
						}
					}
					return error_at(0);
				}
				case IDENT_EFFECT:
				{
					switch(id[1])
					{
						case IDENT_NONE:
						case IDENT_EFFECT_TRANSPARENT:
						case IDENT_EFFECT_BLUR:
						case IDENT_EFFECT_ACRYLIC:
						case IDENT_EFFECT_MICA:
						case IDENT_EFFECT_TABBED:
						case IDENT_ZERO:
							return check(argc == 0, 1);
					}
					return error_at(1);
				}
				case IDENT_WINDOW:
				case IDENT_WND:
				{
					switch(id[1])
					{
						case IDENT_POST:
						case IDENT_SEND:
							return check(argc == 3 || argc == 4, 1);
						case IDENT_COMMAND:
							return check(argc <= 2, 1);
						case IDENT_HANDLE:
						case IDENT_NAME:
						case IDENT_TITLE:
							return check(argc == 0, 1);
						case IDENT_OWNER:
						case IDENT_PARENT:
							switch(id[2])
							{
								case IDENT_HANDLE:
								case IDENT_NAME:
								case IDENT_TITLE:
									return check(argc == 0, 2);
								case IDENT_ZERO:
									return check(argc == 0, 1);
							}
							return error_at(1);
						case IDENT_IS_DESKTOP:
						case IDENT_IS_TASKBAR:
						case IDENT_IS_EXPLORER:
						case IDENT_IS_NAV:
						case IDENT_IS_TREE:
						case IDENT_IS_SIDE:
						case IDENT_IS_EDIT:
						case IDENT_IS_START:
						case IDENT_IS_TITLEBAR:
						case IDENT_IS_CONTEXTMENUHANDLER:
						case IDENT_MODULE:
						case IDENT_INSTANCE:
						case IDENT_ID:
							return check(argc == 0, 1);
					}
					return error_at(0);
				}
				case IDENT_PROCESS:
				{
					switch(id[1])
					{
						case IDENT_ID:
						case IDENT_HANDLE:
						case IDENT_NAME:
						case IDENT_PATH:
						case IDENT_IS_EXPLORER:
						case IDENT_USED:
							return check(argc == 0, 1);
						case IDENT_IS_STARTED:
							return check(argc == 1, 1);
					}
					return error_at(0);
				}
				case IDENT_COMMAND:
				{
					switch(id[1])
					{
						case IDENT_COMMAND_TOGGLE_DESKTOP:
						case IDENT_COMMAND_MINIMIZE_ALL_WINDOWS:
						case IDENT_COMMAND_RESTORE_ALL_WINDOWS:
						case IDENT_COMMAND_SHOW_WINDOWS_SIDE_BY_SIDE:
						case IDENT_COMMAND_SHOW_WINDOWS_STACKED:
						case IDENT_COMMAND_RUN:
						case IDENT_COMMAND_FIND:
						case IDENT_COMMAND_SWITCHER:
						case IDENT_COMMAND_SEARCH:
						case IDENT_COMMAND_CASCADE_WINDOWS:
						case IDENT_COMMAND_REFRESH:
						case IDENT_COMMAND_TOGGLEEXT:
						case IDENT_COMMAND_TOGGLEHIDDEN:
						case IDENT_COMMAND_RESTART_EXPLORER:
						case IDENT_ID_SELECT_ALL:
						case IDENT_ID_INVERT_SELECTION:
						case IDENT_ID_SELECT_NONE:
						case IDENT_ID_FOLDER_OPTIONS:
						case IDENT_ID_CUSTOMIZE_THIS_FOLDER:
						case IDENT_ID_COPY_TO_FOLDER:
						case IDENT_ID_MOVE_TO_FOLDER:
						case IDENT_ID_UNDO:
						case IDENT_ID_REDO:
						case IDENT_COMMAND_GOBACK:
						case IDENT_COMMAND_GOFORWARD:
						case IDENT_COMMAND_GOHOME:
							return check(argc == 0, 1);
						case IDENT_COMMAND_COPY:
						case IDENT_COMMAND_COPY_TO_CLIPBOARD:
						case IDENT_COMMAND_SLEEP:
						case IDENT_COMMAND_NAVIGATE:
							return check(argc == 1, 1);
					}
					return error_at(0);
				}
				case IDENT_RANDOM:
					return check(argc <= 2);
				case IDENT_REGEX:
				{
					switch(id[1])
					{
						case IDENT_REGEX_MATCH:
						case IDENT_REGEX_MATCHES:
							return check(argc == 2, 1);
						case IDENT_REPLACE:
							return check(argc == 3, 1);
						default:
							return error_at(0);
					}
					return error_at(0);
				}
				case IDENT_INVOKE:
				{
					switch(id[1])
					{
						case IDENT_CANCEL:
						case IDENT_ZERO:
							return check(argc == 0, 1);
						default:
							return error_at(1);
					}
					return error_at(0);
				}
				case IDENT_IF:
					return check(argc >= 1  || argc <= 3);
				case IDENT_FOR:
				case IDENT_FOREACH:
					return check(argc == 3, 0);
				case IDENT_BREAK:
				case IDENT_CONTINUE:
					return check(argc == 0);
				case IDENT_EQUAL:
				case IDENT_GREATER:
				case IDENT_LESS:
				case IDENT_SHL:
				case IDENT_SHR:
					return check(argc == 2);
				case IDENT_EQUALS:
					return check(argc >= 2);
				case IDENT_CHAR:
				case IDENT_PRINT:
				case IDENT_QUOTE:
				case IDENT_TOHEX:
				case IDENT_LENGTH:
				case IDENT_LEN:
				case IDENT_TOINT:
				case IDENT_TODOUBLE:
				case IDENT_TOUINT:
				case IDENT_TOFLOAT:
					return check(argc == 1);
				case IDENT_NOT:
				case IDENT_NULL:
				case IDENT_NIL:
					return check(argc <= 1);
				// const
				case IDENT_TRUE:
				case IDENT_YES:
				case IDENT_OK:
				case IDENT_FALSE:
				case IDENT_NO:
				{
					if(length == 1 && argc == 0)
						return ExpressionType::Number;
					return error_at(0);
				}
				case IDENT_DEFAULT:
				case IDENT_AUTO:
				case IDENT_BOOL:
				case IDENT_NONE:
				case IDENT_INHERIT:
				case IDENT_PARENT:
				case IDENT_BOTH:
				case IDENT_TOP:
				case IDENT_BOTTOM:
				case IDENT_BEFORE:
				case IDENT_AFTER:
				case IDENT_HIDDEN:
				case IDENT_REMOVE:
				case IDENT_DISABLE:
				case IDENT_DISABLED:
				case IDENT_ENABLE:
				case IDENT_ENABLED:
				case IDENT_NORMAL:
				case IDENT_SHOW:
				case IDENT_VISIBLE:
				case IDENT_MINIMIZED:
				case IDENT_MAXIMIZED:
				//case IDENT_VIS_HIDDEN:
				case IDENT_VIS_STATIC:
				case IDENT_VIS_LABEL:
					return check(argc == 0);
					//case IDENT_FOREACH:
					//	break;
				case IDENT_INDEXOF:
					return check(argc >= 1 && argc <= 3);
				case IDENT_VAR:
				{
					if(length == 1)
					{
						return check(argc >= 0 && argc <= 2);
					}
					else if(length == 2)
					{
						if(argc == 0 && context.variable_exists(id[1]))
							return ExpressionType::Variable;
						return error_at(1);
					}
					return error_at(0);
				}
				case IDENT_LOC:
				{
					if(length != 2)
						return error_at(0);
					//if(argc == 0 && context.Cache->variables.loc.exists(id[1], false))
						return ExpressionType::Identifier;
					//return error_at(1);
				}
			}

			//if(argc == 0)
			{
				if(context.variables.global)
				{
					if(auto var = context.variables.global->at(id[0]))
					{
						uint32_t i = 1;
					/*	for(; var && i < length; i++)
							var = var->find(id[i]);
					*/
						if(var) 
						{
							if(length > 1)
							{
								if(auto r = verify_string_ident(id[1], argc); r > 0)
									return ExpressionType::StringExt;
								/*for(auto i = 1u; i < length; i++)
								{
									if(auto r = verify_string_ident(id[i], argc); r > 0)
										return ExpressionType::StringExt;
								}*/
							}
							return ExpressionType::Variable; 
						}
						
						return error_at(--i);
					}
				}

				//if(length == 1) return context.variable_exists(id[0]))
				if(length == 1)
				{
				/*	if(context.Variables->exists(id[0], false))
						return ExpressionType::Variable;*/
					return ExpressionType::RuntimeVariable;
				}
			}

			return error_at(0);
		}

		int Parser::verify_string_ident(uint32_t id, const size_t argc)
		{
			auto check = [&](bool condition, int ret = 0)->int
			{
				return condition ? ret : -1;
			};

			switch(id)
			{
				case IDENT_NULL:
				case IDENT_EMPTY:
				case IDENT_LEN:
				case IDENT_LENGTH:
					return check(argc == 0, 0);
				case IDENT_EQ:
				case IDENT_EQUALS:
				case IDENT_NOT:
				case IDENT_START:
				case IDENT_END:
				case IDENT_FIND:
				case IDENT_FINDLAST:
				case IDENT_CONTAINS:
					return check(argc == 1 || argc == 2, 0);
				case IDENT_UPPER:
				case IDENT_LOWER:
					return check(argc == 0, 1);
				case IDENT_TRIM:
				case IDENT_TRIMSTART:
				case IDENT_TRIMEND:
					return check(argc == 0 || argc == 1, 1);
				case IDENT_SET:
				case IDENT_GET:
				case IDENT_CHAR:
				case IDENT_AT:
				case IDENT_LEFT:
				case IDENT_RIGHT:
					return check(argc == 1, 1);
				case IDENT_SUB:
				case IDENT_REMOVE:
				case IDENT_PADLEFT:
				case IDENT_PADRIGHT:
				case IDENT_PADDING:
					return check(argc == 1 || argc == 2, 1);
				case IDENT_REPLACE:
					return check(argc == 2 || argc == 3, 1);
				case IDENT_FORMAT:
					return check(argc > 0, 1);
				case IDENT_ZERO:
					return 0;
				default:
					return -1;
			}
		}
	}
}