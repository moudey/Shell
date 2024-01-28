#include <pch.h>

namespace Nilesoft
{
	namespace Shell
	{
		bool Parser::parse_property_ident(Ident &id, bool singleLineComment)
		{
			skip(singleLineComment);
			prevCol = l->column - 1;

			if(!l->is_alpha()) return false;

			auto col = prevCol;
			uint32_t id_index = 0;
			wchar_t c = 0;
			Hash h;
			do
			{
				c = l->next();
				h.hash(c);
				if(!l->is_iddigit()) id.push_back(h, (uint32_t)col);
				if(l->is({ '.', '-' }))
				{
					h = Hash::offset_basis;
					col = l->column + 1;
					if(++id_index > 5)
						return error(TokenError::IdentifierMaxLength, l->column);

					if(!l->is_iddigit(1)) break;
					id.root = l->tok;
					l->next();
				}
			} while(l->is_iddigit());

			if(id.zero())
			{
				l->column = prevCol;
				return false;
			}
			return true;
		}

		Expression *Parser::parse_property(Ident &id, bool as_value)
		{
			bool singleLineComment = true;
			wchar_t quote = 0;
			//auto col = l->column;

			skip();

			if(as_value)
			{
				Ident val;
				if(l->is_squote())
				{
					singleLineComment = false;
					quote = l->tok;
					l->next(); // skipe quote
					skip(singleLineComment, as_value);
					if(l->is_squote())
					{
						//value is empty
						l->next(); // skipe quote
						return nullptr;
					}
				}

				std::unique_ptr<Expression> e;

				if(l->is_alpha())
				{
					if(id.front({ MENU_IMAGE, MENU_ICON }))
					{
						uint32_t id_value = 0;
						if(auto count = peek_ident({ IDENT_INHERIT, IDENT_PARENT, IDENT_CMD }, &id_value))
						{
							l->next(count);
							id.push_back(id_value);
						}
						else
						{
							e.reset(parse_root_expression());
						}
					}
					else
					{
						error_if(!parse_property_ident(val, singleLineComment), TokenError::PropertyValue);
						id.push_back(val);
					}
				}
				else
				{
					e.reset(parse_root_expression());
				}

				skip(singleLineComment, as_value);

				if(quote != 0)
				{
					error_if(!l->is_squote(), TokenError::CloseSingleQuoteExpected);
					l->next();
				}
				return e.release();
			}

			return parse_root_expression();
		}

		Expression *Parser::parse_property__()
		{
			skip();
			return parse_root_expression();
		}

