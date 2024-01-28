
#include <pch.h>
#include "Expression\Variable.h"

// Windows State (Hidden | Normal | Minimized | Maximized)
namespace Nilesoft
{
	namespace Shell
	{
		auto Parser::parse_operator(OperatorType precedence)
		{
			skip();

			int move_count = 0;

			auto ot = OperatorType::None;

			switch(l->tok)
			{
				case L'?': ot = OperatorType::Question; break;
				case L'^': ot = OperatorType::BitwiseXor; break;
				//case L'~': ot = OperatorType::BitwiseNot; break;

				case L'*': ot = OperatorType::Multiply; break;
				case L'%': ot = OperatorType::Modulo; break;
				case L'/': ot = OperatorType::Divide; break;
				
				// arithmetic
				case L'+': 
				{
					ot = OperatorType::Plus;
					if(l->peek == L'=')
					{
						move_count++;
						ot = OperatorType::PlusEqual;
					}
					break;
				}
				case L'-':
				{
					ot = OperatorType::Minus;
					if(l->peek == L'=')
					{
						move_count++;
						ot = OperatorType::MinusEqual;
					}
					break;
				}
				// equality
				case L'=':
				{
					ot = OperatorType::Equal;
					if(l->peek == L'=')
					{
						move_count++;
						ot = OperatorType::EqualEqual;
					}
					break;
				}
				// compare
				case L'>':
				{
					ot = OperatorType::GreaterThan;
					if(l->peek == L'>')
					{
						move_count++;
						ot = OperatorType::BitwiseRightShift;
					}
					else if(l->peek == L'=')
					{
						move_count++;
						ot = OperatorType::GreaterThanOrEqual;
					}
					break;
				}
				case L'<':
				{
					if(l->peek == L'<')
					{
						move_count++;
						ot = OperatorType::BitwiseLeftShift;
					}
					else if(l->peek == L'=')
					{
						move_count++;
						ot = OperatorType::LessThanOrEqual;
					}
					else ot = OperatorType::LessThan;
					break;
				}
				case L'&':
				{
					if(l->peek == L'&')
					{
						move_count++;
						ot = OperatorType::LogicalAnd;
					}
					else ot = OperatorType::BitwiseAnd;
					break;
				}
				case L'~':
				{
					ot = OperatorType::BitwiseNot;
					break;
				}
				case L'|':
				{
					if(l->peek == L'|')
					{
						move_count++;
						ot = OperatorType::LogicalOr;
					}
					else ot = OperatorType::BitwiseOr;
					break;
				}
				case L'!':
				{
					ot = OperatorType::LogicalNot;
					if(l->peek == L'=')
					{
						move_count++;
						ot = OperatorType::NotEqual;
					}
					break;
				}
				default:
				{
					auto id = l->peek_ident();
					if(id==IDENT_OR)
					{
						move_count += 1;
						ot = OperatorType::LogicalOr;
					}
					else if(id==IDENT_AND)
					{
						move_count += 2;
						ot = OperatorType::LogicalAnd;
					}
					else if(id == IDENT_NOT)
					{
						move_count += 2;
						ot = OperatorType::LogicalNot;
					}
					else 
						return OperatorType::None;
					break;
				}
			}

			if(precedence < ot)
				l->next(move_count + 1);
			return ot;
		}

		/*Expression *Parser::parse_char()
		{
			if(l->tok == '\\')
			{
				auto t = l->peek;
				if(t == 'x'|| t == 'u' || t == 'U')
				{
					l->next(); // skip '\'
					string value;
					auto c = l->next(); // skip the 'x', 'u' or 'U'
					wchar_t surrogate = wchar_t(0);//surrogate
					uint32_t v = 0;

					//\x, which is always followed by 1 to 4 hex digits
					//\u, which is always followed by 4 hex digits
					//\U, which is always followed by 8 hex digits
					if(!l->is_xdigit())
						error(TokenError::IllegalEscape);
					//U+0000 to U+FFFF
					// Unicode character in hexadecimal constants
					// must have at least 4 like [\xHHHH | \uHHHH] or 8 hexadecimal like \U00010000 to \U00010FFFF
					do
					{
						v = (v << 4) + l->to_xint<uint32_t>();
						l->next();
					} while(l->is_xdigit());

					//0x0000u   0x00000000U
					if(t == 'U')
					{
						if(v > 0x0010FFFF)
							error(TokenError::IllegalCharacterHexaDecimal);
						else if(v <= 0x0000FFFF)
							c = wchar_t(v);
						else
						{
							//ASSERT(v > 0x0000FFFF && v <= 0x0010FFFF);
							c = wchar_t((v - 0x00010000) / 0x0400 + 0xD800);
							surrogate = wchar_t((v - 0x00010000) % 0x0400 + 0xDC00);
						}

						value.append(c);
						if(surrogate != 0) value += surrogate;

					}
					else
					{
						if(v > 0x0000FFFF)
							error(TokenError::IllegalCharacterHexaDecimal);
						value.append(wchar_t(v));
					}

					return new StringExpression(value.move());
				}
			}
			return nullptr;
		}*/

