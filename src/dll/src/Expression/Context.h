#pragma once
namespace Nilesoft
{
	namespace Shell
	{
		enum class Privileges : int
		{
			None,
			Default,
			User,
			System,
			TrustedInstaller,
			Full
		};

		enum class Visibility : int
		{
			None = -1,
			Hidden = 0,
			Remove = Hidden,
			Normal,
			Enabled = Normal,
			Disabled,
			Static,
			Label
		};

		enum class Position : int
		{
			None = -5,
			Auto,
			Middle,
			Top,
			Bottom
		};

		// Spacer
		enum class Separator : int
		{
			None,
			Top = 1,
			Bottom = 2,
			Both = Top | Bottom,
		};

		enum class ImageImport : int
		{
			None,
			Disabled,
			Image,
			SVG,
			Command,
			Inherit,
			Parent,
			Path,
			Draw,
			SystemInherit
		};
		/*
		constexpr wchar_t *verbs[]={
			L"null",
			L"open",
			L"openas",
			L"opennew",
			L"runas",
			L"edit",
			L"explore",
			L"properties",
			L"print",
			L"printto",
			L"find",
			L"copy",
			L"cut",
			L"paste",
			L"pastelink",
			L"delete"
		}*/
		constexpr auto COMMAND_INVALID = -1;
		constexpr auto COMMAND_EXPRESSION = 0;
		constexpr auto COMMAND_PROMPT = 1;
		constexpr auto COMMAND_POWERSHELL = 2;
		constexpr auto COMMAND_PWSH = 3;
		constexpr auto COMMAND_SHELL = 4;
		constexpr auto COMMAND_EXPLORER = 5;


		enum class Verbs : int
		{
			Null,		// Specifies that the operation is the default for the selected file type.
			Open,		// Opens a file or a application
			OpenAs,		// Opens dialog when no program is associated to the extension
			OpenNew,	// see MSDN
			RunAs,		// In Windows 7 and Vista, opens the UAC dialog andin others, open the Run as... Dialog
			Edit,		// Opens the default text editor for the file.
			Explore,	// Opens the Windows Explorer in the folder specified in lpDirectory.
			Properties,	// Opens the properties window of the file.
			Print,		// Start printing the file with the default application.
			Printto,	// see MSDN
			Find,		// Start a search
			Copy,		// see MSDN
			Cut,		// see MSDN
			Paste,		// see MSDN
			PasteLink,	// pastes a shortcut
			Delete,		// see MSDN
		};

		struct Application
		{
			string Path;
			string ConfigPortable;
			string Config;
			string Manager;
			string Dirctory;
			string Name;
			string Root;
			string Version;
			string FileName;
		};

		struct this_item
		{
			int type = 0;
			int pos = -4;
			int checked = 0;
			int disabled = 0;
			bool system = false;
			bool is_uwp = false;
			uint32_t id = 0;
			uint32_t parent=0;
			uint32_t length = 0;
			const wchar_t *title{};
			const wchar_t *title_normalize{};
			const wchar_t *verb{};
			int vis = 0;
			int sep = 0;
			int level = 0;
			string clsid;
		};

		//contains information that's global to the interpreter.
		class Context
		{
		public:
			struct CACHE* Cache = nullptr;

			bool Runtime = true;
			bool Break = false;
			bool Continue = false;
			uint32_t invoked = 0;

			Application *Application{};
			Shell::Selections *Selections{};

			struct 
			{
				Scope *global{};
				Scope *local{};
				Scope *runtime{};
			}variables;

			Keyboard *Keyboard{};
			this_item *_this{};
			struct Theme *theme{};
			struct DPI *dpi{};

			HMENU hMenu{};
			uint32_t menu_count = 0;

			struct {
				string text;
				string icon;
			}font;

			struct {
				uint32_t languageId = 1033;
				bool is_primary_monitor = true;
			}helper;

			struct 
			{
				HWND owner{};
				HWND active{};
				HWND focus{};
			}wnd;

			Context() = default;

			bool variable_exists(uint32_t id);
			class Expression* get_variable(uint32_t id, Expression *e);
			void set_variable2(uint32_t id, Expression *e, Object &&value);

			Object &eval_object(Expression *array, Object &obj);
			bool set_variable(uint32_t id, Expression *e);
			bool to_color(const Object &obj, Color *color);
			bool eval_bool(class Expression *e);
			bool eval_number(class Expression *e, Object &obj);
			bool eval_color(class Expression *e, Color *color);
			Object Eval(class Expression *e);
			Object Eval(Expression *e, const Object &obj);
			bool Eval(Expression *e, Object &obj);
			bool Eval(Expression *e, string &value, bool trim = false);
			uint32_t Eval(Expression *e, uint32_t defualt);
			template<typename T = uint32_t>
			T eval_number(Expression *e, T default_value)
			{
				if(e) 
				{
					Object obj = Eval(e).move();
					if(obj.not_default())
						return obj.to_number<T>();
				}
				return default_value;
			}
			uint32_t obj2hash(const Object &obj, auto default_value = 0) const
			{
				auto value = static_cast<uint32_t>(default_value);
				if(!obj.is_null())
				{
					if(obj.is_string())
						value = obj.to_string().trim().hash();
					else if(obj.not_default())
						value = obj;
				}
				return value;
			}

			uint32_t eval_hash(Expression *e, auto default_value = 0)
			{
				return obj2hash(Eval(e), default_value);
			}

			Visibility parse_visibility(Expression *e, Visibility default_value = Visibility::Enabled);
			Position parse_position(Expression *e, Position default_value = Position::Auto);
			Position parse_pos(const Object &o, Position default_value = Position::Auto);
			Separator parse_separator(Expression *e, Separator default_value = Separator::None);
			SelectionMode parse_mode(Expression *e, SelectionMode default_value = SelectionMode::Single);
			Privileges parse_privileges(struct CommandProperty *cmd);
			Verbs parse_verbs(CommandProperty *cmd);
			int parse_window(struct CommandProperty *cmd);
			uint32_t parse_wait(struct CommandProperty *cmd);
			uint32_t parse_invoke(Expression *e);
			bool eval_tip(Expression *e, string &text, uint8_t &type, uint16_t &time);

			bool Image(Expression *e, struct MenuItemInfo *mii, HFONT hfont, bool selected);

			static int is_svg(Object &obj);
	//	private:
	//		inline static const Object empty = {};
		};
	}
}