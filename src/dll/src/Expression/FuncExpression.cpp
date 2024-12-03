#include <pch.h>
#include "Resource.h"
#include "Expression\Constants.h"
#include "Expression\Variable.h"

#include <propsys.h>
#include <propvarutil.h>
#include <propkey.h>

#include <regex>

//#include <filesystem>

namespace Nilesoft
{
	namespace Shell
	{
		template<typename T>
		T FuncExpression::eval_arg(size_t arg, T const &default_value)
		{
			if(arg >= 0 && arg < Arguments.size())
			{
				return (T)context->Eval(Arguments[arg]);
			}
			return default_value;
		}

		Object FuncExpression::eval_arg(size_t arg)
		{
			auto i = ischild ? --arg : arg;
			if(i < Arguments.size())
				return context->Eval(Arguments[i]).move();
			return {};
		}

		Object FuncExpression::operator()(size_t arg)
		{
			return eval_arg(arg);
		}

		Expression *FuncExpression::operator[](size_t arg)
		{
			if(arg >= 0 && arg < Arguments.size())
				return Arguments[arg];
			return nullptr;
		}

		LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM     lParam)
		{
			switch(message)                  /* handle the messages */
			{
				case WM_DESTROY:
					PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
					break;
				default:                      /* for messages that we don't deal with */
					return DefWindowProc(hwnd, message, wParam, lParam);
			}

			return 0;
		}

