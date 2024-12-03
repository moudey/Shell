
#include <pch.h>
#include <tuple>
#include "Expression\Variable.h"
#include "Include\Theme.h"
#include "RegistryConfig.h"

// Windows State (Hidden | Normal | Minimized | Maximized)
namespace Nilesoft
{
	namespace Shell
	{
		extern Logger &_log = Logger::Instance();

		Parser::Parser()
		{
			context.Application = &Initializer::instance->application;

			context.Cache = Initializer::instance->cache;
			context.variables.global = &context.Cache->variables.global;
			context.variables.runtime = &context.Cache->variables.runtime;
			context.Selections = nullptr;

			string cfg;
			if(RegistryConfig::get(nullptr, L"config", cfg) && !cfg.empty())
			{
				//if(Path::IsFileExists(cfg))
				m_path = cfg.move();
			}

			if(m_path.empty() || !Path::IsFileExists(m_path))
			{
				if(Path::IsFileExists(context.Application->ConfigPortable))
					m_path = context.Application->ConfigPortable;
				else
				{
					/*string ss = L"config file ";
					if(!m_path.empty())
						ss.append_format(L"'%s' ", m_path.c_str());
					ss.append(L"not found.");
					_log.warning(ss);
					.*/
					string ss = L"config file ";
					if(!m_path.empty())
					{
						ss += L"'";
						ss += m_path;
						ss += L"' ";
					}
					ss += L"not found.";
					_log.warning(ss);
					return;
				}
			}

			context.Application->Config = m_path;

			_imports.emplace_back(new Lexer);
			l = _imports.front().get();
			
			if(!l->load_File(m_path.c_str()) || l->length < 5)
			{
				if(HasError())
				{
					error(l->length < 5 ? TokenError::InvalidConfigFile : l->error, m_path.c_str());
				}
			}
		}

		Parser::~Parser() { }
		size_t Parser::Line() const { return l ? l->line : 0; };
		size_t Parser::Column() const { return l ? l->column : 0; };
		const wchar_t *Parser::Path() const { return l ? l->path.c_str() : nullptr; };

		bool Parser::HasError() const
		{
			return error_code != TokenError::None;
		}

		TokenError Parser::Error() const
		{
			return error_code;
		}

		bool Parser::error(TokenError tokenError)
		{
			if(!m_error)
			{
				error_code = tokenError;
				m_error = true;
			}

			bool log = true;
			auto le = &Initializer::LastError;
			if(le->code != TokenError::None)
				log = !(le->code == error_code && le->line == l->line && le->col == l->column);

			throw ParserException(tokenError, l->line, l->column, Path::Name(l->path).c_str(), log);
		}

		bool Parser::error(TokenError tokenError, const wchar_t *message)
		{
			if(!m_error)
			{
				error_code = tokenError;
				m_error = true;

				auto le = &Initializer::LastError;
				if(le->code != TokenError::None && (le->code == error_code && le->line == l->line && le->col == l->column))
					return false;

				if(message)
					Logger::Error(error_format1, ParserException::errortostr(error_code), message, Path::Name(l->path).c_str());
				else
					Logger::Error(error_format0, ParserException::errortostr(error_code));
				//Logger::Error(L"error %s, %s file", ParserException::errortostr(error_code), message);
			}
			return false;
		}

		bool Parser::error(TokenError tokenError, size_t column)
		{
			l->column = column;
			return error(tokenError);
		}

		bool Parser::error(TokenError tokenError, size_t column, size_t line)
		{
			l->column = column;
			l->line = line;
			return error(tokenError);
		}

		void Parser::error_if(bool condition, TokenError tokenError)
		{
			if(condition) error(tokenError);
		}

		void Parser::error_if(bool condition, TokenError tokenError, size_t column)
		{
			if(condition) error(tokenError, column);
		}

		void Parser::error_if(bool condition, TokenError tokenError, size_t column, size_t line)
		{
			if(condition) error(tokenError, column, line);
		}

