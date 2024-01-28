#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class FindPattern
		{
		public:
			enum class SearchType
			{
				None,
				Title,
				Extension,
				Path,
				Not,
				Start,
				End,
				Equals,
				Contains,
				Any
			};

			struct Pattern
			{
				std::wstring value;
				bool Not = 0;
				SearchType search = SearchType::Title;
				SearchType Pos = SearchType::Contains;

				Pattern() = default;
				Pattern(Pattern &&other) = default;
				Pattern(const wchar_t *src, const size_t length)
				{
					std::wstring_view s(src, length);
					/*
					!*text  title not end
					.!*text ext not end
					:!*text path not end
					:!*text*
					text*!:
					*/ 

					if(!s.empty())
					{
						if(s.length() == 1 && s.front() == '*')
						{
							Pos = SearchType::Any;
						}
						else
						{
							bool found = false;

							if(s.front() == '.' || s.front() == ':')
							{
								search = s.front() == '.' ? SearchType::Extension : SearchType::Path;
								s.remove_prefix(1);
							}
							
							if(s.front() == '!' || s.front() == '*' || s.front() == '"')
							{
								if(s.front() == '!')
								{
									s.remove_prefix(1);
									Not = true;
								}

								if(s.front() == '*')
								{
									s.remove_prefix(1);
									if(s.back() == '*')
									{
										s.remove_suffix(1);
										Pos = SearchType::Contains;
									}
									else
									{
										Pos = SearchType::End;
									}
									found = true;
								}
								else if(s.length() > 2u && s.front() == '"' && s.back() == '"')
								{
									s.remove_prefix(1);
									s.remove_suffix(1);
									Pos = SearchType::Equals;
									found = true;
								}
								else
								{
									Pos = SearchType::Contains;
								}
							}
							else
							{
								if(s.back() == '.' || s.back() == ':')
								{
									search = s.back() == '.' ? SearchType::Extension : SearchType::Path;
									s.remove_suffix(1);
								}

								if(s.back() == '!' || s.back() == '*')
								{
									if(s.back() == '!')
									{
										s.remove_suffix(1);
										Not = true;
									}

									if(s.back() == '*')
									{
										s.remove_suffix(1);
										if(s.front() == '*')
										{
											s.remove_prefix(1);
											Pos = SearchType::Contains;
										}
										else
										{
											Pos = SearchType::Start;
										}
										found = true;
									}
								}
							}

							if(!s.empty() && (found || search != SearchType::Title))
							{
								value = s;
								return;
							}

							Pos = SearchType::Contains;
							Not = false;
						}	

						value = src;
					}
				}
			};

		public:
			std::vector<Pattern> matches;

			FindPattern() = default;
			FindPattern(FindPattern &&other) = default;

			bool find(Pattern const &pat, string const *title, string const *ext = nullptr, string const *path = nullptr) const
			{
				bool ret = false;
				switch(pat.Pos)
				{
					case SearchType::Any:
						return true;
					case SearchType::Start:
					{
						if(ext && pat.search == SearchType::Extension)
							ret = ext->starts_with(pat.value);
						else if(path && pat.search == SearchType::Path)
							ret = path->starts_with(pat.value);
						else if(title)
							ret = title->starts_with(pat.value);
						break;
					}
					case SearchType::End:
					{
						if(ext && pat.search == SearchType::Extension)
							ret = ext->ends_with(pat.value);
						else if(path && pat.search == SearchType::Path)
							ret = path->ends_with(pat.value);
						else if(title)
							ret = title->ends_with(pat.value);
						break;
					}
					case SearchType::Equals:
					{
						if(ext && pat.search == SearchType::Extension)
							ret = ext->equals(pat.value);
						else if(path && pat.search == SearchType::Path)
							ret = path->equals(pat.value);
						else if(title)
							ret = title->equals(pat.value);
						break;
					}
					default:
					{
						if(ext && pat.search == SearchType::Extension)
							ret = ext->contains(pat.value.c_str());
						else if(path && pat.search == SearchType::Path)
							ret = path->contains(pat.value.c_str());
						else if(title)
							ret = title->contains(pat.value.c_str());
						break;
					}
				}

				if(pat.Not)
				{
					if(ret) return false;
					ret = true; // inc finder
				}

				return ret;
			}

			bool find(string const *title, string const *ext = nullptr, string const *path = nullptr) const
			{
				uint32_t ret = 0;
				for(auto const &pat : matches)
					ret += find(pat, title, ext, path);
				return ret > 0;
			}

			bool split(wchar_t const *text, wchar_t seperator)
			{
				if(!text || *text == 0) return false;

				auto str = text;
				string value;
				size_t count = 0;
				while(*str != 0) count += (*str++ == seperator);
				matches.reserve(count);
				str = text;
				while(*str != 0)
				{
					if(seperator == *str)
					{
						if(!value.trim().empty())
							matches.emplace_back(value.c_str(), value.length());
						value = nullptr;
					}
					else value.append(*str);
					str++;
				};

				if(!value.trim().empty())
					matches.emplace_back(value.c_str(), value.length());

				return !matches.empty();
			}

			bool operator()(string const &text, wchar_t seperator)
			{
				return split(text, seperator);
			}

			bool operator()(string const *title) const
			{
				return find(title, nullptr, nullptr);
			}

			bool operator()(string const *title, string const *ext, string const *path) const
			{
				return find(title, ext, path);
			}

			bool empty() const noexcept { return matches.empty(); }
			size_t size() const noexcept { return matches.size(); }

			const auto begin()const noexcept { return matches.begin(); }
			const auto end()const noexcept { return matches.end(); }
			auto begin() { return matches.begin(); }
			auto end() { return matches.end(); }
		};
	}
}
