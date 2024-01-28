#include <pch.h>
#include <Library/plutovg.h>
#include <Library/plutosvg.h>
#include <Library/PlutoVGWrap.h>

namespace Nilesoft
{
	namespace Shell
	{
		struct RGBA
		{
			uint8_t b{};
			uint8_t g{};
			uint8_t r{};
			uint8_t a{};

			void copy(RGBA* clr)
			{
				b = clr->b;
				g = clr->g;
				r = clr->r;
				a = clr->a;
			}

			void pre(RGBA* clr)
			{
				b = uint8_t((clr->r * a) / 255);
				g = uint8_t((clr->g * a) / 255);
				r = uint8_t((clr->b * a) / 255);
			}

			void toBGRA(RGBA* clr)
			{
				b = clr->r;
				g = clr->g;
				r = clr->b;
				a = clr->a;
			}

			uint32_t toBGRA(uint32_t rgba)
			{
				auto clr = (RGBA*)&rgba;
				uint32_t res = 0;

				clr = (RGBA*)&rgba;
				auto bgra = (RGBA*)&res;

				bgra->b = clr->r;
				bgra->g = clr->g;
				bgra->r = clr->b;
				bgra->a = clr->a;

				return res;
			}

			uint32_t MakeArgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF)
			{
				return(uint32_t)(((uint32_t)(red << 16 | green << 8 | blue << 0 | alpha << 24))) & UINT32_MAX;
			}
		};

		//Interpret | Eval
		//evaluate expressions
		//Evaluate an expression. Returns the result of the expression.

		bool Context::to_color(const Object &obj, Color *color)
		{
			auto ret = false;
			try 
			{
				if(obj.not_default())
				{
					*color = obj.to_color();
					ret = true;
				}
				else if(obj.is_array() && obj.Value.Pointer)
				{
					auto ptr = obj.get_pointer();
					uint32_t ac = ptr[0];

					if(ac >= 1 && ac <= 4)
					{
						if(ac == 1 && ptr[1].not_default())
						{
							*color = ptr[1].to_color();
							ret = true;
						}
						else if(ac == 2)
						{
							if(ptr[1].not_default())
							{
								*color = ptr[1].to_color();
								ret = true;
							}

							if(ptr[2].not_default())
							{
								color->opacity(ptr[2]);
								ret = true;
							}
						}
						else if(ac == 3 || ac == 4)
						{
							RGBA clr;
							clr.b = ptr[1];
							clr.g = ptr[2];
							clr.r = ptr[3];

							if(ac == 4 && ptr[4].not_default())
								clr.a = ptr[4];

							color->from(clr.b, clr.g, clr.r, clr.a);
							ret = true;
						}
					}
				}
				return ret;
			}
			catch(...) {
				return false;
			}
		}

		bool Context::eval_color(Expression *e, Color *color)
		{
			if(e == nullptr) return false;
			Object obj = Eval(e).move();;
			return to_color(obj, color);
		}

		bool Context::eval_number(Expression *e, Object &obj)
		{
			obj = nullptr;
			if(!e) return false;
			obj = Eval(e).move();
			return obj.not_default();
		}

		bool Context::eval_bool(Expression *e)
		{
			if(!e) return false;

			Object obj = Eval(e).move();
			return obj.not_default() && obj.to_bool();
		}

		Object Context::Eval(Expression *e)
		{
			if(e) try 
			{
				return e->Eval(this).move();
			}
			catch(...) {
			}
			return nullptr;
		}

		Object Context::Eval(Expression *e, const Object &defualt)
		{
			return e ? Eval(e).move() : defualt;
		}

		bool Context::Eval(Expression *e, Object &obj)
		{
			if(e)
			{
				obj = Eval(e).move();
				return !obj.is_null();
			}
			return false;
		}

		bool Context::Eval(Expression *e, string &value, bool trim)
		{
			if(e)
			{
				Object obj = Eval(e).move();
				if(!obj.is_null())
				{
					value = obj.to_string().move();
					if(trim) value.trim();
					return true;
				}
			}
			return false;
		}

		uint32_t Context::Eval(Expression *e, uint32_t defualt)
		{
			if(e)
			{
				Object obj = Eval(e).move();
				if(obj.is_number()) return obj;

				if(!obj.is_null())
				{
					uint32_t val;
					if(obj.Value.String.parse_number<uint32_t>(&val, defualt))
						return val;
				}
			}
			return defualt;
		}

		bool Context::variable_exists(uint32_t id)
		{
			if(variables.runtime && variables.runtime->exists(id, true))
				return true;
			else if(variables.local && variables.local->exists(id, true))
				return true;
			else if(variables.global && variables.global->exists(id, true))
				return true;
			return false;
		}

		Expression *Context::get_variable(uint32_t id, Expression *e)
		{
			Expression *var = nullptr;
			
			if(e)
			{
				var = e->Scope.at(id);
				if(!var && e->Parent)
				{
					auto parent = e->Parent;
					while(parent)
					{
						var = parent->Scope.at(id);
						if(var) 
						{
							return var;
						}
						parent = parent->Parent;
					}
				}
			}

			Scope *_variables = nullptr;

			if(var == nullptr && variables.runtime)
			{
				_variables = variables.runtime;
				while(_variables)
				{
					var = _variables->at(id);
					if(var)
					{
						return var;
					}
					_variables = _variables->Parent;
				}
			}

			if(var == nullptr && variables.local)
			{
				_variables = variables.local;
				while(_variables)
				{
					var = _variables->at(id);
					if(var)
					{
						return var;
					}
					_variables = _variables->Parent;
				}
			}
			
			if(var == nullptr && variables.global)
			{
				if(_variables != variables.global)
					var = variables.global->at(id);
			}

			return var;
		}

