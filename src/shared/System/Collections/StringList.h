#pragma once
/*
namespace Nilesoft
{
	namespace Collections
	{
		template<typename T>
		class StringListPattern
		{
		public:
			struct PATTERN
			{
				T *value = nullptr;
				uint32_t length = 0;
				bool pattern_found = false;

				bool _not = 0;
				bool starts_with = false;
				bool ends_with = false;
				bool equals = false;
				bool contains = false;

				PATTERN(const T *src, uint32_t len)
				{
					if(len > 0)
					{
						length = len;
						auto at = [=](uint32_t index, wchar_t ch = '*')
						{
							return (index < length) ? src[index] == ch : false;
						};

						auto skip = 0u;
						if(length > 1)
						{
							auto end = length - 1;
							_not = at(0, '!');

							if(((at(0, '=') && at(1)) || (_not && at(1, '=') && at(2))) && at(end))
							{
								equals = true;
								skip = 3;
								src += size_t(2 + _not);
							}
							else if((at(0) || (_not && at(1))) && at(end))
							{
								contains = true;
								skip = 2;
								src += size_t(1 + _not);
							}
							else if(at(0) || (_not && at(1)))
							{
								ends_with = true;
								skip = 1;
								src += size_t(skip + _not);
							}
							else if(at(end) || (at(end, '!') && at(end - 1)))
							{
								starts_with = true;
								skip = 1;
								_not = src[end] == '!';
							}
						}

						skip += _not;
						if(skip > 0)
						{
							length -= skip;
							pattern_found = true;
						}

						value = new T[static_cast<size_t>(length) + 1]{};
						auto v = value; auto s = src;
						for(uint32_t i = 0; i < length; i++) *v++ = *s++;
					}
				}

				//~PATTERN() { delete[] value; }
			};

		protected:
			std::vector<PATTERN> data;

		public:
			StringListPattern() = default;

			StringListPattern(size_t capacity)
			{
				data.reserve(capacity);
			}

			~StringListPattern() { this->clear(); }

			void clear()
			{
				for(auto &p : data) delete[] p.value;
				data.clear();
			}

			void capacity(size_t capacity)
			{
				data.reserve(capacity);
			}

			void push_back(const T *value, uint32_t length)
			{
				if(value && length > 0)
				{
					// trim right
					auto end = (T *)(value + length);
					for(; ::iswspace(*--end); length--) *end = 0;
					// trim left
					for(; ::iswspace(*value); value++, length--);

					if(length > 0)
					{
						data.emplace_back(PATTERN(value, length));
					}
				}
			}

			auto *at(size_t index)
			{
				return &data.at(index);
			}

			const auto *at(uint32_t index) const
			{
				return &data.at(index);
			}

			auto *operator [](uint32_t index)
			{
				return &data.at(index);
			}

			const auto *operator [](uint32_t index) const
			{
				return &data.at(index);
			}

			bool split(const T *text, T delim)
			{
				if(!text)
					return false;

				uint32_t i = 0, c = 0;
				T ch = text[i++];
				T word[MAX_PATH] = { };

				while(ch)
				{
					if(delim == ch)
					{
						if(c) push_back(word, c);
						word[0] = 0; c = 0;
					}
					else
						word[c++] = ch;
					ch = text[i++];
				}

				if(c) push_back(word, c);

				return !data.empty();
			}

			size_t size() const { return data.size(); }
			bool empty() const { return data.empty(); }

			auto begin() const { return data.begin(); }
			auto end() const { return data.end(); }

			auto begin() { return data.begin(); }
			auto end() { return data.end(); }
		};
	}
}
*/