		// string
		//	::= [@]["string"]
		Expression *Parser::parse_string()
		{
			if(!l->is_dquote()) return nullptr;
			 l->next(); // skip quote
			string value;

			for(; !l->is_dquote();)
			{
				error_if(l->eof, TokenError::StringTerminatedExpected);
				
				if(l->is_dquote())
					break;
				else if(l->tok == '\\')
				{
					l->next();	// Skip '\'
					switch(l->tok)
					{
						case '"':
						case '\\':
							value += l->tok;
						//	error(TokenError::StringCharacterEscapeSequence);
							break;
						case '0': value.append(L'\0'); break;
						case 'q': value.append(L'\"'); break;
						case 'a': value.append(L'\''); break;
						case 'n': value.append(L'\n'); break;
						case 'r': value.append(L'\r'); break;
						case 't': value.append(L'\t'); break;
						case 'v': value.append(L'\v'); break;
						case 'f': value.append(L'\f'); break;
						case 'b': value.append(L'\b'); break;
							//ASCII character in octal notation
							//case '0->7': break;
							// hexadecimal escape sequence /xHH
						// ASCII and Unicode character in hexadecimal notation.
						case 'x': 
						case 'u':
						case 'U':
						{
							auto c = l->next(); // skip the 'x', 'u' or 'U'
							wchar_t surrogate = wchar_t(0);//surrogate
							uint32_t v = 0;

							//\x, which is always followed by 1 to 4 hex digits
							//\u, which is always followed by 4 hex digits
							//\U, which is always followed by 8 hex digits
							
							if(!l->is_xdigit())
								error(TokenError::IllegalEscape);
							else for(int i = 0; i < 8; i++)
							{
								if(c != 'U' && i > 3)
									break;
								//In the case of a Unicode escape sequence, you must specify all four hexadecimal digits. 
								//That is, \u006A is a valid escape sequence, while \u06A and \u6A are not valid.

								//In the case of a hexadecimal escape sequence, you can omit the leading zeros.
								// That is, the \x006A, \x06A, and \x6A escape sequences are valid and correspond to the same character.
								if(!l->is_xdigit())
								{
									//U+0000 to U+FFFF
									// Unicode character escape sequence in hexadecimal constants
									// must have at least 4 like [\xHHHH | \uHHHH] or 8 hexadecimal like \U00010000 to \U00010FFFF
									error_if(c != 'x', TokenError::IllegalEscape);
									break;
								}
							
								v = (v << 4) + l->to_xint<uint32_t>();
								l->next();
							}

							if(v > 0x0010FFFF)
								error(TokenError::IllegalEscape);
							else if(v < 0x00010000)
								c = wchar_t(v);
							else
							{
								//ASSERT(v > 0x0000FFFF && v <= 0x0010FFFF);
								c = wchar_t((v - 0x00010000) / 0x0400 + 0xD800);
								surrogate = wchar_t((v - 0x00010000) % 0x0400 + 0xDC00);
							}

							value.append(c);
							if(surrogate != 0) value += surrogate;
							continue;
						}
						default:
						{
						//	if(!l->is_xdigit())
								error(TokenError::IllegalEscape);
							
							/*uint32_t v = 0;
							for(int i = 0; i < 2; i++)
							{
								error_if(!l->is_xdigit(), TokenError::IllegalEscape);
								v = (v << 4) + l->to_xint<uint32_t>();
								l->next();
							}
							value.append(wchar_t(v));*/
						}
					}
					l->next();
				}
				else
				{
					error_if(l->is({ '\n', '\r' }), TokenError::NewlineInStringUnexpected);//NewlineInConst
					value += l->next();
				}
			}

			// skip end quote
			error_if(!l->next_if(l->is_dquote()), TokenError::StringTerminatedExpected);
			return new StringExpression(value.move());
		}