		bool Parser::skip_comment(bool singleLineComment)
		{
			auto col = start_col, line = start_line;

			if(l->eof) return false;

			auto single = (l->tok == L'/' && l->peek == L'/');	// start single-line comment
			auto multi = (l->tok == L'/' && l->peek == L'*');	// start multi-line comment
					
			if(single)
			{
				if(!singleLineComment) return false;
				//error_if(singleLineComment == 2, TokenError::CommentSinglelineUnexpected);
			}

			if(single || multi)
			{
				if(multi)
				{
					col = l->column, line = l->line;
				}

				//skip start comment and continue to end
				l->next(2);
				for(;;)
				{
					if(l->eof)
					{
						//error(TokenError::CloseCommentExpected, col, line);
						//return false;
						//break;
						//if(single) return false;
						error_if(multi, TokenError::CloseCommentExpected, col, line);
						return false;
					}
					// End single-line comment
					else if(l->tok == L'\n' && single) break;
					// End multi-line comment
					else if((l->tok == L'*' && l->peek == L'/') && multi) { l->next(); break; }
					l->next();
				}
				l->next(); //skip end comment '\n' or '/'
				return true;
			}
			return false;
		}

		bool Parser::skip(bool singleLineComment, bool eat_whitespace)
		{
			for(; ;)
			{
				if(l->eof) return false;
				if(l->is_space() && eat_whitespace) l->next();
				else if(l->tok == L'/' && l->is({ L'/', L'*' }, 1))
				{
					if(!skip_comment(singleLineComment))
						break;
				}
				else
				{
					break;
				}
			}
			return true;
		}

		Parser &Parser::eat(int i)
		{
			skip();
			if(i >= 0)
			{
				l->next(i);
				skip();
			}
			return *this;
		}

		bool Parser::peek_char(wchar_t c, bool singleLineComment)
		{
			auto p = l->index;
			skip(singleLineComment);
			if(l->next_is(c))
				return true;
			l->previous(p);
			return false;
		}

		uint32_t Parser::peek_ident(const std::initializer_list<uint32_t> &ids, uint32_t *value)
		{
			auto s = l->buf();
			if(std::iswalpha(*s))
			{
				uint32_t count = 0;
				Hash h;
				h.hash(*s);
				do
				{
					h.hash(*s++); count++;
				} while(std::iswalnum(*s) || (*s == L'_'));

				for(auto &id : ids)
				{
					if(h == id)
					{
						if(value) *value = h;
						return count;
					}
				}
			}
			if(value) *value = 0;
			return 0;
		}

		bool Parser::expect(wchar_t c, bool singleLineComment)
		{
			skip(singleLineComment);
			if(l->next_is(c))
				return true;
			return false;
		}

		bool Parser::expect(std::initializer_list<wchar_t> chs, bool singleLineComment)
		{
			for(auto c : chs)
			{
				if(l->eof) return false;
				if(expect(c, singleLineComment)) return true;
			}
			return false;
		}


		uint32_t Parser::expect(std::initializer_list<const wchar_t *> words, bool singleLineComment)
		{
			for(auto word : words)
			{
				if(l->eof) return false;
				if(expect(word, singleLineComment)) return true;
			}
			return false;
		}

		bool Parser::expect(const wchar_t *word, bool singleLineComment)
		{
			skip(singleLineComment);

			bool ret = true;
			auto last_index = l->index;

			size_t i = 0;
			while(*word)
			{
				if(l->eof) return false;
				if(++i >= l->length) break;

				if(!l->is(*word++))
				{
					ret = false;
					break;
				}

				l->next();
			}

			if(ret && !l->is_alpha())
				return true;

			l->previous(last_index);
			return false;
		}

		bool Parser::expect_assign(bool expected)
		{
			skip();
			if(l->next_if(l->is_assign())) return true;
			error_if(expected, TokenError::AssignExpected);
			return false;
		}

		bool Parser::expect_openParen()
		{
			return expect(L'(') ? true : error(TokenError::OpenParenExpected);
		}

		bool Parser::expect_closeParen()
		{
			return expect(L')') ? true : error(TokenError::CloseParenExpected);
		}

		bool Parser::expect_openCurly()
		{
			return expect(L'{') ? true : error(TokenError::OpenCurlyExpected);
		}

		bool Parser::expect_closeCurly()
		{
			return expect(L'}') ? true : error(TokenError::CloseCurlyExpected);
		}

		// system environment variable
		//	::= %var%
		bool Parser::parse_environment(string &value)
		{
			if(l->tok != L'%') return false;
			wchar_t percent = l->next();// skip start '%'
			if(l->tok != L'%')
			{
				bool ret = false;
				for(;;)
				{
					error_if(l->eof, TokenError::StringUnterminated);

					if(l->next_is(percent))// skip end %
					{
						ret = true;
						break;
					}
					else if(!(l->is_iddigit() || l->tok == L'(' || l->tok == L')'))
						break;
					value.append(l->next());
				}

				if(ret)
				{
					string var = Environment::Variable(value).move();
					if(!var.empty())
					{
						value = var.move();
						return true;
					}
					value.append(percent);
				}
			}
			value = (percent + value).move();
			return true;
		}

