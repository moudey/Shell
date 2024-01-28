#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		struct DPI
		{
			uint32_t org = 96;
			uint32_t val = 96;

			//scale factor
			double scale() const { return double(val) / double(org); }

			bool scaled() const { return val != org; }

			template<typename T = long>
			T operator()(auto val) const { return value<T>(val); }

			template<typename T = long>
			T value(auto val) const { return static_cast<T>(std::rint(val * double(this->val) / 96.0)); }

			template<typename T = long>
			T valuexx(auto val) const { return static_cast<T>(val * double(this->val) / 96.0); }

			template<typename T = long>
			T original(auto value) const { return static_cast<T>(std::floor(value * double(org) / 96.0)); }

			long value2(auto value) const
			{
				return static_cast<long>(std::rint(value * double(this->val) / double(org)));
			}
			/*template<typename T = long>
			float dpif(T value) const { return float((value * _dpi) / 96.0); }
			double dpi() const { return double(_dpi) / 96.0; }
			*/

			long  scaleFactor() const { return long((val / 96.0) * 100.0); }

			template<typename T = long>
			static T Value(auto val, double dpi = 96.0) { return static_cast<T>(std::rint(val * dpi / 96.0)); }
		};

		//typedef IMMERSIVE_COLOR_NAMED_TYPES *(WINAPI *pGetImmersiveColorNamedTypeByIndex)(unsigned dwImmersiveColorType); //#100
		enum class Immersive_Color : uint32_t
		{
			SystemAccentLight3 = 1,
			SystemAccentLight2,
			SystemAccentLight1,
			SystemAccent,
			SystemAccentDark1,
			SystemAccentDark2,
			SystemAccentDark3,
		};

		class ImmersiveColor
		{
			inline static HMODULE hUxTheme = ::GetModuleHandleW(L"uxtheme.dll");
		public:
			static uint32_t GetColorByColorType(uint32_t colorType);
			static uint32_t GetColorByName(const wchar_t *name);
			static uint32_t GetImmersiveUserColorSetPreference(bool bForceCheckRegistry, bool bSkipCheckOnFail);
			static uint32_t GetImmersiveColorFromColorSetEx(uint32_t dwImmersiveColorSet, uint32_t dwImmersiveColorType, bool bIgnoreHighContrast, uint32_t dwHighContrastCacheMode);
			static uint32_t GetImmersiveColorTypeFromName(const wchar_t *name);
			static uint32_t GetImmersiveColorSetCount();
			static bool IsSupported();
		};

		struct VIEWMODE
		{
			static const int8_t AUTO = -1;
			static const int8_t COMPACT = 0;
			static const int8_t SMALL = 2;
			static const int8_t MEDIUM = 4;
			static const int8_t LARGE = 8;
			static const int8_t WIDE = 10;

			static auto get(int8_t view)
			{
				switch(view)
				{
				case AUTO:		return -1;
				case COMPACT:	return 0;
				case SMALL:		return 2;
				case MEDIUM:	return 4;
				case LARGE:		return 8;
				case WIDE:		return 10;
				default:		return view < -1 ? -1 : view > 48 ? 48 : view;
				}
			}
		};

		enum class ThemeType:uint8_t
		{
			Auto,
			System,
			HighContrast,
			White,
			Black,
			Classic,
			Edge,
			Dark,
			Light,
			Modern,
			Custom
		};

		struct Theme
		{
			inline static auto ver = Windows::Version::Instance();
			static const auto RADIUS = 8;

			struct color_t
			{
				Color text;
				Color back;
			};

			struct state_t
			{
				Color nor;
				Color sel;
				Color nor_dis;
				Color sel_dis;
			};

			struct gradientstop_t
			{
				double offset{};
				Color color;
			};

			struct {
				bool enabled{};
				double linear[5]{};
				double radial[6]{};
				std::vector<gradientstop_t> stpos;
			}gradient;

			struct {
				Color color;
				uint8_t opacity = 0xFF;
				int8_t effect = 0;
				Color tintcolor;
				string image;
			} background;

			struct
			{
				Color color;
				uint8_t size{};
				uint8_t radius{};
				Margin padding;
			}border;

			struct {
				Color color = 0xFF000000;
				uint8_t size = 3;
				uint8_t offset = 2;
				bool enabled = true;
			} shadow;

			struct
			{
				state_t color;
				long size{};
				int8_t tap = -1;
				int8_t alig = -1;
				uint32_t prefix = 0xFFFFFFFF;
				string name;
			} text;

			struct
			{
				state_t color;
				uint8_t opacity = 0xFF;
				state_t border;
				uint8_t radius{};
				Margin padding;
				Margin margin;
				long height() const { return padding.height() + margin.height(); }
				long width() const { return padding.width() + margin.width(); }
			} back;

			struct
			{
				Color color;
				uint8_t size{};
				Margin margin;
			} separator;

			struct
			{
				state_t chevron;
				state_t checked;
				state_t bullet;
			}symbols;

			struct {
				uint32_t min_width = 0u;
				uint32_t max_width = 0u;
				int8_t rtl = 0;
				struct {
					int8_t align = 0x7F;
				}popup;
			}layout;

			struct {
				string font;
				bool enabled = true;
			} glyph;

			struct {
				Color color[3];
				uint32_t size = 16;
				uint8_t gap{};
				bool scale = true;
				bool enabled = true;
				int display{};
			}image;

			struct
			{
				uint8_t radius{};
				uint8_t opacity = 0xFF;
				uint16_t time = 1000;
				uint16_t maxwidth = 2000;
				Margin padding;
				Color border;
				color_t normal;
				color_t primary;
				color_t success;
				color_t info;
				color_t warning;
				color_t danger;
			}
			tip;

			DPI *dpi{};

			ThemeType Type = ThemeType::Auto;
			uint8_t mode{}; //0 = light, 1 = dark, 2= high contrast
			bool transparent = true;
			//int view = -1;
			int view2 = 0;
			LOGFONTW font{};

			bool enableTransparency{};
			bool systemUsesLightTheme = true;
			bool appsUseLightTheme = true;
			bool isHighContrast{};

			struct {
				uint8_t mode{};
				bool transparency{};
			}system;

			static const Color& get_color(const state_t &state, bool selected = false, bool disabled = false)
			{
				if(selected)
					return  disabled ? state.sel_dis : state.sel;
				return disabled ? state.nor_dis : state.nor;
			}

			void set_symbols_as_text(bool mode) 
			{
				symbols.checked = { text.color.nor, text.color.sel, text.color.nor_dis, text.color.sel_dis };
				symbols.bullet = { text.color.nor, text.color.sel, text.color.nor_dis, text.color.sel_dis };
				symbols.chevron = { text.color.nor, text.color.sel, text.color.nor_dis, text.color.sel_dis };

				tip.normal = { text.color.nor,  background.color };
				tip.normal.back.a = 0xff;

				if(mode)
				{
					//tip.danger = { 0xFF000000, 0xFFDDDDFF };
					//tip.warning = { 0xFF000000, 0xFFAAFFFF };
					//tip.info = { 0xFF000000, 0xFFFFFFDD };
					tip.primary = { 0xFFFFFFFF, 0xFF713C26 };
					tip.info = { 0xFFFFFFFF, 0xFF572C39 };
					tip.success = { 0xFFFFFFFF, 0xFF194B28 };
					tip.warning = { 0xFFFFFFFF, 0xFF1B4E61 };
					tip.danger = { 0xFFFFFFFF, 0xFF051053 };
				}
				else 
				{
					tip.primary = { 0xFF000000, 0xFFF7E9DE };
					tip.info = { 0xFF000000, 0xFFFCD8E9 };
					tip.success = { 0xFF000000, 0xFFDEF6E6 };
					tip.warning = { 0xFF000000, 0xFFD0F6FB };
					tip.danger = { 0xFF000000, 0xFFE9E8F6 };
				}
			}

			void scale()
			{
				if(!dpi)
					return;

				auto ev = [&](Margin &m)
				{
					m.left = dpi->value(m.left);
					m.top = dpi->value(m.top);
					m.right = dpi->value(m.right);
					m.bottom = dpi->value(m.bottom);
				};

				text.size = dpi->value<uint8_t>(text.size);
				
				if(text.tap > 0)
					text.tap = dpi->value<int8_t>(text.tap);

				back.radius = dpi->value<uint8_t>(back.radius);
				ev(back.margin);
				ev(back.padding);

				border.size = dpi->value<uint8_t>(border.size);
				border.radius = dpi->value<uint8_t>(border.radius);
				ev(border.padding);

				separator.size = dpi->value<uint8_t>(separator.size);
				ev(separator.margin);

				shadow.size = dpi->value<uint8_t>(shadow.size);
				shadow.offset = dpi->value<uint8_t>(shadow.offset);

				image.size = dpi->value<uint32_t>(image.size);
				image.gap = dpi->value<uint8_t>(image.gap);

				if(layout.popup.align != 0x7F)
					layout.popup.align = dpi->value<int8_t>(layout.popup.align);

				if(layout.min_width > 0)
					layout.min_width = dpi->value<long>(layout.min_width);
				
				if(layout.max_width > 0)
					layout.max_width = dpi->value<long>(layout.max_width);
			}

			static uint8_t radius(uint8_t value) 
			{ 
				switch(value)
				{
					case 0: return 0;
					case 1: return 4;
					case 2: return 8;
					case 3: return 12;
					case 4: return 16;
				}
				return 4; 
			}

			static uint8_t opacity(uint8_t percent) { return (percent * 255) / 100; }

			//template<typename T=long>
			//T dpi(auto value) const { return DPI<T>(value, _dpi); }

			static auto pow(auto v) { return v + v; }

			static Theme Default(auto type = ThemeType::System, uint8_t mode = 0)
			{
				Theme th;
				th.mode = mode;
				th.Type = type;
				th.background.color.from(::GetSysColor(COLOR_MENU), 100);
				th.border.color.from(::GetSysColor(COLOR_WINDOWFRAME), 50);
				th.border.radius = 0;
				th.border.size = 1;
				th.border.padding = { 0, 4, 0, 4 };
				th.shadow.color.opacity(25);

				th.separator.color = th.border.color;
				th.separator.color.opacity(100);
				th.separator.size = 1;
				th.separator.margin = { 8, 3, 8, 3 };

				th.text.size = 12;
				th.text.tap = 12;

				th.text.color = { 
					Color(::GetSysColor(COLOR_MENUTEXT), 0xFF),
					Color(::GetSysColor(COLOR_MENUTEXT), 0xFF),
					Color(::GetSysColor(COLOR_GRAYTEXT), 0xFF),
					Color(::GetSysColor(COLOR_GRAYTEXT), 0xFF)
				};

				th.back.color = { 
					th.background.color,
					Color(::GetSysColor(COLOR_HIGHLIGHT), 0xFF),
					th.background.color,
					th.background.color
				};

				//th.back.border = th.back.color.sel;
				th.back.padding = { 10, 3, 10, 3 };
				th.back.margin = { 4, 0, 4, 0 };
				th.back.radius = 0;

				th.set_symbols_as_text(0);

				th.image.size = 16;
				th.image.gap = 10;

				if(mode == 1)
				{
					th.image.color[0] = Color::Swap(0xFFE0DFDF);
					th.image.color[1] = Color::Swap(0xFF4CC2FF);
					th.image.color[2] = Color::Swap(0x00000000);
				}
				else
				{
					th.image.color[0] = Color::Swap(0xFF555555);
					th.image.color[1] = Color::Swap(0xFF0078D4);
					th.image.color[2] = Color::Swap(0xFFFAFAFA);
				}

				th.tip.padding = {10, 10, 10, 10};
				th.tip.maxwidth = 640;
				th.tip.opacity = 0xff;
				th.tip.time = 1000;
				th.tip.normal = { th.text.color.nor, th.background.color };
				th.tip.normal.back.a = 0xff;

				//th.tip.error = { 0xFF000000, Color::Swap(0xFFDDDDFF) };
				//th.tip.warning = { 0xFF000000, Color::Swap(0xFFAAFFFF) };
				//th.tip.info = { 0xFF000000, Color::Swap(0xFFFFFFDD) };

				return th;
			}

			static auto HighContrast()
			{
				auto th = Default(ThemeType::HighContrast, 2);
				
				th.background.color = Color(::GetSysColor(COLOR_WINDOW), 0xFF);
				if(Windows::Version::Instance().IsWindows11OrGreater())
				{
					th.border.radius = 8;
					th.back.margin = { 4, 4, 4, 4 };
					th.back.radius = 4;
					th.tip.radius = 4;
				}

				th.shadow.color.opacity(25);
				th.separator.margin = { 0, 3, 0, 3 };

				th.text.size = 12;
				//COLOR_BTNTEXT
				th.text.color = { 
					Color(::GetSysColor(COLOR_WINDOWTEXT), 0xFF),
					Color(::GetSysColor(COLOR_HIGHLIGHTTEXT), 0xFF),
					Color(::GetSysColor(COLOR_GRAYTEXT), 0xFF),
					Color(::GetSysColor(COLOR_GRAYTEXT), 0xFF)
				};
				
				th.back.color = { 
					th.background.color,
					Color(::GetSysColor(COLOR_HIGHLIGHT), 0xFF),
					th.background.color,
					th.background.color,
				};

			//	th.back.border = th.back.color.sel;
			//	th.block.margin = { 0, 4 };

				th.set_symbols_as_text(false);
				
				return th;
			}

			static auto System()
			{
				auto th= Default(ThemeType::System, 0);
				th.set_symbols_as_text(0);

				return th;
			}

			static auto Modern(auto type = ThemeType::Light, uint8_t mode = 0, uint8_t enableTransparency = 0)
			{
				auto th = Default(type, mode);

				th.border.radius = 8;
				th.border.size = 0;
				th.border.padding = { 0, 4, 0, 4 };
				th.separator.size = 1;
				th.separator.margin = { 0, 4, 0, 4 };
				th.text.size = 14;
				th.back.padding = { 10, 4, 10, 4 };
				th.back.margin = { 4, 0, 4, 0 };
				th.back.radius = 4;
				th.font.lfHeight = 14;
				
				th.tip.padding = { 10,10,10,10 };
				th.tip.radius = 4;

				th.border.color.opacity(10);

				th.shadow.size = 3;
				th.shadow.offset = 2;
				th.shadow.color = 0x80000000;

				if(mode)
				{
					th.shadow.color.a = 0x20;
					th.border.color = 0xFF2C2C2C;
					th.background.color = 0xFF2C2C2C;
					//th.text.color = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF6D6D6D, 0xFF6D6D6D };
					th.text.color = { 0xFFFFFFFF, 0xFFFFFFFF, 0x80FFFFFF, 0x80FFFFFF };

					if(enableTransparency)
					{
						th.border.size = 1;
						th.border.color = th.background.color;
						th.border.color.a = 0x40;
						th.background.color.a = 0x00;
						th.background.tintcolor = th.background.color;
						th.background.tintcolor.a = 0x40;
						th.back.color = { 0x00000000, 0x10FFFFFF, 0x00000000, 0x05FFFFFF };
						th.separator.color = 0x15FFFFFF;
					}
					else
					{
						th.separator.color = 0xFF3D3D3D;
						th.back.color = { 0xFF2C2C2C, 0xFF383838, 0xFF2C2C2C, 0xFF2C2C2C };
					}
				}
				else
				{
					th.shadow.color.a = 0x10;
					th.border.color = 0xFFF9F9F9;
					th.separator.color = 0xFFEAEAEA;
					//th.text.color = { 0xFF000000, 0xFF000000, 0xFF9F9F9F, 0xFF9F9F9F };
					th.text.color = { 0xFF000000, 0xFF000000, 0x80000000, 0x80000000 };

					if(enableTransparency)
					{
						th.background.color.a = 0x00;
						th.background.tintcolor = th.background.color;
						th.background.tintcolor.a = 0xCF;
						th.back.color = { 0x00000000, 0x08000000, 0x00000000, 0x05000000 };
						th.separator.color = 0x15000000;
					}
					else
					{
						th.background.color = 0xFFF9F9F9;
						th.back.color = { 0xFFF9F9F9, 0xFFF0F0F0, 0xFFF9F9F9, 0xFFF9F9F9 };
					}
				}

				th.set_symbols_as_text(mode);
			//	th.back.border.nor = th.back.color.nor;
				//th.border.color.opacity(10);
				return th;
			}

			static auto Light(bool modern = true, uint8_t enableTransparency = 0)
			{
				auto th = Default(ThemeType::Light, 0);
				auto isw11 = Windows::Version::Instance().IsWindows11OrGreater();

				if(modern)
				{
					th = Modern(ThemeType::Light, 0, enableTransparency);
				}
				else if(isw11)
				{
					th.separator.color = 0xFFD7D7D7;
					th.border.color = 0xFFE5E5E5;
					th.background.color = 0xFFF9F9F9;
					th.text.color = { 0xFF000000, 0xFFFFFFFF, 0xFF9F9F9F, 0xFF9F9F9F };
					th.back.color = { 0xFFF9F9F9, Color::Swap(0xFF0078D4), 0xFFF9F9F9, 0xFFF0F0F0 };
					
					th.border.padding.left = 4;
					th.border.padding.right = 4;
					th.border.radius = th.radius(2);
					th.back.radius = th.radius(1);
					th.back.padding.top = 0;
					th.back.padding.bottom = 0;
					th.back.margin = { 0, 4, 0, 4 };
					th.shadow.size = 3;

					if(enableTransparency)
					{
						th.border.size = 1;
						th.border.color = th.background.color;
						th.border.color.a = 0x40;
						th.background.color.a = 0x00;
						th.background.tintcolor = th.background.color;
						th.background.tintcolor.a = 0x40;
						th.back.color.nor = 0x00000000;
						th.back.color.nor_dis = 0x00000000;
						th.back.color.sel.a = 0xaa;
						th.back.color.sel_dis.a = 0x80;
						th.separator.color = 0x15FFFFFF;
					}
				}
				else
				{
					if(enableTransparency)
					{
						th.background.color.a = 0x00;
						th.background.tintcolor = th.background.color;
						th.background.tintcolor.a = 0xCF;
						th.back.color = { 0x00000000, 0x08000000, 0x00000000, 0x05000000 };
						th.separator.color = 0x15000000;
					}
					else
					{
						th.background.color = 0xFFF9F9F9;
						th.back.color = { 0xFFF9F9F9, 0xFFF0F0F0, 0xFFF9F9F9, 0xFFF9F9F9 };
					}
				}

				th.shadow.color.a = 0x10;
				//th.back.border = th.back.color.nor;
				th.set_symbols_as_text(0);
				return th;
			}

			static auto Dark(bool modern = true, uint8_t enableTransparency = 0)
			{
				auto th = Default(ThemeType::Dark, 1);
				auto isw11 = Windows::Version::Instance().IsWindows11OrGreater();
				th.border.color.opacity(100);
				if(modern)
				{
					th = Modern(ThemeType::Dark, 1, enableTransparency);
					if(!enableTransparency)
					{
						th.border.color.opacity(10);
						th.background.color = 0xFF2C2C2C;
						th.separator.color = 0xFF3D3D3D;
						th.text.color = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF6D6D6D, 0xFF6D6D6D };
						th.back.color = { 0xFF2C2C2C, 0xFF383838, 0xFF2C2C2C, 0xFF2C2C2C };
					}
				}
				else if(isw11)
				{
					th.separator.color = 0xFF3E3E3E;
					th.border.color = 0xFF3E3E3E;
					th.background.color = 0xFF2B2B2B;
					th.text.color = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF6D6D6D, 0xFF6D6D6D };
					th.back.color = { 0xFF2B2B2B, Color::Swap(0xFF0078D4), 0xFF2B2B2B, 0xFF353535 };

					th.border.padding.left = 4;
					th.border.padding.right = 4;
					th.border.radius = th.radius(2);
					th.back.radius = th.radius(1);
					th.back.padding.top = 0;
					th.back.padding.bottom = 0;
					th.back.margin = { 0, 4, 0, 4 };
					th.shadow.size = 3;

					if(enableTransparency)
					{
						th.border.size = 1;
						th.border.color = th.background.color;
						th.border.color.a = 0x40;
						th.background.color.a = 0x00;
						th.background.tintcolor = th.background.color;
						th.background.tintcolor.a = 0x40;
						th.back.color.nor = 0x00000000;
						th.back.color.nor_dis = 0x00000000;
						th.back.color.sel.a = 0xaa;
						th.back.color.sel_dis.a = 0x80;
						th.separator.color = 0x15FFFFFF;
					}
				}
				else
				{
					th.separator.color = 0xFF808080;
					th.border.color = 0xFFA0A0A0;
					th.background.color = 0xFF2B2B2B;
					th.text.color = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF6D6D6D, 0xFF6D6D6D };
					th.back.color = { 0xFF2B2B2B, 0xFF414141, 0xFF2B2B2B, 0xFF2B2B2B };

					if(enableTransparency)
					{
						th.border.size = 1;
						th.border.color = th.background.color;
						th.border.color.a = 0x40;
						th.background.color.a = 0x00;
						th.background.tintcolor = th.background.color;
						th.background.tintcolor.a = 0x40;
						th.back.color = { 0x00000000, 0x10FFFFFF, 0x00000000, 0x05FFFFFF };
						th.separator.color = 0x15FFFFFF;
					}
				}

				th.shadow.color.opacity(25);
				//th.back.border = th.back.color.nor;
				th.set_symbols_as_text(1);

				return th;
			}

			static auto White(uint8_t enableTransparency = 0)
			{
				Theme th;
				if(Windows::Version::Instance().IsWindows11OrGreater())
				{
					th = Modern(ThemeType::White, 0);
					th.background.color = 0xFFFFFFFF;
					th.text.color = { 0xFF000000, 0xFF000000, 0xFF808080, 0xFF808080 };
					th.back.color = { 0xFFFFFFFF, 0xFFF0F0F0, 0xFFFFFFFF, 0xFFFFFFFF };
				}
				else 
				{
					th = Default(ThemeType::White, 0);
					th.border.color = 0xFFE3E3E3;
					th.background.color = 0xFFFFFFFF;
					th.text.color = { 0xFF000000, 0xFF000000, 0xFF808080, 0xFF808080 };
					th.back.color = { 0xFFFFFFFF, 0xFFF0F0F0, 0xFFFFFFFF, 0xFFFFFFFF };
				}

				if(enableTransparency)
				{
					th.border.size = 1;
					th.border.color = th.background.color;
					th.border.color.a = 0x40;
					th.background.color.a = 0x00;
					th.background.tintcolor = th.background.color;
					th.background.tintcolor.a = 0xff;
					th.back.color.nor = 0x00000000;
					th.back.color.nor_dis = 0x00000000;
					th.back.color.sel.a = 0xaa;
					th.back.color.sel_dis.a = 0x80;
					th.separator.color.a = 0;
				}
				else 
				{
					th.separator.color = th.background.color;
				}

				th.shadow.color.a = 0x10;
				//th.back.border = th.back.color.nor;
				
				th.separator.size = 3;
				th.set_symbols_as_text(0);
				return th;
			}

			static auto Black(uint8_t enableTransparency = 0)
			{
				Theme th;
				if(Windows::Version::Instance().IsWindows11OrGreater())
				{
					th = Modern(ThemeType::Black, 1);
					th.background.color = 0xFF000000;
					th.border.color = th.background.color;
					th.border.color = 0xFFFFFFFF;
					th.border.color.opacity(5);
					th.text.color = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF808080, 0xFF808080 };
					th.back.color = { 0xFF000000, 0xFF202020, 0xFF000000, 0xFF000000 };
				}
				else 
				{
					th = Default(ThemeType::Black, 1);
					th.border.color = 0xFF111111;
					th.background.color = 0xFF000000;
					th.separator.margin = { 8, 3, 8, 3 };
					th.text.color = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF808080, 0xFF808080 };
					th.back.color = { 0xFF000000, 0xFF202020, 0xFF000000, 0xFF000000 };
				}

				if(enableTransparency)
				{
					th.border.size = 1;
					th.border.color = th.background.color;
					th.border.color.a = 0x40;
					th.background.color.a = 0x00;
					th.background.tintcolor = th.background.color;
					th.background.tintcolor.a = 0xff;
					th.back.color.nor = 0x00000000;
					th.back.color.nor_dis = 0x00000000;
					th.back.color.sel= 0x10FFFFFF;
					th.back.color.sel_dis = 0x05FFFFFF;
					th.separator.color.a = 0;
				}
				else
				{
					th.separator.color = th.background.color;
				}

				th.shadow.color.opacity(25);
				//th.back.border = th.back.color.nor;
				th.separator.size = 3;
				th.set_symbols_as_text(1);
				return th;
			}

			template<typename T = long>
			static T DPI(double value, double dpi = 96)
			{
				return static_cast<T>(std::rint((value * dpi / 96.0)));
			}

			static bool IsHighContrast()
			{
				HIGHCONTRASTW hc = { sizeof(HIGHCONTRASTW) };
				if(SystemParameters(SPI_GETHIGHCONTRAST, 0, &hc, 0))
					return hc.dwFlags & HCF_HIGHCONTRASTON;
				return false;
			}

			static long GetFont(LOGFONTW *lf, uint32_t dpi = 96)
			{
				NONCLIENTMETRICSW ncm{};
				ncm.cbSize = sizeof(ncm);
				if(SystemParameters(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi))
				{
					::memcpy(lf, &ncm.lfMenuFont, sizeof(LOGFONTW));
					return ncm.lfMenuFont.lfHeight;
				}
				return 0;
			}

			/*
			0 light
			1 dark
			2 system_dark
			3 apps_dark
			*/

			//EnableTransparency
			static constexpr auto key_personalize = LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize)";

			static bool IsDarkMode(bool taskbar)
			{
				//Windows 10 build 10074
				static constexpr auto systemUsesLightTheme = L"SystemUsesLightTheme";
				//Windows 10 build 1809
				constexpr auto appsUseLightTheme = L"AppsUseLightTheme";

				bool _systemUsesLightTheme = true;
				bool _appsUseLightTheme = true;

				if(!IsHighContrast())
				{
					auto personalize = Registry::CurrentUser.OpenSubKey(key_personalize, false, false);
					if(personalize)
					{
						//Windows 10 build 10074
						if(personalize.ExistsValue(systemUsesLightTheme))
						{
							_systemUsesLightTheme = static_cast<bool>(personalize.GetDWord(systemUsesLightTheme));
							//Windows 10 build 1809
							if(personalize.ExistsValue(appsUseLightTheme))
								_appsUseLightTheme = static_cast<bool>(personalize.GetDWord(appsUseLightTheme));
						}
						personalize.Close();
					}
				}
				return taskbar ? !_systemUsesLightTheme : !_appsUseLightTheme;
			}

			static void Personalize(bool *lpSystemUsesLightTheme = nullptr, bool *lpAppsUseLightTheme = nullptr, bool *lpEnableTransparency = nullptr)
			{
				static constexpr auto enableTransparency = L"EnableTransparency";
				//Windows 10 build 10074
				static constexpr auto systemUsesLightTheme = L"SystemUsesLightTheme";
				//Windows 10 build 1809
				constexpr auto appsUseLightTheme = L"AppsUseLightTheme";

				if(!IsHighContrast())
				{
					auto personalize = Registry::CurrentUser.OpenSubKey(key_personalize, false, false);
					if(personalize)
					{
						if(lpEnableTransparency)
						{
							*lpEnableTransparency = false;
							if(personalize.ExistsValue(enableTransparency))
							{
								*lpEnableTransparency = static_cast<bool>(personalize.GetDWord(enableTransparency));
							}
						}

						//Windows 10 build 10074
						if(lpSystemUsesLightTheme)
						{
							*lpSystemUsesLightTheme = true;
							if(personalize.ExistsValue(systemUsesLightTheme))
								*lpSystemUsesLightTheme = static_cast<bool>(personalize.GetDWord(systemUsesLightTheme));
						}

						//Windows 10 build 1809
						if(lpAppsUseLightTheme)
						{
							*lpAppsUseLightTheme = true;
							if(personalize.ExistsValue(appsUseLightTheme))
								*lpAppsUseLightTheme = static_cast<bool>(personalize.GetDWord(appsUseLightTheme));
						}
						personalize.Close();
					}
				}
			}

			static bool SystemParameters(uint32_t uiAction, uint32_t uiParam, void* pvParam, uint32_t fWinIni, uint32_t dpi = 96, bool skip_dpi = false)
			{
				if(!skip_dpi and ver.IsWindowsVersionOrGreater(10, 0, 14393))
					return DLL::User32<BOOL>("SystemParametersInfoForDpi", uiAction, uiParam, pvParam, fWinIni, dpi);
				return ::SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
			}

			template<typename T = int>
			static T SystemMetrics(int nIndex, uint32_t dpi = 96, bool skip_dpi = false)
			{
				if(!skip_dpi and ver.IsWindowsVersionOrGreater(10, 0, 14393))
					return DLL::User32<T>("GetSystemMetricsForDpi", nIndex, dpi);
				return static_cast<T>(::GetSystemMetrics(nIndex));
			}

			static uint32_t GetDpi(HWND hWnd)
			{
				if(ver.IsWindowsVersionOrGreater(10, 0, 14393))
					return DLL::User32<uint32_t>("GetDpiForWindow", hWnd);
				return (uint32_t)DC(hWnd).GetDeviceCapsX();
			}

			static uint32_t GetDpi(Point const &pt, HWND hWnd = nullptr)
			{
				auto dpiX = 96u, dpiY = 96u;
				if(ver.IsWindows81OrGreater())
				{
					if(auto hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST); hMonitor)
					{
						if(S_OK == DLL::Invoke<HRESULT>(L"Shcore.dll", "GetDpiForMonitor", hMonitor, 0, &dpiX, &dpiY) && dpiX != 0)
							return dpiX;
					}
				}

				if(hWnd)
				{
					if(ver.IsWindowsVersionOrGreater(10, 0, 14393))
						return DLL::User32<uint32_t>("GetDpiForWindow", hWnd);
					return (uint32_t)DC(hWnd).GetDeviceCapsY();
				}

				return dpiX;
			}

			static HTHEME OpenThemeData(HWND hWnd, wchar_t const *pszClassList, uint32_t dpi = 96)
			{
				if(ver.IsWindowsVersionOrGreater(10, 0, 14393))
					return DLL::Invoke<HTHEME>(L"uxtheme.dll", "OpenThemeDataForDpi", hWnd, pszClassList, dpi);
				return ::OpenThemeData(hWnd, pszClassList);
			}

		};
	}
}