		//	::= \uhhhh
		Expression* Parser::parse_char()
		{
			//if(l->tok == '\\' && l->is('u', 1, true))
			{
				wchar_t surrogate = wchar_t(0);//surrogate
				uint32_t v = 0;
				auto c = l->next(2); // skip the 'x', 'u' or 'U'

				if(!l->is_xdigit())
					error(TokenError::IllegalEscape);
				else for(int i = 0; i < 8; i++)
				{
					if(c != 'U' && i > 3)
						break;
					//In the case of a Unicode escape sequence, you must specify all four hexadecimal digits. 
					//That is, \u006A is a valid escape sequence, while \u06A and \u6A are not valid.

					//In the case of a hexadecimal escape sequence, you can omit the leading zeros.
					// That is, the \x006A, \x06A, and \x6A escape sequences are valid and correspond to the same character.
					if(!l->is_xdigit())
					{
						//U+0000 to U+FFFF
						// Unicode character escape sequence in hexadecimal constants
						// must have at least 4 like [\xHHHH | \uHHHH] or 8 hexadecimal like \U00010000 to \U00010FFFF
						error_if(c != 'x', TokenError::IllegalEscape);
						break;
					}

					v = (v << 4) + l->to_xint<uint32_t>();
					l->next();
				}

				if(v > 0x0010FFFF)
					error(TokenError::IllegalEscape);
				else if(v < 0x00010000)
					c = wchar_t(v);
				else
				{
					//ASSERT(v > 0x0000FFFF && v <= 0x0010FFFF);
					c = wchar_t((v - 0x00010000) / 0x0400 + 0xD800);
					surrogate = wchar_t((v - 0x00010000) % 0x0400 + 0xDC00);
				}

				string value = c;
				if(surrogate != 0) value += surrogate;

				return new StringExpression(value.move());
			}
			//return nullptr;
		}

		//0x555555#
		// numric
		//	::= [-][+][.][digit][0xHHHHHHHH] '-1.5' | '+1.5' | '-.5' | '1' | '0x000000ff'
		Expression *Parser::parse_number()
		{
			double val{};
			bool sign = false;
			bool isfloat = false;

			if(l->tok == '0' && l->is('x', 1))
			{
				l->next(2);
				int count = 0;
				while(l->is_xdigit())
				{
					val = double(((intptr_t)val << 4) | l->to_xint());
					l->next();
					count++;
				}
				
				error_if(count == 0, TokenError::HexaDecimalExpected);

				/*if(l->tok == 'u' || l->tok == 'U')
				{
					string value;
					auto c = l->next(); // skip the 'x', 'u' or 'U'
					auto v = (uint32_t)val;

					if(c == 'U')
					{
						wchar_t surrogate = wchar_t(0);//surrogate

						if(v > 0x0010FFFF)
							error(TokenError::IllegalCharacterHexaDecimal);
						else if(v <= 0x0000FFFF)
							c = wchar_t(v);
						else
						{
							//ASSERT(v > 0x0000FFFF && v <= 0x0010FFFF);
							c = wchar_t((v - 0x00010000) / 0x0400 + 0xD800);
							surrogate = wchar_t((v - 0x00010000) % 0x0400 + 0xDC00);
						}

						value.append(c);
						if(surrogate != 0) value += surrogate;

					}
					else
					{
						if(v > 0x0000FFFF)
							error(TokenError::IllegalCharacterHexaDecimal);
						value.append(wchar_t(v));
					}
					return new StringExpression(value.move());
				}*/
				
				return new NumberExpression(val);
			}
			/*			else if(!l->isdigit())
						{
							if(l->any_of({ '-', '+' }))
							{
								sign = ch == '-';
								if(l->isdigit(1) || (l->peek == '.' && l->isdigit(2)))
									next();
							}

							if(l->tok == '.' && l->isdigit(1))
							{
								isfloat = true;
								next();
							}

							if(!l->isdigit()) return nullptr;
						}
			*/

			if(l->is({ '-', '+' }))
			{
				sign = l->tok == '-';
				l->next();
			}

			int mantissa = 1;
			while(l->is_digit())
			{
				val = val * 10.0 + (l->next() - '0');
				mantissa = mantissa * 10;
			}

			if(isfloat)
				val = val / (double)mantissa;
			else if(l->tok == '.' && l->is_digit(1))
			{
				l->next();// skip '.'
				double mantissa_val = 0;
				mantissa = 1;
				while(l->is_digit())
				{
					mantissa_val = mantissa_val * 10.0 + (l->tok - '0');
					l->next();
					mantissa = mantissa * 10;
				}
				val += mantissa_val / (double)mantissa;
			}
			
			if(sign && val == 0) sign = false;
			return new NumberExpression(sign ? -val : val);
		}