		uint32_t Parser::parse_ident(bool multiple)
		{
			skip();
			prevCol = l->column;
			Hash ident;
			if(multiple)
				while(l->is_iddigit() || l->is({ L'-', L'.' })) ident.hash(l->next());
			else
				while(l->is_iddigit()) ident.hash(l->next());
			//error_if(eof, TokenError::IdentifierExpected);
			return ident.value();
		}

		void Parser::parse_menu(NativeMenu *menu, bool imported)
		{
			if(!imported)
				expect_openCurly();

			while(true)
			{
				skip();

				if(l->eof || l->tok == L'}')
					break;

				// local variables definition
				if(parse_variable(&menu->variables))
					continue;

				// local image definition
				if(parse_image())
					continue;

				auto type = parse_ident(false);
				
				if(type == CONFIG_IMPORT)
				{
					auto ret = load_import(l->line, l->column, true, false);
					if(ret > 0)
					{
						skip();
						prevCol = l->column;
						parse_menu(menu, true);
						skip();
						pop_import();
					}
					continue;
				}
				else if(type == CONFIG_MODIFY || type == IDENT_REMOVE)
				{
					parse_modify_items(type);
					continue;
				}

				std::unique_ptr<NativeMenu> sub(new NativeMenu(menu));
				if(parse_menu_item(sub.get(), type))
				{
					menu->items.push_back(sub.release());
					TotalMenuCount++;
				}
			//	else if(!imported)
			//		break;
			}

			if(!imported && !l->eof)
				expect_closeCurly();
		}

		bool Parser::parse_menu_item(NativeMenu *menu, uint32_t type)
		{
			switch(type)
			{
				case MENU_TYPE_MENU:
					menu->type = NativeMenuType::Menu;
					break;
				case MENU_TYPE_ITEM:
					menu->type = NativeMenuType::Item;
					break;
				case MENU_TYPE_SEP:
				case MENU_TYPE_SEPARATOR:
					menu->type = NativeMenuType::Separator;
					break;
				default:
					return error(TokenError::MenuTypeExpected, prevCol);
			}

			auto col_after_open_paren = l->column;

			// parse menu properties
			if(!parse_properties(menu))
				return false;

			if(menu->properties == 0 && menu->is_separator())
				return true;

			if(!menu->is_separator())
			{
				error_if(menu->properties <= 0, TokenError::PropertyExpected, l->column - 1);
				
				if(!menu->title && !menu->image.defined)
					error(TokenError::PropertyTitleOrImageExpected, col_after_open_paren);

				if(menu->is_item())
				{
					skip();
					if(l->tok == L'{')
					{
						// parse menu commands property
						if(!parse_properties_commands(menu))
							return false;
					}
					menu->properties += !menu->commands.empty();
				}

				/*
				// fixme
				if(!menu->cmd->admin.expr && menu->cmd->admin.value == Privileges::None)
				{
					if(menu->owner->cmd->admin.value != Privileges::None)
						menu->cmd->admin.value = menu->owner->cmd->admin.value;
					else
					{
						menu->cmd->admin.inherit = true;
						menu->cmd->admin.expr = menu->owner->cmd->admin.expr;
					}
				}*/

				if(menu->is_menu())
					parse_menu(menu, false);
			}
			return true;
		}

		void Parser::parse_modify_items(uint32_t action)
		{
			auto cache = Initializer::instance->cache;
			std::unique_ptr<NativeMenu> item(new NativeMenu(true));
			if(eat().parse_modify_properties(item.get(), action))
				cache->statics.push_back(item.release());
		}

		struct SETTING
		{
			uint32_t id = 0;
			auto_expr *value = nullptr;
			std::vector<SETTING> children;

			SETTING *find(const Ident &ident,  uint32_t i = 1)
			{
				SETTING *ret{};
				for(auto &setting : children)
				{
					if(setting.id == ident[i])
					{
						ret = (i == (ident.length() - 1)) ? &setting : setting.find(ident, ++i);
						break;
					}
				}
				return ret;
			}
		};