		void Context::set_variable2(uint32_t id, Expression *e, Object &&value)
		{
			Expression *var = nullptr;

			if(e)
			{
				var = e->Scope.at(id);
				if(var)
				{
					e->Scope.set(id, value);
					return;
				}

				if(e->Parent)
				{
					auto parent = e->Parent;
					while(parent)
					{
						var = parent->Scope.at(id);
						if(var) 
						{
							parent->Scope.set(id, value);
							return;
						}
						parent = parent->Parent;
					}
				}
			}

			Scope *_variables = nullptr;

			if(variables.runtime)
			{
				_variables = variables.runtime;
				while(_variables)
				{
					var = _variables->at(id);
					if(var)
					{
						_variables->set(id, value);
						return;
					}
					_variables = _variables->Parent;
				}
			}

			if(variables.local)
			{
				_variables = variables.local;
				while(_variables)
				{
					var = _variables->at(id);
					if(var)
					{
						_variables->set(id, value);
						return;
					}
					_variables = _variables->Parent;
				}
			}

			if(variables.global && _variables != variables.global)
			{
				var = variables.global->at(id);
				if(var)
				{
					//_variables->set(id, value);
					if(variables.runtime)
					{
						variables.runtime->set(id, value);
					}
					return;
				}
			}
			
			if(e && e->Parent)
			{
				e->Parent->Scope.set(id, value);
				return;
			}

			if(variables.runtime)
				variables.runtime->set(id, value);

			return;
		}