		Expression *Parser::parse_color()
		{
			double val{};
			if(l->tok == '#' && l->is_xdigit(1))
			{
				l->next(1);
				int count = 0;

				while(l->is_xdigit())
				{
					val = double(((intptr_t)val << 4) | l->to_xint());
					l->next();
					count++;
				}

				/*
					#RGB
					#RGBA
					#RGBAA
					#RRGGBB
					#RRGGBBAA
				*/

				error_if(!((count >= 3 && count <= 6) || count == 8), TokenError::ColorInvalid);

				auto clr = static_cast<uint32_t>(val);

				uint8_t r = 0, g = 0, b = 0, a = 0xff;

				if(count >= 3 && count <= 5)
				{
					if(count == 3)
					{
						r = (clr & 0xf00) >> 8;
						g = (clr & 0x0f0) >> 4;
						b = (clr & 0x00f);
					}
					else if(count == 4)
					{
						r = (clr & 0xf000) >> 12;
						g = (clr & 0x0f00) >> 8;
						b = (clr & 0x00f0) >> 4;
						a = (clr & 0x000f);  a |= (a << 4);
					}
					else if(count == 5)
					{
						r = uint8_t((clr & 0xf0000) >> 16);
						g = (clr & 0x0f000) >> 12;
						b = (clr & 0x00f00) >> 8;
						a = (clr & 0x000ff);
					}

					r |= (r << 4);
					g |= (g << 4);
					b |= (b << 4);
				}
				else if(count == 6)
				{
					r = ((clr & 0xff0000) >> 16) & 0xff;
					g = (clr & 0x00ff00) >> 8;
					b = (clr & 0x0000ff);
				}
				else if(count == 8)
				{
					r = (clr & 0xff000000) >> 24;
					g = ((clr & 0x00ff0000) >> 16) & 0xff;
					b = (clr & 0x0000ff00) >> 8;
					a = (clr & 0x000000ff);
				}

				std::unique_ptr<NumberExpression> ret(new NumberExpression(Color(r, g, b, a), PrimitiveType::Color));
				
				skip();

				if(!l->next_is('.'))
					return ret.release();

				skip();

				auto col = l->column;
				auto line = l->line;
				std::unique_ptr<FuncExpression> func(parse_func());

				if(func->Id.equals({ IDENT_COLOR_LIGHT, IDENT_COLOR_DARK, IDENT_COLOR_INVERT, IDENT_OPACITY }))
				{
					auto id = func->Id[0];
					func->Id.set(0, IDENT_COLOR);
					func->Id.push_back(id);

					func->push_front(ret.release());
					return func.release();
				}
				error(TokenError::IdentifierUnexpected, col, line);
			}
			return nullptr;
		}