		Object FuncExpression::Eval(Context *ctx)
		{
			context = ctx;
			_result = nullptr;

			if(Id.length(0) || Id.zero())
				return _result.move();

			cache = Initializer::instance->cache;

			auto argc = Arguments.size();

			if(Id[0] == IDENT_STR || (ischild && Parent) || extented)
			{
				eval_str();
			}
			else if(Id[0] == IDENT_EVAL)
			{
				if(this->Arguments.empty())
					return nullptr;

				Object obj = eval_arg(0).move();
				/*
				if(eval)
				{
					_result = obj;
					return _result;
				}
				*/
				eval = true;

				delete this->Arguments[0];

				if(obj.is_string())
				{
					this->Arguments[0] = new StringExpression(obj);
				}
				else if(obj.is_color())
				{
					this->Arguments[0] = new NumberExpression(obj, PrimitiveType::Color);
				}
				else if(obj.is_number())
				{
					this->Arguments[0] = new NumberExpression(obj);
				}
				else if(obj.is_array())
				{
					if(obj.is_array(true))
					{
						auto ptr = obj.get_pointer();
						auto count = (uint32_t)ptr[0];
						auto ar = new ArrayExpression;

						for(auto i = 0u; i < count; i++)
						{
							auto p = &ptr[i + 1];

							if(p->is_string())
								ar->push_back(new StringExpression(*p));
							else if(p->is_number())
								ar->push_back(new NumberExpression(*p));
							else
								ar->push_back(new NullExpression);
						}

						this->Arguments[0] = ar;

						_result = context->Eval(ar).move();

						return _result.move();
					}
				}
				else
				{
					this->Arguments[0] = new NullExpression();
				}

				_result = obj;
				return _result.move();
			}
			else if(Id[0] == IDENT_FOR)
			{
				//Object oo = eval_arg(0);
				//MBF(oo);
				return _result.move();
			}
			else switch(Id[0])
			{
			case IDENT_SEL:
				eval_sel();
				break;
			case IDENT_IO:
				eval_io();
				break;
			case IDENT_PATH:
				eval_path();
				break;
			case IDENT_USER:
				eval_user();
				break;
			case IDENT_APP:
				eval_app();
				break;
			case IDENT_SYS:
			case IDENT_SYSTEM:
				eval_sys();
				break;
			case IDENT_STR:
				eval_str();
				break;
			case IDENT_MSG:
				eval_msg();
				break;
			case IDENT_REG:
				eval_reg();
				break;
			case IDENT_IMAGE:
			case IDENT_ICON:
			case IDENT_IMG:
			case IDENT_SVG:
				eval_icon();
				break;
			case IDENT_VAR:
				eval_var(_result);
				break;
			case IDENT_FOR:
				eval_for();
				break;
			case IDENT_FOREACH:
				eval_foreach();
				break;
			case IDENT_KEY:
			case IDENT_KEYS:
				_result = eval_key().move();
				break;
			case IDENT_COLOR:
				_result = eval_color().move();
				break;
			case IDENT_PACKAGE:
			case IDENT_APPX:
			case IDENT_UWP:
			{
				auto packages = &cache->Packages;
				if(Id[1] == IDENT_LIST)
				{
					auto array = new Object[packages->list.size() + 1];
					Object result(array, true);
					*array++ = packages->list.size();
					for(auto &pk : packages->list)
					{
						try {
							*array++ = pk.id;
						}
						catch(...) {}
					}
					_result = result.move();
					break;
				}

				string package_name = eval_arg(0).to_string().trim().move();
				if(!package_name.empty())
				{
					auto package = packages->find(package_name);

					if(Id[1] == IDENT_EXISTS)
					{
						_result = package != nullptr;
						break;
					}

					if(package)
					{
						switch(Id[1])
						{
						case IDENT_ID:
							_result = package->id;
							break;
						case IDENT_NAME:
							_result = package->name;
							break;
						case IDENT_FAMILY:
							_result = package->family;
							break;
						case IDENT_VERSION:
						case IDENT_VER:
							_result = package->version;
							break;
						case IDENT_PATH:
						case IDENT_ZERO:
							_result = package->path;
							break;
						case IDENT_RUN:
						case IDENT_LAUNCH:
						case IDENT_SHELL:
							_result = LR"(shell:appsFolder\)" + package->family + L"!App";
							break;
						}
					}
				}
				break;
			}
			case IDENT_FONT:
			{
				switch(Id[1])
				{
				case IDENT_FONT_LOADED:
				{
					string name = eval_arg(0).to_string().move();
					if(!name.empty())
					{
						_result = name.equals(context->font.text);
						break;						
					}
					_result = false;
					break;
				}
				case IDENT_FONT_SYSTEM:
				case IDENT_FONT_EXISTS:
					_result = Font::Installed(eval_arg(0).to_string());
					break;
				case IDENT_TEXT:
					_result = context->font.text;
					break;
				case IDENT_ICON:
					_result = context->font.icon;
					break;
				case IDENT_FONT_SEGOE_FLUENT_ICONS:
					_result = FontCache::SegoeFluentIcons;
					break;
				case IDENT_FONT_SEGOE_MDL2:
					_result = FontCache::SegoeMDL2Assets;
					break;
				case IDENT_SIZE:
					_result = std::abs(context->theme->font.lfHeight);
					break;
				case IDENT_SCALE:
					//std::round(context->theme->dpi->scale() * 100.0) / 100.0
					_result = round_to(context->theme->dpi->scale(), 0.01);
					break;
				}
				break;
			}
			case IDENT_ID:
			{
				auto is_str = false;
				auto is_name = false;
				auto is_icon = false;

				if(Id.length() == 2)
					_result = 0;
				else  if(Id.length() == 3)
				{
					is_str = Id.equals({ IDENT_TITLE, IDENT_STR });
					is_icon = Id.equals(IDENT_ICON);
					is_name = Id.equals(IDENT_NAME);
				}

				auto muid = Initializer::get_muid(Id[1]);
				if(muid)
				{
					if(is_str)
					{
						string title = muid->get_title().move();
						if(!title.empty())
							_result = title.move();
						break;
					}
					else if(is_name)
					{
						string name = muid->get_name().move();
						if(!name.empty())
							_result = name.move();
						break;
					}
					else if(is_icon)
					{
						_result = new std::tuple<uint32_t, uint32_t, int, MUID *, Object, Object, Object>(
							std::make_tuple(Id[0], Id[1], static_cast<int>(argc), muid, eval_arg(0).move(), eval_arg(1).move(), eval_arg(2).move())
						);
					}
					else
						_result = muid->id;
					break;
				}
				break;
			}
			case IDENT_TIP:
			{
				switch(Id[1])
				{
					case IDENT_PRIMARY:
						_result = 1;
						break;
					case IDENT_INFO:
						_result = 2;
						break;
					case IDENT_SUCCESS:
						_result = 3;
						break;
					case IDENT_WARNING:
						_result = 4;
						break;
					case IDENT_DANGER:
						_result = 5;
						break;
					//case IDENT_DEFAULT:
					default:
						_result = 0;
						break;
				}
				break;
			}
				case IDENT_IF:
				{
					auto condition = eval_arg(0).to_bool();
					if(condition)// condition
						_result = eval_arg(1).move();	//stmt
					else if(argc == 3)
						_result = eval_arg(2).move();	//else stmt
					break;
				}
				case IDENT_NOT:
				{
					Object arg0 = eval_arg(0).move();
					if(arg0.is_number())
						_result = !int32_t(arg0);
					else
						_result = arg0.to_string().is_null_whitespace();
					break;
				}
				case IDENT_TOHEX:
				{
					try
					{
						Object arg0 = eval_arg(0).move();
						if(arg0.is_number())
							_result = string::ToString(L"0x%x", (int32_t)arg0).move();
					} 
					catch(...)
					{
					}
					break;
				}
				case IDENT_PRINT:
					_result = eval_arg(0).to_string().move();
					break;
				case IDENT_CHAR:
				{
					Object arg0 = eval_arg(0).move();
					int count = argc > 1 ? (int)eval_arg(1) : 1;
					if(arg0.is_number())
						_result = string(count, (wchar_t)arg0).move();
					else if(arg0.is_string())
					{
						string val = arg0.to_string().move();
						wchar_t chr = 0;
						if(val.equals(L"quote"))
							chr = L'"';
						else if(val.equals(L"apos"))
							chr = L'\'';
						else
							chr = val.at(0);

						_result = string(count, chr).move();
					}
					break;
				}
				case IDENT_SHR:
				{
					Object arg0 = eval_arg(0).move();
					Object arg1 = eval_arg(1).move();
					if(arg0.is_number() && arg1.is_number())
						_result = (uintptr_t)arg0 >> (uintptr_t)arg1;
					else if(arg0.is_number())
						_result = arg0.move();
					else if(arg1.is_number())
						_result = arg1.move();
					break;
				}
				case IDENT_SHL:
				{
					Object arg0 = eval_arg(0).move();
					Object arg1 = eval_arg(1).move();
					if(arg0.is_number() && arg1.is_number())
						_result = (uintptr_t)arg0 << (uintptr_t)arg1;
					else if(arg0.is_number())
						_result = arg0.move();
					else if(arg1.is_number())
						_result = arg1.move();
					break;
				}
				case IDENT_EQUALS:
				case IDENT_EQUAL:
				{
					_result = false;
					Object arg_first = eval_arg(0).move();
					for(size_t i = 1; i < argc; i++)
					{
						Object arg_second = eval_arg(i).move();
						if(arg_first.equals(arg_second))
						{
							_result = true;
							break;
						}
					}
					break;
				}
				/*case IDENT_EQUAL:
				{
					Object arg0 = eval_arg(0).move();
					Object arg1 = eval_arg(1).move();
					if(arg0.is_number() && arg1.is_number())
						_result = (double)arg0 == (double)arg1;
					else
						_result = arg0.to_string().equals(arg1.to_string(), false);
					break;
				}*/
				case IDENT_GREATER:
				{
					Object arg0 = eval_arg(0).move();
					Object arg1 = eval_arg(1).move();
					if(arg0.is_number() && arg1.is_number())
						_result = (double)arg0 > (double)arg1;
					else
						_result = arg0.length() > arg1.length();
					break;
				}
				case IDENT_LESS:
				{
					Object arg0 = eval_arg(0).move();
					Object arg1 = eval_arg(1).move();
					if(arg0.is_number() && arg1.is_number())
						_result = (double)arg0 > (double)arg1;
					else
						_result = arg0.length() < arg1.length();
					break;
				}
				case IDENT_CMD:
				case IDENT_EXEC:
				case IDENT_RUN:
				case IDENT_LAUNCH:
				{
					if(Id.length() > 1)
					{
						if(Id[1] == IDENT_CMD_INVOKED)
						{
							_result = context->invoked;
							break;
						}

						switch(Id[1])
						{
							case IDENT_WINDOW_HIDDEN:
								_result = SW_HIDE;
								break;
							case IDENT_NORMAL:
								_result = SW_SHOWNORMAL;
								break;
							case IDENT_MINIMIZED:
								_result = SW_SHOWMINIMIZED;
								break;
							case IDENT_MAXIMIZED:
								_result = SW_SHOWMAXIMIZED;
								break;
						}
					}
					else
					{
						if(context->Runtime)
						{
							Object arg0 = eval_arg(0).move();
							if(argc > 1)
							{
								Diagnostics::ShellExec::Run(arg0, eval_arg(1),
															nullptr, L"runas", eval_arg(2, SW_NORMAL));
							}
							else
							{
								Diagnostics::ShellExec::Run(arg0, nullptr,
															nullptr, L"open", SW_NORMAL);
							}
						}
						//else if(Id[0] == IDENT_CMD && argc == 0)
						//	_result = IDENT_CMD;
					}
					break;
				}
				case IDENT_SHELL:
				{
					if(context->Runtime && argc > 0)
					{
						_result = Diagnostics::ShellExec::Run(eval_arg(0), eval_arg(1), nullptr,
															  eval_arg(2), eval_arg(3, SW_NORMAL));
					}
					break;
				}
				case IDENT_THIS:
				{
					if(context->_this)
					{
						switch(Id[1])
						{
						case IDENT_TYPE:
							_result = context->_this->type;
							break;
						case IDENT_CHECKED:
							_result = context->_this->checked;
							break;
						case IDENT_POS:
							_result = context->_this->pos;
							break;
						case IDENT_DISABLED:
							_result = context->_this->disabled;
							break;
						case IDENT_SYS:
							_result = context->_this->system;
							break;
						case IDENT_NAME:
						case IDENT_TITLE:
						{
							string title = Id[1]== IDENT_TITLE ? context->_this->title : context->_this->title_normalize;
							switch(Id[2])
							{
							case IDENT_LEN:
							case IDENT_LENGTH:
								_result = title.length<int>();
								break;
							case IDENT_ZERO:
								if(argc == 0)
									_result = title.move();
								else
								{
									_result = false;
									for(size_t i = 0; i < Arguments.size(); i++)
									{
										string r = eval_arg(i).to_string().trim().move();
										if(title.equals(r))
										{
											_result = true;
											break;
										}
									}
								}
							}
							return _result.move();
						}
						case MENU_VERB:
							_result = context->_this->verb;
							break;
						case IDENT_COUNT:
							_result = context->menu_count;
							break;
						case IDENT_LEVEL:
							_result = context->_this->level + 1;
							break;
						case IDENT_ID:
						{
							if(argc == 0)
								_result = context->_this->id;
							else
							{
								_result = false;
								for(size_t i = 0; i < Arguments.size(); i++)
								{
									auto r = eval_arg(i).to_number<uint32_t>();
									if(r == context->_this->id)
									{
										_result = true;
										break;
									}
								}
							}
							return _result.move();
						}
						case IDENT_PARENT:
						{
							/*switch(Id[2])
							{
								case IDENT_ID:
								case IDENT_NAME:
								case IDENT_TITLE:
									break;
								case IDENT_ZERO:
									break;
							}
							*/
							if(argc == 0)
								_result = context->_this->parent;
							else
							{
								for(size_t i = 0; i < Arguments.size(); i++)
								{
									auto r = eval_arg(i).to_number<uint32_t>();
									if(r == context->_this->parent)
									{
										_result = true;
										return _result.move();
									}
								}
								_result = false;
							}
							break;
						}
						case IDENT_ISUWP:
							_result = context && context->_this ? context->_this->is_uwp : false;
							break;
						case IDENT_CLSID:
							if(context)
							{
								if(argc == 0)
									_result = context->_this->clsid;
								else
								{
									for(size_t i = 0; i < Arguments.size(); i++)
									{
										string r = eval_arg(i).to_string().tolower().move();
										if(context->_this->clsid.equals(r))
										{
											_result = true;
											return _result.move();
										}
									}
									_result = false;
								}
							}
							break;
						}

						if(!_result.is_null())
							return _result.move();
					}

					switch(Id[1])
					{
					case IDENT_COUNT:
						_result = context->menu_count;
						break;
					}
					break;
				}
				case IDENT_INI:
				{
					string file = eval_arg(0).to_string().move();
					string section = eval_arg(1).to_string().move();
					string key = eval_arg(2).to_string().move();

					switch(Id[1])
					{
						case IDENT_GET:
						case IDENT_ZERO:
						{
							string data(1024);
							auto ret = ::GetPrivateProfileStringW(section, key, nullptr, data.data(), 1024, file);
							if(ret > 0)
								_result = data.release(ret).move();
							break;
						}
						case IDENT_SET:
						{
							_result = ::WritePrivateProfileStringW(section, key, eval_arg(3).to_string(), file);
							break;
						}
					}
					return _result.move();
				}
				case IDENT_CLIPBOARD:
				{
					switch(Id[1])
					{
						case IDENT_EMPTY:
							Text::Clipboard::Empty();
							break;
						case IDENT_IS_EMPTY:
						case IDENT_LEN:
						case IDENT_LENGTH:
						{
							_result = 0;
							if(Text::Clipboard::IsFormatAvailable(CF_UNICODETEXT))
							{
								if(::OpenClipboard(nullptr))
								{
									if(auto hglb = ::GetClipboardData(CF_UNICODETEXT); hglb)
									{
										if(Id[1] == IDENT_IS_EMPTY)
										{
											_result = !hglb;
										}
										else if(auto lptstr = (const wchar_t *)::GlobalLock(hglb); lptstr)
										{
											_result = string::Length(lptstr);
											//ReplaceSelection(hwndSelected, pbox, lptstr);
											::GlobalUnlock(hglb);
										}
									}
									::CloseClipboard();
								}
							}
							break;
						}
						case IDENT_IS_BITMAP:
						{
							_result = Text::Clipboard::IsFormatAvailable(CF_BITMAP);
							break;
						}
						case IDENT_ZERO:
						case IDENT_SET:
						case IDENT_GET:
						{
							if(argc == 1 && (Id[1] == IDENT_SET || Id[1] == IDENT_ZERO))
							{
								Text::Clipboard::Set(eval_arg(0).to_string());
							}
							else if(Text::Clipboard::IsFormatAvailable(CF_UNICODETEXT))
							{
								if(::OpenClipboard(nullptr))
								{
									if(auto hglb = ::GetClipboardData(CF_UNICODETEXT); hglb)
									{
										if(auto lptstr = (const wchar_t *)::GlobalLock(hglb); lptstr)
										{
											_result = string(lptstr).move();
											//ReplaceSelection(hwndSelected, pbox, lptstr);
											::GlobalUnlock(hglb);
										}
									}
									::CloseClipboard();
								}
							}
							break;
						}
					}
					return _result.move();
				}
				case IDENT_INPUT:
				{
					static string input_result;

					if(Id.length() > 1)
					{
						_result = input_result;
						return _result.move();
					}

					input_result.clear();
					_result = false;

					struct INPUT_PARAM
					{
						string title;
						string prompt;
						string result;
						bool ret = false;
					} input_param;

					input_param.title = eval_arg(0).to_string().move();
					input_param.prompt = eval_arg(1).to_string().move();
					input_param.result = eval_arg(2).to_string().move();

					auto hWnd = ::CreateDialogParamW(Path::GetCurrentModule(), MAKEINTRESOURCEW(IDD_INPUTBOX), NULL, 
													 [](HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)->INT_PTR
					{
						switch(Msg)
						{
							case WM_INITDIALOG:
							{
								// Set other controls
								auto input_param = (INPUT_PARAM *)lParam;
								::SetPropW(hDlg, L"INPUT-PARAM", input_param);

								if(!input_param->title.empty())
									::SetWindowTextW(hDlg, input_param->title);
								if(!input_param->prompt.empty())
									::SetDlgItemTextW(hDlg, IDC_INPUTBOX_PROMPT, input_param->prompt);
								if(!input_param->result.empty())
									::SetDlgItemTextW(hDlg, IDC_INPUTBOX_EDIT, input_param->result);
								return TRUE;
							}
							case WM_COMMAND:
							{
								auto input_param = (INPUT_PARAM *)::GetPropW(hDlg, L"INPUT-PARAM");
								if(LOWORD(wParam) == IDOK)
								{
									wchar_t input_string[1024]{};
									::GetDlgItemTextW(hDlg, IDC_INPUTBOX_EDIT, input_string, 1024);
									input_param->result = input_string;
									::EndDialog(hDlg, IDOK);
									input_param->ret = true;
								}
								if(LOWORD(wParam) == IDCANCEL)
									::EndDialog(hDlg, IDCANCEL);
								return TRUE;
							}
							case WM_CLOSE:
								::EndDialog(hDlg, IDOK);
								return TRUE;
						}
						return FALSE;
					}, (LPARAM)&input_param);

					if(hWnd)
					{
						::ShowWindow(hWnd, SW_SHOW);
						MSG msg;
						BOOL bRet;
						while((bRet = ::GetMessageW(&msg, nullptr, 0, 0)) != 0)
						{
							if(bRet == -1 || !::IsWindowVisible(hWnd))
								break;
							else if(!::IsWindow(hWnd) || !::IsDialogMessageW(hWnd, &msg))
							{
								::TranslateMessage(&msg);
								::DispatchMessageW(&msg);
							}
						}
						::DestroyWindow(hWnd);
						if(input_param.ret)
						{
							_result = true;
							input_result = input_param.result.move();
						}
					}
					return _result.move();
				}
				case IDENT_REGEX:
				{
					string str = eval_arg(0).to_string().move();
					string pattern = eval_arg(1).to_string().move();
					auto flags = std::regex_constants::ECMAScript | std::regex_constants::icase;

					switch(Id[1])
					{
						case IDENT_REGEX_MATCH:
						{
							_result = 0;
							try {
								std::wregex _regex(pattern.c_str(), flags);
								_result = std::regex_match(str.c_str(), _regex);
							}
							catch(...) {
							}
							break;
						}
						case IDENT_REGEX_MATCHES:
						{
							try {
								std::wstring wstr = str;
								std::wsmatch match;
								std::wregex _regex(pattern.c_str(), flags);
								std::vector<string> list;

								int i = 0;
								while(std::regex_search(wstr, match, _regex))
								{
									list.push_back(std::move(match.str()));
									wstr = std::move(match.suffix().str());
									if(i++ > 1000)
										break;
								}

								if(!list.empty())
								{
									if(Array)
									{
										if(Object oa = context->Eval(Array).move(); oa.is_number())
										{
											int ai = oa;
											if(ai >= 0 && ai < (int)list.size())
												_result = match.str(ai).c_str();
										}
										break;
									}

									auto array = new Object[list.size() + 1];
									Object result(array, true);
									*array++ = list.size();
									for(auto &s : list)
									{
										try {
											*array++ = s.move();
										}
										catch(...) {}
									}
									return result.move();
								}

								/*if(std::regex_match(wstr.cbegin(), wstr.cend(), match, _regex) && match.size() > 1)
								{
									auto sz = match.size() - 1;
									if(Array)
									{
										if(Object oa = context->Eval(Array).move(); oa.is_number())
										{
											int ai = oa;
											if(ai >= 0 && ai < sz)
												_result = match.str(ai).c_str();
										}
										break;
									}

									auto array = new Object[sz + 1];
									Object result(array, true);
									*array++ = sz;
									for(auto i = 0; i < sz; i++)
									{
										try {
											*array++ = match.str(i + 1);
										}
										catch(...) {}
									}
									return result.move();
								}*/
							}
							catch(...) {
							}
							break;
						}
						case IDENT_REPLACE:
						{
							try {
								std::wregex _regex(pattern.c_str(), flags);
								_result = std::regex_replace(str.c_str(), _regex, eval_arg(2).to_string().c_str());
							}
							catch(...) {
								_result = str.move();
							}
							break;
						}
					}
					return _result.move();
				}
				case IDENT_TOINT:
				case IDENT_TODOUBLE:
				case IDENT_TOUINT:
				case IDENT_TOFLOAT:
				{
					Object o = eval_arg(0).move();

					//MBF(L"%d", (int)o.type());
					if(Array)
					{
						if(Object oa = context->Eval(Array).move(); oa.is_number())
						{
							//int ai = oa;
							/*if(ai >= 0 && ai < (int)sel->Count())
								_result = sel->Get(ai)->get(quote, argc == 2 ? eval_arg(1).to_string().c_str() : nullptr).move();
							*/
						}
					}

					if(o.is_number())
						_result = o.to_number<int>();
					else if(o.is_string())
						_result = o.to_string().to_int<int>();
					
					return _result.move();
				}
				case IDENT_TYPE:
				{
					switch(Id[1])
					{
						case IDENT_TYPE_FILE:
							_result = IO::PathType::File; 
							break;
						case IDENT_TYPE_DIR:
						case IDENT_TYPE_DIRECTORY:
							_result = IO::PathType::Directory;
							break;
						case IDENT_TYPE_DRIVE:
							_result = IO::PathType::Drive;
							break;
						case IDENT_TYPE_NAMESPACE:
							_result = IO::PathType::Namespace;
							break;
						case IDENT_TYPE_DESKTOP:
							_result = IO::PathType::Desktop;
							break;
						case IDENT_TYPE_FIXED:
							_result = IO::PathType::Fixed;
							break;
						case IDENT_TYPE_DVD:
							_result = IO::PathType::DVD;
							break;
						case IDENT_TYPE_USB:
							_result = IO::PathType::USB;
							break;
						case IDENT_TYPE_VHD:
							_result = IO::PathType::VHD;
							break; 
						case IDENT_TYPE_REMOTE:
							_result = IO::PathType::Remote;
							break;
						case IDENT_TYPE_TASKBAR:
							_result = IO::PathType::Taskbar;
							break;
						case IDENT_TYPE_BACK:
						{
							switch(Id[2])
							{
								case IDENT_TYPE_DIR:
								case IDENT_TYPE_DIRECTORY:
									_result = IO::PathType::Back_Directory;
									break;
								case IDENT_TYPE_DRIVE:
									_result = IO::PathType::Back_Drive;
									break;
								case IDENT_TYPE_NAMESPACE:
									_result = IO::PathType::Back_Namespace;
									break;
								case IDENT_TYPE_FIXED:
									_result = IO::PathType::Back_Fixed;
									break;
								case IDENT_TYPE_DVD:
									_result = IO::PathType::Back_DVD;
									break;
								case IDENT_TYPE_USB:
									_result = IO::PathType::Back_USB;
									break;
								case IDENT_TYPE_VHD:
									_result = IO::PathType::Back_VHD;
									break;
								case IDENT_TYPE_REMOTE:
									_result = IO::PathType::Back_Remote;
									break;
								case IDENT_ZERO:
									_result = IO::PathType::Back;
									break;
								default:
									_result = IO::PathType::Unknown;
									break;
							}
							break;
						}
						default:
							_result = IO::PathType::Unknown;
							break;
					}
					return _result.move();
				}
				case IDENT_MODE:
				{
					switch(Id[1])
					{
						case IDENT_MODE_NONE:
							_result = (int8_t)SelectionMode::None;
							break;
						case IDENT_MODE_SINGLE:
							_result = (int8_t)SelectionMode::Single;
							break;
						case IDENT_MODE_MULTI_UNIQUE:
							_result = (int8_t)SelectionMode::MultiUnique;
							break;
						case IDENT_MODE_MULTI_SINGLE:
							_result = (int8_t)SelectionMode::MultiSingle;
							break;
						case IDENT_MODE_MULTI:
						case IDENT_MODE_MULTIPLE:
							_result = (int8_t)SelectionMode::Multiple;
							break;
						default:
						{
							auto sel = context->Selections;
							_result = sel ? sel->Mode : SelectionMode::None;
						}
					}
					break;
				}
				case IDENT_VIS:
				case IDENT_VISIBILITY:
				{
					switch(Id[1])
					{
						case IDENT_VIS_HIDDEN:
						case IDENT_VIS_REMOVE:
							_result = (int8_t)Visibility::Hidden;
							break;
						case IDENT_NORMAL:
						case IDENT_VIS_ENABLE:
						case IDENT_VIS_VISIBLE:
							_result = (int8_t)Visibility::Enabled;
							break;
						case IDENT_VIS_DISABLE:
							_result = (int8_t)Visibility::Disabled;
							break;
						case IDENT_VIS_STATIC:
							_result = (int8_t)Visibility::Static;
							break;
						case IDENT_VIS_LABEL:
							_result = (int8_t)Visibility::Label;
							break;
						default:
						{
							auto _this = context->_this;
							_result = _this ? _this->vis : 1;
						}
					}
					break;
				}
				case IDENT_SEP:
				case IDENT_SEPARATOR:
				{
					switch(Id[1])
					{
						case IDENT_NONE:
							_result = (int)Separator::None;
							break;
						case IDENT_TOP:
						case IDENT_BEFORE:
							_result = (int)Separator::Top;
							break;
						case IDENT_BOTTOM:
						case IDENT_AFTER:
							_result = (int)Separator::Bottom;
							break;
						case IDENT_BOTH:
							_result = (int)Separator::Both;
							break;
						default:
						{
							auto _this = context->_this;
							_result = _this ? _this->sep : 0;
							break;
						}
					}
					break;
				}
				case IDENT_POS:
				{
					switch(Id[1])
					{
						case IDENT_TOP:
							_result = (int)Position::Top;
							break;
						case IDENT_BOTTOM:
							_result = (int)Position::Bottom;
							break;
						case IDENT_MIDDLE:
							_result = (int)Position::Middle;
							break;
						case IDENT_AUTO:
							_result = (int)Position::Auto;
							break;
					}
					break;
				}
				case IDENT_VIEW:
				{
					switch(Id[1])
					{
					case IDENT_AUTO:
						_result = VIEWMODE::AUTO;
						break;
					case IDENT_VIEW_COMPACT:
						_result = VIEWMODE::COMPACT;
						break;
					case IDENT_VIEW_SMALL:
						_result = VIEWMODE::SMALL;
						break;
					case IDENT_VIEW_MEDIUM:
						_result = VIEWMODE::MEDIUM;
						break;
					case IDENT_VIEW_LARGE:
						_result = VIEWMODE::LARGE;
						break;
					case IDENT_VIEW_WIDE:
						_result = VIEWMODE::WIDE;
						break;
					}
					break;
				}
				case IDENT_THEME:
				{
					switch(Id[1])
					{
						case IDENT_AUTO:
							_result = (int32_t)ThemeType::Auto;
							break;
						case IDENT_THEME_SYSTEM:
							_result = (int32_t)ThemeType::System;
							break;
						case IDENT_THEME_CLASSIC:
							_result = (int32_t)ThemeType::Classic;
							break;
						case IDENT_THEME_LIGHT:
							_result = (int32_t)ThemeType::Light;
							break;
						case IDENT_THEME_DARK:
							_result = (int32_t)ThemeType::Dark;
							break;
						case IDENT_THEME_HIGHCONTRAST:
							_result = (int32_t)ThemeType::HighContrast;
							break;
						case IDENT_THEME_BLACK:
							_result = (int32_t)ThemeType::Black;
							break;
						case IDENT_THEME_WHITE:
							_result = (int32_t)ThemeType::White;
							break;
						case IDENT_THEME_MODERN:
							_result = (int32_t)ThemeType::Modern;
							break;
						case IDENT_THEME_CUSTOM:
							_result = (int32_t)ThemeType::Custom;
							break;
						case IDENT_ISDARK:
							_result = 0;
							if(context && context->theme)
								_result = context->theme->mode == 1;
							break;
						case IDENT_ISLIGHT:
							_result = 1;
							if(context && context->theme)
								_result = context->theme->mode == 0;
							break;
						case IDENT_MODE:
						{
							_result = 0;
							if(context && context->theme) 
							{
								if(Id[2] == IDENT_ZERO)
									_result = context->theme->mode;
								else if(Id[2] == IDENT_SYSTEM)
									_result = context->theme->systemUsesLightTheme;
							}
							break;
						}
						case IDENT_ISHIGHCONTRAST:
							_result = (context && context->theme) ? context->theme->isHighContrast : Theme::IsHighContrast();
							break;
						case IDENT_DPI:
							_result = context->theme->dpi->val;
							break;
						case IDENT_BACKGROUND:
						{
							if(Id[2] == IDENT_OPACITY)
								_result = context->theme->background.color.a;
							else if(Id[2] == IDENT_EFFECT)
								_result = context->theme->background.effect;
							else
								_result = Object(context->theme->background.color).move();
							break;
						}
						case IDENT_ITEM:
						{
							//theme.item.back.normal.disable
							if(Id.equals(4, { IDENT_NORMAL, IDENT_SELECT }))
							{
								Color color;
								if(Id[3] == IDENT_NORMAL)
								{
									if(Id[4] == IDENT_DISABLE)
										color = Id[2] == IDENT_BACK ? context->theme->back.color.nor_dis : context->theme->text.color.nor_dis;
									else 
										color = Id[2] == IDENT_BACK ? context->theme->back.color.nor : context->theme->text.color.nor;
								}
								else if(Id[3] == IDENT_SELECT)
								{
									if(Id[4] == IDENT_DISABLE)
										color = Id[2] == IDENT_BACK ? context->theme->back.color.sel_dis : context->theme->text.color.sel_dis;
									else
										color = Id[2] == IDENT_BACK ? context->theme->back.color.sel : context->theme->text.color.sel;
								}

								_result = Object(color).move();
							}
							else if(Id[2] == IDENT_EFFECT)
								_result = context->theme->background.effect;
							else
								_result = Object(context->theme->background.color).move();
							break;
						}
					}
					break;
				}
				case IDENT_EFFECT:
				{
					switch(Id[1])
					{
						case IDENT_NONE:
							_result = 0; break;
						case IDENT_EFFECT_TRANSPARENT:
							_result = 1; break;
						case IDENT_EFFECT_BLUR:
							_result = 2; break;
						case IDENT_EFFECT_ACRYLIC:
							_result = 3; break;
						case IDENT_EFFECT_MICA:
							_result = 4; break;
						case IDENT_EFFECT_TABBED:
							_result = 5; break;
						case IDENT_ZERO:
							_result = context->theme->background.effect;
					}
					break;
				}
				case IDENT_WINDOW:
				case IDENT_WND:
				{
					auto hWnd = context->wnd.owner;
					auto hWnd_owner = ::GetAncestor(hWnd, GA_ROOTOWNER);

					if(Id[1] == IDENT_COMMAND)
					{
						Object ocmd = eval_arg(0).move();

						if(!ocmd.not_default())
							break;

						int cmd = ocmd;

						if(argc == 1 && context->wnd.owner)
						{
							if(cmd == 41504)
							{
								hWnd = context->wnd.active;
							}
						}

						if(argc == 2)
						{
							string wndcls = eval_arg(1).to_string().move();
							hWnd = Window::Find(wndcls);

							if(!hWnd)
								hWnd = ::FindWindowExW(nullptr, nullptr, nullptr, wndcls);
						}

						if(hWnd)
							::PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM(cmd, 0), 0);
					}
					else if(Id[1] == IDENT_SEND || Id[1] == IDENT_POST)
					{
						Object obj = eval_arg(0).move();

						if(!obj.is_null())
						{
							string wndcls = obj.to_string().move();
							hWnd = Window::Find(wndcls);

							if(!hWnd)
								hWnd = ::FindWindowExW(nullptr, nullptr, nullptr, wndcls);
						}

						if(hWnd)
						{
							if(Id[1] == IDENT_SEND)
								::SendMessageW(hWnd, eval_arg(1), eval_arg(2), eval_arg(3));
							else
								::PostMessageW(hWnd, eval_arg(1), eval_arg(2), eval_arg(3));
						}
					}
					else if(Id[1] == IDENT_HANDLE)
					{
						_result = reinterpret_cast<uintptr_t>(hWnd_owner ? hWnd_owner : hWnd);
					}
					else if(Id[1] == IDENT_NAME)
					{
						if(auto h = hWnd_owner ? hWnd_owner : hWnd; h)
							_result = Window::class_name(h);
					}
					else if(Id[1] == IDENT_TITLE)
					{
						if(auto h = hWnd_owner ? hWnd_owner : hWnd; h)
						{
							wchar_t title[250]{};
							::GetWindowTextW(h, title, 250);
							_result = title;
						}
					}
					else if(Id[1] == IDENT_PARENT || Id[1] == IDENT_OWNER)
					{
						auto h = Id[1] == IDENT_OWNER ? hWnd_owner : ::GetParent(hWnd);
						if(h)
						{
							if(Id[2] == IDENT_NAME)
							{
								_result = Window::class_name(h);
							}
							else if(Id[2] == IDENT_TITLE)
							{
								wchar_t title[250]{};
								::GetWindowTextW(h, title, 250);
								_result = title;
							}
							else
							{
								_result = reinterpret_cast<uintptr_t>(h);
							}
						}
					}
					else
					{
						auto sel = context->Selections;
						switch(Id[1])
						{
							case IDENT_MODULE:
							case IDENT_INSTANCE:
								_result = Path::Name(Path::Module(Window::get_hInstance(context->wnd.active))).tolower().move();
								break;
							case IDENT_IS_DESKTOP:
								_result = sel->Window.desktop;
								break;
							case IDENT_IS_TASKBAR:
								_result = sel->Window.parent == WINDOW_TASKBAR;
								break;
							case IDENT_IS_EXPLORER:
								_result = sel->Window.explorer;
								break;
							case IDENT_IS_NAV:
							case IDENT_IS_TREE:
							case IDENT_IS_SIDE:
								_result = sel->Window.explorer_tree;
								break;
							case IDENT_IS_EDIT:
								_result = sel->Window.id == WINDOW_EDIT;
								break;
							case IDENT_IS_START:
								//if(sel->Window.parent == WINDOW_TASKBAR)
								_result = sel->Window.id == WINDOW_START;
								break;
							case IDENT_IS_TITLEBAR:
								_result = sel->Window.id == WINDOW_SYSMENU;
								break;
							case IDENT_ID:
								_result = sel->Window.id;
								break;
							case IDENT_IS_CONTEXTMENUHANDLER:
								_result = sel->loader.contextmenuhandler; 
								break;
						}
					}
					break;
				}
				case IDENT_PROCESS:
				{
					auto sel = context->Selections;
					auto initializer = Initializer::instance;
					switch(Id[1])
					{
						case IDENT_HANDLE:
							_result = initializer->process.hModule;
							break;
						case IDENT_ID:
							_result = initializer->process.id;
							break;
						case IDENT_NAME:
							_result = initializer->process.name;
							break;
						case IDENT_PATH:
							_result = initializer->process.path;
							break;
						case IDENT_IS_EXPLORER:
							_result = sel->loader.explorer;
							break;
						case IDENT_USED:
						{
							string proc;
							for(auto &process : Diagnostics::Process::EnumInfo())
							{
								auto hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, process.th32ProcessID);
								if(hProcess)
								{
									for(auto &module : Diagnostics::Process::Modules(hProcess))
									{
										if(Diagnostics::Process::ModuleFileName(hProcess, module).equals(context->Application->Path))
										{
											proc.append_format(L"%d %s\n", process.th32ProcessID, Diagnostics::Process::ModuleFileName(hProcess).c_str());
											break;
										}
									}
									::CloseHandle(hProcess);
								}
							}
							_result = proc.move();
							break;
						}
						case IDENT_IS_STARTED:
						{
							_result = false;

							string proc = eval_arg(0).to_string().trim().move();
							
							if(proc.empty())
								break;

							for(auto &process : Diagnostics::Process::EnumInfo())
							{
								if(proc.equals(process.szExeFile))
								{
									_result = true;
									break;
								}
							}
							break;
						}
					}
					break;
				}
				case IDENT_COMMAND:
				{
					auto ver = &Version::Instance();
					auto id = Id[1];
					auto is11 = ver->IsWindows11OrGreater();

					COM_INITIALIZER com_initializer(true);
					IComPtr<IShellDispatch6> shellDisp;
					IComPtr<IWebBrowser> webBrowser;

					auto shellBrowser = context->Selections->ShellBrowser;

					auto shelldisp = [&]()->bool 
					{
						return shellDisp.CreateInstance(CLSID_Shell, IID_IDispatch, CLSCTX_SERVER) && shellDisp;
					};

					auto WebBrowser = [&]()->bool
					{
						if(shellBrowser)
						{
							IComPtr<IServiceProvider> sp;
							if(S_OK == shellBrowser->QueryInterface(IID_IServiceProvider, sp))
								return S_OK == sp->QueryService(SID_SWebBrowserApp, IID_IWebBrowser, webBrowser);
						}
						return false;
					};

					auto SendCommand = [=](int command)
					{
						/*
						if(shellBrowser)
						{
							IComPtr<IShellView> shellView;
							if(S_OK == shellBrowser->QueryActiveShellView(shellView))
							{
								HWND hwnd = 0;
								if(S_OK == shellView->GetWindow(&hwnd))
									::PostMessageW(hwnd, WM_COMMAND, command, 0);
							}
						}
						*/
						//if(SendMessage(hwnd, WM_COMMAND, 41504, 0, 0) <= 1)
						//	PostMessage(GetParent(hwnd), WM_COMMAND, 41504, 0); // Refresh command
						auto howner = context->wnd.owner;
						for(auto hwnd = howner; hwnd; hwnd = ::GetParent(hwnd))
						{
						//	wchar_t name[250]{};
						//	GetClassName(hwnd, name, _countof(name));
						//	if(_wcsicmp(name, L"ShellTabWindowClass") == 0 || _wcsicmp(name, L"Progman") == 0)
							Hash h = Window::class_hash(hwnd);
							if(h.equals({WindowClass::WC_ShellTabWindowClass, WindowClass::WC_Progman/*,WindowClass::WC_WorkerW*/ }))
							{
								::PostMessageW(hwnd, WM_COMMAND, command, 0);
								return;
							}
						}
						::PostMessageW(howner, WM_COMMAND, command, 0);
					};
					
					auto Sendkey = [=](int vk)
					{
						auto howner = context->wnd.owner;
						for(auto hwnd = howner; hwnd; hwnd = ::GetParent(hwnd))
						{
							//	wchar_t name[250]{};
							//	GetClassName(hwnd, name, _countof(name));
							//	if(_wcsicmp(name, L"ShellTabWindowClass") == 0 || _wcsicmp(name, L"Progman") == 0)
							Hash h = Window::class_hash(hwnd);
							if(h.equals({ WindowClass::WC_ShellTabWindowClass, WindowClass::WC_Progman/*,WindowClass::WC_WorkerW*/ }))
							{
								::PostMessageW(hwnd, WM_KEYDOWN, vk, 0);
								return;
							}
						}
						::PostMessageW(howner, WM_KEYDOWN, vk, 0);
					};

					switch(id)
					{
						case IDENT_COMMAND_NAVIGATE:
						{
							string url = eval_arg(0).to_string().move();
							if(url.empty())
								break;

							if(WebBrowser())
							{
								auto bstrFile = ::SysAllocString(url);
								if(bstrFile)
								{
									VARIANT empty{};
									empty.vt = VT_EMPTY;
									webBrowser->Navigate(bstrFile, &empty, &empty, &empty, &empty);
									::SysFreeString(bstrFile);
								}
							}
							break;
						}
						case IDENT_COMMAND_GOBACK:
							if(WebBrowser())
								webBrowser->GoBack();
							break;
						case IDENT_COMMAND_GOFORWARD:
							if(WebBrowser())
								webBrowser->GoForward();
							break;
						case IDENT_COMMAND_GOHOME:
							if(WebBrowser())
								webBrowser->GoHome();
							break;
						case IDENT_COMMAND_TOGGLE_DESKTOP:
							if(shelldisp())
								shellDisp->ToggleDesktop();
							break;
						case IDENT_COMMAND_MINIMIZE_ALL_WINDOWS:
							if(shelldisp())
								shellDisp->MinimizeAll();
							break;
						case IDENT_COMMAND_RESTORE_ALL_WINDOWS:
							if(shelldisp())
								shellDisp->UndoMinimizeALL();
							break;
						case IDENT_COMMAND_RUN:
							if(shelldisp())
								shellDisp->FileRun();
							break;
						case IDENT_COMMAND_FIND:
							if(shelldisp())
								shellDisp->FindFiles();
							break;
						case IDENT_COMMAND_SWITCHER:
							if(shelldisp())
								shellDisp->WindowSwitcher();
							break;
						case IDENT_COMMAND_SEARCH:
							if(shelldisp())
								shellDisp->SearchCommand();
							break;
						case IDENT_COMMAND_REFRESH:
							//	Explorer::Refresh();
							// if(shelldisp())
							//	shellDisp->RefreshMenu();
							SendCommand(28931);
							// press DOWN "Alt-Tab"

							//Sendkey(VK_F5);
							//MB(Window::class_name(context->wnd.active));
							//::PostMessageW(context->wnd.owner, WM_KEYDOWN, VK_F5, 0);
							break;
						case IDENT_COMMAND_CASCADE_WINDOWS:
							::CascadeWindows(nullptr, 0, nullptr, 0, nullptr);
							break;
						case IDENT_COMMAND_SHOW_WINDOWS_STACKED:
							::TileWindows(nullptr, MDITILE_HORIZONTAL, nullptr, 0, nullptr);
							break;
						case IDENT_COMMAND_SHOW_WINDOWS_SIDE_BY_SIDE:
							::TileWindows(nullptr, MDITILE_VERTICAL, nullptr, 0, nullptr);
							break;
						case IDENT_COMMAND_RESTART_EXPLORER:
							_result = Windows::Explorer::Restart();
							break;
						case IDENT_COMMAND_TOGGLEHIDDEN:
							Explorer::ToggleHiddenFiles();
							break;
						case IDENT_COMMAND_TOGGLEEXT:
							Explorer::ToggleExtensions();
							break;
						case IDENT_COMMAND_SLEEP:
						{
							Object exp = eval_arg(0).move();
							if(exp.is_number())
								::Sleep(exp.to_number<uint32_t>());
							break;
						}
						case IDENT_COPY:
						case IDENT_COMMAND_COPY_TO_CLIPBOARD:
							Text::Clipboard::Set(eval_arg(0).to_string());
							break;
						case IDENT_ID_FOLDER_OPTIONS:
							SendCommand(41251);
							break;
						case IDENT_ID_CUSTOMIZE_THIS_FOLDER:
							SendCommand(28722);
							break;
						case IDENT_ID_COPY_TO_FOLDER:
							SendCommand(28702);
							break;
						case IDENT_ID_MOVE_TO_FOLDER:
							SendCommand(28703);
							break;
						case IDENT_ID_UNDO:
							SendCommand(28699);
							break;
						case IDENT_ID_REDO:
							SendCommand(28704);
							break;
						case IDENT_ID_SELECT_ALL:
						case IDENT_ID_INVERT_SELECTION:
						case IDENT_ID_SELECT_NONE:
						{
							auto SFVIDM_SELECT = is11 ? 28706 : 28704;
							if(id == IDENT_ID_SELECT_ALL)
								SendCommand(SFVIDM_SELECT + 1);
							else if(id == IDENT_ID_INVERT_SELECTION)
								SendCommand(SFVIDM_SELECT + 2);
							else if(id == IDENT_ID_SELECT_NONE)
								SendCommand(SFVIDM_SELECT + 3);
							
							/*
							auto sel = context->Selections;
							if(!sel->ShellBrowser)
								break;
							
							COM_INITIALIZER co(true);

							IComPtr<IShellView> sv;
							if(S_OK != sel->ShellBrowser->QueryActiveShellView(sv))
								break;

							IComPtr<IFolderView2> fv;
							if(S_OK != sv->QueryInterface(IID_IFolderView2, fv))
								break;

							HWND hwnd = 0;
							sel->ShellBrowser->GetWindow(&hwnd);
							//MB(Window::class_name(hwnd));
						
							SHELLEXECUTEINFO sei = { 0 };
							sei.cbSize = sizeof(sei);
							sei.nShow = SW_SHOWNORMAL;
							sei.lpFile = sel->Get(0)->Path;
							sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
							sei.lpVerb = L"Windows.selectall";
							sei.hwnd = hwnd;
							// sei.lpClass = TEXT("Directory");
							ShellExecuteEx(&sei);
							*/
						/*	auto sel = context->Selections;
							if(!sel->ShellBrowser)
								break;
							COM_INITIALIZER co(true);
							IComPtr<IShellView> sv;
							if(S_OK != sel->ShellBrowser->QueryActiveShellView(sv))
								break;

							IComPtr<IFolderView2> fv;
							if(S_OK != sv->QueryInterface(IID_IFolderView2, fv))
								break;
*/
							/*
							const Guid CLSID_SelectAllExecute(L"{aa28fbc7-59f1-4c42-9fd8-ba2be27ea319}");

							//CLSID_SelectAllExecute VALUE:aa28fbc7-59f1-4c42-9fd8-ba2be27ea319
							//CLSID_SelectNoneExecute VALUE:4705a6b4-e4cb-4ed1-af8d-851c644a0459
							//CLSID_InvertSelectionExecute VALUE:0070746c-9a38-4236-822a-72cc4e5c8087
							IComPtr<IExecuteCommand> executeCommand;
							IComPtr<IObjectWithSelection>objectWithSelection;
							IComPtr<IInitializeCommand>initializeCommand;
							
							IComPtr<IShellItemArray> sia;
							HRESULT hr = ::CoCreateInstance(CLSID_SelectAllExecute, NULL, CLSCTX_SERVER, IID_IExecuteCommand, executeCommand);
							if(SUCCEEDED(hr))
							{
								if(S_OK == executeCommand->QueryInterface(IID_IObjectWithSelection, objectWithSelection))
								{
									beep;
									IComPtr<IShellItemArray> siaFolder;
									if(S_OK != fv->GetFolder(IID_IShellItemArray, siaFolder))
										;
									DWORD cc = 0;
									//siaFolder->GetCount(&cc);
									//if(S_OK == fv->GetSelection(FALSE, sia))
									{
										if(S_OK == objectWithSelection->SetSelection(siaFolder))
										{
											if(S_OK == executeCommand->QueryInterface(IID_IInitializeCommand, initializeCommand))
											{
												initializeCommand->Initialize(L"Windows.selectall", 0);
												//E_NOTIMPL
												//beep;
												MBF(L"%d, %x", cc, executeCommand->Execute());
												//	Sleep(2000);
											}
										}	
										
									}
								}
							}*/
						//	Guid iii = __uuidof(IShellWindows), i2= CLSID_ShellWindows;

						//	MBF(L"%s\n%s", iii.to_string(2).c_str(), i2.to_string(2).c_str());
							
							//CLSID_ExecuteFolder
							//Guid clsid(L"{11dbb47c-a525-400b-9e80-a54615a090c0}");

							//R&efresh 
							//SendShellTabCommand(41504);
							
							/*
							GetClassName(parent, name, _countof(name));
							if(_wcsicmp(name, L"SHELLDLL_DefView") == 0)
							{
								// send these commands to the SHELLDLL_DefView window
								if(id == ID_CUT)
								{
									parent.SendMessage(WM_COMMAND, 28696);
									focus.InvalidateRect(NULL);
								}
								if(id == ID_COPY)
									parent.SendMessage(WM_COMMAND, 28697);
								if(id == ID_PASTE)
									parent.SendMessage(WM_COMMAND, 28698);
								if(id == ID_DELETE)
									parent.SendMessage(WM_COMMAND, 28689);
								if(id == ID_PROPERTIES)
									parent.SendMessage(WM_COMMAND, 28691);
								if(id == ID_COPYTO)
									parent.SendMessage(WM_COMMAND, 28702);
								if(id == ID_MOVETO)
									parent.SendMessage(WM_COMMAND, 28703);
							}
							*/
							/*
							if(id == ID_UNDO)
								SendShellTabCommand(28699);
							if(id == ID_REDO)
								SendShellTabCommand(28704);
							if(id == ID_PASTE_SHORTCUT)
								parent.SendMessage(WM_COMMAND, 28700);
							
	R&efresh 28931
							if(id == ID_VIEW_TILES)
								SendShellTabCommand(28748);
							if(id == ID_VIEW_DETAILS)
								SendShellTabCommand(28747);
							if(id == ID_VIEW_LIST)
								SendShellTabCommand(28753);
							if(id == ID_VIEW_CONTENT)
								SendShellTabCommand(28754);
							if(id == ID_VIEW_ICONS1)
								SendShellTabCommand(28752);
							if(id == ID_VIEW_ICONS2)
								SendShellTabCommand(28750);
							if(id == ID_VIEW_ICONS3)
								SendShellTabCommand(28751);
							if(id == ID_VIEW_ICONS4)
								SendShellTabCommand(28749);
							*/
							/*auto sel = context->Selections;
							if(!sel->ShellBrowser)
								break;
							
							IComPtr<IShellView> sv;
							if(S_OK != sel->ShellBrowser->QueryActiveShellView(sv))
								break;
							
							IComPtr<IFolderView2> fv;
							if(S_OK != sv->QueryInterface(IID_IFolderView2, fv))
								break;

							int all_items = 0;
							int sel_items = 0;
							
							fv->ItemCount(SVGIO_ALLVIEW, &all_items);
							*/

							//fv->ItemCount(SVGIO_SELECTION, &sel_items);
							//MBF(L"%d", all_items);
							//MB(Window::class_name(context->wnd.focus));
							/*auto hListView = FindWindowExW(context->wnd.owner, 0, L"DirectUIHWND", 0);
							if(hListView)
							{
								beep;
								SetFocus(hListView);
							}*/
							//fv->SelectItem(-1, SVSI_DESELECTOTHERS);

							/*IComPtr<IShellItemArray> items;
							if(SUCCEEDED(fv->Items(SVGIO_ALLVIEW, __uuidof(IShellItemArray), items)))
							{
								DWORD count = 0;
								items->GetCount(&count);
								for(DWORD i = 0; i < count; i++) 
								{
									IShellItem *_item;
									if(SUCCEEDED(items->GetItemAt(i, &_item)))
									{
									
										try
										{
											if(SUCCEEDED(fv->SelectItem(i, SVSI_FOCUSED | SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE)))
											{
											}
										}
										catch(const std::exception &) {
											beep;
										}
										//break;
									}
								}
							}*/
							/*if(Id[1] == IDENT_ID_SELECT_ALL)
							{
								fv->SelectItem(-1, SVSI_DESELECTOTHERS);
							}
							else if(Id[1] == IDENT_ID_INVERT_SELECTION)
							{
								for(int i = 0; i < all_items; i++)
								{
									LPITEMIDLIST pidl{};
									if(S_OK == fv->Item(i, &pidl))
									{
										SVSIF state;
										if(S_OK == fv->GetSelectionState(pidl, (DWORD *)&state)) 
										{
											if(state == SVSI_DESELECT || state == SVSI_FOCUSED && state != SVSI_SELECT)
											{
												sv->SelectItem(pidl, SVSI_SELECT);
											}
											else
											{
												sv->SelectItem(pidl, SVSI_DESELECT);
											}
										}
									}
								}
							}*/
							break;
						}
					}
					break;
				}
				case IDENT_TITLE:
				{
					if(auto uid = Initializer::get_muid(Id[1]); uid)
					{
						string title = uid->get_title().move();
						if(!title.empty())
							_result = title.move();
					}
					break;
				}
				case IDENT_LENGTH:
				case IDENT_LEN:
				{
					_result = 0;
					Object obj = eval_arg(0).move();
					if(!obj.is_null())
					{
						if(obj.is_array(true))
						{
							auto ptr = obj.get_pointer();
							_result = (uint32_t)ptr[0];
						}
						else if(obj.is_string() || obj.is_number())
						{
							_result = obj.to_string().length();
						}
					}
					break;
				}
				case IDENT_QUOTE:
				{
					if(argc == 0)
						_result = L"\"";
					else
					{
						if(argc == 1)
							_result = (L'"' + eval_arg(0).to_string() + L'"').move();
						else
						{
							_result = (L'\'' + eval_arg(0).to_string() + L'\'').move();
						}
					}
					break;
				}
				case IDENT_RANDOM:
					_result = Environment::RandomNumber(eval_arg(0), eval_arg(1));
					break;
				case IDENT_HIDDEN:
					_result = IDENT_HIDDEN;
					break;
				case IDENT_NORMAL:
					_result = SW_SHOWNORMAL;
					break;
				case IDENT_MINIMIZED:
					_result = SW_SHOWMINIMIZED;
					break;
				case IDENT_MAXIMIZED:
					_result = SW_SHOWMAXIMIZED;
					break;
				case IDENT_BREAK:
					_result = IDENT_BREAK;
					context->Break = true;
					break;
				case IDENT_CONTINUE:
					_result = IDENT_CONTINUE;
					context->Continue = true;
					break;
				case IDENT_NIL:
					_result = nullptr;
					break;
				case IDENT_NULL:
					if(argc > 0) eval_arg(0);
					_result = nullptr;
					break;
				case IDENT_OK:
				case IDENT_YES:
				case IDENT_TRUE:
					_result = true;
					break;
				case IDENT_NO:
				case IDENT_FALSE:
					_result = false;
					break;
				case IDENT_DEFAULT:
				case IDENT_AUTO:
				case IDENT_ENABLE:
				case IDENT_DISABLE:
				case IDENT_ENABLED:
				case IDENT_DISABLED:
				case IDENT_INHERIT:
				case IDENT_NONE:
				case IDENT_BOTH:
				case IDENT_TOP:
				case IDENT_BOTTOM:
				case IDENT_MIDDLE:
				case IDENT_BEFORE:
				case IDENT_AFTER:
				case IDENT_REMOVE:
				case IDENT_MODE_SINGLE:
				case IDENT_MODE_UNIQUE:
				case IDENT_MODE_MULTIPLE:
				case IDENT_MODE_MULTI_SINGLE:
				case IDENT_MODE_MULTI_UNIQUE:
				case IDENT_MODE_MULTI:
				case IDENT_VIS_STATIC:
				case IDENT_VIS_LABEL:
					_result = Id[0];
					break;
				case IDENT_INDEXOF:
					_result = Object(new Object[]{ IDENT_INDEXOF, (int)argc, eval_arg(0).move(), eval_arg(1).move(), eval_arg(2).move() }, true).move();
					break;
				case IDENT_LOC:
				{
					auto e = context->Cache->variables.loc[Id[1]];
					_result = context->Eval(e).move();
					break;
				}
				default:
				{
					eval_var(_result);
					break;
				}
			}

			if(Child)
				_result = context->Eval(Child).move();

			return _result.move();
		}

		void FuncExpression::eval_sel()
		{
			auto sel = context->Selections;
			auto argc = Arguments.size();

			auto array_index = size_t(-1);

			if(Array)
			{
				Object oa = context->Eval(Array).move();
				if(oa.is_number())
					array_index = oa;
			}

			if(!sel || sel->Items.empty() || (Array && (array_index >= sel->Count())))
			{
				if(Id.length() > 1)
				{
					switch(Id[1])
					{
						case IDENT_COUNT:
						case IDENT_BACK:
						case IDENT_LEN:
						case IDENT_LENGTH:
						case IDENT_HIDDEN:
						case IDENT_READONLY:
							_result = 0;
							break;
						case IDENT_TYPE:
						{
							switch(Id[2])
							{
								case IDENT_TYPE_TASKBAR:
									_result = sel->Types[FSO_TASKBAR] > 0;
									break;
								case IDENT_ZERO:
									_result = -1;
									break;
								default:
									_result = 0;
									break;
							}
							break;
						}
						case IDENT_MODE:
						{
							switch(Id[2])
							{
								case IDENT_MODE_SINGLE:
									_result = 1;
									break;
								case IDENT_ZERO:
									_result = 0;
									break;
							}
							break;
						}
						case IDENT_PATH:
						case IDENT_SHORT:
						case IDENT_RAW:
						case IDENT_FULL:
						case IDENT_PARENT:
						case IDENT_LOCATION:
						case IDENT_ITEM:
						case IDENT_NAME:
						case IDENT_FILE:
						case IDENT_DIR:
						case IDENT_DIRECTORY:
						{
							switch(Id[2])
							{
								case IDENT_LEN:
								case IDENT_LENGTH:
									_result = 0;
									break;
							}
							break;
						}
						case IDENT_TYPES:
						case IDENT_SHORTS:
						case IDENT_TITLES:
						case IDENT_NAMES:
						case IDENT_EXTS:
						case IDENT_ROOTS:
						case IDENT_DRIVERS:
						case IDENT_FILES:
						case IDENT_DIRS:
						case IDENT_DIRECTORIES:
						case IDENT_NAMESPACES:
						{
							switch(Id[2])
							{
								case IDENT_COUNT:
									_result = 0;
									break;
							}
							break;
						}
					}
				}
				return;
			}

			auto pathitem = sel->Get(Array ? array_index : 0);

			string quote = nullptr;
			string sep = L" ";

			if(Id.length() == 1)
			{
				//_result = sel->Sel(quote, sep);
			}

			Object arg0;
			
			auto ev_quote = [&](const Object &o) ->string
			{
				string ret;
				if(o.is_string())
					ret = o.to_string().move();
				else if(o.is_number())
				{
					if(o.is_default())
						ret = L"\"";
					else
					{
						int q = o;
						if(q == 1)
							ret = L"\"";
						else if(q == 2)
							ret = L"'";
					}
				}
				return ret.move();
			};

			if(argc > 0)
			{
				arg0 = eval_arg(0).move();

				if(argc > 1)
					sep = eval_arg(1).to_string().move();
				quote = ev_quote(arg0).move();
			}

			/*if(Id.length() == 1)
			{
				if(Array)
				{
					if(Object oa = context->Eval(Array).move(); oa.is_number())
					{
						int ai = oa;
						if(ai >= 0 && ai < (int)sel->Count())
							_result = sel->Get(ai)->get(quote, argc == 2 ? eval_arg(1).to_string().c_str() : nullptr).move();
					}
					return;
				}
				_result = sel->Sel(quote, sep, argc == 3 ? eval_arg(2).to_string().c_str() : nullptr).move();
			}*/

				
			if(Id.length() == 1)
			{
				if(Array)
					_result = sel->Get(array_index)->get(quote, argc == 2 ? eval_arg(1).to_string().c_str() : nullptr).move();
				else
					_result = sel->Sel(quote, sep, argc == 3 ? eval_arg(2).to_string().c_str() : nullptr).move();
				return;
			}

			auto ii = Array ? array_index : sel->Index;

			switch(Id[1])
			{
				case IDENT_GET:
				{
					if(arg0.is_number())
						_result = sel->Path(arg0.to_number<size_t>(), argc > 1 ? ev_quote(eval_arg(1)).c_str() : nullptr);
					break;
				}
				case IDENT_COUNT:
					_result = sel->count();
					break;
				case IDENT_READONLY:
					_result = sel->Get(ii)->ReadOnly;
					break;
				case IDENT_HIDDEN:
					_result = sel->Get(ii)->Hidden;
					break;
				case IDENT_BACK:
					_result = sel->Background;
					break;
				case IDENT_LEN:
				case IDENT_LENGTH:
					_result = sel->Path(ii).length();
					break;
				case IDENT_RAW:
				{
					if(auto p = sel->Get(ii); p)
					{
						if(Id.equals(2, { IDENT_LEN, IDENT_LENGTH }))
							_result = p->Raw.length();
						else
							_result = p->get_raw(quote).move();
					}
					break;
				}
				case IDENT_WORKDIR:
				case IDENT_CURDIR:
					_result = sel->Directory;
					break;
					/*case IDENT_FULL:
					{
						auto path = Path::GetFullPath(sel->Get(sel->Index)->Path);
						if(id->Equals(2, IDENT_PATH_LEN))
							return new NumberExpression(path.Length());
						else if(id->equals(2, IDENT_QUOTE, IDENT_QUOT, IDENT_Q) || qute)
							return  _result = L'"' + path + L'"');
						return  _result = path);
					}*/
				case IDENT_FULL:
				case IDENT_PATH:
				{
					if(auto p = sel->Get(ii); p)
					{
						switch(Id[2])
						{
							case IDENT_LEN:
							case IDENT_LENGTH:
								_result = p->Path.length();
								break;
							case IDENT_NAME:
								_result = p->get_name(quote).move();
								break;
							case IDENT_TITLE:
								_result = p->get_title(quote).move();
								break;
							case IDENT_QUOTE:
								_result = p->get(quote.empty() ? string(L"\"") : quote, argc == 1 ? sep.c_str() : nullptr).move();
								break;
							default:
								_result = p->get(quote, argc == 2 ? sep.c_str() : nullptr).move();
								break;
						}
					}
					break;
				}
				case IDENT_SHORT:
				{
					auto p = sel->Get(ii);
					string short_path = Path::Short(p->Path).move();
					if(Id.equals(2, { IDENT_LEN, IDENT_LENGTH }))
						_result = short_path.length();
					else
						_result = short_path.move();
					break;
				}
				case IDENT_PARENT:
				case IDENT_LOCATION:
				{
					if(Id.length() > 2)
					{
						switch(Id[2])
						{
							case IDENT_NAME:
							case IDENT_TITLE:
							{
								string text;
								if(Id[2] == IDENT_NAME)
									text = Path::Name(sel->Parent).move();
								else
									text = Path::Title(sel->Parent).move();

								if(Id.is_length(3))
									_result = text.length();
								else
									_result = text.move();
								break;
							}
							case IDENT_RAW:
								_result = sel->ParentRaw;
								break;
							case IDENT_QUOTE:
								_result = sel->Quote(sel->Parent, quote.empty() ? string(L"\"") : quote).move();
								break;
							case IDENT_LEN:
							case IDENT_LENGTH:
								_result = sel->Parent.length();
								break;
						}
					}
					else
					{
						_result = quote.empty() ? sel->Parent : sel->Quote(sel->Parent).move();
					}
					break;
				}
				case IDENT_ROOT:
				{
					_result = sel->Root(ii).move();
					break;
				}
				case IDENT_ITEM:
				{
					if(Id.is_length(2))
						_result = sel->Path(ii).length();
					else
						_result = L"@sel.item";
					break;
				}
				case IDENT_NAME:
				case IDENT_TITLE:
				{
					auto p = sel->Get(ii);
					string text = (Id[1] == IDENT_NAME) ? p->Name : p->Title;
					if(Id.is_length(2))
						_result = text.length();
					else
						_result = text.move();
					break;
				}
				case IDENT_FILE:
				{
					auto p = sel->Get(ii);
					if(Id.length() > 2)
					{
						switch(Id[2])
						{
							case IDENT_EXT:
							case IDENT_NAME:
							case IDENT_TITLE:
							{
								string text;
								if(Id.equals(2, IDENT_EXT))
									text = p->Extension;
								else if(Id.equals(2, IDENT_TITLE))
									text = p->Title;
								else
									text = p->Name;

								if(Id.is_length(3))
									_result = text.length();
								else
									_result = text;
								break;
							}
							case IDENT_LEN:
							case IDENT_LENGTH:
								_result = p->Name.length();
								break;
						}
						return;
					}
					_result = p->Name;
					break;
				}
				case IDENT_FILES:
				{
					string result;
					if(argc == 1)
						sep = arg0.to_string().move();

					if(Id.length() == 2)
						_result = sel->SelFiles(quote, sep).move();
					else switch(Id[2])
					{
						case IDENT_COUNT:
							_result = sel->count.FILE;
							break;
						case IDENT_EXT:
							_result = sel->GetFilesExtension(quote, sep).move();
							break;
						case IDENT_TITLE:
							_result = sel->FilesNameWithoutExtension(quote, sep).move();
							break;
						case IDENT_NAME:
							_result = sel->FilesName(quote, sep).move();
							break;
						case IDENT_QUOTE:
							_result = sel->SelFiles(quote.empty() ? string(L"\"") : quote, sep).move();
							break;
					}
					break;
				}
				case IDENT_DIR:
				case IDENT_DIRECTORY:
				{
					string s;
					if(int32_t(pathitem->Type) > 1)
						s = pathitem->Path;
					else
						s = sel->Parent;
/*
					if(Id.is_quote(2))
						_result = (L'"' + s + L'"').move();
					else if(Id.is_length(2))
						_result = s.length();
					else if(Id.is_title(2))
						_result = Path::Title(s);
					else
						_result = quote ? (L'"' + s + L'"').move() : s.move();
*/
					switch(Id[2])
					{
						case IDENT_QUOTE:
							_result = sel->Quote(s, quote.empty() ? string(L"\"") : quote).move();
							break;
						case IDENT_LEN: case IDENT_LENGTH:
							_result = s.length();
							break;
						case IDENT_NAME:
						case IDENT_TITLE:
							if(quote)
								_result = sel->Quote(Path::Title(s)).move();
							else 
								_result = Path::Title(s).move();
							break;
						default:
							_result = quote ? sel->Quote(s).move() : s.move();
							break;
					}
					break;
				}
				case IDENT_DIRS:
				case IDENT_DIRECTORIES:
				{
					switch(Id[2])
					{
						case IDENT_QUOTE:
							_result = sel->SelDirs(quote.empty() ? string(L"\"") : quote, sep).move();
							break;
						case IDENT_COUNT:
							_result = sel->count.DIRECTORY;
							break;
						case IDENT_NAME:
						case IDENT_TITLE:
							_result = sel->DiresName(quote, sep).move();
							break;
						default:
							_result = sel->SelDirs(quote, sep).move();
							break;
					}
					break;
				}
				case IDENT_PATHS:
				{
					switch(Id[2])
					{
						case IDENT_QUOTE:
							_result = sel->Sel(quote.empty() ? string(L"\"") : quote, sep).move();
							break;
						case IDENT_COUNT:
							_result = sel->count();
							break;
						case IDENT_NAME:
						case IDENT_TITLE:
							_result = sel->PathsName(quote, sep).move();
							break;
						default:
							_result = sel->Sel(quote, sep).move();
							break;
					}
					break;
				}
				case IDENT_SHORTS:
				{
					switch(Id[2])
					{
						case IDENT_COUNT:
							_result = sel->count();
							break;
						default:
							_result = sel->ShortPaths(sep).move();
							break;
					}
					break;
				}
				case IDENT_ROOTS:
				case IDENT_DRIVERS:
				{
					switch(Id[2])
					{
						case IDENT_COUNT:
							_result = sel->count.DRIVE;
							break;
						default:
							_result = sel->SelDrives(sep).move();
							break;
					}
					break;
				}
				case IDENT_NAMESPACES:
				{
					switch(Id[2])
					{
						case IDENT_COUNT:
							_result = sel->count.NAMESPACE;
							break;
						default:
							_result = sel->SelNamespaces(quote, sep).move();
							break;
					}
					break;
				}
				case IDENT_TITLES:
					_result = sel->FilesNameWithoutExtension(nullptr, sep).move();
					break;
				case IDENT_NAMES:
					_result = sel->FilesName(nullptr, sep).move();
					break;
				case IDENT_EXTS:
					_result = sel->GetFilesExtension(nullptr, sep).move();
					break;
				case IDENT_TYPES:
				{
					switch(Id[2])
					{
						case IDENT_COUNT:
							_result = sel->count();
							break;
					}
					break;
				}
				case IDENT_INDEX:
				case IDENT_I:
				{
					int32_t i = eval_arg(0);
					string q = nullptr;
					if(argc == 2)
						q = eval_arg(1).to_string();
					if(auto p = sel->Get(i))
						_result = p->get(q).move();
					break;
				}
				case IDENT_TOFILE:
				{
					auto warning = L"Unable to interpret 'sel.tofile' File path not found.";
					string path;
					sep = L"\n";
					
					if(argc == 3)
						path = eval_arg(2).to_string();
					else
						path = Path::Combine(Path::TempDirectory(), L"shell-selections.txt").move();

					if(path.empty())
					{
						Logger::Warning(warning);
						break;
					}

					if(argc >= 1)
					{
						if(argc > 1)
							sep = eval_arg(1).to_string();
					}

					auto_handle hTempFile = ::CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
														  CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
					if(hTempFile)
					{
						//write all paths for the selected items to a file
						DWORD dwBytesWritten = 0;
						//unsigned char utf8[3] = { 0xEF, 0xBB, 0xBF };
						//::WriteFile(hTempFile, utf8, 3, &dwBytesWritten, nullptr);

						size_t len = 0;
						char *lputf8 = nullptr;

						for(size_t i = 0; i < sel->Items.size(); i++)
						{
							auto const &p = sel->Path(i, quote);
							len = UTF8::FromUnicode(p, p.length(), &lputf8);
							autostr<char> s(lputf8, len);
							::WriteFile(hTempFile, s, (DWORD)s.len, &dwBytesWritten, nullptr);
							if(sep.length() > 0 && i < sel->Items.size() - 1)
							{
								lputf8 = nullptr;
								len = UTF8::FromUnicode(sep, sep.length(), &lputf8);
								autostr<char> sep_(lputf8, len);
								::WriteFile(hTempFile, sep_,(DWORD)sep_.len, &dwBytesWritten, nullptr);
							}
						}
						_result = path.move();
					}
					else
					{
						Logger::Warning(warning);
					}
					
					break;
				}
				case IDENT_TYPE:
				{
					auto t = PathType::Unknown;

					if(argc == 0)
						t = sel->Get(ii)->Type;
					else if(argc > 0)
					{
						Object a1 = eval_arg(0);

						if(a1.is_number())
							t = sel->Get(a1.to_number<size_t>())->Type;

						switch(t)
						{
							case PathType::Back:
								_result = sel->Types[FSO_BACK] > 0;
								return;
							case PathType::Back_Namespace:
								_result = sel->Types[FSO_BACK_NAMESPACE] > 0;
								break;
							case PathType::Back_Directory:
								_result = sel->Types[FSO_BACK_DIRECTORY] > 0;
								break;
							case PathType::Back_Drive:
								_result = sel->Types[FSO_BACK_DRIVE] > 0;
								break;
							case PathType::Back_Fixed:
								_result = sel->Types[FSO_BACK_DRIVE_FIXED] > 0;
								break;
							case PathType::Back_USB:
								_result = sel->Types[FSO_BACK_DRIVE_USB] > 0;
								break;
							case PathType::Back_VHD:
								_result = sel->Types[FSO_BACK_DRIVE_VHD] > 0;
								break;
							case PathType::Back_DVD:
								_result = sel->Types[FSO_BACK_DRIVE_DVD] > 0;
								break;
							case PathType::Back_Removable:
								_result = sel->Types[FSO_BACK_DRIVE_REMOVABLE] > 0;
								break;
							case PathType::Back_Remote:
								_result = sel->Types[FSO_BACK_DRIVE_REMOTE] > 0;
								break;
							case PathType::Taskbar:
								_result = sel->Types[FSO_TASKBAR] > 0;
								break;
							case PathType::Drive:
								_result = sel->Types[FSO_DRIVE] > 0;
								break;
							case PathType::Directory:
								_result = sel->Types[FSO_DIRECTORY] > 0;
								break;
							case PathType::File:
								_result = sel->Types[FSO_FILE] > 0;
								break;
							case PathType::Fixed:
								_result = sel->Types[FSO_DRIVE_FIXED] > 0;
								break;
							case PathType::VHD:
								_result = sel->Types[FSO_DRIVE_VHD] > 0;
								break;
							case PathType::DVD:
								_result = sel->Types[FSO_DRIVE_DVD] > 0;
								break;
							case PathType::USB:
								_result = sel->Types[FSO_DRIVE_USB] > 0;
								break;
							case PathType::Removable:
								_result = sel->Types[FSO_DRIVE_REMOVABLE] > 0;
								break;
							case PathType::Namespace:
								_result = sel->Types[FSO_NAMESPACE] > 0;
								break;
							case PathType::Remote:
								_result = sel->Types[FSO_DRIVE_REMOTE] > 0;
								break;
							case PathType::Desktop:
								_result = sel->Types[FSO_DESKTOP] > 0;
								break;
							case PathType::Computer:
								_result = sel->Types[FSO_NAMESPACE_COMPUTER] > 0;
								break;
							case PathType::Recyclebin:
								_result = sel->Types[FSO_NAMESPACE_RECYCLEBIN] > 0;
								break;
						}
					}
					_result = (int32_t)t;
					break;
				}	
				case IDENT_MODE:
					_result = (int8_t)sel->Mode;
					break;
				case IDENT_LNK:
				case IDENT_LNKTARGET:
				{
					if(sel == nullptr)
						break;
					auto p = sel->Get(ii);
					if(p == nullptr)
						break;

					if(Id[2] == IDENT_DIR || Id[2] == IDENT_LOCATION)
					{
						string w;
						if(Path::GetLinkInfo(p->get(), nullptr, &w, nullptr) && !w.empty())
							_result = w.move();
					}
					else if(Id[2] == IDENT_ICON)
					{
						string icon;
						if(Path::GetLinkInfo(p->get(), nullptr, nullptr, &icon) && !icon.empty())
							_result = icon.move();
					}
					else
					{
						string target;
						if(Path::GetLinkInfo(p->get(), &target, nullptr) && !target.empty())
						{
							switch(Id[2])
							{
								case IDENT_TYPE:
								{
									if(target.length() == 3 && target.ends_with(L":\\", false))
										_result = 3;
									else
									{
										auto attr = ::GetFileAttributesW(target.data());
										if(Path::IsDirectory(attr))
											_result = 2;
										else if(Path::IsFile(attr))
											_result = 1;
										else
											_result = 0;
									}
									break;
								}
								case IDENT_LNK_TARGET:
								case IDENT_ZERO:
									_result = target.move();
									break;
							}
						}
					}
					break;
				}
				case IDENT_META:
				{
					auto sb = context->Selections->ShellBrowser;
					if(sb)
					{
						IComPtr<IShellView> sv;

						if(S_OK != sb->QueryActiveShellView(sv))
							break;

						IComPtr<IFolderView2> fv;
						if(S_OK != sv->QueryInterface(IID_IFolderView2, fv))
							break;

						IComPtr<IShellItemArray> sia;
						if(S_OK != fv->GetSelection(FALSE, sia))
							break;
						
						DWORD sel_count = 0;
						if(S_OK != sia->GetCount(&sel_count) || sel_count == 0)
							break;

						uint32_t ai = 0;

						if(Array)
						{
							if(Object oa = context->Eval(Array).move(); oa.is_number())
							{
								ai = oa;
								if(ai < 0 || ai >= sel_count)
									return;
							}
						}

						IComPtr<IShellItem2> si;
						if(S_OK != sia->GetItemAt(ai, si))
							break;

						/*string path = eval_arg(0).to_string().trim().move();
						
						if(path.empty())
							break;

						string name = eval_arg(1).to_string().trim().move();
						string sys = L"System.";

						if(name.empty())
							name = sys + L"Title";
						else if(!name.starts_with(sys))
							name = sys + name;

						PROPERTYKEY key{};
						auto hr = ::PSGetPropertyKeyFromName(name, &key);
						if(SUCCEEDED(hr))
						{
							IComPtr<IPropertyStore> ps;
							// Call the helper to get the property store for the initialized item
							hr = ::SHGetPropertyStoreFromParsingName(path, nullptr, GPS_DEFAULT, __uuidof(IPropertyStore), ps);
							if(SUCCEEDED(hr))
							{
								PROPVARIANT propvarValue = { 0 };
								hr = ps->GetValue(key, &propvarValue);
								if(SUCCEEDED(hr))
								{
									wchar_t* pszDisplayValue = nullptr;
									hr = ::PSFormatForDisplayAlloc(key, propvarValue, PDFF_DEFAULT, &pszDisplayValue);
									if(SUCCEEDED(hr))
									{
										_result = pszDisplayValue;
										::CoTaskMemFree(pszDisplayValue);
									}
									::PropVariantClear(&propvarValue);
								}
							}
						}*/

						string name = eval_arg(0).to_string().trim().move();
						string sys = L"System.";

						if(name.empty())
							name = sys + L"Title";
						else if(!name.starts_with(sys))
							name = sys + name;

						PROPERTYKEY property_key{};
						auto hr = ::PSGetPropertyKeyFromName(name, &property_key);
						if(SUCCEEDED(hr))
						{
							wchar_t *value = nullptr;
							hr = si->GetString(property_key, &value);
							if(SUCCEEDED(hr))
							{
								_result = value;
								::CoTaskMemFree(value);
							}
						}
					}
					break;
				}
				/*case IDENT_FOREACH:
				{
					string sel_foreach;

					for(uint32_t i = 0; i < sel->Count(); i++)
					{
						autoexpr args(Eval(f->Get(1)));
						if(!args.isNull())
						{

						}
						autoexpr cmd(Eval(param));
						if(!cmd.isNull())
							sel_foreach.Append(cmd.getString());
					}

					if(!sel_foreach.IsNullOrEmpty())
						return  _result = sel_foreach);
					return nullptr;
				}*/
			}
			return;
		}

		void FuncExpression::eval_path()
		{
			auto argc = Arguments.size();
			string arg0 = eval_arg(0).to_string().move();

			switch(Id[1])
			{
				case IDENT_CURDIR:
				case IDENT_CURRENTDIRECTORY:
				{
					if(argc > 0)
						_result = ::SetCurrentDirectoryW(arg0);
					else 
					{
						auto sel = context->Selections;
						if(sel != nullptr)
							_result = sel->Directory;
						else
							_result = Path::CurrentDirectory().move();
					}
					break;
				}
				case IDENT_ROOT:
					_result = Path::GetRoot(arg0);
					break;
				case IDENT_NAME:
					_result = Path::Name(arg0).move();
					break;
				case IDENT_TITLE:
					_result = Path::Title(arg0).move();
					break;
				case IDENT_SHORT:
					_result = Path::Short(arg0).move();
					break;
				case IDENT_FULL:
					_result = Path::Full(arg0).move();
					break;
				case IDENT_PARENT:
				case IDENT_LOCATION:
				{
					string parent = Path::Parent(arg0).move();
					if(Id.equals(2, IDENT_NAME))
						_result = Path::Name(parent).move();
					else if(Id.equals(2, IDENT_TITLE))
						_result = Path::Title(parent).move();
					else
						_result = parent.move();
					break;
				}
				case IDENT_EMPTY:
				{
					uint32_t empties = Path::IsDirectoryEmpty(arg0);
					for(size_t i = 1; i < argc; i++)
						empties += Path::IsDirectoryEmpty(eval_arg(i).to_string());
					_result = empties != 0;
					break;
				}
				case IDENT_EXISTS:
				{
					uint32_t exists = Path::Exists(arg0);
					if(exists == 0)
					{
						for(size_t i = 1; i < argc; i++)
							exists += Path::Exists(eval_arg(i).to_string());
					}
					_result = exists != 0;
					break;
				}
				case IDENT_JOIN:
				case IDENT_COMBINE:
				{
					string path = arg0.move();
					for(size_t i = 1; i < argc; i++)
						path = Path::Combine(path, eval_arg(i).to_string()).move();
					_result = path.move();
					break;
				}
				case IDENT_SEP:
				case IDENT_SEPARATOR:
				{
					string sep =  L"/";
					if(argc == 2)
						sep = eval_arg(1).to_string().move();
					if(sep)
						_result = Path::FixSeparator(arg0, sep).move();
					else
						_result = Path::FixSeparator(arg0, L"\\").move();
					break;
				}
				case IDENT_ISABSOLUTE:
					_result = Path::IsAbsolute(arg0);
					break;
				case IDENT_ISRELATIVE:
					_result = Path::IsRelative(arg0);
					break;
				case IDENT_ISFILE:
					_result = Path::IsFileExists(arg0);
					break;
				case IDENT_ISDIR:
				case IDENT_ISDIRECTORY:
					_result = Path::IsDirectoryExists(arg0);
					break;
				case IDENT_ISROOT:
				case IDENT_ISDRIVE:
					_result = Path::IsDrive(arg0);
					break;
				case IDENT_ISCLSID:
				case IDENT_ISNAMESPACE:
					_result = Path::IsNameSpace(arg0);
					break;
				case IDENT_ISEXE:
					_result = Path::IsExe(arg0);
					break;
				case IDENT_REMOVEEXTENSION:
					_result = Path::RemoveExtension(arg0);
					break;
				case IDENT_GETKNOWNFOLDER:
					CLSID id; 
					if(S_OK == ::IIDFromString(arg0.c_str(), &id))
						_result = Path::GetKnownFolder(id).move();
					break;
				case IDENT_ABSOLUTE:
					//std::filesystem::path(arg0.c_str()).absolute_path();
					break;
				case IDENT_RELATIVE:
				//	std::filesystem::path(arg0.c_str()).relative_path();
					break;
				case IDENT_LNK:
				case IDENT_LNKTARGET:
				{
					if(Id[2] == IDENT_DIR || Id[2] == IDENT_LOCATION)
					{
						string w;
						if(Path::GetLinkInfo(arg0, nullptr, &w))
							_result = w.move();
					}
					else if(Id[2] == IDENT_ICON)
					{
						string icon;
						if(Path::GetLinkInfo(arg0, nullptr, nullptr, &icon) && !icon.empty())
							_result = icon.move();
					}
					else if(Id[2] == IDENT_CREATE)
					{
						if(arg0.empty())
						{
							_result = false;
							break;
						}
						//path.lnk.create(path, target, args, icon, dir, run, comment)
						string target = eval_arg(1).to_string().move();
						if(target.empty())
						{
							_result = false;
							break;
						}

						string iconPath = argc > 3 ? eval_arg(3).to_string().c_str() : nullptr;
						
						if(iconPath.empty())
							iconPath = target;

						int iconIndex = Path::ParseLocation(iconPath);

						string wdir = argc > 4 ? eval_arg(4).to_string().c_str() : nullptr;
						if(wdir.empty()/* && argc < 4*/)
							wdir = Path::Parent(target);
						
						_result = Path::CreateLnk(arg0, eval_arg(1).to_string(),
												  argc > 2 ? eval_arg(2).to_string().c_str() : nullptr,
												  iconPath,
												  iconIndex,
												  wdir.empty() ? nullptr : wdir.c_str(),
												  argc > 5 ? (int)eval_arg(5) : SW_NORMAL,
												  argc > 6 ? eval_arg(6).to_string().c_str() : nullptr,
												  argc > 7 ? (bool)eval_arg(7).to_number() : false);
					}
					else 
					{
						string target;
						if(Path::GetLinkInfo(arg0, &target, nullptr) && !target.empty())
						{
							switch(Id[2])
							{
								case IDENT_TYPE:
								{
									if(target.length() == 3 && target.ends_with(L":\\", false))
										_result = 3;
									else
									{
										auto attr = ::GetFileAttributesW(target.data());
										if(Path::IsDirectory(attr))
											_result = 2;
										else if(Path::IsFile(attr))
											_result = 1;
										else
											_result = 0;
										/*auto ret = PathType::Unknown;
										auto attr = ::GetFileAttributesW(target.data());
										if(Path::IsDirectory(attr))
											ret = PathType::Directory;
										else if(Path::IsFile(attr))
											ret = PathType::File;
										else if(Path::IsAttribNameSpace(attr))
											ret = PathType::Namespace;
										else
											ret = PathType::Unknown;
										_result = (int)ret;*/
									}
									break;
								}
								case IDENT_LNK_TARGET:
								case IDENT_ZERO:
									_result = target.move();
									break;
							}
						}
					}
					break;
				}
				case IDENT_FILE:
				{
					switch(Id[2])
					{
						case IDENT_NAME:
							_result = Path::Name(arg0).move();
							break;
						case IDENT_TITLE:
							_result = Path::Title(arg0).move();
							break;
						case IDENT_EXT:
							_result = Path::Extension(arg0);
							break;
						case IDENT_BOX:
						{

							OPENFILENAMEW ofn = { sizeof(OPENFILENAMEW) };

							string initialDir;
							std::vector<wchar_t> flt;
							wchar_t szFile[MAX_PATH]{ };

							if(argc == 0)
							{
								for(auto c : L"All Files (*.*)\0*.*\0")
									flt.push_back(c);
							}
							else
							{
								for(auto c : arg0)
									flt.push_back(c == L'|' ? L'\0' : c);

								if(argc == 2)
									initialDir = eval_arg(1).to_string().move();

								if(argc == 3)
									string::Copy(szFile, eval_arg(2).to_string());
							}

							flt.push_back(0);
							flt.push_back(0);

							ofn.hwndOwner = context->Runtime ? context->wnd.active : nullptr;
							ofn.lpstrFilter = flt.data();
							ofn.lpstrInitialDir = initialDir;
							ofn.lpstrFile = szFile;
							ofn.nMaxFile = MAX_PATH;
							_result = Path::OpenFileDialog(&ofn).move();

							break;
						}
						case IDENT_ZERO:
							break;
					}
					break;
				}
				case IDENT_DIR:
				case IDENT_DIRECTORY:
				{
					switch(Id[2])
					{
						case IDENT_NAME:
							_result = Path::Name(arg0).move();
							break;
						case IDENT_TITLE:
							_result = Path::Title(arg0).move();
							break;
						case IDENT_BOX:
						{
							auto hwnd = context->Runtime ? context->wnd.active : nullptr;
							_result = Path::OpenFolderDialog(string::Extract(IDS_SELECT_FOLDER), hwnd).move();
							break;
						}
						case IDENT_ZERO:
							break;
					}
					break;
				}
				case IDENT_FILES:
				{
					// path.files(path, sep, quots, type[0=all,1=dirs,2=files], full path)
					// path.files(path,'**', sep, [2=files | 3=dirs | 5=files+dirs | 8=quots | 16=full path]|32=titles|64=ext)
					// path.files(path, sep, 1|2|4|8|16)

					arg0.trim(L"\"'");

					string file_name = arg0;
					string flt = L"*";
					bool quots = false;
					bool full_path = false;
					uint8_t types = 0;
					uint32_t flags = 0;
					
					if(argc > 1)
					{
						Object obj = eval_arg(1).move();
						if(!obj.is_default())
						{
							flt = obj.to_string().trim().move();
							if(flt.empty())
							{
								flt = L"*";
							}
						}

						if(argc > 2)
						{
							obj = eval_arg(2).move();
							if(obj.not_default())
								flags = obj.to_number<uint32_t>();

							if(flags > 0)
							{
								types = flags & 2;
								types |= flags & 3;
								quots = flags & 8;
								full_path = flags & 16;
							}
						}
					}

					WIN32_FIND_DATA ffd{};
					auto hFind = ::FindFirstFileW(Path::Combine(file_name, flt), &ffd);
					if(hFind != INVALID_HANDLE_VALUE)
					{
						int index = -1;
						std::vector<string> list;

						if(Array)
						{
							if(Object oa = context->Eval(Array).move(); oa.is_number())
								index = oa;
						}

						auto i = 0;

						auto add = [&]()
						{
							string fn;
							if(quots)
								fn = L"\"";

							if(full_path)
								fn += Path::Combine(file_name, ffd.cFileName);
							else
								fn += ffd.cFileName;

							if(quots)
								fn += L"\"";

							list.push_back(fn.move());
						};

						do
						{
							if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
							{
								// skip current and parent directories
								if(ffd.cFileName[0] == L'.' && (ffd.cFileName[1] == L'.' || ffd.cFileName[1] == 0))
									continue;

								if(types == 3)
									continue;
							}
							else if(types == 2)
								continue;

							if(index >= 0)
							{
								if(i == index)
								{
									add();
									break;
								}
							}
							else
							{
								add();
							}

							i++;

						} while(::FindNextFileW(hFind, &ffd));

						::FindClose(hFind);

						if(!list.empty())
						{
							if(index >= 0)
								_result = list.back();
							else
							{
								auto array = new Object[list.size() + 1];
								Object result(array, true);
								*array++ = list.size();
								for(auto &s : list)
								{
									try {
										*array++ = s.move();
									}
									catch(...) {}
								}
								_result = result.move();
							}
						}
					}
					break;
				}
				case IDENT_PACKAGE:
				{
					auto ver = &Version::Instance();
					if(ver->IsWindows81OrGreater())
					{
						const wchar_t *packageFamily = arg0;
						if(!arg0.empty())
						{
							auto GetPackagesByPackageFamily = "GetPackagesByPackageFamily";
							uint32_t count = 0, bufferLength = 0;
							auto res = DLL::Kernel32<long>(GetPackagesByPackageFamily, packageFamily, &count, nullptr, &bufferLength, nullptr);
							if(res == ERROR_INSUFFICIENT_BUFFER && count > 0)
							{
								wchar_t buffer[MAX_PATH] {};
								auto packageFullNames = new wchar_t[260];
								res = DLL::Kernel32<long>(GetPackagesByPackageFamily, packageFamily, &count, &packageFullNames, &bufferLength, buffer);
								if(res == ERROR_SUCCESS)
								{
									uint32_t length = 0;
									//GetStagedPackagePathByFullName
									auto GetPackagePathByFullName = "GetPackagePathByFullName";
									res = DLL::Kernel32<long>(GetPackagePathByFullName, packageFullNames, &length, nullptr);
									if(res == ERROR_INSUFFICIENT_BUFFER && length > 0)
									{
										string packagePath(length);
										res = DLL::Kernel32<long>(GetPackagePathByFullName, packageFullNames, &length, packagePath.buffer(length));
										_result = packagePath.release(length-1).trim().move();
									}
								}
							}
						}
					}
					break;
				}
				case IDENT_WSL:
				{
					if(!arg0.empty())
					{
						string wsl = L"/mnt/";
						string path = Path::FixSeparator(arg0, L"/").move();
						
						if(path[1] == L':')
						{
							wsl += Char::ToLower(path[0]);
							path = path.substr(2).move();
						}

						wsl += path.replace(L" ", "\\ ", false).move();

						_result = wsl.move();
					}
					break;
				}
				case IDENT_REMOVE_ARGS:
				{
					if(!arg0.empty())
						::PathRemoveArgsW(arg0.data());
					_result = arg0.release().move();
					break;
				}
				case IDENT_ARGS:
				{
					if(!arg0.empty())
						arg0 = ::PathGetArgsW(arg0);
					_result = arg0.move();
					break;
				}
				case IDENT_COMPACT:
				{
					uint32_t dx = argc > 1 ? eval_arg(1).to_number() : 50;
					::PathCompactPathW(nullptr, arg0.data(), dx);
					_result = arg0.release().move();
					break;
				}
				case IDENT_QUOTE_SPACES:
				{
					::PathQuoteSpacesW(arg0.data());
					_result = arg0.release().move();
					break;
				}
				case IDENT_REMOVE_EXTENSION:
				{
					::PathRemoveExtensionW(arg0.data());
					_result = arg0.release().move();
					break;
				}
				case IDENT_RENAME_EXTENSION:
				{
					::PathRenameExtensionW(arg0.data(), eval_arg(1).to_string());
					_result = arg0.release().move();
					break;
				}
				case IDENT_ADD_EXTENSION:
				{
					::PathAddExtensionW(arg0.data(), eval_arg(1).to_string());
					_result = arg0.release().move();
					break;
				}
				case IDENT_REFRESH:
				{
					::SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH, arg0.c_str(), nullptr);
					/*
					INPUT inputs[4] = {};
					ZeroMemory(inputs, sizeof(inputs));

					inputs[0].type = INPUT_KEYBOARD;
					inputs[0].ki.wVk = VK_CONTROL;

					inputs[1].type = INPUT_KEYBOARD;
					inputs[1].ki.wVk = 'T';

					inputs[2].type = INPUT_KEYBOARD;
					inputs[2].ki.wVk = 'T';
					inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

					inputs[3].type = INPUT_KEYBOARD;
					inputs[3].ki.wVk = VK_CONTROL;
					inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

					SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
					*/
				}
			}
		}

		STDAPI SHBindToObjectEx(IShellFolder *psf, LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv)
		{
			HRESULT hr;
			IShellFolder *psfRelease = nullptr;

			if(!psf)
			{
				std::ignore = SHGetDesktopFolder(&psf);
				psfRelease = psf;
			}

			if(psf)
			{
				if(!pidl || ILIsEmpty(pidl))
					hr = psf->QueryInterface(riid, ppv);
				else
					hr = psf->BindToObject(pidl, pbc, riid, ppv);
			}
			else
			{
				*ppv = NULL;
				hr = E_FAIL;
			}

			if(psfRelease)
				psfRelease->Release();
			

			if(SUCCEEDED(hr) && (*ppv == NULL))
			{
				// Some shell extensions (eg WS_FTP) will return success and a null out pointer
				hr = E_FAIL;
			}

			return hr;
		}

		STDAPI BindCtx_CreateWithMode(DWORD grfMode, IBindCtx **ppbc)
		{
		//	ASSERTMSG(ppbc != NULL, "Caller must pass valid ppbc");

			HRESULT hr = CreateBindCtx(0, ppbc);
			if(SUCCEEDED(hr))
			{
				BIND_OPTS bo = { sizeof(bo), 0, grfMode, 0 };
				hr = (*ppbc)->SetBindOptions(&bo);

				if(FAILED(hr))
				{
					//ATOMICRELEASE(*ppbc);
				}
			}

			//ASSERT(SUCCEEDED(hr) ? (*ppbc != NULL) : (*ppbc == NULL));
			return hr;
		}

		STDAPI StgBindToObject(LPCITEMIDLIST pidl, DWORD grfMode, REFIID riid, void **ppv)
		{
			IBindCtx *pbc;
			HRESULT hr = BindCtx_CreateWithMode(grfMode, &pbc);
			if(SUCCEEDED(hr))
			{
				hr = SHBindToObjectEx(NULL, pidl, pbc, riid, ppv);

				pbc->Release();
			}
			return hr;
		}

		/*STDAPI SHGetUIObjectFromFullPIDL(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv)
		{
			*ppv = NULL;

			LPCITEMIDLIST pidlChild;
			IShellFolder *psf;
			HRESULT hr = SHBindToIDListParent(pidl, IID_PPV_ARG(IShellFolder, &psf), &pidlChild);
			if(SUCCEEDED(hr))
			{
				hr = psf->GetUIObjectOf(hwnd, 1, &pidlChild, riid, NULL, ppv);
				psf->Release();
			}

			return hr;
		}

		STDAPI SHGetTargetFolderIDList(LPCITEMIDLIST pidlFolder, LPITEMIDLIST *ppidl)
		{
			*ppidl = NULL;

			// likely should ASSERT() that pidlFolder has SFGAO_FOLDER
			IShellLink *psl;
			HRESULT hr = SHGetUIObjectFromFullPIDL(pidlFolder, NULL, IID_PPV_ARG(IShellLink, &psl));
			if(SUCCEEDED(hr))
			{
				hr = psl->GetIDList(ppidl);
				psl->Release();
			}

			*ppidl = ::ILClone(pidlFolder);
			return hr && *ppidl;
		}
		*/
