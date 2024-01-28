#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class ParserException : public std::exception
		{
		protected:
			size_t _col = 0;
			size_t _line = 0;
			string _path;
			TokenError _error = TokenError::None;

		public:
			ParserException() {}
			ParserException(TokenError error, size_t line, size_t col, const string &path, bool log = true)
				: _error{ error }, _col(col), _line(line), _path{ path }
			{
				if(log)
					Logger::Error(L"line[%d] column[%d], %s \"%s\"", line, col, errortostr(error), path.c_str());
			}

			static const wchar_t *errortostr(TokenError tokerr)
			{
				switch(tokerr)
				{
					case TokenError::None: return L"None";
					case TokenError::Eof: return L"Eof";
					case TokenError::Unknown: return L"Unknown";
					case TokenError::Invalid: return L"Invalid";
					case TokenError::InvalidPath: return L"Invalid path";
					case TokenError::InvalidFile: return L"Invalid file";
					case TokenError::CannotFoundFile: return L"Cannot found file";
					case TokenError::InvalidConfigPath: return L"Invalid config path";
					case TokenError::InvalidConfigFile: return L"Invalid config file";
					case TokenError::CannotFoundConfigFile: return L"Cannot found config file";
					case TokenError::InvalidDefinitionPath: return L"Invalid definition path";
					case TokenError::InvalidDefinitionFile: return L"Invalid definition file";
					case TokenError::UnknownFileUnicode: return L"Unknown file unicode";
					case TokenError::FileUnicodeNotSupport: return L"File unicode not support";
					case TokenError::FileUnicodeUTF32NotSupport: return L"File unicode UTF32 not support";
					case TokenError::InvalidConfigFileVersion: return L"Invalid config file version";
					case TokenError::FolderCannotBeFound: return L"folder cannot be found";
					case TokenError::PathNotFound: return L"Path not found";
					case TokenError::FailOpenFile: return L"Fail open file";
					case TokenError::FileCannotBeRead: return L"File cannot be read";
					case TokenError::ImportPathExpected: return L"Import path expected";
					case TokenError::ImportUnexpected: return L"Import unexpected";
					case TokenError::DataCorrupted: return L"Data corrupted";
					case TokenError::True: return L"true";
					case TokenError::False: return L"false";
					case TokenError::Syntax: return L"syntax";
					case TokenError::Expression: return L"Expression";
					case TokenError::Numeric: return L"Numeric";
					case TokenError::HexaDecimalExpected: return L"Hexa decimal expected";
					case TokenError::CharacterHexaDecimalExpected: return L"Character hexa decimal expected";
					case TokenError::IllegalEscape: return L"Illegal escape";
					case TokenError::IllegalCharacter: return L"Illegal character";
					case TokenError::IllegalCharacterHexaDecimal: return L"Illegal character hexa decimal";
					case TokenError::ColorInvalid: return L"Color invalid";
					case TokenError::String: return L"String";
					case TokenError::Operator: return L"Operator";
					case TokenError::Expected: return L"Expected";
					case TokenError::Unexpected: return L"Unexpected";
					case TokenError::CharacterUnexpected: return L"Character unexpected";
					case TokenError::OperatorUnexpected: return L"Operator unexpected";
					case TokenError::OperatorExpected: return L"Operator expected";
					case TokenError::NumericUnexpected: return L"Numeric unexpected";
					case TokenError::NumericExpected: return L"Numeric expected";
					case TokenError::StringUnexpected: return L"String unexpected";
					case TokenError::StringExpected: return L"String expected";
					case TokenError::ExpressionExpected: return L"Expression expected";
					case TokenError::ExpressionUnexpected: return L"Expression unexpected";
					case TokenError::UnaryExpressionExpected: return L"UnaryExpression expected";
					case TokenError::UnaryExpressionUnexpected: return L"Unary expression Unexpected";
					case TokenError::BinaryExpressionExpected: return L"Binary expression expected";
					case TokenError::BinaryExpressionUnexpected: return L"Binary expression unexpected";
					case TokenError::TernaryExpressionExpected: return L"Ternary expression expected";
					case TokenError::TernarySemicolonExpected: return L"Ternary semicolon expected";
					case TokenError::Identifier: return L"Identifier";
					case TokenError::IdentifierExpected: return L"Identifier expected";
					case TokenError::IdentifierUnexpected: return L"Identifier unexpected";
					case TokenError::IdentifierUndefined: return L"Identifier undefined";
					case TokenError::IdentifierMaxLength: return L"Identifier max length";
					case TokenError::IdentifierArguments: return L"Identifier arguments";
					case TokenError::IdentifierArgumentsUnexpected: return L"Identifier arguments unexpected";
					case TokenError::IdentifierArgumentsExpected: return L"Identifier arguments expected";
					case TokenError::IdentifierConfig: return L"Identifier config";
					case TokenError::IdentifierConfigExpected: return L"Identifier config expected";
					case TokenError::IdentifierConfigUnexpected: return L"Identifier config unexpected";
					case TokenError::CloseCommentExpected: return L"Close comment expected";
					case TokenError::OpenEndedValue: return L"Open ended value";
					case TokenError::StringTerminatedExpected: return L"String terminated expected";
					case TokenError::StringUnterminated: return L"String unterminated";
					case TokenError::StringCharacterEscapeSequence: return L"String character escape sequence";
					case TokenError::NewlineInStringUnexpected: return L"Newline in string unexpected";
					case TokenError::MenuTypeExpected: return L"MenuType expected";
					case TokenError::ShellExpected: return L"Shell expected";
					case TokenError::DynamicExpected: return L"Dynamic expected";
					case TokenError::PropertyUndefined: return L"Property undefined";
					case TokenError::PropertyExpected: return L"Property expected";
					case TokenError::PropertyUnexpected: return L"Property unexpected";
					case TokenError::PropertyTypeUnexpected: return L"Property type unexpected";
					case TokenError::PropertyType: return L"Property type";
					case TokenError::PropertyDuplicate: return L"Property duplicate";
					case TokenError::PropertyValueUnexpected: return L"Property value unexpected";
					case TokenError::PropertyValueEmptyUnexpected: return L"Property value empty unexpected";
					case TokenError::PropertyName: return L"Property name";
					case TokenError::PropertyValue: return L"Property value";
					case TokenError::PropertyValueExpected: return L"Property value expected";
					case TokenError::PropertyValueUnterminated: return L"Property value unterminated";
					case TokenError::PropertyTitleOrImageExpected: return L"Property title or image expected";
					case TokenError::PropertyCommandAndArgumentsCannotCombine: return L"Property command and arguments cannot combine";
					case TokenError::PropertyTypeAndSubTypeCannotCombine: return L"Property type and sub type cannot combine";
					case TokenError::PropertyNoneExcludeValueUnexpected: return L"Property none exclude value unexpected";
					case TokenError::PropertyFindOrWhereExpected: return L"Property find or where expected";
					case TokenError::PropertyCommandExpected: return L"Property command expected";
					case TokenError::OpenCurlyExpected: return L"'{' expected";
					case TokenError::CloseCurlyExpected: return L"'}' expected";
					case TokenError::OpenCurlyUnexpected: return L"'{' unexpected";
					case TokenError::CloseCurlyUnexpected: return L"'}' unexpected";
					case TokenError::OpenParenExpected: return L"'(' expected";
					case TokenError::CloseParenExpected: return L"')' expected";
					case TokenError::OpenBracketExpected: return L"'[' expected";
					case TokenError::CloseBracketExpected: return L"']' xpected";
					case TokenError::EOFUnexpected: return L"EOF unexpected";
					case TokenError::QuoteExpected: return L"Quote expected";
					case TokenError::SingleQuoteExpected: return L"Single quote expected";
					case TokenError::DoubleQuoteExpected: return L"Double quote expected";
					case TokenError::MissingQuote: return L"Missing quote";
					case TokenError::OpenQuoteExpected: return L"Open quote expected";
					case TokenError::OpenSingleQuoteExpected: return L"Open single quote expected";
					case TokenError::OpenDoubleQuoteExpected: return L"Open double quote expected";
					case TokenError::CloseQuoteExpected: return L"Close quote expected";
					case TokenError::CloseSingleQuoteExpected: return L"Close single quote expected";
					case TokenError::CloseDoubleQuoteExpected: return L"Close double quote expected";
					case TokenError::QuoteOrExprestionExpected: return L"Quote or exprestion expected";
					case TokenError::VariableExpected: return L"Variable expected";
					case TokenError::VariableUnexpected: return L"Variable unexpected";
					case TokenError::AssignExpected: return L"'=' expected";
					case TokenError::AssignUnexpected: return L"'=' unexpected";
					case TokenError::AssignLeftUnexpected: return L"Assign left unexpected";
					case TokenError::AssignRightUnexpected: return L"Assign right unexpected";
					case TokenError::AssignOrOpenBracketExpected: return L"Assign or '[' Expected";
					case TokenError::ColonExpected: return L"':' expected";
					case TokenError::SemicolonExpected: return L"';' expected";
					case TokenError::CommaExpected: return L"',' expected";
					case TokenError::CommaOrClosParenExpected: return L"',' or ')' expected";
					case TokenError::StringOrNumericExpected: return L"String or numeric expected";
					case TokenError::CommentSinglelineUnexpected: return L"Comment single-line unexpected";
					case TokenError::CacheOverflow: return L"Cache overflow";
					case TokenError::CannotOpenWriteFile: return L"Cannot open write file";
					case TokenError::CannotWriteFile: return L"Cannot write file";
				}
				return L"";
			}
		};

		//TextFileBuffer

		struct Lexer
		{
			//static std::vector<Lexer *> lexers;

			bool eof = true;
			size_t length = 0;
			size_t index = 0;
			size_t line = 1;
			size_t column = 1;
			wchar_t *buffer = nullptr;
			string path = nullptr;
			string location;
			wchar_t tok = 0;
			wchar_t prev = 0;
			wchar_t peek = 0;
			TokenError error = TokenError::None;
			Lexer *parent{};

			Lexer() 
			{
				//if(lexers.empty())
				//	lexers.push_back(new Lexer);
			}

			~Lexer() 
			{
				clear();
			}

			void clear()
			{
				eof = true;
				length = 0;
				index = 0;
				line = 1;
				column = 1;

				delete[] buffer;
				buffer = nullptr;
				//path = nullptr;
				tok = 0;
				prev = 0;
				peek = 0;
			}

			bool load_File(const wchar_t *path, bool ignore_failed = false)
			{
				this->path = path;
				return load_File(ignore_failed);
			}

			bool load_File(bool ignore_failed = false)
			{
				auto err = TokenError::InvalidFile;
				//path = Path::Full(path).move();

				try
				{
					clear();

					// we don't know what kind of file it is, so assume its standard ascii with no BOM encoding
					if(path.empty())
						goto failed_none;

					File file(path);
					if(!file.Open(false, File::Encoding::Binary))
						goto failed_none;

					location = Path::Parent(Path::Full(path)).move();

					size_t numbytes = file.Size();
					if(numbytes < 4)
						goto failed_none;

					std::unique_ptr<uint8_t[]> buf(new uint8_t[numbytes + 4]{ 0 });

					numbytes = file.Read(&buf[0], sizeof(uint8_t), numbytes);
					size_t headerSz = 0;
					auto encodeType = Encoding::GetType(buf.get(), numbytes);
					switch(encodeType)
					{
						case EncodingType::ANSI:
						{
							length = Unicode::FromANSI((char *)buf.get(), uint32_t(numbytes), &buffer);
							if(length > 0)
								goto succeeded;
							break;
						}
						// UTF-8
						case EncodingType::UTF8:
						case EncodingType::UTF8BOM:
						{
							if(encodeType == EncodingType::UTF8BOM)
								headerSz = 3;

							auto utf8 = (char *)(buf.get() + headerSz);
							length = (size_t)Unicode::FromUTF8(utf8, numbytes - headerSz, &buffer);
							if(length > 0)
								goto succeeded;
							break;
						}
						// UTF-16
						case EncodingType::UTF16LE:
						case EncodingType::UTF16LEBOM:
						case EncodingType::UTF16BE:
						case EncodingType::UTF16BEBOM:
						{
							if(encodeType == EncodingType::UTF16BEBOM ||
							   encodeType == EncodingType::UTF16LEBOM)
							{
								headerSz = 2;
							}

							auto sz = (numbytes - headerSz);
							auto data = buf.get() + headerSz;

							if(encodeType == EncodingType::UTF16BE ||
							   encodeType == EncodingType::UTF16BEBOM)
							{
								//	swab(data, sz);

								auto *src = data;
								uint8_t c0 = 0, c1 = 0;
								for(auto i = sz; i > 1; i -= 2, src += 2)
								{
									c0 = src[0];
									c1 = src[1];

									src[0] = c1;
									src[1] = c0;
								}
							}

							length = (sz / sizeof(wchar_t)) - 1;
							buffer = new wchar_t[length + 4]{ 0 };

							::memcpy(buffer, data, sz);

							goto succeeded;
						}
						case EncodingType::UTF32LE:
						case EncodingType::UTF32BE:
						{
							err = TokenError::FileUnicodeUTF32NotSupport;
							goto failed;
						}
					}
					
					err = TokenError::UnknownFileUnicode;
					goto failed;
				}
				catch(...)
				{
#ifdef _DEBUG
					Logger::Exception(__func__);
#endif
				}

				goto failed;

			succeeded:
				if(buffer && length > 1)
				{
					tok = buffer[0];
					peek = buffer[1];
					eof = tok == 0;
					return true;
				}

			failed:
				if(!ignore_failed)
					error = err;
				return false;

			failed_none:
				error = TokenError::None;
				return false;
			}

			wchar_t next()
			{
				prev = tok;
				eof = index++ >= length || buffer[index] == 0;
				if(eof)
				{
				}
				else
				{
					tok = buffer[index];
					peek = buffer[index + 1];
					column++;

					if(is_newline())
					{
						line++;
						column = 0;
					}
					return prev;
				}

				tok = 0;
				peek = 0;
				return prev;
			}

			wchar_t next(uint32_t count)
			{
				for(uint32_t i = 0; i < count; i++)
				{
					if(eof) break;
					next();
				}
				return tok;
			}

			bool next_if(bool condition)
			{
				if(eof) return false;
				if(condition) next();
				return condition;
			}

			bool next_is(wchar_t c, size_t i = 0)
			{
				if(is(c, i))
				{
					next();
					return true;
				}
				return false;
			}

			bool next_is(std::initializer_list<wchar_t> chs, size_t i = 0)
			{
				if(is(chs, i))
				{
					next();
					return true;
				}
				return false;
			}

			wchar_t peek_tok(size_t i) const
			{
				return at(i);
			}

			wchar_t previous(size_t i)
			{
				i -= 1;
				column = column - (index - i);
				index = i;
				return next();
			}

			wchar_t back(size_t idx)
			{
				index = idx;
				return tok;
			}

			const wchar_t *buf()
			{
				return &buffer[index];
			}

			wchar_t at(size_t i = 0) const { return (index + i) < length ? buffer[index + i] : 0; }
			bool is_null(size_t i = 0) const { return at(i) == 0; }
			bool is_eof(size_t i = 0) const { return is_null(i); }

			bool is_space(size_t i = 0) const { return std::iswspace(at(i)); }
			bool is_alpha(size_t i = 0) const { return std::iswalpha(at(i)); }
			bool is_digit(size_t i = 0) const { return std::iswdigit(at(i)); }
			bool is_alnum(size_t i = 0) const { return std::iswalnum(at(i)); }
			bool is_xdigit(size_t i = 0) const { return std::iswxdigit(at(i)); }
			bool is_ident(size_t i = 0) const { return is_alpha(i) || is(L'_', i, false); }
			bool is_iddigit(size_t i = 0) const { return is_ident(i) || is_digit(i); }
			bool is_newline(size_t i = 0) const { return is(L'\n', i, false); }
			bool is_dquote(size_t i = 0) const { return is(L'"', i, false); }
			bool is_squote(size_t i = 0) const { return is(L'\'', i, false); }
			bool is_quote(size_t i = 0) const { return is_squote(i) || is_dquote(i); }// or ||  ch == '`';
			bool is_blank(size_t i = 0) const { return std::iswblank(at(i)); }
			bool is_assign() const { return tok == L'=' || tok == L':'; }
			//
			bool is_lparen(size_t i = 0) const { return is(L'(', i, false); }	// (
			bool is_rparen(size_t i = 0) const { return is(L')', i, false); }	// )
			bool is_lcurly(size_t i = 0) const { return is(L'{', i, false);; }	// {
			bool is_rcurly(size_t i = 0) const { return is(L'}', i, false); }	// }
			bool is_lsquare(size_t i = 0) const { return is(L'[', i, false); }	// [
			bool is_rsquare(size_t i = 0) const { return is(L']', i, false); } // ]
			bool is_lbrackets(size_t i = 0) const { return is_lparen(i) || is_lcurly(i); }
			bool is_rbrackets(size_t i = 0) const { return is_rparen(i) || is_rcurly(i); }

			bool is(wchar_t c, size_t i = 0, bool ignore_case = false) const
			{
				auto c0 = at(i);
				if(c0 == c) return true;
				if(c0 != 0 && ignore_case)
				{
					return std::towupper(c0) == std::towupper(c);
				}
				return false;
			}

			bool is(std::initializer_list<wchar_t> cs, size_t i = 0, bool ignore_case = false) const
			{
				for(auto c : cs)
					if(is(c, i, ignore_case)) return true;
				return false;
			}

			bool peek_is(wchar_t c, size_t i = 0, bool ignore_case = false) const
			{
				auto c0 = at(index + 1 + i);
				if(c0 == c) return true;
				if(c0 != 0 && ignore_case)
					return std::towupper(c0) == std::towupper(c);
				return false;
			}

			bool peek_is(std::initializer_list<wchar_t> cs, size_t i = 0, bool ignore_case = false) const
			{
				for(auto c : cs)
					if(is(c, i, ignore_case)) return true;
				return false;
			}

			template<typename T = int>
			T to_int(size_t i = 0) const
			{
				auto c = at(i);
				if(c != 0)
				{
					if(std::iswdigit(c))
						return (T)(int(c) - 0x30);
				}
				return (T)0;
			}

			template<typename T = int>
			T to_xint(size_t i = 0) const
			{ 
				auto c = at(i);
				if(c != 0)
				{
					if(std::iswdigit(c))
						return (T)(int(c) - 0x30);
					else if(std::iswxdigit(c))
						return (T)((int(c) & 0xdf) - 0x41 + 10);
				}
				return (T)0;
			}
			
			bool operator==(wchar_t c) const { return is(c, 0, false); }
			bool operator!=(wchar_t c) const { return !is(c, 0, false); }
			explicit operator bool() const { return at(0) != 0; }

			template<typename T = size_t>
			bool peek_token(const wchar_t *value, T count, bool ignoreCase = true) const
			{
				if(value && count > 0 && count <= length)
				{
					auto str = &buffer[index];
					for(size_t i= 0; i < count; i++)
					{
						if(*str != *value)
						{
							if(ignoreCase)
							{
								if(std::towupper(*str) != std::towupper(*value))
									return false;
							}
							return false;
						}
						str++;
						value++;
					}

					/*auto sz = static_cast<size_t>(count * sizeof(wchar_t));
					if(ignoreCase)
						return (0 == ::_memicmp(str, value, sz));
					
					return (0 == ::memcmp(str, value, sz));
						*/
				}
				return true;
			}

			bool peek_token(const wchar_t *value, size_t len, bool ignoreCase = true) const
			{
				auto cbsize = len * sizeof(wchar_t);
				auto str = &buffer[index];
				if(ignoreCase)
					return 0==::_memicmp(value, str, cbsize);
				return 0== ::memcmp(value, str, cbsize);
			};

			uint32_t peek_token(const wchar_t *value, bool ignoreCase = true) const
			{
				auto count = 0u;
				if(buffer && length > 0 && !eof && value)
				{
					auto str = &buffer[index];
					for(size_t i = index; *value; i++)
					{
						if(i >= length)
							return 0;

						if(*str != *value)
						{
							if(ignoreCase)
							{
								if(std::towupper(*str) != std::towupper(*value))
									return 0;
							}
							return 0;
						}
						count++;
						str++;
						value++;
					}
				}
				return count;
			}

			uint32_t peek_ident() const
			{
				if(!eof)
				{
					auto str = &buffer[index];
					if(std::iswalpha(*str))
					{
						Hash h;
						do
						{
							h.hash(*str++);
						} while(std::iswalnum(*str) || *str == '_');
						return h;
					}
				}
				return 0;
			}

			bool peek_ident(uint32_t id) const
			{
				return id == peek_ident();
			}

			bool peek_ident(std::initializer_list<uint32_t>  ids) const
			{
				auto ident = peek_ident();
				for(auto id : ids)
				{
					if(id == ident) return true;
				}
				return false;
			}

			bool peek_for()
			{
				if(!eof)
				{
					auto b = &buffer[index];
					if(b[0]== L'f' && 
					   b[1]== L'o' && 
					   b[2]== L'r')
					{
						next(3); // skip for
						true;
					}
				}
				return false;
			}

			bool skip_import()
			{
				if(IDENT_IMPORT == peek_ident())
				{
					next(6); // skip import
					return true;
				}
				return false;
			}

			/*
void TextFileBuffer::next()
{
	if (_pos < _len)
	{
	  switch(*_info)
	  {   case '\t':
			  _column = ((_column + TAB_POS) % TAB_POS) * TAB_POS;
			  break;
		  case '\n':
			  _line++;
			  _column = 1;
			  break;
		  default:
			  if (!_utf8encoded || (*_info & 0xC0) != 0x80)
				_column++;
			  break;
	  }
	  _pos++;
	  _info++;
	}
}

		void TextFileBuffer::advance(unsigned int steps)
		{
			_pos += steps;
			if(_pos > _len)
				_pos = _len;
			_info = _buffer + _pos;
			_column += steps;
		}
*/
		};
	}
}