#pragma once

namespace Nilesoft
{
#include <windows.h>
	class CommandLine
	{
	public:
		class Option
		{
		public:
			static auto equals(const wchar_t *str1, const wchar_t *str2, bool ignoreCase = false)
			{
				return string::Equals(str1, str2, ignoreCase);
			}

		public:
			string Name;
			string Argument;
			string Value;
			wchar_t Kind = 0;

			Option() {}

			Option(const string &argument, wchar_t kind = 0,
				   const string &name = nullptr, const string &value = nullptr) 
				: Argument{ argument }, Kind{ kind }, Name{ name }, Value{ value }
			{
			}

			~Option() {}

			auto has_name(const wchar_t *name, bool ignoreCase = true) const
			{
				return Name.equals(name, ignoreCase);
			}

			auto has_name(std::initializer_list<const wchar_t *> names, bool ignoreCase = true) const
			{
				return Name.equals(names, ignoreCase);
			}

			auto has_value(const wchar_t* value, bool ignoreCase = true) const
			{
				return Value.equals(value, ignoreCase);
			}

			auto has_value(std::initializer_list<const wchar_t *> values, bool ignoreCase = true) const
			{
				return Value.equals(values, ignoreCase);
			}
		};

	private:
		uint32_t	m_size;
		string		m_path;

	public:
		bool ShowHelp = false;
		std::vector<Option*> Options;

		CommandLine(const wchar_t* _cmdLine = nullptr)
			: ShowHelp(false)
		{
			Parse(_cmdLine);
		}

		~CommandLine()
		{
			this->clrear();
		}

		void clrear()
		{
			for(auto o : Options) delete o;
			Options.clear();
			ShowHelp = false;
		}

		CommandLine& operator=(const wchar_t*str)
		{
			this->clrear();
			this->m_path.clear();
			this->Parse(str);
			return *this;
		}

		auto size() const { return Options.size(); }
		auto empty() const { return Options.empty(); }

		auto get(const wchar_t* name, const Option *&lpOption, bool ignoreCase = false) const
		{
			if(name)
			{
				for(auto option : Options)
				{
					if(option && option->has_name(name, ignoreCase))
					{
						lpOption = option;
						return true;
					}
				}
			}
			return false;
		}

		auto get(const wchar_t* name, const wchar_t**lpValue, bool ignoreCase = false) const
		{
			const Option *option = nullptr;
			if(get(name, option, ignoreCase))
			{
				*lpValue = option->Value;
				return true;
			}
			return false;
		}

		auto get(const wchar_t* name, bool ignoreCase = false) const
		{
			const wchar_t* lpValue = nullptr;
			get(name, &lpValue, ignoreCase);
			return lpValue;
		}

		const auto operator[](uint32_t index) const
		{
			return Options[index];
		}

		auto operator[](const wchar_t* name) const
		{
			return get(name);
		}

		auto find(const wchar_t* name, bool ignoreCase = true)
		{
			const Option *option = nullptr;
			return get(name, option, ignoreCase);
		}

		auto find(std::initializer_list<const wchar_t*> names, bool ignoreCase = true)
		{
			const Option *option = nullptr;
			for(auto o : names)
			{
				if(get(o, option, ignoreCase))
					return true;
			}
			return false;
		}

		auto indexOf(const wchar_t* name, bool ignoreCase = true)
		{
			if(name)
			{
				for(uint32_t i = 0; i < m_size; i++)
				{
					auto option = Options[i];
					if(option && option->has_name(name, ignoreCase))
					{
						return i;
					}
				}
			}
			return uint32_t(-1);
		}

		auto const& GetApplicationPath() const
		{
			return m_path;
		}

		const auto GetArguments() const
		{
			return Options.begin();
		}

		auto const begin() const { return Options.begin(); }
		auto const end() const { return Options.end(); }

		auto begin() { return Options.begin(); }
		auto end() { return Options.end(); }

	private:
		bool Parse(const wchar_t* _cmdLine)
		{
			if(!Options.empty())
				this->~CommandLine();

			/*
			int argc = 0;
			auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
			if( nullptr != argv )
			{
				for(int i = 0; i < argc; i++) printf("%d: %ws\n", i, argv[i]);
				::LocalFree(argv);
			}
			*/

			bool fcmd = _cmdLine != nullptr;
			auto cmdLine = fcmd ? _cmdLine : ::GetCommandLineW();
			const auto cmdLine_len = string::Length(cmdLine) + 1;
			if(cmdLine_len <= 1)
				return false;

			auto argv = new wchar_t* [cmdLine_len]{ };
			auto arg = new wchar_t[cmdLine_len]{ };

			uint32_t argc = 0, i = 0, j = 0;
			
			argv[argc] = arg;

			bool in_QM = false;
			bool in_TEXT = false;
			bool in_SPACE = true;

			wchar_t c;
			while((c = cmdLine[i]) != L'\0')
			{
				if(in_QM)
				{
					if(c == L'\"')
						in_QM = false;
					else
						arg[j++] = c;
				}
				else
				{
					switch(c)
					{
						case L'\"':
						{
							in_QM = true;
							in_TEXT = true;
							if(in_SPACE)
								argv[argc++] = arg + j;
							in_SPACE = false;
							break;
						}
						case L' ': case L'\t':
						case L'\n': case L'\r':
						{
							if(in_TEXT)
							{
								*(arg+j++) = L'\0';
							}
							in_TEXT = false;
							in_SPACE = true;
							break;
						}
						default:
						{
							in_TEXT = true;
							if(in_SPACE)
								argv[argc++] = arg + j;
							*(arg+j++) = c;
							in_SPACE = false;
							break;
						}
					}
				}
				i++;
			}

			Options.reserve(argc);

			*(arg+j) = L'\0';
			*(argv+argc) = nullptr;

			if(argc > 0)
			{
				i = 0;
				if(!fcmd)
				{
					m_path = argv[0];
					i = 1;
				}

				auto size = uint32_t(argc - i);

				for(j = 0; j < size; j++, i++)
				{
					c = argv[i][0];
					bool kind = false;
					if((c == L'/') || (c == L'-'))
					{
						argv[i]++;
						kind = true;
					}

					string a = argv[i];
					if(a.equals({ L"help", L"h",L"?" }))
						this->ShowHelp = true;
					else
					{
						auto option = new Option(a, c);
						
						if(kind) option->Kind = c;

						auto p = a.index_of(L':', 0, false);

						if(p == a.npos)
							p = a.index_of(L':', 0, false);

						if(p == string::npos)
							option->Name = a.move();
						else
						{
							option->Name = a.substr(0, p).move();
							option->Value = a.substr(p + 1).trim().move();
						}

						Options.push_back(option);
					}
				}
			}

			// cleanup
			delete[] arg;
			delete[] argv;

			return true;
		}
	};
}
