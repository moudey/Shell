#pragma once

namespace Nilesoft
{
	namespace Collections
	{
		/*template<typename T, uint32_t _capacity = 16>
		class List
		{
			using uint32_t = unsigned;
			//using iterator = T *;

		private:
			uint32_t m_size;
			uint32_t m_capacity;
			T **m_data;

		public:
			List() : List(_capacity) {}

			List(uint32_t capacity)
				: m_data{ new T * [capacity] {} },
				m_capacity{ capacity }, m_size{ 0 }
			{
			}

			~List()
			{
				clear();
			}

			// destroy and deallocate
			void clear(bool destroy = false)
			{
				if(m_data)
				{
					if(destroy)
						for(uint32_t i = 0; i < m_size; i++) delete m_data[i];
					delete[] m_data;
				}
				m_data = nullptr;
				m_size = 0;
				m_capacity = 0;
			}

			void reserve(uint32_t capacity)
			{
				if(!m_data || capacity >= m_capacity)
				{
					m_capacity = capacity + (capacity / 2);
					if(m_capacity < _capacity) m_capacity = _capacity;

					if(m_size >= m_capacity)
						m_capacity = m_size + (m_size / 2);

					auto old = m_data;
					m_data = new T * [m_capacity] {};
					if(old)
					{
						// copy old to m_data
						for(uint32_t i = 0; i < m_size; i++)
						{
							m_data[i] = old[i];
							old[i] = nullptr;
						}
						// delete the space allocated to data 'old memory'
						delete[] old;
					}
					else
					{
						m_size = 0;
					}
				}
			}

			T *at(uint32_t index)
			{
				return (index < m_size) ? m_data[index] : nullptr;
			}

			T const *at(uint32_t index) const
			{
				return (index < m_size) ? m_data[index] : nullptr;
			}

			T *add(T *object)
			{
				return insert(m_size, object);
			}

			T *push_back(T *object)
			{
				return insert(m_size, object);
			}

			void push_back(List<T> &list)
			{
				return insert(m_size, list);
			}

			T *push_front(T *object)
			{
				return insert(0, object);
			}

			void push_front(List<T> &list)
			{
				insert(0, list);
			}

			T *insert(uint32_t index, T *object)
			{
				reserve(m_size);
				if(index > m_size)
					index = m_size;
				else
				{
					for(uint32_t i = m_size; i > index; i--)
						m_data[i] = m_data[i - 1];
					//std::memmove(m_data + index + 1, m_data + index, (m_size - index) * sizeof(m_data[0]));
				}
				m_size++;
				m_data[index] = object;
				return object;
			}

			void insert(uint32_t index, List<T> &list)
			{
				for(auto item : list) insert(index++, item);
			}

			bool erase(uint32_t index, bool destory = true)
			{
				if(index >= m_size) return false;
				if(destory) delete m_data[index];

				--m_size;
				for(uint32_t i = index; i < m_size; i++)
					m_data[i] = m_data[i + 1];
				//std::memmove(m_data + index, m_data + index + 1, (m_size - index) * sizeof(m_data[0]));
				m_data[m_size] = nullptr;
				return true;
			}

			bool pop_front(bool destroy = true)
			{
				return erase(0U, destroy);
			}

			bool pop_back(bool destroy = true)
			{
				return erase(m_size - 1, destroy);
			}

			bool assign(uint32_t index, T *object, bool destroy = true)
			{	
				if(index < m_size)
				{
					if(destroy) delete m_data[index];
					m_data[index] = object;
					return true;
				}
				return false;
			}

			List<T> &copy(const List<T> &src, bool destory = true)
			{
				if(this != &src)
				{
					clear(destory);
					if(m_size > 0)
					{
						m_capacity = src.m_capacity;
						m_size = src.m_size;
						m_data = new T * [m_capacity] {};
						for(uint32_t i = 0; i < m_size; i++)
							m_data[i] = src.m_data[i];
					}
				}
				return *this;
			}

			List<T> &operator=(const List<T> &rhs)
			{
				return copy(rhs);
			}

			T *operator [](uint32_t index) { return at(index); }
			T const *operator [](uint32_t index) const { return at(index); }

			bool empty() const { return m_size == 0; }
			uint32_t size() const { return m_size; }
			uint32_t capacity() const { return m_capacity; }

			T *front() { return at(0); }
			T const *front() const { return at(0); }
			T *back() { return at(m_size - 1); }
			T const *back() const { return at(m_size - 1); }

			//iterator
			T const **begin() const { return (T const **)m_data; }
			T const **end() const { return (T const **)(m_data + m_size); }

			T **begin() { return m_data; }
			T **end() { return m_data + m_size; }

			T **data() { return m_data; }

			template <class F>
			F foreach(F func)
			{
				auto first = begin();
				const auto last = end();
				for(; first != last; ++first)
				{
					func(*first);
				}
				return func;
			}

			// extension
			uint32_t indexOf(T *key)
			{
				if(key)
				{
					for(uint32_t i = 0; i < m_size; i++)
					{
						if(key == m_data[i]) return i;
					}
				}
				return uint32_t(-1);
			}

			bool erase(T *key, bool destroy = true)
			{
				if(key)
				{
					for(uint32_t i = 0; i < m_size; i++)
					{
						if(key == m_data[i])
							return erase(i, destroy);
					}
				}
				return false;
			}
		};*/
	}
}
