#pragma once
/*
namespace Nilesoft
{
	namespace Collections
	{
		template<typename T, bool delete_object = true>
		class LList
		{
		public:
			struct Node
			{
			public:
				T object;
				Node* Next = nullptr;
				Node* Prev = nullptr;
				Node() : object(nullptr) {}
				Node(T obj) : object(obj) {}
				~Node() { if(object && delete_object) delete object; }

				T operator*() { return object; }

				void operator++() 
				{
					Prev = this;
					Next = Next->Next;
				}

				T get() { return object; }

				//void operator++(int) { Next = Next->Next; }
			};

		private:
			uint32_t count;
			Node *head, *tail;

		public:
			LList() :
				head(nullptr), tail(nullptr), count(0)
			{
			}

			~LList()
			{
				RemoveAll();
			}

			void RemoveAll()
			{
				if(head != nullptr)
				{
					auto node = head;
					while(node != nullptr)
					{
						auto curr = node;
						node = node->Next;
						delete curr;
					}
					head = nullptr;
				}
				tail = nullptr;
				count = 0;
			}

			bool Add(T object)
			{
				if(object == nullptr)
					return false;
				auto node = new Node(object);
				if(head == nullptr)
				{
					head = node;
				}
				else
				{
					node->Prev = tail;
					tail->Next = node;
				}
				tail = node;
				count++;
				return true;
			}

			Node* begin() const { return head; }
			Node* back() const { return tail; }
			Node* end() const { return nullptr; }

			T Get(uint32_t index) const
			{
				if(index < count)
				{
					uint32_t i = 0;
					auto node = head;
					while(node && (i < count))
					{
						if(i++ == index)
						{
							return(node ? node->object : nullptr);
						}
						node = node->Next;
					}
				}
				return nullptr;
			}

			T operator [](uint32_t index) const
			{
				return Get(index);
			}

			T Head() const { return head ? head->object : nullptr; }
			T Tail() const { return tail ? tail->object : nullptr; }

			uint32_t Count() const { return count; }
			uint32_t IsEmpty() const { return count == 0; }
		};
	}
}
*/