#define IID_PPV_ARG0(arg) __uuidof(arg), (void**)&arg

		HRESULT CreateStreamOrStorage(IStorage *pStorageParent, LPCTSTR pszName, REFIID riid, void **ppv)
		{
			DWORD grfModeCreated = STGM_READWRITE;
			HRESULT hr = E_INVALIDARG;

			if(IsEqualGUID(riid, IID_IStorage))
			{
				IStorage *pStorageCreated;
				hr = pStorageParent->CreateStorage(pszName, grfModeCreated, 0, 0, &pStorageCreated);

				if(SUCCEEDED(hr))
				{
					hr = pStorageParent->Commit(STGC_DEFAULT);
					*ppv = pStorageCreated;
				}
			}
			else if(IsEqualGUID(riid, IID_IStream))
			{
				IStream *pStreamCreated;
				hr = pStorageParent->CreateStream(pszName, grfModeCreated, 0, 0, &pStreamCreated);

				if(SUCCEEDED(hr))
				{
					hr = pStorageParent->Commit(STGC_DEFAULT);
					*ppv = pStreamCreated;
				}
			}

			return hr;
		}

		LPITEMIDLIST GeneratePidlFromName(PCIDLIST_ABSOLUTE pidlFolder, LPWSTR pszFile)
		{
			LPITEMIDLIST ppidl = nullptr;
			IComPtr<IShellFolder> psf;
			if(SUCCEEDED(SHBindToObject(nullptr, pidlFolder, 0, IID_IShellFolder, psf)))
			{
				LPITEMIDLIST pidlItem;
				if(SUCCEEDED(psf->ParseDisplayName(nullptr, nullptr, pszFile, nullptr, &pidlItem, nullptr)))
				{
					ppidl = ::ILCombine(pidlFolder, pidlItem);
					::ILFree(pidlItem);
				}
				psf->Release();
			}
			return ppidl;
		};

		void FuncExpression::eval_io()
		{
			try
			{
				auto argc = Arguments.size();
				Object obj0 = eval_arg(0).move();
				string arg0 = obj0.to_string();

				switch(Id[1])
				{
					case IDENT_DELETE:
						_result = Path::Delete(arg0);
						break;
					case IDENT_COPY:
					{
						auto co = uint32_t(std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing);
						if(argc > 2)
							co = eval_arg(2).to_number();
						
						std::error_code err;
						std::filesystem::copy(arg0.c_str(), eval_arg(1).to_string().c_str(),
											  (std::filesystem::copy_options)co, err);
						_result = err.value() == 0;
						//_result = ::CopyFileW(arg0.c_str(), eval_arg(1).to_string().c_str(), true);
						break;
					}
					case IDENT_MOVE:
						_result = ::MoveFileW(arg0.c_str(), eval_arg(1).to_string().c_str());
						break;
					case IDENT_RENAME:
						_result = ::_wrename(arg0.c_str(), eval_arg(1).to_string().c_str()) == 0;
						break;
					case IDENT_FILE:
					{
						switch(Id[2])
						{
							case IDENT_SIZE:
								_result = Path::FileSize(arg0.c_str());
								break;
							case IDENT_EXISTS:
							{
								auto exists = (uint32_t)Path::IsFileExists(arg0);
								if(exists == 0)
								{
									for(size_t i = 1; i < argc; i++)
										exists += (uint32_t)Path::IsFileExists(eval_arg(i).to_string());
								}
								_result = exists != 0;
								break;
							}
							case IDENT_MAKE:
							case IDENT_CREATE:
							case IDENT_WRITE:
							case IDENT_APPEND:
							{
								try
								{
									if(arg0.empty())
									{
										_result = 0;
										return;
									}

									auto_handle hFile = ::CreateFileW(arg0, GENERIC_WRITE,
																	  FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, Id.back() == IDENT_APPEND ? OPEN_ALWAYS : CREATE_ALWAYS,
																	  FILE_ATTRIBUTE_NORMAL, nullptr);
									if(hFile)
									{
										string arg1 = eval_arg(1).to_string().move();
										if(arg1.empty())
											_result = 0;
										else
										{
											if(Id.back() == IDENT_APPEND)
												::SetFilePointer(hFile, 0, nullptr, FILE_END);

											std::string utf8 = std::move(UTF8::Utf16ToUtf8(arg1.c_str(), arg1.length()));
											DWORD lpNumberOfBytesWritten = 0;
											//auto nNumberOfBytesToWrite = DWORD(arg1.length() * sizeof(arg1[0]));
											if(::WriteFile(hFile, utf8.c_str(), (DWORD)utf8.size(), &lpNumberOfBytesWritten, nullptr))
												_result = lpNumberOfBytesWritten;
										}
										
										bool rename = eval_arg(2);
										auto sel = context->Selections;
										if(rename && sel && sel->ShellBrowser)
										{
											IComPtr<IShellView2> sv;
											if(S_OK == sel->ShellBrowser->QueryActiveShellView(sv))
											{
												IComPtr<IFolderView> fv;
												if(S_OK == sv->QueryInterface(IID_IFolderView, fv))
												{
													IComPtr<IPersistFolder2> ppf2;
													if(S_OK == fv->GetFolder(IID_IPersistFolder2, ppf2))
													{
														LPITEMIDLIST pidlFolder = nullptr;
														if(S_OK == ppf2->GetCurFolder(&pidlFolder))
														{
															if(auto pidlCreatedItem = GeneratePidlFromName(pidlFolder, arg0.data()); pidlCreatedItem)
															{
																auto dwFlagsSelFlags = SVSI_SELECT | SVSI_POSITIONITEM | SVSI_EDIT;
																sv->SelectAndPositionItem(::ILFindLastID(pidlCreatedItem), dwFlagsSelFlags, nullptr);
																::ILFree(pidlCreatedItem);
															}
															::ILFree(pidlFolder);
														}
													}
												}
											}
											return;
										}
										::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, arg0.c_str(), nullptr);
										return;
									}
								}
								catch(...)
								{
								}
								_result = 0;
								break;
							}
							case IDENT_READ:
							{
								_result = File::ReadText(arg0, argc == 1 ? uint32_t(-1) : eval_arg(1).to_number<uint32_t>());
								break;
							}
							case IDENT_ZERO:
								break;
						}
						break;
					}
					case IDENT_DIR:
					case IDENT_DIRECTORY:
					{
						switch(Id[2])
						{
							case IDENT_MAKE:
							case IDENT_CREATE:
							{
								int32_t creates = 0;
								try
								{
									creates = ::CreateDirectoryW(arg0.c_str(), nullptr);
									for(size_t i = 1; i < argc; i++)
										creates += ::CreateDirectoryW(eval_arg(i).to_string(), nullptr);

									auto sel = context->Selections;
									if(creates == 1 && sel && sel->ShellBrowser)
									{
										IComPtr<IShellView2> sv;
										if(S_OK == sel->ShellBrowser->QueryActiveShellView(sv))
										{
											IComPtr<IFolderView> fv;
											if(S_OK == sv->QueryInterface(IID_IFolderView, fv))
											{

												IComPtr<IPersistFolder2> ppf2;
												if(S_OK == fv->GetFolder(IID_IPersistFolder2, ppf2))
												{
													LPITEMIDLIST pidlFolder = nullptr;
													if(S_OK == ppf2->GetCurFolder(&pidlFolder))
													{
														if(auto pidlCreatedItem = GeneratePidlFromName(pidlFolder, arg0.data()); pidlCreatedItem)
														{
															DWORD dwFlagsSelFlags = SVSI_SELECT | SVSI_POSITIONITEM | SVSI_EDIT;
															sv->SelectAndPositionItem(::ILFindLastID(pidlCreatedItem), dwFlagsSelFlags, nullptr);
															::ILFree(pidlCreatedItem);
														}
														::ILFree(pidlFolder);
													}
												}
											}
										}
									}

									if(creates == 1)
										::SHChangeNotify(SHCNE_MKDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, arg0.c_str(), nullptr);
									else if(creates > 1)
										::SHChangeNotify(SHCNE_MKDIR, SHCNF_FLUSHNOWAIT, Path::Parent(arg0), nullptr);
								}
								catch(...)
								{
									//_result = false;
								}
								_result = creates != 0;
								return;
								break;
							}
							case IDENT_EMPTY:
							{
								auto empties = (uint32_t)Path::IsDirectoryEmpty(arg0);
								for(size_t i = 1; i < argc; i++)
									empties += (uint32_t)Path::IsDirectoryEmpty(eval_arg(i).to_string());
								_result = empties != 0;
								break;
							}
							case IDENT_EXISTS:
							{
								auto exists = (uint32_t)Path::IsDirectoryExists(arg0);
								if(exists == 0)
								{
									for(size_t i = 1; i < argc; i++)
										exists += (uint32_t)Path::IsDirectoryExists(eval_arg(i).to_string());
								}
								_result = exists != 0;
								break;
							}
							case IDENT_ZERO:
								break;
						}
						break;
					}
					case IDENT_ATTRIBUTES:
					{
						if(argc == 1)
							_result = (int32_t)::GetFileAttributesW(arg0.c_str());
						else
							_result = ::SetFileAttributesW(arg0.c_str(), eval_arg(1).to_number<DWORD>());
						break;
					}
					case IDENT_IO_ATTRIB:
					case IDENT_ATTRIBUTE:
					{
						auto attribute = [=](int32_t a)->int32_t
						{
							if(argc == 1)
							{
								int32_t attr = 0;
								if(obj0.is_number())
									attr = obj0.to_number<int32_t>();
								else
									attr = (int32_t)::GetFileAttributesW(arg0.c_str());

								return(attr != INVALID_FILE_ATTRIBUTES) ? (attr & a) == a : FALSE;
							}
							return a;
						};

						switch(Id[2])
						{
							case IDENT_IO_ATTRIBUTE_INVALID:
								_result = attribute(INVALID_FILE_ATTRIBUTES);
								break;
							case IDENT_IO_ATTRIBUTE_NORMAL:
								_result = attribute(FILE_ATTRIBUTE_NORMAL);
								break;
							case IDENT_IO_ATTRIBUTE_ARCHIVE:
								_result = attribute(FILE_ATTRIBUTE_ARCHIVE);
								break;
							case IDENT_IO_ATTRIBUTE_COMPRESSED:
								_result = attribute(FILE_ATTRIBUTE_COMPRESSED);
								break;
							case IDENT_IO_ATTRIBUTE_DEVICE:
								_result = attribute(FILE_ATTRIBUTE_DEVICE);
								break;
							case IDENT_IO_ATTRIBUTE_DIRECTORY:
								_result = attribute(FILE_ATTRIBUTE_DIRECTORY);
								break;
							case IDENT_IO_ATTRIBUTE_ENCRYPTED:
								_result = attribute(FILE_ATTRIBUTE_ENCRYPTED);
								break;
							case IDENT_IO_ATTRIBUTE_HIDDEN:
								_result = attribute(FILE_ATTRIBUTE_HIDDEN);
								break;
							case IDENT_IO_ATTRIBUTE_READONLY:
								_result = attribute(FILE_ATTRIBUTE_READONLY);
								break;
							case IDENT_IO_ATTRIBUTE_SPARSEFILE:
								_result = attribute(FILE_ATTRIBUTE_SPARSE_FILE);
								break;
							case IDENT_IO_ATTRIBUTE_SYSTEM:
								_result = attribute(FILE_ATTRIBUTE_SYSTEM);
								break;
							case IDENT_IO_ATTRIBUTE_TEMPORARY:
								_result = attribute(FILE_ATTRIBUTE_TEMPORARY);
								break;
							case IDENT_IO_ATTRIBUTE_VIRTUAL:
								_result = attribute(FILE_ATTRIBUTE_VIRTUAL);
								break;
							case IDENT_IO_ATTRIBUTE_OFFLINE:
								_result = attribute(FILE_ATTRIBUTE_OFFLINE);
								break;
						}
						break;
					}
					case IDENT_IO_DT:
					case IDENT_IO_DATETIME:
					{
						auto_handle hFile;

					/*	if(argc > 1)
						{
							hFile = ::CreateFileW(arg0.c_str(), FILE_WRITE_ATTRIBUTES,
													   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
													   nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
						}
						else */
						{
							hFile = ::CreateFileW(arg0.c_str(), FILE_READ_ATTRIBUTES,
												  FILE_SHARE_READ | FILE_SHARE_DELETE,
												  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
						}

						if(!hFile) break;
						bool ret = false;
						bool set = false;
						SYSTEMTIME st{};
						string fmt;
						auto c = argc - 1;

						if(c)
						{
							if(c >= 1)
							{
								Object o = eval_arg(1).move();
								if(o.is_string() && c == 1)
									fmt = o.to_string().move();
								else
								{
									set = true;
									st.wYear = o.to_number<uint16_t>();
									if(c >= 2)
										st.wMonth = eval_arg(2).to_number<uint16_t>();
									if(c >= 3)
										st.wDay = eval_arg(3).to_number<uint16_t>();
									if(c >= 4)
										st.wHour = eval_arg(4).to_number<uint16_t>();
									if(c >= 5)
										st.wMinute = eval_arg(5).to_number<uint16_t>();
									if(c == 6)
										st.wSecond = eval_arg(6).to_number<uint16_t>();
								}
							}
						}
						else 
						{
							fmt = L"y-m-d H.M.S";
						}
						
						switch(Id[2])
						{
							case IDENT_IO_DATETIME_CREATED:
								ret = IO::DateTime(set, hFile, &st, nullptr, nullptr);
								break;
							case IDENT_IO_DATETIME_MODIFIED:
								ret = IO::DateTime(set, hFile, nullptr, nullptr, &st);
								break;
							case IDENT_IO_DATETIME_ACCESSED:
								ret = IO::DateTime(set, hFile, nullptr, &st, nullptr);
								break;
							case IDENT_ZERO:
								break;
						}

						if(!set && ret)
							_result = string::TimeFormat(&st, fmt).move();
						break;
					}
					case IDENT_META:
					{
						string path = eval_arg(0).to_string().trim().move();
						
						if(path.empty())
							break;

						string name = eval_arg(1).to_string().trim().move();
						string sys = L"System.";

						if(name.empty())
							name = sys + L"Title";
						else if(!name.starts_with(sys))
							name = sys + name;

						PROPERTYKEY key{};
						auto hr = ::PSGetPropertyKeyFromName(name, &key);
						if(SUCCEEDED(hr))
						{
							IComPtr<IPropertyStore> ps;
							// Call the helper to get the property store for the initialized item
							hr = ::SHGetPropertyStoreFromParsingName(path, nullptr, GPS_DEFAULT, __uuidof(IPropertyStore), ps);
							if(SUCCEEDED(hr))
							{
								PROPVARIANT propvarValue = { 0 };
								hr = ps->GetValue(key, &propvarValue);
								if(SUCCEEDED(hr))
								{
									wchar_t* pszDisplayValue = nullptr;
									hr = ::PSFormatForDisplayAlloc(key, propvarValue, PDFF_DEFAULT, &pszDisplayValue);
									if(SUCCEEDED(hr))
									{
										_result = pszDisplayValue;
										::CoTaskMemFree(pszDisplayValue);
									}
									::PropVariantClear(&propvarValue);
								}
							}
						}
						break;
					}
				}
			}
			catch(...)
			{
			}
		}