		void Parser::parse_settings(std::vector<SETTING> *settings, const Ident &id, bool imported)
		{
			if(!settings)
				return;

			auto ln = l->line;
			auto col = l->column;

			skip();

			Ident ident;
			if(id != 0)
				ident = id;
			else if(!parse_ident(ident, true))
				return;

			auto has_bracket = eat().l->next_is(L'{');
			
			if(!has_bracket && !l->is_assign())
			{
				if(l->eof && imported)
					return;
				error(TokenError::AssignExpected);
			}

			for(auto &setting : *settings)
			{
				if(ident == setting.id)
				{
					auto _setting = &setting;
					if(ident.length() > 1)
					{
						_setting = _setting->find(ident);
						if(!_setting) goto error_undefined;
					}

					if(has_bracket)
					{
						parse_settings(_setting, ident, imported);
						return;
					}

					if(!eat().l->next_if(l->is_assign()))
					{
						if(l->eof && imported)
							return;
						error(TokenError::AssignExpected);
					}

					*_setting->value = parse_expression();
					return;
				}
			}

		error_undefined:
			error(TokenError::IdentifierUndefined, col, ln);
		}

		void Parser::parse_settings(SETTING *setting, const Ident &id, bool imported)
		{
			auto cache = Initializer::instance->cache;

			if(!setting)
				return;

			skip();

			auto ln = l->line;
			auto col = l->column;
			Ident _ident;
			prevCol = l->column;
			if(id != 0)
				_ident = id;
			else if(!parse_ident(_ident, true))
				return;
			
			skip();

			col = l->column;
			auto has_bracket = false;
			if(l->next_if(l->is_assign()))
			{
				if(_ident == setting->id)
				{
					*setting->value = parse_expression();
					return;
				}

				if(l->eof && imported)
					return;
				goto error_undefined;
			}
			else if(l->tok == L'{')
			{
				l->next();
				has_bracket = true;
			}
			
			while(true)
			{
				skip();

				if(l->eof || l->tok == L'}')
					break;

				skip();
				col = l->column;
				Ident ident;
				if(!parse_ident(ident, true))
				{
					if(l->next_is(L'{'))
						continue;

					if(l->is(L'}'))
						break;

					if(parse_variable(&cache->variables.global))
						continue;

					if(l->eof && imported)
						break;
					
					goto error_undefined;
				}
				
				if(ident[0] == IDENT_IMPORT)
				{
					if(ident.length() > 1)
						goto error_undefined;

					if(auto ret = load_import(l->line, l->column, true, false); ret > 0)
					{
						while(!l->eof) parse_settings(&setting->children, 0, true);
						pop_import();
						if(!has_bracket)
							return;
					}
					continue;
				}

				if(!ident.empty() && l->tok == L'}')
					break;
				parse_settings(&setting->children, ident);
			}

			expect_closeCurly();

			return;

		error_undefined:
			error(TokenError::IdentifierUndefined, col, ln);
		}