		bool Parser::parse_property(FileSystemObjects &fso)
		{
			error_if(!l->is_quote(), TokenError::OpenQuoteExpected, prevCol);

			auto start_column = l->column, first_col = l->column;// skip the end single quote;

			int backs = 0;
			int drives = 0;
			int namespaces = 0;

			uint32_t count = 0;
			// skip the start Quote
			auto quote = l->next();// Skip surrounding quotation marks

			while(l->tok != quote)
			{
				skip(false);
				start_column = l->column;
				
				error_if(l->eof, TokenError::PropertyValueUnterminated, start_column);

				if(l->tok == quote) break;
				else if(l->tok == '|') l->next();
				else if(l->tok == '*')
				{
					fso.any_types = true;
					l->next();
				}
				else if(l->is_alpha() || l->is({ '-', '~' }))
				{
					auto ret = FALSE;
					start_column = l->column;
					Hash id;
					if(l->next_if(l->is({ '-', '~' })))
					{
						ret = FileSystemObjects::EXCLUDE;
						fso.exclude++;
						 error_if(!l->is_alpha(), TokenError::PropertyType, start_column);
					}
					else
					{
						ret = TRUE;
						count++;
					}

					start_column = l->column;

					while(l->is_alpha() && !l->eof) id.hash(l->next());

					Hash front = id;
					bool back = front.equals(IDENT_TYPE_BACK);
					bool back_all = back;

					//back.dir, dir.back
					if(l->next_is('.'))
					{
						back_all = false;
						auto type_back = fso.is_type_back(front);
						if(!back && !type_back) goto _error;

						start_column = l->column;

						id = Hash::offset_basis;
						while(l->is_alpha() && !l->eof) id.hash(l->next());
						start_column = l->column;

						auto _back = id.equals(IDENT_TYPE_BACK);
						auto _type = fso.is_type_back(id);

						if(back && (_back || !_type)) goto _error;

						if(_back) id = front;

						back = true;
					}

					switch(id)
					{
						case IDENT_TYPE_TITLEBAR:
							fso.Types[FSO_TITLEBAR] = ret;
							fso.Types[FSO_COUNT]++;
							break;
						case IDENT_TYPE_EDIT:
							fso.Types[FSO_EDIT] = ret;
							fso.Types[FSO_COUNT]++;
							break;
						case IDENT_TYPE_START:
							fso.Types[FSO_START] = ret;
							fso.Types[FSO_COUNT]++;
							break;
						case IDENT_TYPE_TASKBAR:
							fso.Types[FSO_TASKBAR] = ret;
							fso.Types[FSO_COUNT]++;
							break;
						case IDENT_TYPE_BACK:
							fso.Types[FSO_BACK] = ret;
							break;
						case IDENT_TYPE_DESKTOP:
							fso.Types[FSO_DESKTOP] = ret;
							fso.Types[FSO_COUNT]++;
							break;
						case IDENT_TYPE_DIR:
						case IDENT_TYPE_DIRECTORY:
							if(back)
							{
								if(fso.excluded(FSO_BACK)) goto _error;
								fso.Types[FSO_BACK_DIRECTORY] = ret;
								backs++;
							}
							else
							{
								if(fso.excluded(FSO_DIRECTORY)) goto _error;
								fso.Types[FSO_DIRECTORY] = ret;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_FILE:
							fso.Types[FSO_FILE] = ret;
							fso.Types[FSO_COUNT]++;
							break;
						case IDENT_TYPE_NAMESPACE:
							if(back)
							{
								if(fso.excluded(FSO_BACK)) goto _error;
								fso.Types[FSO_BACK_NAMESPACE] = ret;
								backs++;
							}
							else
							{
								fso.Types[FSO_NAMESPACE] = ret;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_COMPUTER:
							if(back)
							{
								fso.Types[FSO_BACK_NAMESPACE_COMPUTER] = ret;
								backs++;
							}
							else
							{
								fso.Types[FSO_NAMESPACE_COMPUTER] = ret;
								namespaces++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_RECYCLEBIN:
							if(back)
							{
								fso.Types[FSO_BACK_NAMESPACE_RECYCLEBIN] = ret;
								backs++;
							}
							else
							{
								fso.Types[FSO_NAMESPACE_RECYCLEBIN] = ret;
								namespaces++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_ROOT:
						case IDENT_TYPE_DRIVE:
							if(back)
							{
								if(fso.excluded(FSO_BACK_DRIVE)) goto _error;
								fso.Types[FSO_BACK_DRIVE] = ret;
								backs++;
							}
							else
							{
								//	if(fso.excluded({ FSO_FIXED, FSO_DVD, FSO_USB, FSO_REMOVABLE }))
								//		goto _error;
								fso.Types[FSO_DRIVE] = ret;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_FIXED:
							if(back)
							{
								if(fso.excluded(FSO_BACK_DRIVE)) goto _error;
								fso.Types[FSO_BACK_DRIVE_FIXED] = ret;
								backs++;
							}
							else
							{
								if(fso.excluded(FSO_DRIVE)) goto _error;
								fso.Types[FSO_DRIVE_FIXED] = ret;
								drives++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_VHD:
							if(back)
							{
								if(fso.excluded(FSO_BACK_DRIVE)) goto _error;
								fso.Types[FSO_BACK_DRIVE_VHD] = ret;
								backs++;
							}
							else
							{
								if(fso.excluded(FSO_DRIVE)) goto _error;
								fso.Types[FSO_DRIVE_VHD] = ret;
								drives++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_REMOVABLE:
							if(back)
							{
								if(fso.excluded(FSO_BACK_DRIVE)) goto _error;
								fso.Types[FSO_BACK_DRIVE_REMOVABLE] = ret;
								backs++;
							}
							else
							{
								if(fso.excluded(FSO_DRIVE)) goto _error;
								fso.Types[FSO_DRIVE_REMOVABLE] = ret;
								drives++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_USB:
							if(back)
							{
								if(fso.excluded(FSO_BACK_DRIVE)) goto _error;
								fso.Types[FSO_BACK_DRIVE_USB] = ret;
								backs++;
							}
							else
							{
								if(fso.excluded(FSO_DRIVE)) goto _error;
								fso.Types[FSO_DRIVE_USB] = ret;
								drives++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_DVD:
							if(back)
							{
								if(fso.excluded(FSO_BACK_DRIVE)) goto _error;
								fso.Types[FSO_BACK_DRIVE_DVD] = ret;
								backs++;
							}
							else
							{
								if(fso.excluded(FSO_DRIVE)) goto _error;
								fso.Types[FSO_DRIVE_DVD] = ret;
								drives++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						case IDENT_TYPE_REMOTE:
							if(back)
							{
								fso.Types[FSO_BACK_DRIVE_REMOTE] = ret;
								backs++;
							}
							else
							{
								fso.Types[FSO_DRIVE_REMOTE] = ret;
								drives++;
								fso.Types[FSO_COUNT]++;
							}
							break;
						default:
							goto _error;
					}

					if(fso.exclude && count)
					{
						// error_if((fso.exclude != fso.count), TokenError::PropertyNoneExcludeValueUnexpected, start_column + 1);
					}
				}
				else
					goto _error;
			}

			if(count > 0)
				fso.count = count;
			else if(!fso.exclude)
			{
				fso.set(true);
				fso.any_types = true;
			}

			fso.defined = true;

			// skip the end single quote
			error_if(!l->next_is(quote), TokenError::CloseQuoteExpected, first_col);

			if((fso.Types[FSO_BACK] == TRUE && backs) ||
			   (fso.Types[FSO_DRIVE] == TRUE && drives) ||
			   (fso.Types[FSO_NAMESPACE] == TRUE && namespaces))
			{
				return error(TokenError::PropertyTypeAndSubTypeCannotCombine, start_column);
			}
			return true;

		_error:
			return error(TokenError::PropertyType, start_column);
		}

/*						case MENU_IMAGE:
						{
							error_if(menu->checked.expr || menu->checked.value != 0,
								TokenError::PropertyImageAndCheckedCannotCombine, start_column);

							if(menu->image.expr || menu->image.import != ImageImport::None)
								return error(TokenError::PropertyDuplicate, start_column);

							if(id.equals(IDENT_DISABLED))
								menu->image.import = ImageImport::Disabled;
							else if(id.equals(IDENT_ENABLED))
								menu->image.import = ImageImport::None;
							else if(id.equals({ IDENT_INHERIT, IDENT_PARENT }))
								menu->image.import = ImageImport::Inherit;
							else if(id.equals(IDENT_CMD))
								menu->image.import = ImageImport::Command;
							else if(id.equals({IDENT_NONE, IDENT_NULL }))
								menu->image.import = ImageImport::None;
							else
							{
								if(!id.equals({ MENU_IMAGE, MENU_ICON })) goto property_value;

								if(!id.signer)
									menu->image.import = ImageImport::Command;
								else if(expr)
								{
									menu->image.expr = expr.release();
									menu->image.import = ImageImport::Image;
								}
							}
							break;
						}*/
		bool Parser::parse_properties_command(CommandProperty *cmd, Ident const &id, std::unique_ptr<Expression> *expr)
		{
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
						{
							if(id[1] == MENU_CMD_LINE || id[1] == MENU_CMD_PROMPT)
								cmd->command.type = COMMAND_PROMPT;
							else if(id[1] == MENU_CMD_POWERSHELL || id[1] == MENU_CMD_PS)
								cmd->command.type = COMMAND_POWERSHELL;
							else if(id[1] == MENU_CMD_PWSH)
								cmd->command.type = COMMAND_PWSH;
							else if(id[1] == MENU_CMD_SHELL)
								cmd->command.type = COMMAND_SHELL;
							else if(id[1] == MENU_CMD_EXPLORER)
								cmd->command.type = COMMAND_EXPLORER;
							
							error_if(cmd->arguments, TokenError::PropertyCommandAndArgumentsCannotCombine);
							cmd->arguments = expr->release();
							break;
						}
						default:
						{
							if(!id.signer)
								cmd->command.type = COMMAND_PROMPT;
							else
							{
								cmd->command.type = COMMAND_EXPRESSION;
								if(cmd->command.expr) goto property_duplicate;
								cmd->command.expr = expr->release();
							}
							break;
						}
					}
					break;
				}
				case MENU_ARG:
				case MENU_ARGUMENT:
				case MENU_ARGS:
				case MENU_ARGUMENTS:
				{
					error_if(cmd->command.type > COMMAND_EXPRESSION && cmd->command.expr,
							 TokenError::PropertyCommandAndArgumentsCannotCombine);

					if(cmd->arguments) goto property_duplicate;

					cmd->arguments = expr->release();
					//menu->for_each = id.equals({ MENU_ARGS, MENU_ARGUMENTS }) ? 1 : 0;
					break;
				}
				case MENU_ADMIN:
					cmd->admin = id.signer ? expr->release() : new NumberExpression((int)Privileges::Default);
					break;
				case MENU_WAIT:
					cmd->wait = id.signer ? expr->release() : new NumberExpression(1);
					break;
				case MENU_INVOKE:
					cmd->invoke = id.signer ? expr->release() : new NumberExpression(1);
					break;
				//case state
				case MENU_WINDOW:
					cmd->window = expr->release();
					break;
				case MENU_VERB:
					cmd->verb = expr->release();
					break;
				case MENU_DIR:
				case MENU_DIRECTORY:
					cmd->directory = id.signer ? expr->release() : new NumberExpression(1);
					break;
				default: goto property_value_unexpected;
			}
			return !m_error;

		property_duplicate:
			return error(TokenError::PropertyDuplicate, prevCol);

		property_value_unexpected:
			return error(TokenError::PropertyValue);
		}

		bool Parser::parse_properties_commands(CommandProperty *cmd)
		{
			while(l->tok != '}')
			{
				auto start_column = l->column;
				Ident id;
				parse_property_ident(id);
				id.signer = expect_assign();
				auto verified = verify_command(id, id.signer);
				if(verified != TokenError::None)
					return error(verified, start_column);
				
				std::unique_ptr<Expression> expr;
				auto col = l->column;

				if(!skip()) return false;
				
				if(id.signer)
				{
				//	auto prarse_as_value = id.front({ MENU_WINDOW });
					const auto l = id.length();
					expr.reset(parse_property(id, false));
					//	return error(TokenError::PropertyValue, col);
					if(l == id.length() && !expr)
						continue;

				}
				else if(l->tok == '{')
				{
					error_if(!id.equals({ MENU_ARGS, MENU_ARGUMENTS }),
							 TokenError::AssignExpected, col);

					col = l->column;
					l->next();
					expr.reset(parse_statement());
					error_if(!expr, TokenError::PropertyValue, col);
				}

				error_if(!parse_properties_command(cmd, id, &expr),
					  TokenError::PropertyValue, start_column);
				
				skip();

				if(l->is({ '}', ',' })) break;
			}
			
			//	if(!(cmd->command.expr || cmd->command.console))
			//		error(TokenError::PropertyCommandExpected, start_column0);

			return !m_error;
		}

		bool Parser::parse_properties_commands(NativeMenu *menu)
		{
			expect_openCurly();

			while(l->tok != '}')
			{
				skip();
				if(l->tok == '}') break;
				if(l->next_is(',')) continue;

				start_col = prevCol;
				std::unique_ptr<CommandProperty> cmd (new CommandProperty());
				if(!parse_properties_commands(cmd.get()))
					return false;

				error_if(cmd->command.type == COMMAND_INVALID,
						 TokenError::PropertyCommandExpected, start_col);

				menu->commands.push_back(cmd.release());
			}

			return expect_closeCurly();
		}

		//Properties-Property
		bool Parser::parse_properties(NativeMenu *menu)
		{
			if(!expect('('))
			{
				if(menu->is_separator() || menu->is_main()) return true;
				error(TokenError::OpenParenExpected, prevCol);
			}

			auto start_column = l->column;

			// parse menu properties
			while(l->tok != ')')
			{
				skip();

				if(l->tok == ')') break;
				
				if(!l->is_ident())
					error(TokenError::PropertyUnexpected, l->column);

				start_column = l->column;
				Ident id;
				parse_property_ident(id);

				id.signer = expect_assign();
				auto verified = verify(menu, id, id.signer);
				error_if(verified != TokenError::None, verified, start_column);

				std::unique_ptr<Expression> expr;

				prevCol = l->column - 1;

				if(id.signer)
				{
					skip();
					prevCol = l->column - 1;
					if(id.front(MENU_TYPE))
					{
						FileSystemObjects fso;
						if(!parse_property(fso))
							return false;

						//if(fso.count || fso.exclude)
							menu->fso = fso;
					}
					else
					{
						//auto col = l->column;
					//	auto prarse_as_value = id.front({MENU_WINDOW});

						const auto l = id.length();
						expr.reset(parse_property(id, false));
						//	return error(TokenError::PropertyValue, col);
						if(l == id.length() && !expr)
							continue;
					}
				}

				if(id.front(MENU_COMMANDS))
				{
					if(!parse_properties_commands(menu))
						return false;
				}
				else
				{
					if(!expr && menu->properties > 0)
					{
						if(id.signer && !id.front(MENU_TYPE))
							menu->properties--;
					}
					
					switch(id[0])
					{
						case MENU_TYPE:
							break;
						case MENU_CONDITION:
						case MENU_WHERE:
							menu->where = expr.release();
							break;
						case MENU_VIS:
						case MENU_VISIBILITY:
							menu->visibility = expr.release();
							break;
						case MENU_SEL:
						case MENU_MODE:
							menu->mode = expr.release();
							break;
						case MENU_FIND:
							menu->find = expr.release();
							break;
						case MENU_POS:
						case MENU_POSITION:
							menu->position = expr.release();
							break;
						case MENU_TEXT:
						case MENU_TITLE:
							menu->title = expr.release();
							break;
						case MENU_TIP:
							menu->tip = expr.release();
							break;
						case MENU_SUB:
						case IDENT_MENU:
						case IDENT_MOVE:
						case MENU_PARENT:
							menu->moveto = expr.release();
							break;
						case MENU_SEP:
						case MENU_SEPARATOR:
							menu->separator = id.signer ? expr.release() : new NumberExpression(IDENT_BOTH);
							break;
						case MENU_CHECKED:
							menu->checked = id.signer ? expr.release() : new NumberExpression(1);
							break;
						case MENU_COLUMN:
						case MENU_COL:
							menu->column = id.signer ? expr.release() : new NumberExpression(1);
						case MENU_KEYS:
							menu->keys = expr.release();
							break;
						case MENU_EXPANDED:
							menu->expanded = id.signer ? expr.release() : new NumberExpression(1);
							break;
						case MENU_ICON:
						case MENU_IMAGE:
						{
							if(id.length() == 2 && (id[1] == IDENT_SEL || id[1] == IDENT_SELECT))
							{
								menu->images.select = expr.release();
								break;
							}

							if(menu->image.expr || menu->image.import != ImageImport::None)
								return error(TokenError::PropertyDuplicate, start_column);

							if(id.equals(IDENT_DISABLED))
								menu->image.import = ImageImport::Disabled;
							else if(id.equals(IDENT_ENABLED))
								menu->image.import = ImageImport::None;
							else if(id.equals({ IDENT_INHERIT, IDENT_PARENT }))
								menu->image.import = ImageImport::Inherit;
							else if(id.equals(IDENT_CMD))
								menu->image.import = ImageImport::Command;
							else if(id.equals({ IDENT_NONE, IDENT_NULL, IDENT_NIL }))
								menu->image.import = ImageImport::None;
							else
							{
								if(!id.equals({ MENU_IMAGE, MENU_ICON })) goto error_property_value;

								if(!id.signer)
									menu->image.import = ImageImport::Command;
								else if(expr)
								{
									menu->image.expr = expr.release();
									menu->image.import = ImageImport::Image;
								}
							}

							menu->image.defined =
								menu->image.import == ImageImport::Inherit ||
								menu->image.import == ImageImport::Command ||
								menu->image.import == ImageImport::Image;

							break;
						}
						default:
						{
							if(!parse_properties_command(menu->cmd, id, &expr))
								goto error_property_value;
							break;
						}
					}
				}
				menu->properties++;
			}

			return expect_closeParen();

	//	error_property_duplicate:
	//		return error(TokenError::PropertyDuplicate, prevCol);

		error_property_value:
			return error(TokenError::PropertyValue, start_column);
		}

//#define case_(value) breake; case (vlaue):
//#define case_or(value) case (vlaue):
//#define case_or(value) case (vlaue):


		bool Parser::parse_clsid(NativeMenu *item)
		{
			error_if(!l->is_quote(), TokenError::OpenQuoteExpected, prevCol);

			auto start_column = l->column, first_col = l->column;// skip the end single quote;

			string value;

			// skip the start Quote
			auto quote = l->next();// Skip surrounding quotation marks
			while(l->tok != quote)
			{
				skip();
				start_column = l->column;
				error_if(l->eof, TokenError::PropertyValueUnterminated, start_column);

				if(l->eof || l->tok == quote)
					break;
				if(l->tok == L'|')
				{
					if(!value.trim().empty())
					{
						Guid guid;
						if(guid.parse(value.c_str()))
							item->clsid.emplace_back(guid);
					}
					value = nullptr;
					l->next();
					continue;
				}
				value.append(l->next());
			}

			if(!value.trim().empty())
			{
				Guid guid;
				if(guid.parse(value.c_str()))
					item->clsid.emplace_back(guid);
			}

			// skip the end single quote
			error_if(!l->next_is(quote), TokenError::CloseQuoteExpected, first_col);
			return true;
		}

		bool Parser::parse_modify_properties(NativeMenu *item, uint32_t action)
		{
			auto start_item = l->column;
			error_if(!expect('('), TokenError::OpenParenExpected, prevCol);

			auto _parse_expression = [&](auto_expr& dst, bool skip = false) {
				dst = parse_expression();
				if(skip) dst.reset();
				item->properties += dst != nullptr;
			};

			auto start_column = l->column;
			// parse static  properties
			while(l->tok != ')')
			{
				skip();

				if(l->tok == ')') break;

				start_column = l->column;
				Ident id;
				parse_property_ident(id);
				id.signer = expect_assign(!id.equals(0, { MENU_SEP, MENU_SEPARATOR }));

				prevCol = l->column;

				switch(id[0])
				{
					case MENU_TYPE:
					{
						FileSystemObjects fso;
						if(!parse_property(fso))
							return false;
						//if(fso.count || fso.exclude)
							item->fso = fso;
							item->properties++;
						break;
					}
					case MENU_CLSID:
					{
						item->properties++;
						parse_clsid(item);
						item->has_clsid = true;
						break;
					}
					case IDENT_PATH:
					case IDENT_LOCATION:
					case MENU_IN:
						_parse_expression(item->location);
						break;
					case MENU_WHERE:
					case MENU_CONDITION:
						_parse_expression(item->where);
						break;
					case MENU_FIND:
						_parse_expression(item->find);
						break;
					case MENU_VIS:
					case MENU_VISIBILITY:
						_parse_expression(item->visibility);
						break;
					case MENU_SEL:
					case MENU_MODE:
						_parse_expression(item->mode);
						break;
					case MENU_TEXT:
					case MENU_TITLE:
						_parse_expression(item->title, action == IDENT_REMOVE);
						break;
					case MENU_TIP:
						_parse_expression(item->tip, action == IDENT_REMOVE);
						break;
					case MENU_SUB:
					case IDENT_MENU:
					case IDENT_MOVE:
					case MENU_PARENT:
						_parse_expression(item->moveto, action == IDENT_REMOVE);
						break;
					case MENU_POS:
					case MENU_POSITION:
						_parse_expression(item->position, action == IDENT_REMOVE);
						break;
					case MENU_SEP:
					case MENU_SEPARATOR:
						if(id.signer)
							_parse_expression(item->separator, action == IDENT_REMOVE);
						else if(action != IDENT_REMOVE)
						{
							item->separator =new NumberExpression(IDENT_BOTH);
							item->properties++;
						}
						break;
					case MENU_ICON:
					case MENU_IMAGE:
						if(id.length() == 2 && id.equals(1, { IDENT_SEL, IDENT_SELECT }))
							_parse_expression(item->images.select, action == IDENT_REMOVE);
						else
							_parse_expression(item->images.normal, action == IDENT_REMOVE);
						break;
					case MENU_KEYS:
						_parse_expression(item->keys, action == IDENT_REMOVE);
						break;
					case MENU_CHECKED:
						if(id.signer)
							_parse_expression(item->checked, action == IDENT_REMOVE);
						else if(action != IDENT_REMOVE)
						{
							item->checked = new NumberExpression(1);
							item->properties++;
						}
						break;
					case MENU_INVOKE:
						if(id.signer)
							_parse_expression(item->invoke, action == IDENT_REMOVE);
						else if(action != IDENT_REMOVE)
						{
							item->invoke = new NumberExpression(1);
							item->properties++;
						}
						break;
					default:
						return error(TokenError::PropertyUndefined, start_column);
				}
			}

			if(!item->has_clsid)
			{
				if(action == IDENT_REMOVE && !item->visibility)
				{
					item->visibility = new NumberExpression(0);
					item->properties++;
				}

				error_if(!item->find && !item->where, TokenError::PropertyFindOrWhereExpected, start_item);
				error_if(item->properties < 2, TokenError::PropertyExpected, start_item);
			}

			return expect_closeParen();
		}
	}
}
