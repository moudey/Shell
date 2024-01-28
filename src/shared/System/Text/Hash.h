#pragma once

namespace Nilesoft
{
	namespace Text
	{
		class Hash
		{
		private:
			uint32_t _value = 0;

		public:
		//static const auto offset_basis_64 = 14695981039346656037ULL;
		//static const auto prime_64 = 1099511628211ULL;
		//	static const auto offset_basis = 2166136261U;
		//	static const auto prime = 16777619U;

			static const uint32_t offset_basis = 5381U;
			static const auto prime = 33U;

			Hash() : _value(offset_basis) {}
			Hash(uint32_t value): _value(value) { }
			Hash(int value) : _value(value) {}
			Hash(const wchar_t* value) : _value(Generate(value, true)) { }

			explicit operator bool() const { return !zero(); }

			operator uint32_t() const { return value(); }
			//operator int() const { return (int)value(); }
			
			//Hash &operator =(uint32_t rhs) { m_value = rhs; return *this; }

			bool operator ==(uint32_t rhs) const { return _value == rhs; }
			bool operator !=(uint32_t rhs) const { return _value != rhs; }

			bool operator ==(int rhs) const { return _value == (uint32_t)rhs; }
			bool operator !=(int rhs) const { return _value != (uint32_t)rhs; }

			Hash& hash(const wchar_t &ch, bool ignoreCase = true)
			{
				_value = Generate(_value, ch, ignoreCase);
				return *this;
			}

			uint32_t value() const
			{
				return (_value == Hash::offset_basis) ? 0U : _value;
			}

			void value(uint32_t val)
			{
				_value = val;
			}

			bool equals(uint32_t hash) const
			{
				return _value == hash;
			}

			bool equals(std::initializer_list<uint32_t> args) const
			{
				for(auto a : args)
					if(_value == a) return true;
				return false;
			}

			bool zero() const
			{
				return Result(_value) == 0U;
			}

			Hash& append(const wchar_t *str, bool ignoreCase = true) noexcept
			{
				if(str)
				{
					while(*str)
					{
						_value = Generate(_value, *str++, ignoreCase);
					}
				}
				return *this;
			}

			static bool IsZero(uint32_t hash)
			{
				return (Result(hash) == 0);
			}

			static uint32_t Result(uint32_t hash)
			{
				return (hash == Hash::offset_basis) ? 0U : hash;
			}

			static uint32_t Generate(uint32_t hash, const wchar_t &ch, bool ignoreCase = true)
			{
				auto c = ch;
				if(ignoreCase)
				{
					//c = (((ch >= L'A') && (ch <= L'Z')) ? (ch - L'A' + L'a') : ch);
					c = (ch >= 0x41 && ch <= 0x5a) ? (ch - 0x41 + 0x61) : ch;
				}
				return hash * prime + static_cast<uint32_t>(c);
			}

			static uint32_t Generate(const wchar_t& ch, bool ignoreCase)
			{
				return Generate(Hash::offset_basis, ch, ignoreCase);
			}
			
			static uint32_t Generate(const wchar_t *str, bool ignoreCase = true) noexcept
			{
				auto h = offset_basis;
				if(str)
				{
					while(*str)
					{
						h = Generate(h, *str++, ignoreCase);
					}
				}
				return h == offset_basis ? 0 : h;
			}
		
			static bool Equals(uint32_t hash1, uint32_t hash2)
			{
				return hash1 == hash2;
			}

			static bool Equals(uint32_t hash, std::initializer_list<uint32_t> args)
			{
				for(auto &a : args)
					if(hash == a) return true;
				return false;
			}

			static uint32_t HashNum(uint32_t value)
			{
				return offset_basis * prime + uint32_t((value << 13) | (value >> 19));
			}

			static uint32_t HashInt(uint32_t hash, uint32_t value)
			{
				return hash + uint32_t((value << 13) | (value >> 19));
			}

			template <class T>
			static uint32_t dohash(T val, uint32_t hash = offset_basis)
			{
				uint32_t result = hash;
				auto val_bytes = reinterpret_cast<const unsigned char *>(&val);
				for(size_t i = 0; i < sizeof(T); ++i)
				{
					result ^= static_cast<uint32_t>(val_bytes[i]);
					result *= prime;
				}
				return result;
			}

			/*public override int GetHashCode() 
        {
            // similar to Font.GetHashCode().
            return (int)((((UInt32)this.Style   << 13) | ((UInt32)this.Style   >> 19)) ^
                         (((UInt32)this.CharSet << 26) | ((UInt32)this.CharSet >>  6)) ^
                         (((UInt32)this.Size    <<  7) | ((UInt32)this.Size    >> 25)));
        }*/
		};
	}
}