		void Parser::parse_theme()
		{
			auto st = &context.Cache->settings.theme;

			SETTING _item = { IDENT_ITEM, nullptr, {
				{ IDENT_TEXT, &st->item.text.color.value, {
					{ IDENT_NORMAL, &st->item.text.color.normal, {
						{ IDENT_DISABLED, &st->item.text.color.normal_disabled }
					}},
					{ IDENT_SELECT, &st->item.text.color.select, {
						{ IDENT_DISABLED, &st->item.text.color.select_disabled }
					}},
					{ IDENT_ALIGN, &st->item.text.align }
				}},
				{ IDENT_BACK, &st->item.back.value, {
					{ IDENT_NORMAL, &st->item.back.normal, {
					{ IDENT_DISABLED, &st->item.back.normal_disabled }
					}},
					{ IDENT_SELECT, &st->item.back.select, {
					{ IDENT_DISABLED, &st->item.back.select_disabled }
					}}
				}},
				{ IDENT_BORDER, &st->item.border.value, {
					{ IDENT_NORMAL, &st->item.border.normal, {
						{ IDENT_DISABLED, &st->item.border.normal_disabled }
					}},
					{ IDENT_SELECT, &st->item.border.select, {
						{ IDENT_DISABLED, &st->item.border.select_disabled }
					}}
				}},
				{ IDENT_OPACITY, &st->item.opacity},
				{ IDENT_RADIUS, &st->item.radius},
				{ IDENT_PREFIX, &st->item.prefix},
				{ IDENT_PADDING, &st->item.padding.value, {
					{ IDENT_LEFT, &st->item.padding.left },
					{ IDENT_TOP, &st->item.padding.top },
					{ IDENT_RIGHT, &st->item.padding.right},
					{ IDENT_BOTTOM, &st->item.padding.bottom }
				}},
				{ IDENT_MARGIN, &st->item.margin.value, {
					{ IDENT_LEFT, &st->item.margin.left },
					{ IDENT_TOP, &st->item.margin.top },
					{ IDENT_RIGHT, &st->item.margin.right},
					{ IDENT_BOTTOM, &st->item.margin.bottom }
				}}
			} };

			SETTING _shadow = { IDENT_SHADOW, &st->shadow.enabled, {
				{IDENT_ENABLED, &st->shadow.enabled},
				{IDENT_SIZE , &st->shadow.size},
				{IDENT_COLOR , &st->shadow.color},
				{IDENT_OPACITY, &st->shadow.opacity},
				{IDENT_OFFSET , &st->shadow.offset}
			} };

			SETTING _border = { IDENT_BORDER, &st->border.size, {
				{IDENT_ENABLED, &st->border.enabled},
				{IDENT_SIZE, &st->border.size},
				{IDENT_COLOR, &st->border.color},
				{IDENT_OPACITY, &st->border.opacity},
				{IDENT_RADIUS, &st->border.radius},
				{IDENT_PADDING, &st->border.padding.value, {
					{ IDENT_LEFT, &st->border.padding.left },
					{ IDENT_TOP, &st->border.padding.top },
					{ IDENT_RIGHT, &st->border.padding.right},
					{ IDENT_BOTTOM, &st->border.padding.bottom }
				}},
			} };

			SETTING _separator = { IDENT_SEPARATOR, &st->separator.color, {
				{IDENT_COLOR, &st->separator.color},
				{IDENT_OPACITY, &st->separator.opacity},
				{IDENT_SIZE, &st->separator.size},
				{IDENT_MARGIN, &st->separator.margin.value, {
					{ IDENT_LEFT, &st->separator.margin.left },
					{ IDENT_TOP, &st->separator.margin.top },
					{ IDENT_RIGHT, &st->separator.margin.right},
					{ IDENT_BOTTOM, &st->separator.margin.bottom }
				}}
			} };

			SETTING _symbol = { IDENT_SYMBOL, &st->symbol.color.value, {
				{IDENT_NORMAL, &st->symbol.color.normal, {
					{ IDENT_DISABLED, &st->symbol.color.normal_disabled }
				}},
				{IDENT_SELECT, &st->symbol.color.select, {
					{ IDENT_DISABLED, &st->symbol.color.select_disabled }
				}},
				{IDENT_CHECKMARK, &st->symbol.checkmark.value, {
					{ IDENT_NORMAL, &st->symbol.checkmark.normal, {
						{ IDENT_DISABLED, &st->symbol.checkmark.normal_disabled }
					}},
					{ IDENT_SELECT, &st->symbol.checkmark.select, {
						{ IDENT_DISABLED, &st->symbol.checkmark.select_disabled }
					}}
				}},
				{IDENT_BULLET, &st->symbol.bullet.value, {
					{ IDENT_NORMAL, &st->symbol.bullet.normal, {
					{ IDENT_DISABLED, &st->symbol.bullet.normal_disabled }
				}},
					{ IDENT_SELECT, &st->symbol.bullet.select, {
						{ IDENT_DISABLED, &st->symbol.bullet.select_disabled }
					}},
				}},
				{IDENT_CHEVRON, &st->symbol.chevron.value, {
					{ IDENT_NORMAL, &st->symbol.chevron.normal, {
					{ IDENT_DISABLED, &st->symbol.chevron.normal_disabled }
				}},
					{ IDENT_SELECT, &st->symbol.chevron.select, {
						{ IDENT_DISABLED, &st->symbol.chevron.select_disabled }
					}},
				}}
			} };

			SETTING _image = { IDENT_IMAGE, &st->image.enabled, {
				{IDENT_ENABLED, &st->image.enabled},
				{IDENT_COLOR, &st->image.color},
				{IDENT_GAP, &st->image.gap},
				{IDENT_SIZE, &st->image.size},
				{IDENT_GLYPH, &st->image.glyph},
				{IDENT_SCALE, &st->image.scale},
				{IDENT_ALIGN, &st->image.display},
				{IDENT_DISPLAY, &st->image.display}
			} };

			SETTING  _layout = { IDENT_LAYOUT, nullptr, {
				{IDENT_WIDTH, &st->layout.width},
				{IDENT_RTL, &st->layout.rtl},
				{IDENT_POPUP, &st->layout.popup.align, {
					{IDENT_ALIGN, &st->layout.popup.align}
				}}
			} };

			SETTING _gradient = { IDENT_GRADIENT, nullptr, {
				{IDENT_ENABLED, &st->gradient.enabled},
				{IDENT_GRADIENT_LINEAR, &st->gradient.linear},
				{IDENT_GRADIENT_RADIAL, &st->gradient.radial},
				{IDENT_GRADIENT_STOP, &st->gradient.stop},
			} };

			SETTING _font = { IDENT_FONT, &st->font.value, {
				{ IDENT_SIZE, &st->font.size },
				{ IDENT_NAME, &st->font.name },
				{ IDENT_FONT_WEIGHT, &st->font.weight },
				{ IDENT_FONT_ITALIC, &st->font.italic }
			} };

			auto sets = &context.Cache->settings;
			SETTING _tip = { IDENT_TIP, &sets->tip.enabled, {
				{IDENT_ENABLED, &sets->tip.enabled},
				{IDENT_DEFAULT, &sets->tip.normal},
				{IDENT_PRIMARY, &sets->tip.primary},
				{IDENT_INFO, &sets->tip.info},
				{IDENT_SUCCESS, &sets->tip.success},
				{IDENT_WARNING, &sets->tip.warning},
				{IDENT_DANGER, &sets->tip.danger},
				{IDENT_BORDER, &sets->tip.border},
				{IDENT_WIDTH, &sets->tip.width},
				{IDENT_OPACITY, &sets->tip.opacity},
				{IDENT_RADIUS, &sets->tip.radius},
				{IDENT_TIME, &sets->tip.time},
				{IDENT_PADDING, &sets->tip.padding.value, {
					{ IDENT_LEFT, &sets->tip.padding.left },
					{ IDENT_TOP, &sets->tip.padding.top },
					{ IDENT_RIGHT, &sets->tip.padding.right},
					{ IDENT_BOTTOM, &sets->tip.padding.bottom }
				}}
			} };

			SETTING _theme = { IDENT_THEME, &st->name, {
				{IDENT_NAME, &st->name},
				{IDENT_DARK, &st->dark},
				{IDENT_VIEW, &st->view},
				{IDENT_BACKGROUND, &st->background.color, {
					{IDENT_COLOR, &st->background.color},
					{IDENT_OPACITY, &st->background.opacity},
					{IDENT_EFFECT, &st->background.effect},
					{IDENT_TINTCOLOR, &st->background.tintcolor},
					{IDENT_IMAGE, &st->background.image},
					_gradient
				}},
				_font, _item, _shadow, _border, _separator, _symbol, _layout, _image, _tip
			} };

			auto _setting = &_theme;
			Ident ident;
			parse_ident(ident, true);

			if(ident.length() > 1)
			{
				_setting = _setting->find(ident);
				ident = ident.back();
			}
			parse_settings(_setting, ident, false);
		}