		// identifier
		//   ::= [@]letter[_][digit][.]
		bool Parser::parse_ident(Ident &id, bool allow_dash)
		{
			if(allow_dash)
			{
				id.clear();
				skip();
			}

			if(!l->is_alpha()) return false;

			prevCol = l->column - 1;
			auto col = l->column;
			uint32_t id_index = 0;
			wchar_t c = 0;
			Hash h;
			do
			{
				c = l->next();
				h.hash(c);
				if(!l->is_iddigit())
				{
					id.push_back(h, (uint32_t)col);
				}

				if(l->tok == '.' || (l->tok == '-' && allow_dash))
				{
					h = Hash::offset_basis;
					col = l->column + 1;

					error_if(++id_index >= Ident::max_size, TokenError::IdentifierMaxLength, l->column);

					if(!l->is_alpha(1)) break;
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

		// identifier
		//	::= [@]identifier[([[expr][,expr]])]
		//	ident | ident() | ident(arg, ...)	
		//  str.sub('12 354', 2).trim(' ');
		FuncExpression *Parser::parse_func(Expression *parent)
		{
			prevCol = l->column - 1;
			//auto ident_col = l->column;
			Ident ident;

			if(!parse_ident(ident))
				return nullptr;

			prevCol = l->column - 1;

			std::vector<std::unique_ptr<Expression>> args;

			bool brackets = false;
			bool _hasdot = false;

			if(l->next_is('(')) // skip '('
			{
				brackets = true;
				// create the list of arguments
				while(l->tok != ')' && !l->eof)
				{
					prevCol = l->column;
					std::unique_ptr<Expression> arg(parse_expression());
					error_if(!arg, TokenError::ExpressionUnexpected, prevCol);

					args.emplace_back(std::move(arg));

					if(l->tok == ')') break;
					// skip comma
					error_if(!l->next_is(','), TokenError::ExpressionUnexpected, prevCol);
				}

				// skip ')'
				error_if(!l->next_is(')'), TokenError::CloseParenExpected);

				_hasdot = l->next_is('.');
			}

			std::unique_ptr<FuncExpression> func(new FuncExpression(ident));
			func->Brackets = brackets;
			func->Parent = parent;

			for(auto &arg : args)
				func->push_back(arg.release());

			return func.release();
			/*
			auto type = verify_ident(ident, args.size(), hasedot);
			switch(type)
			{
				case ExpressionType::Variable:
				case ExpressionType::RuntimeVariable:
					return new VariableExpression(ident);
				case ExpressionType::Constant:
					return new ConstantExpression(ident);
				case ExpressionType::Identifier:
				{
					if(ident.equals(IDENT_FOREACH))
					{
						error_if(!args[0]->IsVariable(), TokenError::IdentifierArgumentsUnexpected, ident_col);

						auto id = &args[1]->ident()->Id;
						if(!id->equals({ IDENT_SEL, IDENT_TYPES, IDENT_PATHS, IDENT_SHORTS,
									   IDENT_DIRS,IDENT_DIRECTORIES,IDENT_FILES, IDENT_NAMES,IDENT_EXTS,
									   IDENT_TITLES, IDENT_DRIVERS, IDENT_ROOTS, IDENT_NAMESPACES, IDENT_USERS }))
						{
							error(TokenError::IdentifierArgumentsUnexpected, ident_col);
						}
					}
					else if(ident.equals(IDENT_FOR))
					{
						error_if(!args[0]->IsAssign(), TokenError::IdentifierArgumentsUnexpected, ident_col);
					}

					std::unique_ptr<IdentifierExpression> func(new IdentifierExpression(ident));
					func->Brackets = brackets;

					if(_hasdot)
						func->SetChild(parse_identifier(true));

					for(auto &arg : args)
						func->push_back(arg.release());

					return func.release();
				}
				default:
					error(error_code, ident_col);
			}
			return nullptr;*/
		}

		// identifier
		//	::= [@]identifier[([[expr][,expr]])]
		//	ident | ident() | ident(arg, ...)	
		//  str.sub('12 354', 2).trim(' ');
		Expression *Parser::parse_identifier(Expression *parent = nullptr, bool hasedot, bool inside_quotes, bool is_var)
		{
			prevCol = l->column - 1;
			auto ident_col = l->column;
			Ident ident;

			if(!parse_ident(ident))
				return nullptr;

			prevCol = l->column - 1;

			if(ident[0] == IDENT_FOR)
				return parse_for_statement(parent);

			std::vector<std::unique_ptr<Expression>> args;
			std::unique_ptr<Expression> _array;

			bool brackets = false;
			bool _hasdot = false;
			
			skip(true, !inside_quotes);

			if(l->next_is('[')) // skip '('
			{
				skip(true, !inside_quotes);
				prevCol = l->column;
				_array.reset(parse_expression(parent));
				error_if(!_array, TokenError::ExpressionUnexpected, prevCol);
				error_if(!l->next_is(']'), TokenError::CloseBracketExpected);
				skip(true, !inside_quotes);
			}

			if(l->next_is('.'))
			{
				Ident ident2;
				if(parse_ident(ident2))
				{
					for(auto i = 0u; i< ident2.length(); i++)
					{
						ident.push_back(ident2[i]);
					}
					skip(true, !inside_quotes);
				}
			}

			if(l->next_is('(')) // skip '('
			{
				brackets = true;
				if(is_var)
					skip(true, !inside_quotes);
				else 
				{
					// create the list of arguments
					while(l->tok != ')' && !l->eof)
					{
						prevCol = l->column;
						std::unique_ptr<Expression> arg(parse_expression(parent));
						error_if(!arg, TokenError::ExpressionUnexpected, prevCol);

						args.emplace_back(std::move(arg));

						if(l->tok == ')') break;
						// skip comma
						error_if(!l->next_is(','), TokenError::ExpressionUnexpected, prevCol);
					}
				}

				// skip ')'
				error_if(!l->next_is(')'), TokenError::CloseParenExpected);

				if(!is_var)
					_hasdot = l->next_is('.');
			}
			
			if(is_var)
			{
				std::unique_ptr<VariableExpression> exp(new VariableExpression(ident, parent));
				exp->Array = _array.release();

				if(_hasdot && brackets)
				{
					std::unique_ptr<FuncExpression> func((FuncExpression *)(parse_identifier(parent, true)));
					exp->Parent = func.get();
					func->Arguments.insert(func->Arguments.begin(), exp.release());
					func->extented = true;
					return func.release();
				}
				return exp.release();
			}

			auto type = verify_ident(ident, args.size(), hasedot);
			switch(type)
			{
				case ExpressionType::Variable:
				case ExpressionType::RuntimeVariable:
				{
					std::unique_ptr<VariableExpression> exp(new VariableExpression(ident, parent));
					exp->Array = _array.release();

					if(_hasdot && brackets)
					{
						std::unique_ptr<FuncExpression> func((FuncExpression *)(parse_identifier(parent, true)));
						exp->Parent = func.get();
						func->Arguments.insert(func->Arguments.begin(), exp.release());
						func->extented = true;
						return func.release();
					}

					return exp.release();
				}
				case ExpressionType::StringExt:
				{
					Ident ident_var;
					Ident ident_str;

					ident_var.push_back(ident[0]);

					ident_str.push_back(IDENT_STR);
					ident_str.push_back(ident[1]);

					std::unique_ptr<VariableExpression> exp(new VariableExpression(ident, parent));
					exp->Array = _array.release();

					std::unique_ptr<FuncExpression> func(new FuncExpression(ident_str, parent));
					exp->Parent = func.get();
					func->Arguments.insert(func->Arguments.begin(), exp.release());

					return func.release();
				}
				case ExpressionType::Number:
					return new NumberExpression(ident.equals({ IDENT_FALSE, IDENT_NO }) ? false : true);
				case ExpressionType::Identifier:
				{
					if(!_hasdot)
					{
						skip(true, !inside_quotes);

						if(ident.equals(IDENT_FOREACH))
						{
							error_if(!args[0]->IsVariable(), TokenError::IdentifierArgumentsUnexpected, ident_col);

							auto id = &args[1]->ident()->Id;
							if(!id->equals({ IDENT_SEL, IDENT_TYPES, IDENT_PATHS, IDENT_SHORTS,
										   IDENT_DIRS,IDENT_DIRECTORIES,IDENT_FILES, IDENT_NAMES,IDENT_EXTS,
										   IDENT_TITLES, IDENT_DRIVERS, IDENT_ROOTS, IDENT_NAMESPACES, IDENT_USERS }))
							{
								error(TokenError::IdentifierArgumentsUnexpected, ident_col);
							}
						}
						else if(ident.equals(IDENT_FOR))
						{
							error_if(!args[0]->IsAssign(), TokenError::IdentifierArgumentsUnexpected, ident_col);
						}
						else if(ident.equals(IDENT_IF) && args.size() == 1)
						{
							auto parse_stmt = [&]()
							{
								skip();
								expect_openCurly();
								std::unique_ptr<Expression> stmt(parse_expression(parent));
								args.push_back(std::move(stmt));
								expect_closeCurly();
								skip();
							};

							if(l->is(L'{'))
							{
								parse_stmt();
								if(l->peek_ident(IDENT_ELSE))
								{
									l->next(4);
									parse_stmt();
								}
							}
						}
					}

					std::unique_ptr<FuncExpression> func(new FuncExpression(ident, parent));
					func->Brackets = brackets;
					func->Array = _array.release();
					
					if(func->Array)
						func->Array->Parent = func.get();

					if(_hasdot)
						func->SetChild(parse_identifier(parent, true));

					for(auto &arg : args)
						func->push_back(arg.release());

					return func.release();
				}
				default:
					error(error_code, ident_col);
			}
			return nullptr;
		}

		Expression *Parser::parse_statement(Expression *parent)
		{
			std::unique_ptr<StatementExpression> stmt(new StatementExpression(parent, false));
			start_line = l->line;
			start_col = l->column;
			while(l->tok != L'}')
			{
				if(l->eof) break;
				stmt->push_back(parse_expression(OperatorType::None, stmt.get()));
			}
			// skip close curly
			error_if(!l->next_is(L'}'), TokenError::CloseCurlyExpected, start_col, start_line);
			return stmt.release();
		}

		Expression *Parser::parse_for_statement(Expression *parent)
		{
			skip();
			error_if(!l->next_is(L'('), TokenError::OpenParenExpected);
			skip();

			std::unique_ptr<ForStatement> _for(new ForStatement(parent));
			_for->Init = parse_ident(false);
			
			if(_for->Init != 0)
			{
				skip();
				if(l->next_is(L'='))
					_for->Scope.set(_for->Init, parse_expression(_for.get()));
			}

			skip();
			error_if(!l->next_is(L','), TokenError::CommaExpected);
			skip();

			_for->Condition = parse_expression(_for.get());

			bool has_expr = false;

			skip();
			if(l->next_is(L','))
			{
				skip();
				_for->Body1 = parse_expression(_for.get());
				has_expr = true;
			}

			skip();
			error_if(!l->next_is(L')'), TokenError::CloseParenExpected);

			if(!has_expr)
			{
				expect_openCurly();
				_for->Body2 = (StatementExpression *)parse_statement(_for.get());
			}

			return _for.release();
		}

		Expression *Parser::parse_interpolated_string(wchar_t close, Expression *parent)
		{
			std::unique_ptr<StatementExpression> stmt(new StatementExpression(parent, true));

			auto close_curly = close == L'}';
			prevCol = l->column;

			start_line = l->line;
			start_col = l->column;

			while(l->tok != close)
			{
				string value;
				if(l->tok == close) break;
				else if(l->tok == L'%') parse_environment(value);
				else if(l->next_is(L'@'))
				{
					if(l->is_dquote())
						stmt->push_back(parse_string());
					else if(l->is_lbrackets())
						stmt->push_back(parse_brackets(stmt.get()));
					else if(l->is_alpha())
						stmt->push_back(parse_identifier(stmt.get(), false, true));
					else
					{
						value.append(l->prev);
						l->next_is(L'@');// skip @ with double @@
					}
				}

				while(l->tok != close)
				{
					if(!skip(close_curly, false)) break;
					if(l->is({ L'\0', close, L'%', L'@' }))
						break;
					/*if(ch == '\\')
					{
						if(l->peek == close && l->issquote(1))
							next();
					}*/
					value.append(l->next());
				}
				stmt->push_back(value.move());
			}

			error_if(l->tok != close,
					 close_curly ? TokenError::CloseCurlyExpected : TokenError::CloseSingleQuoteExpected,
					 start_col, start_line);
			l->next(); // skip close
			return stmt.release();
		}

		Expression *Parser::parse_array(Expression *parent)
		{
			if(l->tok == L'[')
			{
				std::unique_ptr<ArrayExpression> array(new ArrayExpression(parent));
				l->next(); // skip [
				// create the list of arguments
				while(l->tok != L']')
				{
					array->push_back(parse_expression(array.get()));
					if(l->tok == ']') break;
					else if(!l->next_is(L',')) // skip comma
						error(TokenError::ExpressionUnexpected);
				}
				
				if(!l->next_is(L']')) // skip '['
					error(TokenError::CloseBracketExpected);
				return array.release();
			}
			return nullptr;
		}

		//parentheses, braces, brackets
		Expression *Parser::parse_brackets(Expression *parent)
		{
			std::unique_ptr<Expression> expr;
			if(l->next_if(l->is({ L'(', L'{' })))
			{
				if(l->prev == L'{')
					expr.reset(parse_statement(parent));
				else
				{
					skip();
					error_if(l->tok == L')', TokenError::ExpressionExpected);
					expr.reset(parse_expression(parent));
					expect_closeParen();
				}
			}
			return expr.release();
		}

		// <unary_expression> ::= <unary_operator> '(' <expression> ')'
		Expression *Parser::parse_unary_expression(Expression *parent)
		{
			auto op = OperatorType::Invalid;
			switch(l->tok)
			{
				case L'+': op = OperatorType::UnaryPlus; break;
				case L'-': op = OperatorType::UnaryMinus; break;
				case L'!': op = OperatorType::UnaryNot; break;
				case L'~': op = OperatorType::UnaryNegation; break;
				default: break;
			}

			if(op != OperatorType::Invalid)
			{
				l->next(); // skip operator
				if(auto expr = parse_term(parent))
					return new UnaryExpression(op, expr, parent);
				error(TokenError::UnaryExpressionExpected);
			}

			error(TokenError::Unexpected);

			return nullptr;
		}

		// <primary_expression> ::= <identifier>
		//                        | <number>
		//                        | <string>
		//						  | <string>
		//                        | '(' <expression> ')'
		//                        | <unaray-expression>
		//parse_primary_expression
		Expression *Parser::parse_term(Expression *parent)
		{
			skip();

			if(l->eof) return nullptr;

			auto verbatim = l->tok == L'@';
			// verbatim identifier or group expressions
			if(verbatim && (l->is_squote(1) || l->is_dquote(1) || l->is_alpha(1) || l->is_lbrackets(1)))
			{
				l->next();
			}

			std::unique_ptr<Expression> lhs;

			if(l->tok == L'$' && l->is_ident(1))
			{
				l->next();
				lhs.reset(parse_identifier(parent, false, false, true));
			}
			/*else if(l->peek_for())
			{
				lhs.reset(parse_for_statment());
			}*/
			else if(l->is_alpha())
			{
				lhs.reset(parse_identifier(parent));
			}
			else if(l->is_dquote())
			{
				lhs.reset(parse_string());
			}
			else if(l->is_squote() || l->tok == L'`')
			{
				lhs.reset(parse_interpolated_string(l->next(), parent));
			}
			else if(l->tok == L'\\' && l->is(L'u', 1, true))
			{
				lhs.reset(parse_char());
			}
			else if(l->tok == L'[')
			{
				lhs.reset(parse_array(parent));
			}
			else if(l->is({ L'(', L'{' }))
			{
				lhs.reset(parse_brackets(parent));
			}
			/*else if(l->tok == '\\'/)
			{
				lhs.reset(parse_char());
			}*/
			else if(is_number())
			{
				lhs.reset(parse_number());
			}
			else if(is_color())
			{
				lhs.reset(parse_color());
			}
			else
			{
				lhs.reset(parse_unary_expression(parent));
			}

			skip();
			if(lhs) lhs->Parent = parent;
			return lhs.release();
		}

		Expression *Parser::parse_expression(OperatorType precedence, Expression *parent)
		{
			std::unique_ptr<Expression> lhs(parse_term(parent));
			while(lhs)
			{
				if(l->eof) break;

				auto ot = parse_operator(precedence);
				if(precedence >= ot) break;

				if(ot == OperatorType::Question)
				{
					std::unique_ptr<TernaryExpression> ternary(new TernaryExpression(lhs.release()));
					ternary->True = parse_expression(parent);
					error_if(!l->next_is(':'), TokenError::SemicolonExpected);
					ternary->False = parse_expression(parent);
					lhs = std::move(ternary);
				}
				else if(ot >= OperatorType::Equal && ot <= OperatorType::PlusEqual)
				{
					error_if(!lhs->IsVariable(), TokenError::AssignLeftUnexpected, l->column - 1);

					Ident id = lhs->ident()->Id;

					if(ot == OperatorType::Equal)
					{
						std::unique_ptr<Expression> rhs(parse_expression(parent));
						lhs.reset(new AssignExpression(id, rhs.release()));
					}
					else if(ot == OperatorType::PlusEqual)
					{
						std::unique_ptr<AssignExpression> assign(new AssignExpression(id, nullptr, parent));
						assign->Right = new BinaryExpression(OperatorType::Plus, lhs.release(), parse_expression(assign.get()), assign.get());
						lhs.reset(assign.release());
					}
					else if(ot == OperatorType::MinusEqual)
					{
						std::unique_ptr<AssignExpression> assign(new AssignExpression(id, nullptr, parent));
						assign->Right = new BinaryExpression(OperatorType::Minus, lhs.release(), parse_expression(assign.get()), assign.get());
						lhs.reset(assign.release());
					}

					/*
					skip();
					if(l->next_is(';'))
					{
						std::unique_ptr<StatementExpression> stmt(new StatementExpression(parent));
							stmt->push_back(lhs.release());
						stmt->push_back(parse_expression(parent));
						return stmt.release();
					}
					*/
				}
				else
				{
					std::unique_ptr<Expression> rhs(parse_expression(ot, parent));
					lhs.reset(new BinaryExpression(ot, lhs.release(), rhs.release()));
				}
			}

			if(lhs)
				lhs->Parent = parent;

			return lhs.release();
		}

		Expression *Parser::parse_expression(Expression *parent)
		{
			std::unique_ptr<Expression> lhs(parse_expression(OperatorType::None, parent));
			error_if(!lhs, TokenError::ExpressionExpected);
			return lhs.release();
		}

		Expression *Parser::parse_root_expression(Expression *parent)
		{
			return parse_expression(OperatorType::None, parent);
		}
	}
}