		Object &Context::eval_object(Expression *array, Object &obj)
		{
			if(array && !obj.is_default())
			{
				if(Object oa = Eval(array).move(); oa.is_number())
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
			return obj;
		}

		bool Context::set_variable(uint32_t id, Expression *e)
		{
			if(variables.runtime)
			{
				variables.runtime->set(id, e);
				return true;
			}
			return false;
		}


		Visibility Context::parse_visibility(Expression *e, Visibility default_value)
		{
			if(e)
			{
				switch(eval_hash(e, default_value))
				{
					case 0:
					case IDENT_REMOVE:
					case IDENT_HIDDEN:
						return Visibility::Hidden;
					case 1:
					case IDENT_ENABLED:
						return Visibility::Enabled;
					case 2:
					case IDENT_DISABLE:
					case IDENT_DISABLED:
						return Visibility::Disabled;
					case 3:
					case IDENT_VIS_STATIC:
						return Visibility::Static;
					case 4:
					case IDENT_VIS_LABEL:
						return Visibility::Label;
				}
			}
			return default_value;
		}

		Position Context::parse_position(Expression *e, Position default_value)
		{
			return parse_pos(Eval(e), default_value);
		}

		Position Context::parse_pos(const Object &o, Position default_value)
		{
			auto pos = obj2hash(o, default_value);
			switch(pos)
			{
				case -1:
				case IDENT_BOTTOM:
					return Position::Bottom;
				case -2:
				case IDENT_TOP:
					return Position::Top;
				case -3:
				case IDENT_MIDDLE:
					return Position::Middle;
				case -4:
				case IDENT_AUTO:
					return Position::Auto;
				default:
					return (Position)pos;
			}
		}

		Separator Context::parse_separator(Expression *e, Separator default_value)
		{
			if(e)
			{
				switch(eval_hash(e, default_value))
				{
					case 0:
					case IDENT_NONE:
						return Separator::None;
					case 1:
					case IDENT_BEFORE:
					case IDENT_TOP:
						return Separator::Top;
					case 2:
					case IDENT_AFTER:
					case IDENT_BOTTOM:
						return Separator::Bottom;
					case 3:
					case IDENT_BOTH:
						return Separator::Both;
				}
			}
			return default_value;
		}

		SelectionMode Context::parse_mode(Expression *e, SelectionMode default_value)
		{
			if(e)
			{
				switch(eval_hash(e, default_value))
				{
					case 0:
					case IDENT_MODE_NONE:
						return SelectionMode::None;
					case 1:
					case IDENT_MODE_SINGLE:
						return SelectionMode::Single;
					case 2:
					case IDENT_MODE_MULTI_UNIQUE:
						return SelectionMode::MultiUnique;
					case 3:
					case IDENT_MODE_MULTI_SINGLE:
						return SelectionMode::MultiSingle;
					case 4:
					case IDENT_MODE_MULTIPLE:
					case IDENT_MODE_MULTI:
						return SelectionMode::Multiple;
				}
			}
			return default_value;
		}

		Privileges Context::parse_privileges(CommandProperty *cmd)
		{
			if(cmd and cmd->admin)
			{
				switch(eval_hash(cmd->admin, Privileges::None))
				{
					case 0:
					case IDENT_FALSE:
					case IDENT_PRIVILEGES_NONE:
						return Privileges::None;
					case 1:
					case IDENT_TRUE:
					case IDENT_PRIVILEGES_DEFAULT:
						return Privileges::Default;
					case 2:
					case IDENT_PRIVILEGES_USER:
						return Privileges::User;
					case 3:
					case IDENT_PRIVILEGES_SYSTEM:
						return Privileges::System;
					case 4:
					case IDENT_PRIVILEGES_TRUSTEDINSTALLER:
						return Privileges::TrustedInstaller;
				}
			}
			return Privileges::None;
		}

		Verbs Context::parse_verbs(CommandProperty *cmd)
		{
			auto ret = Verbs::Null;
			if(cmd && cmd->verb)
			{
				ret = eval_number<Verbs>(cmd->verb, ret);
				if(ret > Verbs::OpenAs) ret = Verbs::Null;
			}
			return ret;
		}

		int Context::parse_window(CommandProperty *cmd)
		{
			if(cmd && cmd->window)
			{
				switch(eval_hash(cmd->window, SW_SHOWNORMAL))
				{
					case 0:
					case IDENT_FALSE:
					case IDENT_HIDDEN:
						return SW_HIDE;
					case 1:
					case IDENT_TRUE:
					case IDENT_SHOW:
					case IDENT_NORMAL:
					case IDENT_VISIBLE:
						return SW_SHOWNORMAL;
					case 2:
					case IDENT_MINIMIZED:
						return SW_SHOWMINIMIZED;
					case 3:
					case IDENT_MAXIMIZED:
						return SW_SHOWMAXIMIZED;
				}
			}
			return SW_SHOWNORMAL;
		}

		uint32_t Context::parse_wait(CommandProperty *cmd)
		{
			return cmd ? eval_number(cmd->wait, 0u) : 0;
		}
		
		uint32_t Context::parse_invoke(Expression *e)
		{
			if(e)
			{
				auto ret = eval_hash(e, 0);
				switch(ret)
				{
					case 0:
					case IDENT_FALSE:
					case IDENT_MODE_SINGLE:
						return 0;
					case 1:
					case IDENT_TRUE:
					case IDENT_MODE_MULTIPLE:
						return 1;
					default:
						return ret;
				}
			}
			return 0;
		}

		bool Context::eval_tip(Expression *e, string &text, uint8_t &type, uint16_t &time)
		{
			Object obj;
			if(Eval(e, obj))
			{
				type = 0;
				if(obj.is_array() && obj.Value.Pointer)
				{
					auto ptr = obj.get_pointer();
					auto c = ptr[0].to_number<uint32_t>();

					if(c >= 1)
						text = ptr[1].to_string().move();

					if(c >= 2)
						type = ptr[2].to_number<uint8_t>();

					if(c == 3)
					{
						auto dtime = ptr[3].to_number<double>();
						time = static_cast<uint16_t>(dtime * 1000);
						if(time > 10000)
							time = 10000;
					}
					return true;
				}
				else
				{
					text = obj.to_string().move();
					return true;
				}
			}
			return false;
		}

		int Context::is_svg(Object &obj)
		{
			int ret = 0;
			if(obj.is_string())
			{
				auto str = &obj.Value.String.trim();
				if(str->starts_with(L"<svg") && str->ends_with(L"</svg>"))
					ret = 1;
				else if(str->starts_with({ L"<path ", L"<rect ", L"<circle ", L"<ellipse ", L"<polygon ", L"<polyline ", L"<line ", L"<g " }))
					ret = 2;
			/*	else if(str->length() > 3 && str->starts_with(L'M') && str->ends_with(L'Z'))
				{
					*str = (L"<path d=\"" + *str + L"\"/>").move();
					ret = 3;
				}*/

				if(ret > 1)
					*str = (L"<svg>" + *str + L"</svg>").move();
			}
			return ret;
		}

		bool Context::Image(Expression *e, MenuItemInfo* mii, HFONT hfont, bool selected)
		{
			try
			{
				auto image = &mii->image;
				if(selected)
				{
					if(!e) return false;
					image = &mii->image_select;
				}

				auto cache = Initializer::instance->cache;

				const auto image_size = (long)theme->image.size;

				if(!e and (mii->ui or mii->image.expr))
				{
					image->destroy();

					if(mii->image.expr)
					{
						string value;
						if(Eval(mii->image.expr, value) && !value.empty())
						{
							std::string vutf8 = std::move(string::ToUTF8(value, value.length<int>()));
							PlutoVG plutovg;
							plutovg.load_from_memory(vutf8.c_str(), (int)vutf8.size(), image_size, image_size);
							if(plutovg)
							{
								//plutovg.rgba0();
								image->hbitmap = plutovg.tobitmap();
								image->inherited = false;
								image->import = ImageImport::SVG;
								image->size = { image_size, image_size };
								return true;
							}
						}
						return false;
					}

					if(!mii->ui)
						return false;

					image->import = ImageImport::Draw;

					Color color_[2];

					if(theme->mode != 2)
					{
						if(theme->mode == 1)
						{
							color_[0] = Color::Swap(0xFFE0DFDF);
							color_[1] = Color::Swap(0xFF4CC2FF);
						}
						else
						{
							color_[0] = Color::Swap(0xFF555555);
							color_[1] = Color::Swap(0xFF0078D4);
						}
					}

					if(theme->image.color[0])
						color_[0] = this->theme->image.color[0];

					if(theme->image.color[1])
						color_[1] = this->theme->image.color[1];

					/*auto sv = [](uint32_t val, uint32_t def)->uint32_t {
						if(val == CLR_INVALID || val == IDENT_DEFAULT)
							return def;
						return val;
					};*/

					image->draw.type = image->draw.DT_GLYPH;
					image->draw.glyph.font = hfont;
					image->draw.glyph.size = { image_size, image_size };

					image->draw.glyph.code[0] = mii->ui->image_glyph[0];
					image->draw.glyph.code[1] = mii->ui->image_glyph[1];

					image->draw.glyph.color[0] = color_[0];// sv(idui->image_color[0], color_[0]);
					image->draw.glyph.color[1] = color_[1];// sv(idui->image_color[1], color_[1]);

					return true;
				}


				mii->ui = nullptr;

				Object obj = Eval(e).move();
				
				if(obj.is_null())
				{
					if(e)
					{
						image->import = ImageImport::Disabled;
					}
					return false;
				}

				long width = image_size, height = image_size;

				if(is_svg(obj))
				{
					string value = obj.to_string().move();
					std::string vutf8 = std::move(string::ToUTF8(value, value.length<int>()));
					PlutoVG plutovg;
					plutovg.load_from_memory(vutf8.c_str(), (int)vutf8.size(), width, height);
					if(plutovg)
					{
						image->hbitmap = plutovg.tobitmap();
						image->inherited = false;
						image->import = ImageImport::SVG;
						image->size = { width, height };
						return true;
					}
				}
				else if(obj.is_string() && obj.length() > 2)
				{
					string path = obj.to_string().trim(str_trim).move();
					switch(path.hash())
					{
						case 0:
						case IDENT_NONE:
						case IDENT_DISABLED:
							return false;
						case IDENT_INHERIT:
						case IDENT_PARENT:
						{
							if(image->hbitmap)
							{
								image->import = ImageImport::Inherit;
							}
							else if(mii->owner && image->import == ImageImport::Draw)
							{
								std::memcpy(&mii->image, &mii->owner->image, sizeof(mii->image));
							}
							return true;
						}
					}
					
					image->destroy();
					image->hbitmap = Image::From(path.c_str(), image_size);
					if(image->hbitmap)
					{
						image->import = ImageImport::Image;
						image->size = { image_size, image_size };
						return true;
					}
					return false;
				}

				Color color;
				string font_name, icon_path;
				int font_size = image_size, icon_id = 0;
				
				wchar_t glyph = 0;

				if(obj.is_pointer(true))
				{
					if(obj.is_array())
					{
						bool ret = false;

						//image=[glyph, [light, dark], [font, size], [16,16]]
						//image=[glyph, color, size, font name]
						//image = [[glyph, color],[glyph, color], size, font name]

						auto parse_glyph = [](Object const *obj, wchar_t *glyph, Color *color)->bool
						{
							if(!obj || !obj->is_array(true))
								return false;

							auto ptr = obj->get_pointer();
							auto argc = ptr[0].to_number<int>();

							if(argc == 0 || ptr[1].is_null())
								return false;

							if(ptr[1].is_number())
								*glyph = (wchar_t)ptr[1].to_number<uint16_t>();
							else if(ptr[1].is_string())
								*glyph = ptr[1].to_string().trim()[0];

							if(argc > 1 && ptr[2].not_default())
								*color = ptr[2].to_color();

							return true;
						};

						//[\uxxxx, #xxx, size, name]
						auto parse_arg = [](Object const *ptr, int index, Color *color, int *size, string *name)
						{
							//if(!ptr || !ptr->is_pointer())
							//	return;

							auto argc = ptr[0].to_number<int>();
							if(index > argc)
								return;

							auto obj = &ptr[index];

							if(obj->is_string())
								*name = obj->to_string().move();
							else if(obj->not_default())
							{
								if(obj->is_color())
									*color = obj->to_color();
								else
									*size = obj->to_number();
							}
						};

						auto ptr = obj.get_pointer();
						auto argc = ptr[0].to_number<int>();

						if(argc == 0 || ptr[1].is_null())
							return false;
						
						wchar_t g[2] = {};
						Color clr[2];
					
						int i = 2;

						//[[],[],size,name]
						if(ptr[1].is_pointer())
						{
							if(!parse_glyph(&ptr[1], &g[0], &clr[0]))
							   return false;
							i += (argc > 1 && parse_glyph(&ptr[2], &g[1], &clr[1]));
						}
						else
						{
							if(ptr[1].is_number())
								g[0] = (wchar_t)ptr[1].to_number<uint16_t>();
							else
								g[0] = ptr[1].to_string().trim()[0];

							if(g[0] == 0)
								return false;
						}
						
						if(argc > 1 && argc >= i)
						{
							parse_arg(ptr, i++, &clr[0], &font_size, &font_name);
							parse_arg(ptr, i++, &clr[0], &font_size, &font_name);
							parse_arg(ptr, i, &clr[0], &font_size, &font_name);
						}

						if(font_size < 2)
							font_size = 2;

						if(font_size > image_size)
							font_size = image_size;

						if(g[0] || g[1])
						{
							if(font_name.trim().empty())
							{
								if(font_size != image_size)
								{
									font_name = cache->glyph.name;
									hfont = cache->fonts.add(cache->glyph.name, font_size, dpi->val);
								}
							}
							else
							{
								hfont = cache->fonts.add(font_name, font_size, dpi->val);
							}

							if(hfont)
							{
								image->destroy();
								image->import = ImageImport::Draw;
								image->draw.type = image->draw.DT_GLYPH;
								image->draw.glyph.code[0] = g[0];
								image->draw.glyph.code[1] = g[1];
								image->draw.glyph.color[0] = clr[0];
								image->draw.glyph.color[1] = clr[1];
								image->draw.glyph.size = { font_size, font_size };
								image->draw.glyph.font = hfont;
								ret = true;
							}
						}
						return ret;
					}
					else
					{
						auto& args = *obj.get_pointer<std::tuple<uint32_t, uint32_t, int,
							Object, Object, Object, Object, Object>>();
						
						auto func_front = std::get<0>(args);
						Hash func = std::get<1>(args);
						auto argc = std::get<2>(args);

						auto &params = *obj.get_pointer<std::tuple<uint32_t, uint32_t, int, 
							MUID*, Object, Object, Object>>();
						auto func_0 = std::get<0>(params);
						// image(path) image from path
						// image(path, index|resid) for icon
						// image(path, resid, restype)	for any
						// image(glyph, color, font, )

						// image.glyph(glyph, color, font)
						// image.icon(path, index|resid)
						// image.res(path, resid, restype)
						if(func_0 == IDENT_ID)
						{
							//auto func_1 = std::get<1>(params);
							//auto func_2 = std::get<2>(params);
							auto idmui = std::get<3>(params);

							image->destroy();
							image->import = ImageImport::Draw;

							Color color_[2];

							if(theme->mode != 2)
							{
								if(theme->mode == 1)
								{
									color_[0] = Color::Swap(0xFFE0DFDF);
									color_[1] = Color::Swap(0xFF4CC2FF);
								}
								else
								{
									color_[0] = Color::Swap(0xFF555555);
									color_[1] = Color::Swap(0xFF0078D4);
								}
							}

							if(theme->image.color[0])
								color_[0] = this->theme->image.color[0];

							if(theme->image.color[1])
								color_[1] = this->theme->image.color[1];

							auto sv = [](Object o, Color &clr)->uint32_t {
								if(o.is_number())
								{
									auto v = o.to_color();
									if(v) clr = v;
								}
								return clr;
							};

							sv(std::get<4>(params).move(), color_[0]);
							sv(std::get<5>(params).move(), color_[1]);
							/*
							auto sv = [](uint32_t val)->uint32_t {
								if(val != CLR_INVALID && val != IDENT_DEFAULT)
									return val;
								return CLR_INVALID;
							};

							color_[0] = sv(idui->image_color[0]);
							color_[1] = sv(idui->image_color[1]);
							*/

							image->draw.type = image->draw.DT_GLYPH;
							image->draw.glyph.font = hfont;
							image->draw.glyph.size = { image_size, image_size };

							image->draw.glyph.code[0] = idmui->image_glyph[0];
							image->draw.glyph.code[1] = idmui->image_glyph[1];

							image->draw.glyph.color[0] = color_[0];
							image->draw.glyph.color[1] = color_[1];

							return true;
						}
						else if(func_front == IDENT_IMAGE || func_front == IDENT_ICON)
						{
							if(func.equals({ IDENT_SVG, IDENT_SVGF }))
							{
								string value = std::get<3>(args).to_string().move();
								if(!value.empty())
								{
									//std::string s = "<svg fill='none' viewBox='0 0 16 16'><path fill='#E0DFDF' fill-rule='evenodd' d='M1 3a2 2 0 012-2h2a2 2 0 012 2v2a2 2 0 01-2 2H3a2 2 0 01-2-2V3zm2-1a1 1 0 00-1 1v2a1 1 0 001 1h2a1 1 0 001-1V3a1 1 0 00-1-1H3zM9 3a2 2 0 012-2h2a2 2 0 012 2v2a2 2 0 01-2 2h-2a2 2 0 01-2-2V3zm2-1a1 1 0 00-1 1v2a1 1 0 001 1h2a1 1 0 001-1V3a1 1 0 00-1-1h-2zM1 11a2 2 0 012-2h2a2 2 0 012 2v2a2 2 0 01-2 2H3a2 2 0 01-2-2v-2zm2-1a1 1 0 00-1 1v2a1 1 0 001 1h2a1 1 0 001-1v-2a1 1 0 00-1-1H3zM9 11a2 2 0 012-2h2a2 2 0 012 2v2a2 2 0 01-2 2h-2a2 2 0 01-2-2v-2zm2-1a1 1 0 00-1 1v2a1 1 0 001 1h2a1 1 0 001-1v-2a1 1 0 00-1-1h-2z' clip-rule='evenodd'/></svg>";
									std::string vutf8 = std::move(string::ToUTF8(value, value.length<int>()));
									PlutoVG plutovg;
									if(func.equals(IDENT_SVG))
										plutovg.load_from_memory(vutf8.c_str(), (int)vutf8.size(), width, height);
									else
										plutovg.load_from_file(vutf8.c_str(), width, height);

									if(plutovg)
									{
										//plutovg.rgba0();
										image->hbitmap = plutovg.tobitmap();
										image->inherited = false;
										image->import = ImageImport::SVG;
										image->size = { image_size, image_size };
										return true;
									}
								}
							}
							// char|string, color, size, font
							else if(func.equals(IDENT_GLYPH))
							{
								Object a1 = std::get<3>(args).move();
								if(!a1.is_null())
								{
									if(a1.is_number())
										glyph = (wchar_t)a1.to_number<uint16_t>();
									else
										glyph = a1.to_string().trim()[0];

									Color clr;

									if(argc > 1)
									{
										if(argc > 1)
										{
											Object a2 = std::get<4>(args).move();

											if(a2.is_string())
												font_name = a2.to_string().move();
											else if(a2.is_number())
											{
												if(a2.is_color())
													clr = a2.to_color();
												else
													font_size = a2;
											}
										}

										if(argc > 2)
										{
											Object a3 = std::get<5>(args).move();

											if(a3.is_string())
												font_name = a3.to_string().move();
											else if(a3.is_number())
											{
												if(a3.is_color())
													clr = a3.to_color();
												else
													font_size = a3;
											}
										}

										if(argc > 3)
										{
											Object a4 = std::get<6>(args).move();

											if(a4.is_string())
												font_name = a4.to_string().move();
											else if(a4.is_number())
											{
												if(a4.is_color())
													clr = a4.to_color();
												else
													font_size = a4.to_number();
											}
										}

										if(clr) color = clr;

										if(font_size < 2)
											font_size = 2;
									}

									if(glyph)
									{
										if(!font_name.trim().empty())
										{
											hfont = cache->fonts.add(font_name, font_size, dpi->val);
										}
										else if(font_size != image_size)
										{
											hfont = cache->fonts.add(FontCache::Default, font_size, dpi->val);
										}

										if(hfont)
										{
											image->destroy();
											image->import = ImageImport::Draw;
											image->draw.type = image->draw.DT_GLYPH;
											image->draw.glyph.code[0] = glyph;
											image->draw.glyph.color[0] = color;
											image->draw.glyph.size = { font_size, font_size };
											image->draw.glyph.font = hfont;
											return true;
										}
									}
								}
							}
							// char|string, color, size
							else if(func.equals({ IDENT_ICONS_SEGOE, IDENT_ICON_FLUENT, IDENT_ICON_MDL }))
							{
								Object a1 = std::get<3>(args).move();
								if(!a1.is_null())
								{
									if(a1.is_pointer())
									{
										if(!a1.is_array(true))
											return false;

										//image=[glyph, [light, dark], [font, size], [16,16]]
										//image=[glyph, color, size, font name]
										//image = [[glyph, color],[glyph, color], size, font name]
										//image.fluent([[glyph, color],[glyph, color]], size)

										auto parse_glyph = [](Object const *obj, wchar_t *glyph, Color *color)->bool
										{
											if(!obj || !obj->is_array(true))
												return false;

											auto ptr = obj->get_pointer();
											auto argc = ptr[0].to_number<int>();

											if(argc == 0 || ptr[1].is_null())
												return false;

											if(ptr[1].is_number())
												*glyph = (wchar_t)ptr[1].to_number<uint16_t>();
											else if(ptr[1].is_string())
												*glyph = ptr[1].to_string().trim()[0];

											if(argc > 1 && ptr[2].not_default())
												*color = ptr[2].to_color();

											return true;
										};

										//[\uxxxx, #xxx, size, name]
										auto parse_arg = [](Object const *ptr, int index, Color *color, int *size, string *name)
										{
											//if(!ptr || !ptr->is_pointer())
											//	return;

											auto argc = ptr[0].to_number<int>();
											if(index > argc)
												return;

											auto obj = &ptr[index];

											if(obj->is_string())
												*name = obj->to_string().move();
											else if(obj->not_default())
											{
												if(obj->is_color())
													*color = obj->to_color();
												else
													*size = obj->to_number();
											}
										};

										if(argc > 1)
										{
											Object a2 = std::get<4>(args).move();
											if(a2.not_default())
												font_size = a2.to_number<int>();
										}

										auto ptr = a1.get_pointer();
										auto _argc = ptr[0].to_number<int>();

										if(_argc == 0 || ptr[1].is_null())
											return false;

										wchar_t g[2] = {};
										Color clr[2];

										int i = 2;

										//[[],[],size,name]
										if(ptr[1].is_pointer())
										{
											if(!parse_glyph(&ptr[1], &g[0], &clr[0]))
												return false;
											i += (_argc > 1 && parse_glyph(&ptr[2], &g[1], &clr[1]));
										}
										else
										{
											if(ptr[1].is_number())
												g[0] = (wchar_t)ptr[1].to_number<uint16_t>();
											else
												g[0] = ptr[1].to_string().trim()[0];

											if(g[0] == 0)
												return false;
										}

										if(_argc > 1 && _argc >= i)
										{
											parse_arg(ptr, i++, &clr[0], &font_size, &font_name);
											parse_arg(ptr, i++, &clr[0], &font_size, &font_name);
											parse_arg(ptr, i, &clr[0], &font_size, &font_name);
										}

										if(font_size < 2)
											font_size = 2;

										if(font_size > image_size)
											font_size = image_size;

										if(g[0] || g[1])
										{
											if(func.equals(IDENT_ICONS_SEGOE))
											{
												hfont = cache->fonts.add(FontCache::SegoeFluentIcons, font_size, dpi->val);
												if(!hfont)
													hfont = cache->fonts.add(FontCache::SegoeMDL2Assets, font_size, dpi->val);
											}
											else
											{
												font_name = func == IDENT_ICON_FLUENT ? FontCache::SegoeFluentIcons : FontCache::SegoeMDL2Assets;
												hfont = cache->fonts.add(font_name, font_size, dpi->val);
											}

											if(hfont)
											{
												image->destroy();
												image->import = ImageImport::Draw;
												image->draw.type = image->draw.DT_GLYPH;
												image->draw.glyph.code[0] = g[0];
												image->draw.glyph.code[1] = g[1];
												image->draw.glyph.color[0] = clr[0];
												image->draw.glyph.color[1] = clr[1];
												image->draw.glyph.size = { font_size, font_size };
												image->draw.glyph.font = hfont;
												return true;
											}
										}
										return false;
									}

									if(a1.is_number())
										glyph = (wchar_t)a1.to_number<uint16_t>();
									else
										glyph = a1.to_string().trim()[0];

									Color clr;

									if(argc > 1)
									{
										if(argc > 1)
										{
											Object a2 = std::get<4>(args).move();

											if(a2.is_color())
												clr = a2.to_color();
											else
												font_size = a2.to_number();
										}

										if(argc > 2)
										{
											Object a3 = std::get<5>(args).move();

											if(a3.is_color())
												clr = a3.to_color();
											else
												font_size = a3.to_number();
										}

										if(clr)
											color = clr;

										if(font_size < 2)
											font_size = 2;
									}

									if(glyph)
									{
										font_size = dpi->value(font_size);

										if(func.equals(IDENT_ICONS_SEGOE))
										{
											hfont = cache->fonts.add(FontCache::SegoeFluentIcons, font_size, dpi->val);
											if(!hfont)
												hfont = cache->fonts.add(FontCache::SegoeMDL2Assets, font_size, dpi->val);
										}
										else
										{
											font_name = func == IDENT_ICON_FLUENT ? FontCache::SegoeFluentIcons : FontCache::SegoeMDL2Assets;
											hfont = cache->fonts.add(font_name, font_size, dpi->val);
										}

										if(hfont)
										{
											image->destroy();
											image->import = ImageImport::Draw;
											image->draw.type = image->draw.DT_GLYPH;
											image->draw.glyph.code[0] = glyph;
											image->draw.glyph.color[0] = color;
											image->draw.glyph.size = { font_size, font_size };
											image->draw.glyph.font = hfont;
											return true;
										}
									}
								}
							}
							else if(func.equals({ IDENT_DEFAULT, IDENT_AUTO }))
							{
								image->destroy();

								if(!this->theme->glyph.enabled)
									return false;

								if(argc > 0)
								{
									image->import = ImageImport::Draw;

									image->draw.type = image->draw.DT_GLYPH;
									image->draw.glyph.size = { image_size, image_size };
									image->draw.glyph.font = hfont;
									image->draw.glyph.code[0] = std::get<3>(args).to_string()[0];

									if(argc >= 2)
									{
										auto clr = std::get<4>(args).to_color();
										if(clr)
											image->draw.glyph.color[0] = clr;
									}

									/*if(argc == 3)
									{
										auto size = std::get<5>(args).to_number<int>();
										if(size > 0 && size < 32)
											image->draw.glyph.size = { size, size };
									}*/
									return true;
								}
								else if(this->Selections->Count() > 0)
								{
									if(auto p = this->Selections->Get(0); p)
									{
										string sztype;
										string target = p->Extension;
										bool is_file = false;
										
										if(p->IsLink)
										{
											if(Path::GetLinkInfo(p->get(), &target, nullptr) && !target.empty())
											{
												is_file = Path::IsFile(::GetFileAttributesW(target.data()));
												target = Path::Extension(target);
											}
										}
										else if(p->IsFile())
										{
											is_file = true;
										}
									
										if(is_file)
										{
											if(target.equals({ L".exe", L".cmd", L".bat", L".com", L".scr" }))
											//if(target.equals({L".exe", L".cmd", L".bat", L".com", L".scr"}))
												image->hbitmap = Image::FromHICON(L"shell32.dll", 2, image_size);
											else
												sztype = target.move();
										}
										else 
										{
											//sztype = L"FOLDER";
											image->hbitmap = Image::FromHICON(L"explorer.exe", 0, image_size);
										}

										if(!sztype.empty())
										{
											wchar_t pszPath[MAX_PATH]{};
											DWORD cbBufSize = sizeof(pszPath);

											TResult hr = ::AssocQueryStringW(ASSOCF_INIT_IGNOREUNKNOWN, ASSOCSTR_EXECUTABLE, sztype, nullptr, pszPath, &cbBufSize);
											if(hr.failed())
												hr = ::AssocQueryStringW(ASSOCF_INIT_IGNOREUNKNOWN, ASSOCSTR_APPICONREFERENCE, sztype, nullptr, pszPath, &cbBufSize);
											if(hr.succeeded())
												image->hbitmap = Image::From(pszPath, image_size);
										}

										if(image->hbitmap)
										{
											image->import = ImageImport::Image;
											image->size = { image_size, image_size };
											return true;
										}

										return false;
										/*
										auto set_glyph = [=](auto ch) -> bool
										{
											image->import = ImageImport::Draw;
											image->draw.type = image->draw.DT_GLYPH;
											image->draw.glyph.size = { image_size, image_size };
											image->draw.glyph.font = hfont;
											image->draw.glyph.code[0] = ch;
											return true;
										};

										//SHQueryString;
										if(p->Type == PathType::Directory || p->Group == PathType::Drive)
										{
											return set_glyph(L'\uE0D0');
										}
										else if(p->IsFile())
										{
											Hash hash = p->Extension.hash();
											if(hash == IDENT_EXT_EXE)
											{
												return set_glyph(L'\uE067');
											}
											else if(hash.equals({ IDENT_EXT_PNG,IDENT_EXT_JPG,IDENT_EXT_JPEG,
																IDENT_EXT_BMP,IDENT_EXT_GIF,IDENT_EXT_TTIF,
																IDENT_EXT_WEBP,IDENT_EXT_SVG,IDENT_EXT_ICO }))
											{
												return set_glyph(L'\uE150');
											}
											else if(hash.equals({ IDENT_EXT_MKV, IDENT_EXT_AVI,IDENT_EXT_MP4,IDENT_EXT_WMV,
																IDENT_EXT_MPG,IDENT_EXT_FLV,IDENT_EXT_WMA,IDENT_EXT_MP3,IDENT_EXT_WAV }))
											{
												return set_glyph(L'\uE154');
											}
											else if(hash.equals({ IDENT_EXT_ZIP,IDENT_EXT_CAB,IDENT_EXT_7Z,IDENT_EXT_TAR,
																IDENT_EXT_RAR,IDENT_EXT_ISO,IDENT_EXT_IMG,IDENT_EXT_WIM }))
											{
												return set_glyph(L'\uE0AA');
											}
											else if(hash.equals({ IDENT_EXT_TXT,IDENT_EXT_INI,IDENT_EXT_INF,
																IDENT_EXT_H,IDENT_EXT_C,IDENT_EXT_CPP,
																IDENT_EXT_CS,IDENT_EXT_HTML,IDENT_EXT_CSHTML,
																IDENT_EXT_JS,IDENT_EXT_XML,IDENT_EXT_RAZOR,
																IDENT_EXT_JSON,IDENT_EXT_CSS,IDENT_EXT_BAT,
																IDENT_EXT_CMD,IDENT_EXT_PS1 }))
											{
												//\uE113
												return set_glyph(L'\uE10A');
											}
											else 
											{
												image->hbitmap = Image::FromFileExtension(p->Path, image_size);
												if(image->hbitmap)
												{
													image->import = ImageImport::Image;
													image->size = { image_size, image_size };
												}
											}
											return true;
										}
										else if(p->IsNamespace())
										{
											return set_glyph(L'\uE0D2');
										}
										else 
										{
											image->hbitmap = Image::FromFileExtension(p->Path, image_size);
											if(image->hbitmap)
											{
												image->import = ImageImport::Image;
												image->size = { image_size, image_size };
												return true;
											}
										}*/
									}
								}
								return false;
							}
							else if(func == 0 || func.equals(IDENT_RES))
							{
								icon_path = std::get<3>(args).to_string().move();
								if(argc > 1)
									icon_id = std::get<4>(args).to_number();

								if(!icon_path.empty())
								{
									if(func.equals(IDENT_ICON) || argc > 1)
										image->hbitmap = Image::FromHICON(icon_path.c_str(), icon_id, image_size);
									else
										image->hbitmap = Image::From(icon_path.c_str(), image_size);
									
									if(image->hbitmap)
									{
										image->import = ImageImport::Image;
										image->size = { image_size, image_size };
										return true;
									}
								}
							}
							else
							{
								image->destroy();

								/*if(func.value() == IDENT_ID_SHIELD)
								{
									SHSTOCKICONINFO ssii {};
									ssii.cbSize = sizeof(ssii);
									if(S_OK == ::SHGetStockIconInfo(SIID_SHIELD, SHGSI_ICON | (image_size <= 16 ? SHGSI_SMALLICON : SHGFI_LARGEICON), &ssii))
									{
										image->hbitmap = Image::FromHICON(ssii.hIcon, image_size);
										image->import = ImageImport::Image;
										return true;
									}
									return false;
								}*/

								auto muid = Initializer::get_muid(func.value());
								if(!muid)
									return false;

								Color color_[2];
								long size = image_size;

								//if(argc == 0)
								{
									if(theme->mode == 2)
									{
									}
									else
									{
										if(theme->mode == 1)
										{
											color_[0] = Color::Swap(0xFFE0DFDF);
											color_[1] = Color::Swap(0xFF4CC2FF);
										}
										else
										{
											color_[0] = Color::Swap(0xFF555555);
											color_[1] = Color::Swap(0xFF0078D4);
										}
									}
								}

								if(theme->image.color[0])
									color_[0] = this->theme->image.color[0];

								if(theme->image.color[1])
									color_[1] = this->theme->image.color[1];

								Color val;
								if(argc >= 1)
								{
									val = std::get<3>(args).to_color();
									if(val)
									{
										color_[0] = val;
									}
								}

								if(argc >= 2)
								{
									val = std::get<4>(args).to_color();
									if(val)
										color_[1] = val;
								}

								if(argc == 3)
									size = std::get<5>(args).to_number();

								if(size > image_size || size <= 0)
									size = image_size;

								image->import = ImageImport::Draw;

								if(size != image_size)
									hfont = cache->fonts.add(FontCache::Default, font_size, dpi->val);
								
								// name,size,dpi
								image->draw.type = image->draw.DT_GLYPH;
								image->draw.glyph.font = hfont;
								image->draw.glyph.size = { size, size };

								image->draw.glyph.code[0] = muid->image_glyph[0];
								image->draw.glyph.code[1] = muid->image_glyph[1];

								image->draw.glyph.color[0] = color_[0];
								image->draw.glyph.color[1] = color_[1];
								
								return true;
							}
						}
					}
				}
				else if(obj.is_color())
				{
					color = obj.to_color();
					if(color)
					{
						image->destroy();
						//bitmap = Image::DrawRectangle(color, width, height, true);
						image->import = ImageImport::Draw;
						image->draw.type = image->draw.DT_SHAPE;
						image->draw.shape.color[0] = color;
						image->draw.shape.size = { width, height };
						image->draw.shape.solid = true;
						return true;
					}
				}
				else
				{
					if(obj.is_number())
					{
						uintptr_t n = obj;
						switch(n)
						{
						case 0:
						case IDENT_NULL:
						case IDENT_NONE:
						case IDENT_DISABLED:
							image->import = ImageImport::Disabled;
							return false;
						case IDENT_INHERIT:
						case IDENT_PARENT:
							image->import = ImageImport::Inherit;
							return true;
						}
						glyph = static_cast<wchar_t>(n);
					}
					else if(obj.is_string() && obj.length() == 1)
					{
						glyph = obj.to_string().trim()[0];
					}

					if(hfont && glyph)
					{
						image->destroy();
						image->import = ImageImport::Draw;
						image->draw.type = image->draw.DT_GLYPH;
						image->draw.glyph.code[0] = glyph;
						image->draw.glyph.color[0] = color;
						image->draw.glyph.size = { font_size, font_size };
						image->draw.glyph.font = hfont;
						return true;
					}
				}
			}
			catch(...)
			{
			}
			return false;
		}
	}
}