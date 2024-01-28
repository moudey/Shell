#pragma once

namespace Nilesoft
{
	namespace Collections
	{
		/*template <class T, uint32_t length>
		class Array
		{
			typedef T& ref;
			typedef const T& const_ref;

		private:
			T m_data[length]{ };

		public:
			Array() { }

			~Array() { }

			void set(uint32_t index, const T& value)
			{
				if(m_data && index < length)
				{
					m_data[index] = value;
					return true;
				}
				return false;
			}

			bool set(uint32_t index, T& value)
			{
				if(m_data && index < length)
				{
					value = m_data[index];
					return true;
				}
				return false;
			}

			T& at(uint32_t index)
			{
				return m_data[index];
			}

			void fill(T const& value)
			{
				for(uint32_t i = 0; i < length; i++)
					m_data[i] = value;
			}

			void copy(Array<T, length> const& src)
			{
				for(uint32_t i = 0; i < length; i++)
					m_data[i] = src[i];
			}

			T &operator[](uint32_t index) { return at(index); }

			const T& operator[](uint32_t index) const { return at(index); }

			T* data() const { return m_data; }
			const uint32_t size() const { return length; }
		};*/
	}
}
