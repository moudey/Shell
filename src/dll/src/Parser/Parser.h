#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		using namespace Windows;
		using namespace Drawing;
		
		class Parser
		{
			static constexpr auto error_format0 = L"error %s";
			static constexpr auto error_format1 = L"error %s, %s, \"%s\"";
			static constexpr auto error_format2 = L"error line[%d] column[%d], %s \"%s\"";

		private:
			Lexer *l = nullptr;
			string m_path;

			size_t	prevCol = 0;
			size_t	start_col = 0;
			size_t	start_line = 0;

			bool	m_error = false;
			TokenError error_code = TokenError::None;

			std::vector<uint32_t> m_imports;
			std::vector<std::unique_ptr<Lexer>> _imports;

		private:

			bool peek_char(wchar_t c, bool singleLineComment);
			uint32_t peek_ident(const std::initializer_list<uint32_t> &ids, uint32_t *value = nullptr);

			ExpressionType make_error(TokenError tokenError, size_t column);
			bool error(TokenError tokenError);
			bool error(TokenError tokenError, const wchar_t *message);
			bool error(TokenError tokenError, size_t column);
			bool error(TokenError tokenError, size_t column, size_t line);

			void error_if(bool condition, TokenError tokenError);
			void error_if(bool condition, TokenError tokenError, size_t column);
			void error_if(bool condition, TokenError tokenError, size_t column, size_t line);

			bool skip_comment(bool singleLineComment = true);
			bool skip(bool singleLineComment = true, bool eat_whitespace = true);
			Parser &eat(int i = -1);

			bool expect(wchar_t c, bool singleLineComment = true);
			bool expect(const wchar_t *word, bool singleLineComment = true);

			bool expect(std::initializer_list<wchar_t> words, bool singleLineComment = true);
			uint32_t expect(std::initializer_list<const wchar_t *> words, bool singleLineComment = true);

			bool expect_assign(bool expected = false);
			bool expect_openParen();
			bool expect_closeParen();

			bool expect_openCurly();
			bool expect_closeCurly();
			bool parse_environment(string &value);
			auto parse_operator(OperatorType precedence = OperatorType::None);
			bool is_number() const;
			bool is_color() const;

			//OperatorType get_operator(wchar_t op1, wchar_t op2 = 0) const;

			// literal
			//Expression *parse_char();
			Expression *parse_color();
			Expression *parse_number();
			Expression* parse_char();
			Expression *parse_string();
			FuncExpression *parse_func(Expression *parent = nullptr);
			Expression *parse_identifier(Expression *parent, bool hasedot = false, bool inside_quotes = false, bool is_var = false);
			Expression *parse_for_statement(Expression *parent = nullptr);
			Expression *parse_statement(Expression *parent = nullptr);
			Expression *parse_array(Expression *parent = nullptr);
			Expression *parse_brackets(Expression *parent = nullptr);
			Expression *parse_interpolated_string(wchar_t close, Expression *parent = nullptr);
			Expression *parse_unary_expression(Expression *parent = nullptr);
			Expression *parse_term(Expression *parent = nullptr);

			Expression *parse_expression(OperatorType precedence, Expression *parent = nullptr);
			Expression *parse_expression(Expression* parent = nullptr);
			Expression *parse_root_expression(Expression *parent = nullptr);

			bool parse_properties_command(CommandProperty *cmd, Ident const &id, std::unique_ptr<Expression> *expr);
			bool parse_properties_commands(CommandProperty *cmd);
			bool parse_properties_commands(NativeMenu *menu);
			bool parse_properties(NativeMenu *menu);
			bool parse_property(FileSystemObjects &fso);
			bool parse_clsid(NativeMenu *item);

			Expression *parse_property(Ident &id, bool as_value);
			Expression *parse_property__();
			bool parse_property_ident(Ident &id, bool singleLineComment = true);

			
			bool parse_ident(Ident &id, bool allow_dash = false);
			uint32_t parse_ident(bool multiple = true);

			bool parse_variable(Scope *variables, bool has_sign = true);
			int load_import(size_t line, size_t col, bool ignore_failed = true, bool parse_import = false);

			void parse_modify_items(uint32_t action = 0);

			uint32_t parse_image_ident();
			bool parse_image();

			bool parse_menu_item(NativeMenu *menu, uint32_t type);
			void parse_menu(NativeMenu *menu, bool has_curly = true);
			bool parse_modify_properties(NativeMenu *item, uint32_t action = 0);

			void parse_loc(bool has_curly = false);

			void parse_settings(std::vector<struct SETTING> *settings, const Ident &id, bool imported = false);
			void parse_settings(struct SETTING *setting, const Ident &id = 0, bool imported = false);
			void parse_theme();
			void parse_settings();
			void parse_config();

			bool is_lexer() const { return l == _imports.front().get(); };

			ExpressionType verify_ident(const Ident &id, const size_t argc, bool hasdot = false);
			int verify_string_ident(uint32_t id, const size_t argc);

			TokenError verify_command(const Ident &id, bool signer);
			TokenError verify(const Ident &id, bool signer);
			TokenError verify(const NativeMenu *menu, const Ident &id, bool _signed);

			Lexer *import_push()
			{
				_imports.emplace_back(new Lexer);
				return _imports.back().get();
			}

			void pop_import(bool chk = true)
			{
				if(chk && _imports.size() >= 2)
				{
					_imports.pop_back();
				}
				l = _imports.back().get();
			}
/*
			bool log(TokenError err, size_t line, size_t column)
			{
				if(!m_error)
				{
					error_code = err;
					m_error = true;
					Logger::Error(error_format2, err, line, column,
								  Path::Name(context.Application->Config).c_str());
				}
				return false;
			}

			bool log(TokenError err, const wchar_t *message)
			{
				if(!m_error)
				{
					error_code = err;
					m_error = true;
					if(message)
						Logger::Error(error_format1, error_code, message,
									  Path::Name(context.Application->Config).c_str());
					else
					{
						Logger::Error(error_format0, error_code, message,
									  Path::Name(context.Application->Config).c_str());
					}
				}
				return false;
			}


			bool log(TokenError err, size_t column) { return log(err, Line, column); }
			bool log(TokenError err) { return log(err, Line, Column); }
			bool log() { return log(error_code, Line, Column); }
*/

		public:
			Parser();
			~Parser();


			bool			Load();
			bool			HasError() const;
			TokenError		Error() const;

			size_t			Line() const;
			size_t			Column() const;
			const wchar_t*	Path() const;
			uint32_t		TotalMenuCount = 0;
			string			ConfigVersion;
			Context			context;
			string			location;

			private:
				struct FUNC
				{
					uint32_t argc{};
					uint32_t id{};
					std::vector<FUNC> idents;

					FUNC *scope(const Ident &ident, uint32_t i = 1)
					{
						FUNC *ret{};
						for(auto &func : idents)
						{
							if(func.id == ident[i])
							{
								if(i == (ident.length() - 1))
									ret = &func;
								else
									ret = func.scope(ident, ++i);
								break;
							}
						}
						return ret;
					}

					int isf(const Ident &ident, uint32_t arg = 0 , uint32_t i = 1) const
					{
						if(ident.length() == 1)
						{
							if(id == ident[0])
								return argc == arg;
						}
						else 
						{
							for(auto &func : idents)
							{
								if(func.id == ident[i])
								{
									if(i == (ident.length() - 1))
										return true;
									else
										return func.isf(ident, ++i);
									break;
								}
							}
						}
						return 0;
					}

					static bool is(const FUNC& f, const Ident &ident)
					{
						return f.isf(ident);
					}
				};

				FUNC _theme;
		};
	}
}
