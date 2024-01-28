#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		constexpr auto NILESOFTSHELL_SIGN = 0x1989D5CDU;

		struct IProperty
		{
			Expression *expr = nullptr;
			~IProperty() { delete expr; }
		};

		struct Settings
		{
			auto_expr showdelay;
			auto_expr priority;
			struct
			{
				auto_expr enabled;
				auto_expr directory;
			}screenshot;

			struct
			{
				auto_expr value;
				auto_expr window;
				auto_expr process;
			} exclude;

			struct
			{
				auto_expr enabled;
				auto_expr auto_image_group;
				auto_expr auto_group;
				auto_expr auto_image;

				auto_expr image;
				auto_expr title;
				auto_expr visibility;
				auto_expr parent;
				auto_expr position;
				auto_expr separator;
				auto_expr keys;
				struct
				{
					auto_expr disabled;
					auto_expr separator;
					auto_expr duplicate;
				}remove;
			} modify_items;

			struct
			{
				auto_expr enabled;
				auto_expr image;
				auto_expr keys;
			} new_items;

			struct MARGIN
			{
				uint32_t id = IDENT_MARGIN;
				auto_expr value;
				auto_expr left;
				auto_expr top;
				auto_expr right;
				auto_expr bottom;
			};

			struct COLOR
			{
				auto_expr value;
				auto_expr normal;
				auto_expr select;
				auto_expr normal_disabled;
				auto_expr select_disabled;
			};

			struct
			{
				auto_expr dark;
				auto_expr name;
				auto_expr view;

				/*struct {
					auto_expr enabled;
					auto_expr color;
					auto_expr font;
					auto_expr size;
				} glyphs;*/

				struct {
					auto_expr color;
					auto_expr opacity;
					auto_expr effect;
					auto_expr tintcolor;
					auto_expr image;
				} background;

				struct
				{
					auto_expr enabled;
					auto_expr size;
					auto_expr color;
					auto_expr opacity;
					auto_expr radius;
					MARGIN padding;
				} border;

				struct
				{
					auto_expr enabled;
					auto_expr size;
					auto_expr color;
					auto_expr opacity;
					auto_expr offset;
				} shadow;

				struct
				{
					struct {
						COLOR color;
						auto_expr align;
					} text;
					
					COLOR back;
					COLOR border;
					auto_expr opacity;
					auto_expr radius;
					MARGIN padding;
					MARGIN margin;
					auto_expr prefix;
				} item;

				struct
				{
					auto_expr color;
					auto_expr size;
					auto_expr opacity;
					MARGIN margin;
				} separator;

				struct {
					auto_expr value;
					auto_expr name;
					auto_expr size;
					auto_expr weight;
					auto_expr italic;
				} font;

				struct
				{
					COLOR color;
					COLOR checkmark;
					COLOR bullet;
					COLOR chevron;
					auto_expr opacity;
				} symbol;

				struct {
					auto_expr width;
					auto_expr rtl;
					struct {
						auto_expr align;
					}popup;
				}layout;

				struct {
					auto_expr enabled;
					auto_expr linear;
					auto_expr radial;
					auto_expr stop;
				} gradient;

				struct
				{
					auto_expr enabled;
					auto_expr color;
					auto_expr size;
					auto_expr opacity;
					auto_expr gap;
					auto_expr glyph;
					auto_expr scale;
					auto_expr display;
				} image;

			} theme;

			struct
			{
				auto_expr enabled;
				auto_expr normal;
				auto_expr primary;
				auto_expr success;
				auto_expr info;
				auto_expr warning;
				auto_expr danger;
				auto_expr border;
				auto_expr width;
				auto_expr opacity;
				auto_expr radius;
				auto_expr time;
				MARGIN padding;
			} tip;
		};

		struct CommandProperty
		{
			struct : IProperty {
				int type = COMMAND_INVALID;
			} command;

			/*struct : IProperty {
				Privileges value = Privileges::None;
				bool inherit = false;
			} admin;*/
		
			auto_expr window;
			auto_expr directory;
			auto_expr verb;
			auto_expr wait;
			auto_expr admin;
			auto_expr invoke;
			auto_expr arguments;

			bool inherit = false;

			~CommandProperty()
			{
			}
		};

		enum class NativeMenuType : int32_t
		{
			None,
			Main,
			Menu,		// SubMenu
			Item,		// Command
			Separator	// Spacer
		};

		class NativeMenu
		{
		public:
			uint32_t sign = NILESOFTSHELL_SIGN + 1;

			static constexpr auto TYPE_NONE = 0;
			static constexpr auto TYPE_MAIN = 1;
			static constexpr auto TYPE_MENU = 2;
			static constexpr auto TYPE_ITEM = 3;
			static constexpr auto TYPE_SEPARATOR = 4;

		public:
			int32_t		MenuId = 0;
			int32_t		id = 0;
			NativeMenuType	type = NativeMenuType::None;
			NativeMenu *owner{};
			Scope		variables;
			int32_t		for_each = 0;
			auto_expr	expanded;
			std::vector<NativeMenu *> items;
			CommandProperty	*cmd{};

			uint32_t properties{};
			FileSystemObjects fso;
			auto_expr mode;
			auto_expr visibility;
			auto_expr tip;
			auto_expr position;
			auto_expr separator;
			auto_expr find;
			auto_expr location;
			auto_expr where;
			auto_expr keys;
			auto_expr moveto;
			auto_expr title;
			auto_expr checked;
			auto_expr column;
			// static item property
			auto_expr invoke;
			bool has_clsid = false;
			std::vector<Guid> clsid;

			struct {
				auto_expr normal;
				auto_expr select;
			} images;

			struct : IProperty {
				ImageImport	import = ImageImport::None;
				bool		inherit = false;
				bool		defined = true;

				bool enabled() const
				{
					return !(import == ImageImport::None || import == ImageImport::Disabled);
				}

				bool disabled() const
				{
					return import == ImageImport::Disabled;
				}

			} image;

			std::vector<CommandProperty*> commands;

			NativeMenu(NativeMenu *owner = nullptr) noexcept
				: owner(owner)
			{
				if(owner)
				{
					fso = owner->fso;
					variables.Parent = &owner->variables;
				}
				cmd = new CommandProperty;
				commands.push_back(cmd);
			}

			NativeMenu(bool set_any_type) noexcept
			{
				fso.set(set_any_type);
			}

			virtual ~NativeMenu()
			{
				/*if(owner)
				{
					if(mode.inherit)
						::memset(&mode, 0, sizeof(mode));

					if(cmd->admin.inherit)
						cmd->admin.expr = nullptr;
				}
				*/
				for(auto it : items) delete it;
				for(auto it : commands) delete it;
			}

			bool is_main() const { return (this->type == NativeMenuType::Main); }
			bool is_item() const { return (this->type == NativeMenuType::Item); }
			bool is_separator() const { return (this->type == NativeMenuType::Separator); }
			bool is_menu() const { return (this->type == NativeMenuType::Menu || this->type == NativeMenuType::Main); }

			bool skip_separator(const std::vector<NativeMenu *> &list)
			{
				if(this->is_separator())
				{
					return(this == list.front() || this == list.back());
				}
				return false;
			}

			Expression *get_variable(uint32_t ident)
			{
				auto p = this->owner;
				while(p)
				{
					auto e = p->variables.at(ident);
					if(e) return e;
					p = p->owner;
				}

				return nullptr;
			}

			static bool is_skip_separator(std::vector<NativeMenu*> const &list)
			{
				// front
				if(list.empty()) return true;
				// back
				return list.back()->is_separator();
			}
		};
	}
}