		void Parser::parse_settings()
		{
			auto sets = &context.Cache->settings;

			SETTING _modify = { IDENT_MODIFY, &sets->modify_items.enabled, {
				{IDENT_ENABLED, &sets->modify_items.enabled},
				{MENU_IMAGE, &sets->modify_items.image},
				{MENU_TITLE, &sets->modify_items.title},
				{MENU_VISIBILITY, &sets->modify_items.visibility},
				{MENU_PARENT, &sets->modify_items.parent},
				{MENU_POSITION, &sets->modify_items.position},
				{MENU_SEPARATOR, &sets->modify_items.separator},
				{IDENT_AUTO, &sets->modify_items.auto_image_group},
				{IDENT_REMOVE, nullptr, {
					{IDENT_DUPLICATE, &sets->modify_items.remove.duplicate },
					{IDENT_DISABLED, &sets->modify_items.remove.disabled },
					{IDENT_SEPARATOR, &sets->modify_items.remove.separator }
				}}
			} };

			SETTING _new = { CONFIG_NEW, &sets->new_items.enabled, {
				{IDENT_ENABLED, &sets->new_items.enabled},
				{MENU_IMAGE, &sets->new_items.image},
			} };

			SETTING _tip = { IDENT_TIP, &sets->tip.enabled, {
				{IDENT_ENABLED, &sets->tip.enabled},
				{IDENT_DEFAULT, &sets->tip.normal},
				{IDENT_PRIMARY, &sets->tip.primary},
				{IDENT_INFO, &sets->tip.info},
				{IDENT_SUCCESS, &sets->tip.success},
				{IDENT_WARNING, &sets->tip.warning},
				{IDENT_DANGER, &sets->tip.danger},
				{IDENT_BORDER, &sets->tip.border},
				{IDENT_WIDTH, &sets->tip.width},
				{IDENT_OPACITY, &sets->tip.opacity},
				{IDENT_RADIUS, &sets->tip.radius},
				{IDENT_TIME, &sets->tip.time},
				{IDENT_PADDING, &sets->tip.padding.value, {
					{ IDENT_LEFT, &sets->tip.padding.left },
					{ IDENT_TOP, &sets->tip.padding.top },
					{ IDENT_RIGHT, &sets->tip.padding.right},
					{ IDENT_BOTTOM, &sets->tip.padding.bottom }
				}}
			}};

			SETTING _settings = { CONFIG_SETTINGS, &sets->priority, {
				{ IDENT_PRIORITY, &sets->priority },
				{ IDENT_SHOWDELAY, &sets->showdelay },
				{ IDENT_SCREENSHOT, &sets->screenshot.enabled,{
					{ IDENT_ENABLED, &sets->screenshot.enabled },
					{ IDENT_DIRECTORY, &sets->screenshot.directory }
				}},
				{ IDENT_EXCLUDE, &sets->exclude.value,{
					{ IDENT_WHERE, &sets->exclude.value },
					{ IDENT_WINDOW, &sets->exclude.window },
					{ IDENT_PROCESS, &sets->exclude.process }
				}},
				_tip, _modify, _new,
			} };

			auto _setting = &_settings;
			Ident ident;
			parse_ident(ident, true);
			if(ident.length() > 1)
			{
				_setting = _setting->find(ident);
				ident = ident.back();
			}
			parse_settings(_setting, ident, false);
		}