#pragma comment(lib, "propsys.lib")

		void FuncExpression::eval_user()
		{
			auto trim = L"\\/";

			if(Id.length() == 1)
				_result = Windows::UserName().move();
			else switch(Id[1])
			{
				case IDENT_NAME:
					_result = Windows::UserName().move();
					break;
				case IDENT_HOME:
				case IDENT_DIR:
				case IDENT_DIRECTORY:
				case IDENT_PROFILE:
					_result = Path::GetKnownFolder(FOLDERID_Profile).trim_end(trim).move();
					break;
				case IDENT_APPDATA:
					_result = Path::GetKnownFolder(FOLDERID_RoamingAppData).trim_end(trim).move();
					break;
				case IDENT_LOCALAPPDATA:
					_result = Path::GetKnownFolder(FOLDERID_LocalAppData).trim_end(trim).move();
					break;
				case IDENT_TEMP:
					_result = Path::TempDirectory().trim_end(trim).move();
					break;
				case IDENT_DESKTOP:
					_result = Path::GetKnownFolder(FOLDERID_Desktop).trim_end(trim).move();
					break;
				case IDENT_PERSONAL:
				case IDENT_DOCUMENTS:
					_result = Path::GetKnownFolder(FOLDERID_Documents).trim_end(trim).move();
					break;
				case IDENT_SENDTO:
					_result = Path::GetKnownFolder(FOLDERID_SendTo).trim_end(trim).move();
					break;
				case IDENT_STARTMENU:
					_result = Path::GetKnownFolder(FOLDERID_StartMenu).trim_end(trim).move();
					break;
				case IDENT_FAVORITES:
					_result = Path::GetKnownFolder(FOLDERID_Favorites).trim_end(trim).move();
					break;
				case IDENT_PICTURES:
					_result = Path::GetKnownFolder(FOLDERID_Pictures).trim_end(trim).move();
					break;
				case IDENT_VIDEOS:
					_result = Path::GetKnownFolder(FOLDERID_Videos).trim_end(trim).move();
					break;
				case IDENT_MUSIC:
					_result = Path::GetKnownFolder(FOLDERID_Music).trim_end(trim).move();
					break;
				case IDENT_TEMPLATES:
					_result = Path::GetKnownFolder(FOLDERID_Templates).trim_end(trim).move();
					break;
				case IDENT_CONTACTS:
					_result = Path::GetKnownFolder(FOLDERID_Contacts).trim_end(trim).move();
					break;
				case IDENT_LIBRARIES:
					_result = Path::GetKnownFolder(FOLDERID_Libraries).trim_end(trim).move();
					break;
				case IDENT_DOCUMENTSLIBRARY:
					_result = Path::GetKnownFolder(FOLDERID_DocumentsLibrary).trim_end(trim).move();
					break;
				case IDENT_QUICKLAUNCH:
					_result = Path::GetKnownFolder(FOLDERID_QuickLaunch).trim_end(trim).move();
					break;
				case IDENT_DOWNLOADS:
					_result = Path::GetKnownFolder(FOLDERID_Downloads).trim_end(trim).move();
					break;
				case IDENT_EXPAND:
					_result = Environment::Expand(eval_arg(0).to_string(), true).move();
					break;
			}
		}

		void FuncExpression::eval_sys()
		{
			auto trim = L"\\/";
			auto ver = &Windows::Version::Instance();
			if(Id.length(1))
			{
				//IsWindowsVersionOrGreater
				_result = Path::GetKnownFolder(FOLDERID_Windows).move();
			}
			else switch(Id[1])
			{
				//sys.Is7OrGreater
				//sys.Is8OrGreater
				//sys.Is81OrGreater
				//sys.Is10OrGreater
				//sys.Is11OrGreater
				//sys.Is7OrEarlier
				//sys.Is8OrEarlier
				//sys.Is81OrEarlier
				//sys.Is10OrEarlier
				//sys.Is11OrEarlier
				case IDENT_TYPE:
				{
					_result = (ver->Type == 1) ? 64 : 32;
					break;
				}
				case IDENT_IS64:
					_result = ver->Type;
					break;
				case IDENT_IS11:
					_result = ver->IsWindows11OrGreater();
					break;
				case IDENT_IS10:
					_result = ver->Major == 10 && ver->Build < 22000;
					break;
				case IDENT_IS81:
					_result = ver->Major == 6 && ver->Minor == 3;
					break;
				case IDENT_IS8:
					_result = ver->Major == 6 && ver->Minor == 2;
					break;
				case IDENT_IS7:
					_result = ver->Major == 6 && ver->Minor == 1;
					break;
				case IDENT_ISOREARLIER:
				{
					auto major = eval_arg(0).to_number<uint32_t>();
					_result = false;
					if(ver->Major <= major)
					{
						auto minor = eval_arg(1).to_number<uint32_t>();
						if(ver->Minor <= minor)
						{
							_result = true;
						}
					}
					break;
				}
				case IDENT_ISORGREATER:
				{
					auto major = eval_arg(0).to_number<uint32_t>();
					_result = false;
					if(ver->Major >= major)
					{
						auto minor = eval_arg(1).to_number<uint32_t>();
						if(ver->Minor >= minor)
						{
							_result = true;
						}
					}
					break;
				}
				case IDENT_IS7ORGREATER:
					_result = ver->Major >= 6 && ver->Minor >= 1;
					break;
				case IDENT_IS8ORGREATER:
					_result = ver->Major >= 6 && ver->Minor >= 2;
					break;
				case IDENT_IS81ORGREATER:
					_result = ver->Major >= 6 && ver->Minor >= 3;
					break;
				case IDENT_IS10ORGREATER:
					_result = ver->Major >= 10;
					break;
				case IDENT_S11ORGREATER:
					_result = ver->Major >= 10 && ver->Build >= 22000;
					break;
				case IDENT_IS7OREARLIER:
					_result = ver->Major <= 6 && ver->Minor <= 1;
					break;
				case IDENT_IS8OREARLIER:
					_result = ver->Major <= 6 && ver->Minor <= 2;
					break;
				case IDENT_IS81OREARLIER:
					_result = ver->Major <= 6 && ver->Minor <= 3;
					break;
				case IDENT_IS10OREARLIER:
					_result = ver->Major <= 10 && ver->Build < 22000;
					break;
				case IDENT_IS11OREARLIER:
					_result = ver->Major <= 10 && ver->Build <= 22000;
					break;
				case IDENT_IS_PRIMARY_MONITOR:
					_result = context->helper.is_primary_monitor;
					break;
				case IDENT_VER:
				case IDENT_VERSION:
				{
					if(Id.length(3))
					{
						switch(Id[2])
						{
							case IDENT_MAJOR:
								_result = ver->Major;
								break;
							case IDENT_MINOR:
								_result = ver->Minor;
								break;
							case IDENT_BUILD:
								_result = ver->Build;
								break;
							case IDENT_TYPE:
								_result = ver->Type;
								break;
							case IDENT_NAME:
								_result = ver->Name;
								break;
						}
						return;
					}

					if(Id[1] == IDENT_VER)
						_result = ver->BuildStr;
					else
						_result = ver->BuildNumber;

					break;
				}
				case IDENT_DARK:
					_result = context->theme->system.mode;
					break;
				case IDENT_NAME:
					_result = Windows::ComputerName().move();
					break;
				case IDENT_ROOT:
					_result = Path::GetKnownFolder(FOLDERID_Windows).substr(0, 2).move();
					break;
				case IDENT_PATH:
				case IDENT_DIR:
				case IDENT_DIRECTORY:
					_result = Path::GetKnownFolder(FOLDERID_Windows).trim_end(trim).move();
					break;
					// sys.system, sys.bin, sys.sysdir
				case IDENT_BIN:
				case IDENT_BIN64:
					_result = Path::GetKnownFolder(FOLDERID_System).trim_end(trim).move();
					break;
				case IDENT_BIN32:
				case IDENT_WOW:
					_result = Path::GetKnownFolder(FOLDERID_SystemX86).trim_end(trim).move();
					break;
				case IDENT_PROG:
					_result = Path::GetKnownFolder(FOLDERID_ProgramFiles).trim_end(trim).move();
					break;
				case IDENT_PROG32:
					_result = Path::GetKnownFolder(FOLDERID_ProgramFilesX86).trim_end(trim).move();
					break;
				case IDENT_TEMP:
					_result = Path::TempDirectory().trim_end(trim).move();
					break;
				case IDENT_EXTENDED:
					_result = (::GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
					break;
				case IDENT_VAR:
					_result = Environment::Variable(eval_arg(0).to_string()).move();
					break;
				case IDENT_EXPAND:
					_result = Environment::Expand(eval_arg(0).to_string()).move();
					break;
				case IDENT_APPDATA:
					_result = Path::GetKnownFolder(FOLDERID_RoamingAppData).trim_end(trim).move();
					break;
				case IDENT_PROGRAMDATA:
					_result = Path::GetKnownFolder(FOLDERID_ProgramData).trim_end(trim).move();
					break;
				case IDENT_USERS:
					_result = Path::GetKnownFolder(FOLDERID_Windows).substr(0, 3).append(L"users").trim_end(trim).move();
					break;
				case IDENT_TEMPLATES:
					_result = Path::GetKnownFolder(FOLDERID_CommonTemplates).trim_end(trim).move();
					break;
				case IDENT_LANG:
				case IDENT_LOC:
				{
					wchar_t szISOLang[51] = { };
					if(Id.length() == 2)
					{
						auto len = ::GetUserDefaultLocaleName(szISOLang, 50);
						if(len > 0)
							_result = szISOLang;
					}
					else if(Id.length() == 3)
					{
						auto locid = ::GetThreadUILanguage();

						switch(Id[2])
						{
							case IDENT_ID:
								_result = locid; //context->helper.languageId;
								break;
							case IDENT_NAME:
							{
								
								auto ret = ::GetLocaleInfoW(locid,
															LOCALE_SISO639LANGNAME,
															szISOLang,
															sizeof(szISOLang) / sizeof(wchar_t));
								if(ret)
									_result = szISOLang;
								break;
							}
							case IDENT_COUNTRY:
							{
								auto ret = ::GetLocaleInfoW(locid,
													   LOCALE_SISO3166CTRYNAME,
															szISOLang,
													   sizeof(szISOLang) / sizeof(wchar_t));
								if(ret)
									_result = szISOLang;
								break; 
							}
						}
					}
					
					/*wchar_t szISOLang[10] = { 0 };
					wchar_t szISOCountry[10] = { 0 };
					auto lid = ::GetThreadUILanguage();

					auto ret = ::GetLocaleInfoW(lid,
									LOCALE_SISO639LANGNAME,
									szISOLang,
									sizeof(szISOLang) / sizeof(wchar_t));
					if(ret)
					{
						loc = szISOLang;
						ret = ::GetLocaleInfoW(lid,
										 LOCALE_SISO3166CTRYNAME,
										 szISOCountry,
										 sizeof(szISOCountry) / sizeof(wchar_t));
						if(ret)
						{
							loc += L"-";
							loc += szISOCountry;
						}
					}
					*/
					
					break;
				}
				case IDENT_DATETIME:
				{
					SYSTEMTIME lt = { };
					::GetLocalTime(&lt);

					if(Id.length() == 3)
					{
						switch(Id[2])
						{
							case IDENT_TIME_PM:
								_result = (lt.wHour >= 12 ? L"PM" : L"AM");
								break;
							case IDENT_DATE_YY:
								_result = lt.wYear % 100;
								break;
							case IDENT_DATE_Y:
							case IDENT_DATE_YEAR:
								_result = lt.wYear;
								break;
							case IDENT_DATE_M:
							case IDENT_DATE_MONTH:
								_result = lt.wMonth;
								break;
							case IDENT_DATE_DW:
							case IDENT_DATE_DAYOFWEEK:
								_result = lt.wDayOfWeek + 1;
								break;
							case IDENT_DATE_D:
							case IDENT_DATE_DAY:
								_result = lt.wDay;
								break;
							case IDENT_TIME_H:
							case IDENT_TIME_HOUR:
								_result = lt.wHour;
								break;
							case IDENT_TIME_MIN:
							case IDENT_TIME_MINUTE:
								_result = lt.wMinute;
								break;
							case IDENT_TIME_S:
							case IDENT_TIME_SECOND:
								_result = lt.wSecond;
								break;
							case IDENT_TIME_MS:
							case IDENT_TIME_MILLISECONDS:
								_result = lt.wMilliseconds;
								break;
							case IDENT_DATE:
								_result = string::TimeFormat(&lt, L"y.m.d").move();
								break;
							case IDENT_TIME:
								_result = string::TimeFormat(&lt, L"H:M:S").move();
								break;
							case IDENT_SHORT:
								_result = string::TimeFormat(&lt, L"y.m.d-H:M:S").move();
								break;
						}
					}
					else if(Arguments.size() == 0)
					{
						_result = string::TimeFormat(&lt, L"y.m.d-H:M:S").move();
					}
					else
					{
						Object exp = eval_arg(0).move();
						if(!exp.is_null())
							_result = string::TimeFormat(&lt, exp.to_string().c_str()).move();
					}
					break;
				}
			}
		}

		void FuncExpression::eval_app()
		{
			auto app = &Initializer::instance->application;
			switch(Id[1])
			{
				case IDENT_VER:
				case IDENT_VERSION:
					_result = app->Version;
					switch(Id[2])
					{
						case IDENT_MAJOR:
							_result = VERSION_MAJOR;
							break;
						case IDENT_MINOR:
							_result = VERSION_MINOR;
							break;
						case IDENT_BUILD:
							_result = VERSION_BUILD;
							break;
						case IDENT_ZERO:
							_result = app->Version;
							break;
					}
					break;
				case IDENT_ROOT:
					_result = app->Root;
					break;
				case IDENT_DIR:
				case IDENT_DIRECTORY:
					_result = app->Dirctory;
					break;
				case IDENT_NAME:
					_result = app->Name;
					break;
				case IDENT_CFG:
					_result = app->Config;
					break;
				case IDENT_DLL:
					_result = app->Path;
					break;
				case IDENT_EXE:
					_result = app->Manager;
					break;
				case IDENT_IS64:
					_result = Windows::Version::Is64BitProcess();
					break;
				case IDENT_RELOAD:
					Initializer::Status.Refresh = true;
					break;
				case IDENT_UNLOAD:
					Initializer::Status.Disabled = true;
					break;
				case IDENT_ABOUT:
				{
					string about;
					about.format(L"%s\nversion %s\n%s\n© %d %s.",
								 APP_COMPANY L" " APP_NAME,
								 APP_VERSION,
								 APP_WEBSITE,
								 VERSION_YEAR, APP_COMPANYLTD);
					::MessageBoxW(nullptr, about, L"About", MB_OK | MB_ICONINFORMATION);
					break;
				}
				case IDENT_USED:
				case IDENT_PROCESS:
				{
					string proc;
					for(auto &process : Diagnostics::Process::EnumInfo())
					{
						auto hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, process.th32ProcessID);
						if(hProcess)
						{
							for(auto &module : Diagnostics::Process::Modules(hProcess))
							{
								if(Diagnostics::Process::ModuleFileName(hProcess, module).equals(context->Application->Path))
								{
									proc.append_format(L"%d %s\n", process.th32ProcessID, Diagnostics::Process::ModuleFileName(hProcess).c_str());
									break;
								}
							}
							::CloseHandle(hProcess);
						}
					}
					_result = proc.move();
					break;
				}
			}
		}

		void FuncExpression::eval_str()
		{
			auto argc = Arguments.size();
			auto id = extented ? Id[0] : Id[1];

			Object arg0;
			string str;
			
			size_t a = 0;

			if(ischild && Parent)
			{
				id = Id[0];
				arg0 = ((FuncExpression *)Parent)->_result.move();
				str = arg0.to_string();
				a = 1;
			}
			else if(argc > 0)
			{
				arg0 = eval_arg(0).move();

				if(id != IDENT_RES && id != IDENT_GUID)
					str = arg0.to_string().move();
			}
			
			bool eval_cheld = true;

			switch(id)
			{
				case IDENT_LEN:
				case IDENT_LENGTH:
					_result = (int32_t)str.length();
					eval_cheld = false;
					break;
				case IDENT_NULL:
				case IDENT_EMPTY:
					_result = str.empty();
					eval_cheld = false;
					break;
				case IDENT_UPPER:
					_result = str.toupper().move();
					break;
				case IDENT_LOWER:
					_result = str.tolower().move();
					break;
				case IDENT_CAPITALIZE:
					_result = str.capitalize().move();
					
					break;
				case IDENT_HASH:
				{
					//string fmt; 
					//fmt.format(L"%0.8X", MenuItemInfo::hashing(str));
					//_result = fmt.move();
					_result = MenuItemInfo::hashing(str);
					break;
				}
				case IDENT_TRIM:
					if(argc > 1 - a)
						_result = str.trim(eval_arg(1).to_string().c_str()).move();
					else
						_result = str.trim().move();
					break;
				case IDENT_TRIMSTART:
					if(argc > 1 - a)
						_result = str.trim_start(eval_arg(1).to_string().c_str()).move();
					else
						_result = str.trim_start().move();
					break;
				case IDENT_TRIMEND:
					if(argc == 2 - a)
						_result = str.trim_end(eval_arg(1).to_string().c_str()).move();
					else
						_result = str.trim_end().move();
					break;
				case IDENT_SET:
				case IDENT_CHAR:
					//if(params == 2)
				{
					str[eval_arg(1)] = eval_arg(2);
					_result = str.move();
					break;
				}
				case IDENT_GET:
				case IDENT_AT:
					//if(params == 2)
				{
					_result = str.at(eval_arg(1));
					break;
				}
				case IDENT_SUB:
				{
					int32_t start = eval_arg(1);
					if(argc == 2 - a)
						_result = str.substr(start).move();
					else
						_result = str.substr(start, eval_arg(2).to_number<int32_t>()).move();
					break;
				}
				case IDENT_LEFT:
					//if(params == 2)
				{
					_result = str.left(eval_arg(1)).move();
					break;
				}
				case IDENT_RIGHT:
					//if(params == 2)
				{
					_result = str.right(eval_arg(1)).move();
					break;
				}
				case IDENT_PADDING:
				case IDENT_PADLEFT:
				case IDENT_PADRIGHT:
				{
					int32_t count = eval_arg(1);
					if(count <= 0)
						_result = str.move();
					else
					{
						auto paddingChar = argc == (2 - a) ? L' ' : (wchar_t)eval_arg(2);
						uint32_t _id = Id[1];

						if(_id == IDENT_PADDING)
							_result = str.pad_left(count, paddingChar).pad_right(count, paddingChar).move();
						else if(_id == IDENT_PADLEFT)
							_result = str.pad_left(count, paddingChar).move();
						else
							_result = str.pad_right(count, paddingChar).move();
					}
					break;
				}
				case IDENT_NOT:
					eval_cheld = false;
					_result = !str.equals(eval_arg(1).to_string(), eval_arg(2, true));
					break;
				case IDENT_EQ:
				case IDENT_EQUALS:
				{
					_result = false;
					eval_cheld = false;
					auto f = (bool)eval_arg(2, 1);
					Object o = eval_arg(1).move();
					if(!o.is_array(true))
						_result = str.equals(o.to_string(), f);
					else
					{
						auto ptr = o.get_pointer();
						auto count = (uint32_t)ptr[0];
						for(auto i = 0u; i < count; i++)
						{
							auto p = &ptr[i + 1];
							if(str.equals(p->to_string(), f))
							{
								_result = true;
								break;
							}
						}
					}
					break;
				}
				case IDENT_START:
					eval_cheld = false;
					_result = str.starts_with(eval_arg(1).to_string(), eval_arg(2, true));
					break;
				case IDENT_END:
					eval_cheld = false;
					_result = str.ends_with(eval_arg(1).to_string(), eval_arg(2, true));
					break;
				case IDENT_FIND:
					eval_cheld = false;
					_result = (int32_t)str.index_of(eval_arg(1).to_string().c_str(), 0, eval_arg(2, true));
					break;
				case IDENT_FINDLAST:
					eval_cheld = false;
					_result = (int32_t)str.last_index_of(eval_arg(1).to_string().c_str(), eval_arg(2, true));
					break;
				case IDENT_CONTAINS:
					eval_cheld = false;
					_result = str.contains(eval_arg(1).to_string().c_str(), eval_arg(2, true));
					break;
				case IDENT_REPLACE:
				{
					bool ignoreCase = argc > 3 ? eval_arg(3).to_bool() : false;
					_result = str.replace(eval_arg(1).to_string(), eval_arg(2).to_string(), ignoreCase).move();
					break;
				}
				case IDENT_REMOVE:
				{
					Object exp = eval_arg(1).move();
					if(exp.is_number())
					{
						size_t from = exp;
						size_t to = str.length() - 1;
						if(argc == (3 - a))
							to = eval_arg(2).move();
						_result = str.remove(from, to).move();
					}
					else
					{
						string pat = exp.to_string().move();
						if(pat.length() == 1)
							str.remove(pat.at(0));
						else
							str.remove_chars(pat.c_str());
						_result = str.move();
					}
					break;
				}
				case IDENT_RES:
				{
					auto shell32 = L"shell32.dll";
					Object resid;

					if(argc == 1)
					{
						if(arg0.is_number())
						{
							str = shell32;
							resid = eval_arg(0).move();
						}
						else
						{
							str = arg0.to_string().move();
							if(str.starts_with(L"@{", false) &&
							   str.back(L'}', false))
							{
								string displayName(1024);
								if(S_OK == ::SHLoadIndirectString(str, displayName.data(), displayName.capacity<uint32_t>(),nullptr))
								{
									_result = displayName.release().move();
									break;
								}
							}
							else 
							{
								resid = string::ParseMUILocation(str);
							}
						}
					}
					else
					{
						str = arg0.to_string().move();
						resid = eval_arg(1);
					}

					if(resid.is_number() && !str.empty())
					{
						_result = string::Extract(str.c_str(), std::abs(resid.to_number<int>())).move();
					}
					break;
				}
				case IDENT_GUID:
				{
					//0=32 digits:
					//1=32 digits separated by hyphens:
					//2=32 digits separated by hyphens, enclosed in braces:
					//3=32 digits separated by hyphens, enclosed in parentheses:
					GUID guid{};
					int style = 0;
					if(S_OK == ::CoCreateGuid(&guid) && arg0.is_number())
						style = arg0.to_number<int>();
					_result = string::ToString(guid, style).move();
					break;
				}
				case IDENT_JOIN:
				{
					string sep = eval_arg(argc - 1).to_string().move();
					string r;
					for(auto i = 0u; i < argc - 1; i++)
					{
						r += eval_arg(i).too(sep).move();
						if(i != argc - 2)
							r += sep;
					}
					_result = r.move();
					break;
				}
				case IDENT_SPLIT:
				{
					std::vector<string> list;
					str.split(list, eval_arg(1).to_string()[0]);
					if(!list.empty())
						_result.from(list);
					break;
				}
				case IDENT_TAG:
				{
					string tag = eval_arg(1).to_string();
					_result = (tag + str + ((argc == 3) ? eval_arg(2).to_string() : tag)).move();
					break;
				}
				case IDENT_FORMAT:
					break;
				case IDENT_DECODE:
				{
					if(Id[2] == IDENT_URL)
						_result = string::url_decode(str).move();
					break;
				}
			}

		//	if(xChild)
		//	_result = context->Eval(xChild).move();
		}

		void FuncExpression::eval_for()
		{
			Expression *expr = nullptr;
			auto ident = (IdentExpression *)Arguments[0];
			uint32_t id = ident->Id;

			if(ident->IsAssign())
			{
				//context->Eval(ident);
				expr = ident->assign()->Right;
			}
			else
			{
				expr = context->get_variable(id, this);
			}

			if(expr)
			{
				Object ofrom = context->Eval(expr).move();
				Object oto = eval_arg(1).move();
				if(ofrom.is_number() && oto.is_number())
				{
					string result;
					int32_t to = oto, from = ofrom;

					auto run = [&](int32_t i)
					{
						this->Scope.set(id, i);
						//if(i >= std::numeric_limits<uint16_t>::max())
						if(i >= USHRT_MAX)
						{
							Logger::Warning(L"overflow loop %d", i);
							return 0;
						}

						Object o = eval_arg(2).move();

						if(context->Break)
						{
							context->Break = false;
							return 0;
						}

						if(context->Continue)
						{
							context->Continue = false;
							return 2;
						}

						result.append(o.to_string().move());
						return 1;
					};

					if(from > to)
					{
						for(int32_t i = from; i > to; i--)
						{
							auto r = run(i);
							if(r == 0)
								break;
							else if(r == 2)
								continue;
						}
					}
					else
					{
						for(int32_t i = from; i < to; i++)
						{
							auto r = run(i);
							if(r == 0)
								break;
							else if(r == 2)
								continue;
						}
					}
					_result = result.move();
					this->Scope.set(id, from);
				}
			}
		}

		void FuncExpression::eval_foreach()
		{
			uint32_t id = Arguments[0]->ident()->Id;
			auto &id_array = Arguments[1]->ident()->Id;

			if(id && id_array)
			{
				string result;
				uint32_t i = 0;
				for(auto item : context->Selections->Items)
				{
					if(i++ >= USHRT_MAX)
					{
						Logger::Warning(L"overflow loop %d", i);
						break;
					}

					string it;
					switch(id_array.back())
					{
						case IDENT_PATHS: it = item->Path; break;
						case IDENT_SHORTS: it = Path::Short(item->Path).move(); break;
						case IDENT_NAMES: it = item->Name; break;
						case IDENT_TITLES:
						{
							if(item->IsFile())
								it = Path::RemoveExtension(item->Name).move();
							else
								it = item->Name;
							break;
						}
						case IDENT_FILES:
						{
							if(!item->IsFile()) continue;
							it = item->Path;
							break;
						}
						case IDENT_EXTS: 
						{
							if(!item->IsFile()) continue;
							it = item->Extension;
							break;
						}
						case IDENT_DIRS: 
						case IDENT_DIRECTORIES:
							if(!item->IsDirectory()) continue;
							it = item->Path; 
							break;
						case IDENT_DRIVERS:
						case IDENT_ROOTS:
							if(!item->IsRoot()) continue;
							it = item->Path;
							break;
						default: continue;
					}

					Object o = eval_arg(2).move();

					if(context->Break)
					{
						context->Break = false;
						break;
					}

					if(context->Continue)
					{
						context->Continue = false;
					}

					result.append(o.to_string().move());
					this->Scope.set(id, it);
				}
				_result = result.move();
			}
		}

		void FuncExpression::eval_msg()
		{
			if(Id.length() == 2)
			{
				if(Id[1] == IDENT_BEEP)
				{
					if(Arguments.size() == 1)
					{
						Object exp = eval_arg(0).move();
						if(exp.is_number())
							::MessageBeep(exp.to_number<uint32_t>());
					}
					else
					{
						::MessageBeep(0xFFFFFFFF);
					}
					return;
				}

				for(auto &t : MSG_FLAGS)
				{
					auto id = std::get<0>(t);
					if(id == Id[1])
					{
						_result = std::get<1>(t);
						break;
					}
				}
				return;
			}

			auto hwnd = context->Runtime ? context->wnd.active : nullptr;
			int32_t ret = 0;
			auto argc = Arguments.size();
			ret = ::MessageBoxW(hwnd,
								eval_arg(0).to_string(),
								(argc > 1) ? eval_arg(1).to_string().c_str() : APP_FULLNAME,// context->Initializer->Application.Name,
								eval_arg(2, 0U));
			_result = ret;
		}

		void FuncExpression::eval_reg()
		{
			if(Id.length() == 2)
			{
				switch(Id[1])
				{
					case REG_NONE:
					case IDENT_REG_NONE:
						_result = REG_NONE;
						return;
					case REG_SZ:
					case IDENT_REG_SZ:
						_result = REG_SZ;
						return;
					case REG_EXPAND_SZ:
					case IDENT_REG_EXPAND_SZ:
					case IDENT_REG_EXPAND:
						_result = REG_EXPAND_SZ;
						return;
					case REG_MULTI_SZ:
					case IDENT_REG_MULTI_SZ:
					case IDENT_REG_MULTI:
						_result = REG_MULTI_SZ;
						return;
					case REG_DWORD:
					case IDENT_REG_DWORD:
						_result = REG_DWORD;
						return;
					case REG_QWORD:
					case IDENT_REG_QWORD:
						_result = REG_QWORD;
						return;
					case REG_BINARY:
					case IDENT_REG_BINARY:
						_result = REG_BINARY;
						return;
				}
			}

			auto argc = Arguments.size();
			string sub_key = eval_arg(0).to_string().trim().move();
			HKEY root_key = nullptr;

			struct regkey_t 
			{
				string name[2];
				HKEY value = nullptr;

				int8_t eq(const string *key)
				{
					if(key->starts_with(name[0]))
						return 0;
					if(key->starts_with(name[1]))
						return 1;
					return -1;
				}
			} _regkey[] = {
				{ {L"HKCR", L"HKEY_CLASSES_ROOT"},		HKEY_CLASSES_ROOT },
				{ {L"HKCU", L"HKEY_CURRENT_USER"},		HKEY_CURRENT_USER },
				{ {L"HKLM", L"HKEY_LOCAL_MACHINE"},		HKEY_LOCAL_MACHINE },
				{ {L"HKU",  L"HKEY_USERS"},				HKEY_USERS }/*,
				{ {L"HKPD", L"HKEY_PERFORMANCE_DATA"},	HKEY_PERFORMANCE_DATA },
				{ {L"HKCC", L"HKEY_CURRENT_CONFIG"},	HKEY_CURRENT_CONFIG },
				{ {L"HKDD", L"HKEY_DYN_DATA"},			HKEY_DYN_DATA }*/
			};

			sub_key.trim(L'\\');

			if(!sub_key.empty())
			{
				for(auto i = 0; i < _countof(_regkey); i++)
				{
					if(auto r = _regkey[i].eq(&sub_key); r >= 0)
					{
						root_key = _regkey[i].value;
						sub_key = sub_key.substr(_regkey[i].name[r].length()).trim().move();
						break;
					}
				}

				if(!root_key)
					root_key = _regkey[1].value;
				else if(!sub_key.empty())
				{
					if(sub_key[0] == L':')
						sub_key.remove(0, 1);
					sub_key.trim(L'\\');
				}
			}

			if(root_key)
			{
				uint32_t cbdata = 0;
				uint32_t dwtype = REG_SZ;

				if(Id[1] == IDENT_ZERO || Id[1] == IDENT_GET || Id[1] == IDENT_EXISTS)
				{
					// reg.get("key", "name" = null, type = reg.sz)

					//if(ERROR_SUCCESS == ::RegGetValueW(root_key, sub_key, L"", 0, (DWORD*)&dwtype, nullptr, (DWORD *)&cbdata)){}

					auto keybase = RegistryKey::OpenBaseKey(root_key, false, 0);
					if(auto key = keybase.OpenSubKey(sub_key); key)
					{
						key.auto_close();

						string name;

						if(argc >= 2)
							name = eval_arg(1).to_string().move();

						if(Id[1] == IDENT_EXISTS)
						{
							if(argc == 1)
								_result = true;
							else if(argc >= 2)
								_result = key.ExistsValue(name);
							return;
						}

						if(argc == 3)
							dwtype = eval_arg(2).move();
						else if(!key.get_value(name, &dwtype, &cbdata))
							return;

						if(dwtype == REG_SZ || dwtype == REG_EXPAND_SZ)
						{
							string value;
							uint32_t len = (cbdata / sizeof(wchar_t));
							if(key.get_value(name, &dwtype, &cbdata, value.buffer(len)))
							{
								value.release(len - 1);
								if(dwtype == REG_EXPAND_SZ)
									Environment::Expand(value).move();
								_result = value.move();
							}
						}
						else if(dwtype == REG_DWORD)
						{
							uint32_t value = 0;
							if(key.get_value(name, &dwtype, &cbdata, &value))
								_result = value;
						}
						else if(dwtype == REG_QWORD)
						{
							uint64_t value = 0;
							if(key.get_value(name, &dwtype, &cbdata, &value))
								_result = value;
						}
					}
				}
				else if(Id[1] == IDENT_SET)
				{
					// reg.set("key", "name" = null, value = null, type = reg.sz)

					if(argc == 1)
					{
						_result = Registry::SetKeyValue(root_key, nullptr, nullptr, REG_NONE, nullptr, 0);
						return;
					}

					_result = false;
					
					string name;
					Object value;

					if(argc > 1)
						name = eval_arg(1).to_string().trim().move();

					if(argc > 2)
						value = eval_arg(2).move();

					if(argc == 3)
						dwtype = value.is_number() ? REG_DWORD : REG_SZ;
					else if(argc == 4)
						dwtype = eval_arg(3).move();

					if(dwtype == REG_DWORD)
						_result = Registry::SetKeyValue(root_key, sub_key, name, value.to_number<uint32_t>());
					else if(dwtype == REG_QWORD)
						_result = Registry::SetKeyValue(root_key, sub_key, name, value.to_number<uint64_t>());
					else if(dwtype == REG_SZ || dwtype == REG_EXPAND_SZ)
					{
						string v = value.to_string().move();
						_result = Registry::SetKeyValue(root_key, sub_key, name, v, v.length(), dwtype == REG_EXPAND_SZ);
					}
				}
				else if(Id[1] == IDENT_DELETE)
				{
					// reg.get("key", "name" = null, type = auto)
					_result = false;
					if(sub_key.empty())
						return;
					else if(argc == 1)
						_result = Registry::DeleteSubKey(root_key, sub_key); // ::RegDeleteKeyExW
					else if(argc == 2)
						_result = Registry::DeleteKeyValue(root_key, sub_key, eval_arg(1).to_string().trim());
				}
				else if(Id[1] == IDENT_KEYS || Id[1] == IDENT_VALUES)
				{
					auto keybase = RegistryKey::OpenBaseKey(root_key, false, 0);
					if(auto key = keybase.OpenSubKey(sub_key); key)
					{
						std::vector<string> list;

						LSTATUS retCode = ERROR_SUCCESS;
						for(auto i = 0u; retCode == ERROR_SUCCESS; i++)
						{
							DWORD name_length = 260;
							string name(name_length);

							if(Id[1] == IDENT_KEYS)
								retCode = ::RegEnumKeyExW(key, i, name.data(), &name_length, nullptr, nullptr, nullptr, nullptr);
							else
								retCode = ::RegEnumValueW(key, i, name.data(), &name_length, nullptr, nullptr, nullptr, nullptr);
							
							if(retCode == ERROR_SUCCESS)
								list.push_back(name.release(name_length).move());
						}

						key.Close();

						auto array = new Object[list.size() + 1];
						Object result(array, true);
						*array++ = list.size();
						for(auto &s : list)
						{
							try {
								*array++ = s.move();
							}
							catch(...) {}
						}
						_result = result.move();
					}
				}
			}

			if(Id[1] == IDENT_EXISTS)
				_result = false;
		}

		void FuncExpression::eval_icon()
		{
			auto argc = static_cast<int>(Arguments.size());
			auto image = &context->theme->image;

			auto make_ret = [&](uint32_t func1, uint32_t func2, int c, Object &&arg1, Object &&arg2, Object &&arg3, Object &&arg4, Object &&arg5)
			{
				return new std::tuple<uint32_t, uint32_t, int, Object, Object, Object, Object, Object>(
					std::make_tuple(func1, func2, c, arg1.move(), arg2.move(), arg3.move(), arg4.move(), arg5.move())
				);
			};

			if(Id.length() == 2 && Id[0] == IDENT_ICON)
			{
				if(Id[1] == IDENT_BOX)
				{
					wchar_t icon_path[MAX_PATH + 1]{};
					int icon_index = 0;
					if(argc >= 1)
						string::Copy(icon_path, eval_arg(0).to_string());
					if(argc == 2)
						icon_index = eval_arg(1);

					auto hwnd = context->Runtime ? context->wnd.active : nullptr;
					if(::PickIconDlg(hwnd, icon_path, MAX_PATH, &icon_index) && icon_path[0])
					{
						string str; str.format(L"%s,%d", icon_path, icon_index);
						_result = str.move();
					}
					return;
				}

				if(Id[1] == IDENT_CHANGE)
				{
					string path = eval_arg(0).to_string().move();
					if(Path::IsDirectoryExists(path))
					{
						string icon_file(MAX_PATH + 1);

						int icon_index = 0;
						auto hwnd = context->Runtime ? context->wnd.active : nullptr;
						if(argc == 1)
						{
							::PickIconDlg(hwnd, icon_file.data(), MAX_PATH, &icon_index);
							icon_file.release();
						}
						else
						{
							icon_file = eval_arg(1).to_string().move();

							if(argc == 3)
								icon_index = eval_arg(2);
							else
							{
								auto p = icon_file.last_index_of(',', false);
								if(p != icon_file.npos)
								{
									icon_index = icon_file.substr(p + 1).to_int<int>();
									icon_file.trim().remove(p);
								}
							}
						}
						
						if(!icon_file.empty())
						{
							SHFOLDERCUSTOMSETTINGS fcs = { sizeof(fcs) };
							fcs.dwMask = FCSM_ICONFILE;
							fcs.pszIconFile = icon_file;
							fcs.cchIconFile = (DWORD)icon_file.length();
							fcs.iIconIndex = icon_index;
							::SHGetSetFolderCustomSettings(&fcs, path.c_str(), FCS_FORCEWRITE);
							::SHChangeNotify(SHCNE_RENAMEFOLDER, SHCNF_PATH, path.c_str(), path.c_str());
						}
					}
					return;
				}

				for(const auto &it : cache->images)
				{
					if(!it.equals(Id[1]))
						continue;

					const Color color[] = { image->color[0], image->color[1], image->color[2] };

					if(argc > 0)
					{
						if(argc >= 1)
							context->to_color(eval_arg(0), &image->color[0]);
						if(argc >= 2)
							context->to_color(eval_arg(1), &image->color[1]);
						if(argc >= 3)
							context->to_color(eval_arg(2), &image->color[2]);
					}

					Object obj = context->Eval(it.value).move();

					if(argc > 0)
					{
						image->color[0] = color[0];
						image->color[1] = color[1];
						image->color[2] = color[2];
					}

					if(!obj.is_null())
					{
						if(context->is_svg(obj))
							obj = make_ret(IDENT_IMAGE, IDENT_SVG, argc, obj.move(), nullptr, nullptr, nullptr, nullptr);
						_result = obj.move();
					}
					return;
				}
			}

			switch(Id[1])
			{
				case IDENT_GLYPH:
					_result = make_ret(Id[0], Id[1], argc, eval_arg(0), eval_arg(1), eval_arg(2), eval_arg(3), nullptr);
					break;
				case IDENT_ICON_FLUENT:
				case IDENT_ICON_MDL:
				case IDENT_ICONS_SEGOE:
					_result = make_ret(Id[0], Id[1], argc, eval_arg(0), eval_arg(1), eval_arg(2), nullptr, nullptr);
					return;
				case IDENT_SVG:
				case IDENT_SVGF:
				{
					if(Id[2] == 0)
					{
						Object obj = eval_arg(0).move();
						if(Id[1] == IDENT_SVG)
							context->is_svg(obj);
						_result = make_ret(Id[0], Id[1], argc, obj.move(), nullptr, nullptr, nullptr, nullptr);
					}
					break;
				}
				//case IDENT_ICON:
				//case IDENT_RES:
				default:
				{
					if(argc == 0)
					{
						if(Id[0] == IDENT_IMAGE || Id[0] == IDENT_ICON)
						{
							switch(Id[1])
							{
								case IDENT_COLOR1:
									_result = Object(image->color[0]).move();
									return;
								case IDENT_COLOR2:
									_result = Object(image->color[1]).move();
									return;
								case IDENT_COLOR3:
									_result = Object(image->color[2]).move();
									return;
							}
						}
					}

					_result = make_ret(Id[0], Id[1], argc, eval_arg(0), eval_arg(1), eval_arg(2), nullptr, nullptr);
					return;
				}
			}
		}
		
		Object FuncExpression::eval_key()
		{
			auto keys = context->Keyboard;

			if(Id.length() == 1)
			{
				if(keys && keys->count() > 0)
				{
					if(Arguments.empty())
						return keys->first();
					else
					{
						//key(key.shift, key.contol)
						int key_count = 0;
						for(size_t i = 0; i < Arguments.size(); i++)
						{
							if(Object k = eval_arg(i); k.is_number())
								key_count += int(keys->key(k.to_number<uint8_t>()));
						}

						return (key_count == (int)Arguments.size());
					}
				}
			}
			else if(Id.length() == 2)
			{
				auto key = [=](uint8_t k)->uint8_t
				{
					if(this->Brackets)
						return keys ? keys->key(k) : 0;
					return k;
				};

				if(Id[1] == IDENT_COUNT)
					return keys->count();
				else if(Id[1] == IDENT_KEY_NONE)
					return 0;
				else if(Id[1] == IDENT_KEY_SEND)
				{
					std::vector<uint8_t> vkeys;
					for(size_t i = 0; i < Arguments.size(); i++)
					{
						Object k = eval_arg(i).move();
						if(k.is_number())
							vkeys.push_back(k);
						else if(k.is_string())
						{
							string sk = k.to_string().trim().move();
							if(!sk.empty())
								vkeys.push_back((uint8_t)Char::ToUpper(sk[0]));
						}
					}

					for(size_t i = 0; i < vkeys.size(); i++)
						::keybd_event(vkeys[i], 0, 0, 0);
					for(size_t i = 0; i < vkeys.size(); i++)
						::keybd_event(vkeys[i], 0, KEYEVENTF_KEYUP, 0);
					return 0;
				}
				else if(Id[1] == IDENT_KEY_WIN)
				{
					if(this->Brackets)
						return key(VK_LWIN) || key(VK_RWIN);
					return VK_LWIN;
				}
				else if(Id[1] == IDENT_KEY_SHIFT)
				{
					if(this->Brackets)
						return key(VK_LSHIFT) || key(VK_RSHIFT);
					return VK_LSHIFT;
				}
				else if(Id[1] == IDENT_KEY_CONTROL)
				{
					if(this->Brackets)
						return key(VK_LCONTROL) || key(VK_RCONTROL);
					return VK_LCONTROL;
				}
				else if(Id[1] == IDENT_KEY_ALT)
				{
					if(this->Brackets)
						return key(VK_LMENU) || key(VK_RMENU);
					return VK_LMENU;
				}
				else for(auto &t : KeyTable)
				{
					auto id = std::get<0>(t);
					if(id == Id[1])
					{
						return key((uint8_t)std::get<1>(t));
					}
				}
			}
			return int(0);
		}

		Object FuncExpression::eval_color()
		{
			auto argc = Arguments.size();

			if(Id.length() == 1 || Id[1] == IDENT_COLOR_RGB)
			{
				Object color;
				if(argc == 1)
					color = eval_arg(0).to_color().to_ABGR();
				else
				{
					uint8_t r = eval_arg(0);
					uint8_t g = eval_arg(1);
					uint8_t b = eval_arg(2);
					uint8_t a = argc == 4 ? 0xff : eval_arg(3).to_number<uint8_t>();
					color = Color(r, g, b, a).to_ARGB();
				}
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_BOX)
			{
				Object color;
				
				CHOOSECOLORW cc = { sizeof(cc) };
				cc.hwndOwner = context->wnd.active;

				COLORREF acrCustClr[16]{};
				cc.lpCustColors = (LPDWORD)acrCustClr;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;

				if(argc >= 1)
				{
					cc.rgbResult = eval_arg(0).to_color().to_BGR();
				}

				if(DLL::Invoke<BOOL>(L"Comdlg32.dll", "ChooseColorW", &cc))
				{
					color = cc.rgbResult;
					color.inner(PrimitiveType::Color);
				}

				return color.move();
			}
			else if(Id[1] == IDENT_COLOR_RANDOM)
			{
				uint8_t min = argc == 1 ? 0 : eval_arg(0).to_number<uint8_t>();
				uint8_t max = eval_arg(argc == 1 ? 0 : 1).to_number<uint8_t>();

				auto r = Environment::RandomNumber(min, max);
				auto g = Environment::RandomNumber(min, max);
				auto b = Environment::RandomNumber(min, max);

				Object color = RGB(r, g, b);
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_INVERT)
			{
				Object color = eval_arg(0).to_color().invert();
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_RGBA)
			{
				Object color;
				if(argc == 2)
				{
					color = Color(eval_arg(0).to_color(), 
								  eval_arg(1).to_number<uint8_t>()).to_ABGR();
				}
				else 
				{
					color = Color(eval_arg(0).to_number<uint8_t>(),
								  eval_arg(1).to_number<uint8_t>(),
								  eval_arg(2).to_number<uint8_t>(),
								  eval_arg(3).to_number<uint8_t>()).to_ABGR();
				}

				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_LIGHT ||
					Id[1] == IDENT_COLOR_DARK ||
					Id[1] == IDENT_COLOR_LIGHTEN ||
					Id[1] == IDENT_COLOR_DARKEN ||
					Id[1] == IDENT_COLOR_ADJUST)
			{
				Object ret;
				int value = 50;
				auto color = eval_arg(0).to_color();

				if(Arguments.size() == 2)
					value = eval_arg(1);

				if(value == 0)
					ret = color.to_ABGR();
				else
				{
					if(Id[1] == IDENT_COLOR_DARK || Id[1] == IDENT_COLOR_DARKEN)
						value = -value;
					ret = color.adjust(value).to_ABGR();
				}
				return ret.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_OPACITY)
			{
				auto color = eval_arg(0).to_color();
				color.opacity(eval_arg(1));
				return Object(color).move();
			}
			else if(Id[1] == IDENT_COLOR_ACCENT)
			{
				Object color = 0x00000000;
				if(ImmersiveColor::IsSupported())
					color = (uint32_t)Color(ImmersiveColor::GetColorByColorType(4));
				else
				{
					/*DWORD color = 0;
					BOOL opaque = FALSE;
					HRESULT hr = DwmGetColorizationColor(&color, &opaque);
					if(SUCCEEDED(hr))
					{
						// Update the application to use the new color.
					}
					*/
					//SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Accent

					auto colorizationColor = Registry::CurrentUser.OpenSubKey(LR"(SOFTWARE\Microsoft\Windows\DWM)", false, false);
					if(colorizationColor)
					{
						color = colorizationColor.GetDWord(L"ColorizationColor");
					}
				}
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_ACCENT_LIGHT1)
			{
				Object color = Color(ImmersiveColor::GetColorByColorType(3)).to_ABGR();
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_ACCENT_LIGHT2)
			{
				Object color = Color(ImmersiveColor::GetColorByColorType(2)).to_ABGR();
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_ACCENT_LIGHT3)
			{
				Object color = Color(ImmersiveColor::GetColorByColorType(1)).to_ABGR();
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_ACCENT_DARK1)
			{
				Object color = Color(ImmersiveColor::GetColorByColorType(5)).to_ABGR();
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_ACCENT_DARK2)
			{
				Object color = Color(ImmersiveColor::GetColorByColorType(6)).to_ABGR();
				return color.inner(PrimitiveType::Color).move();
			}
			else if(Id[1] == IDENT_COLOR_ACCENT_DARK3)
			{
				Object color = Color(ImmersiveColor::GetColorByColorType(7)).to_ABGR();
				return color.inner(PrimitiveType::Color).move();
			}
			
			for(auto &t : ColorTable)
			{
				auto id = std::get<0>(t);
				if(id == Id[1])
					return Color::Swap(std::get<1>(t));
			}
			return nullptr;
		}

		bool FuncExpression::eval_var(Object &obj)
		{
			if(Id.length() == 1)
				eval_var(Id[0], obj);
			else if(Id.length() == 2)
				eval_var(Id[1], obj);
			else if(!Arguments.empty())
			{
				Object arg0 = eval_arg(0).move();
				if(arg0.is_null())
					return false;

				auto _id = Id[0];

				if(Id.length() == 2)
					_id = Id[1];

				if(Arguments[0]->Type() == ExpressionType::Identifier)
				{
					auto ff = (FuncExpression *)Arguments[0];
					_id = ff->Id[0];
				}
				else if(arg0.is_string())
				{
					if(arg0.is_null())
						return false;

					string var = arg0.to_string().move();
					if(!var.empty())
					{
						_id = var.hash();
					}
				}

				if(Arguments.size() == 1)
					eval_var(_id, obj);
				else if(Arguments.size() == 2)
					context->set_variable(_id, Arguments[1]->Copy());
			}
			return false;
		}

		bool FuncExpression::eval_var(uint32_t id, Object &obj)
		{
			auto var = context->get_variable(id, this);
			if(var)
			{
				obj = context->Eval(var).move();
				/*if(obj)
				{
					if(Array && !obj.is_default())
					{
						MB(0);
						if(Object oa = context->Eval(Array).move(); oa.is_number())
						{
							int ai = oa;
							if(ai >= 0)
							{
								if(obj.is_array(true))
								{
									Object obj_ret2;
									obj.get(ai + 1, obj_ret2);
									obj = obj_ret2.move();
								}
								else
								{
									obj = obj.to_string()[ai];
								}
							}
						}
					}
				}*/
			}
			return var != nullptr;
		}
	}
}