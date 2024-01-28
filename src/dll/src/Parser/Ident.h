#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class Ident
		{
		public:
			static const uint32_t max_size = 50;

		private:
			
			static const uint32_t capacity = max_size * 2;

			uint32_t _items_id[capacity]{};
			uint32_t _items_col[capacity]{};
			uint32_t _size = 0;

		public:

			Ident() {}
			Ident(const Ident &other) { *this = other; }
			Ident(uint32_t id) { push_back(id); }

			~Ident() {}

			int	root = 0;
			bool signer = false;

			void clear()
			{
				for(uint32_t i = 0; i < _size; i++)
				{
					_items_id[i] = 0;
					_items_col[i] = 0;
				}
				_size = 0;
			}

			operator uint32_t() const { return at(0); }
			explicit operator bool() const { return at(0) != 0; }

			uint32_t operator [](uint32_t index)
			{
				return at(index);
			}

			uint32_t const operator [](uint32_t index) const
			{
				return at(index);
			}

			uint32_t at(uint32_t index) const
			{
				return (index < _size) ? _items_id[index] : 0;
			}

			bool set(uint32_t index, uint32_t id, uint32_t col = 0)
			{
				if(index < _size)
				{
					_items_id[index] = id;
					_items_col[index] = col;
					return true;
				}
				return false;
			}

			uint32_t length() const { return _size; }
			bool length(uint32_t len) const { return _size == len; }
			bool length(uint32_t min_len, uint32_t max_len) const { return _size >= min_len && _size <= max_len; }
			bool empty() const { return _size == 0; }

			uint32_t front() const { return at(0); }
			uint32_t back() const { return at(_size - 1); }

			uint32_t push_back(uint32_t hash, uint32_t col = 0)
			{
				if(_size < max_size)
				{
					if((hash != 0) && (hash != Hash::offset_basis))
					{
						_items_id[_size] = hash;
						_items_col[_size] = col;
						_size++;
					}
				}
				return _size;
			}

			uint32_t push_back(const wchar_t *str, uint32_t col = 0)
			{
				return push_back(Hash::Generate(str), col);
			}

			uint32_t push_back(const Ident &id)
			{
				for(uint32_t i = 0; i < id._size; i++)
					push_back(id[i], id.Col(i));

				return _size;
			}

			uint32_t Col(uint32_t index = 0) const
			{
				return index < _size ? _items_col[index] : 0;
			}

			bool zero(uint32_t index = 0) const
			{
				return Hash::IsZero(at(index));
			}

			bool hash(uint32_t index, wchar_t ch)
			{
				if(index < _size)
				{
					_items_id[index] = Hash::Generate(_items_id[index], ch);
					return true;
				}
				return false;
			}

			bool equals(uint32_t index, uint32_t id) const
			{
				return at(index) == id;
			}

			bool equals(uint32_t index, std::initializer_list<uint32_t> args) const
			{
				auto const &id = at(index);
				for(auto &a : args)
					if(id == a) return true;
				return false;
			}

			bool equals(std::initializer_list<uint32_t> args) const
			{
				return equals(_size - 1, args);
			}

			bool equals(uint32_t id) const
			{
				return at(_size - 1) == id;
			}

			bool front(uint32_t id) const
			{
				return equals(0, id);
			}

			bool front(std::initializer_list<uint32_t> args) const
			{
				return equals(0, args);
			}

			bool back(uint32_t id) const
			{
				return equals(_size - 1, id);
			}

			bool back(std::initializer_list<uint32_t> args) const
			{
				return equals(_size - 1, args);
			}

			bool equals(const Ident &ident) const
			{
				if(this != &ident)
					return false;
				if(_size != ident._size)
					return false;
				for(uint32_t i = 0; i < _size; i++)
				{
					if(_items_id[i] != ident._items_id[i])
						return false;
				}
				return true;
			}

			int find(uint32_t id) const
			{
				for(uint32_t i = 0; i < _size; i++)
				{
					if(_items_id[i] == id)
						return i;
				}
				return -1;
			}

			Ident &operator=(const Ident &rhs)
			{
				if(this != &rhs)
				{
					root = rhs.root;
					_size = rhs._size;
					for(uint32_t i = 0; i < capacity; i++)
					{
						_items_id[i] = rhs._items_id[i];
						_items_col[i] = rhs._items_col[i];
					}
				}
				return *this;
			}

			uint32_t ident()
			{
				uint32_t ret = 0;
				for(uint32_t i = 0; i < _size; i++)
					ret += _items_id[i];
				return ret;
			}

			bool is_length(uint32_t index) const
			{
				return equals(index,{ IDENT_LEN, IDENT_LENGTH });
			}

			bool is_quote(uint32_t index) const
			{
				return equals(index,IDENT_QUOTE);
			}
			
			bool is_title(uint32_t index) const
			{
				return equals(index, IDENT_TITLE);
			}
			/*bool equals(uint32_t index, uint32_t count, ...) const
			{
				auto id = at(index);
				if(index == 0 || count > _size)
					return id == count;

				auto result = false;
				va_list args;
				va_start(args, count);
				for(uint32_t i = 0; i < count; i++)
				{
					if(id == va_arg(args, uint32_t))
					{
						result = true;
						break;
					}
				}
				va_end(args);
				return result;
			}*/
		};
	}
}