		uint32_t Parser::parse_image_ident()
		{
			prevCol = l->column - 1;
			Hash h;
			
			auto isq = l->is_quote();
			wchar_t c = 0;

			auto is_normalize = [](const wchar_t &c)->bool
			{
				return !std::iswpunct(c) and !std::iswcntrl(c) and !iswblank(c);
			};

			if(isq) 
			{
				bool last_punct = false;
				auto q = l->tok;
				l->next(); // eat quote
				while(l->tok != q)
				{
					c = l->next();

					if(l->tok == 0)
						break;

					if(c == L'&')
					{
						if(l->tok != L'&') continue;
						l->next();
					}

					if(c == L'_')
					{
						if(last_punct)
							continue;
						last_punct = true;
						h.hash(c);
					}
					else if(is_normalize(c))
					{
						h.hash(c);
						last_punct = false;
					}
					else if(!last_punct)
					{
						if(!l->peek_is(q))
							h.hash(L'_');
						last_punct = true;
					}
				}
				error_if(!l->next_is(q), TokenError::CloseQuoteExpected);
			}
			else if(l->is_alpha())
			{
				while(l->is_iddigit())
				{
					h.hash(l->next());
				}
			}
			if(h.zero()) l->column = prevCol;
			return h;
		}

		//predefined constant variable
		bool Parser::parse_variable(Scope *variables, bool has_sign)
		{
			skip();

			if(has_sign)
			{
				if(l->tok != L'$')
					return false;

				skip();
				prevCol = l->column;
				l->next(); // skip $
			}

			error_if(!l->is_ident(0), TokenError::VariableExpected, prevCol);
			Ident id;
			id.push_back(parse_ident(true));
			prevCol = l->column;
			expect_assign(true);
			prevCol = l->column;
			
			variables->set(id, parse_expression());

			return true;
		}

		bool Parser::parse_image()
		{
			auto cache = Initializer::instance->cache;

			skip();

			if(l->tok != L'@')
				return false;
			
			l->next(); // skip @
			skip();

			prevCol = l->column;

			std::vector<uint32_t> ids;
			int i = 0;
			while(l->is_alpha() || l->is_quote())
			{
				if(auto ident = parse_image_ident(); ident)
				{
					i++;
					ids.push_back(ident);
				}
				skip();
				if(l->next_is(L',')) 
				{
					skip();
					if(l->tok == L'@')
					{
						l->next(); // skip @
						skip();
					}
				}
			}

			if(i == 0)
			{
				error_if(!l->eof, TokenError::IdentifierExpected, prevCol);
				return false;
			}

			expect_assign(true);
			cache->add_image(std::move(ids), parse_root_expression());
			return true;
		}

		// ... load data from disk and populate
		int Parser::load_import(size_t line, size_t col, bool ignore_failed, bool parse_import)
		{
			skip();
			
			if(parse_import)
			{
				if(!l->skip_import())
					return -1;
			}

			std::unique_ptr<Expression> epath(parse_root_expression());
			error_if(!epath, TokenError::ImportPathExpected, prevCol);

			Object obj = context.Eval(epath.get()).move();

			if(obj.is_null())
				return 0;

			string path = obj.to_string().trim().move();

			if(path.empty())
				return 0;
			
			if(path.length() > 2)
			{
				if(!((path[1] == L':' && path[2] == L'\\') || (path[0] == L'\\' && path[1] == L'\\')))
					path = Path::Combine(l->location, path).move();
			}

			path = Path::FixSeparator(path).move();

			auto hash = path.hash();
			
			if(hash)
			{
				for(auto &h : m_imports)
				{
					if(h == hash)
					{
						//if (!ignore_duplicated)
						{
							__trace(L"line[%d] column[%d] already imported '%s'",
											line, col, path.c_str());
							//return false;
						}
						break;
					}
				}
				m_imports.push_back(hash);
			}

			auto lex = import_push();

			if(hash && lex->load_File(path, ignore_failed))
			{
				__trace(L"import '%s'", path.c_str());

				l = lex;
				skip();
				prevCol = l->column;
				return 1;
			}

			if(lex->error != TokenError::None)
			{
				Logger::Error(L"line[%d] column[%d], %s '%s'",
							  line, col, ParserException::errortostr(lex->error), l->path.c_str());
			}
			else
			{
				Logger::Warning(L"line[%d] column[%d] invalid import file, '%s'",
								line, col, path.c_str());
			}

			pop_import();

			return 0;
		}

		void Parser::parse_loc(bool has_curly)
		{
			if(has_curly)
				expect_openCurly();
			while(!l->eof)
			{
				if(has_curly && l->tok == L'}')
					break;
				parse_variable(&context.Cache->variables.loc, false);
			}
			if(has_curly)
				expect_closeCurly();
		}

		// load data from disk and populate
		void Parser::parse_config()
		{
			auto cache = context.Cache;
			while(!l->eof)
			{
				skip();
				prevCol = l->column;
				
				if(l->peek_ident(IDENT_THEME))
				{
					parse_theme();
					continue;
				}
				else if(l->peek_ident(CONFIG_SETTINGS))
				{
					parse_settings();
					continue;
				}

				Hash id = parse_ident();
				switch(id)
				{
					case IDENT_THEME:
						parse_theme();
						break;
					case CONFIG_SETTINGS:
						parse_settings();
						break;
					case CONFIG_SET:
					case CONFIG_MODIFY:
					case CONFIG_UPDATE:
					case CONFIG_CHANGE:
					case IDENT_REMOVE:
						parse_modify_items(id);
						break;
					case CONFIG_MENU:
					case CONFIG_ITEM:
					case CONFIG_SEP:
					case CONFIG_SEPARATOR:
					{
						std::unique_ptr<NativeMenu> item(new NativeMenu(&cache->dynamic));
						if(parse_menu_item(item.get(), id))
						{
							cache->dynamic.items.push_back(item.release());
							TotalMenuCount++;
						}
						break;
					}
					case IDENT_IMPORT:
					{
						skip();
						bool bloc = false;
						if(l->peek_ident(IDENT_LOC))
						{
							bloc = true;
							l->next(3);
						}
						else if(l->peek_ident(IDENT_LANG))
						{
							bloc = true;
							l->next(4);
						}

						auto ret = load_import(l->line, l->column, true, false);
						if(ret == 1)
						{
							if(bloc)
								parse_loc(false);
							else
								parse_config();
							pop_import();
						}
						break;
					}
					case IDENT_LANG:
					case IDENT_LOC:
					{
						parse_loc(true);
						break;
					}
					default:
					{
						if(id == 0)
						{
							if(l->eof)
								break;
							if(parse_variable(&cache->variables.global))
								break;
							if(parse_image())
								break;
						}
						error(TokenError::IdentifierConfigUnexpected, prevCol);
						break;
					}
				}
			}
		}

		//expected a ')'
		//params
		bool Parser::Load()
		{
			bool result = false;
			try
			{
				_theme = { 
					0, IDENT_BACKGROUND, {
						{ 0, IDENT_OPACITY },
						{ 0, IDENT_EFFECT }
					}
				};

				if(_imports.empty() || (l->length == 0 && !m_error))
					return true;
			
				context.Runtime = false;

				auto cache = context.Cache;
				cache->dynamic.type = NativeMenuType::Menu;
				cache->dynamic.fso.set(TRUE);

				location = Path::Parent(l->path);
				parse_config();

				result = /*parse_config() && */!m_error;
				if(result)
				{
				}
			}
			catch(const ParserException&)
			{
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return result;
		}
	}
}