#pragma once
/*#include <Windows.h>
#include <new>
*/
namespace Nilesoft
{
	class MemoryManager
	{
	protected:

		struct Node
		{
			void *object = nullptr;
			bool array = false;
			Node *prev = nullptr;
			Node *next = nullptr;

			Node(void *object, bool array, Node *next = nullptr, Node *prev = nullptr)
				noexcept : object(object), array(array), next(next), prev(prev)
			{
			}

			~Node()
			{
				if(array)
					delete[] object;
				else
					delete object;

				object = nullptr;
				next = nullptr;
				prev = nullptr;
			}
		};

		Node *head, *tail;
		uint32_t count;

	protected:

		bool empty() { return head == nullptr; }

		template<typename T>
		T* add(T *object, bool array = false)
		{
			auto n = new Node(object, array);
			if(head == nullptr)
			{
				head = n;
			}
			else
			{
				n->prev = tail;
				tail->next = n;
			}
			tail = n;
			count++;
			return object;
		}
		
		template<typename T>
		T* remove(T *object, bool delete_ = true)
		{
			auto n = find(object);
			if(n != nullptr)
			{
				if(n->prev)
					n->prev->next = n->next;
				else
					head = n->next;
				if(delete_)
					delete n;
				count--;
			}
			return nullptr;
		}

		template<typename T>
		Node* find(T *object)
		{
			for(auto n = head; n != nullptr;
				n = n->next)
			{
				if(n->object == object)
					return n;
			}
			return nullptr;
		}

	public:

		MemoryManager() noexcept
			: head(nullptr), tail(nullptr), count(0)
		{
		}

		~MemoryManager()
		{
			CleanUp();
		}

		void CleanUp()
		{
			if(head != nullptr)
			{
				auto n = head;
				while(n != nullptr)
				{
					auto curr = n;
					n = n->next;
					delete curr;
				}
				head = nullptr;
			}
			tail = nullptr;
			count = 0;
		}

		template<typename T>
		T* Allocate()
		{
			return add(new T{}, false);
		}

		template<typename T>
		T* Allocate(size_t size)
		{
			return add(new T[size]{}, true);
		}

		template<typename T>
		T* Allocate(T *object, bool array = false)
		{
			return add(object, array);
		}

		template<typename T>
		T* Attach(T *object, bool array = false)
		{
			return add(object, array);
		}

		template<typename T>
		T* Detach(T *object)
		{
			return remove(object, false);
		}

		template<typename T>
		T* Free(T* &object)
		{
			return object = remove(object);
		}
	};

	template<typename T>
	class Heap
	{
		HANDLE handle;
		uintptr_t size;
		T *ptr;
		bool destroy;

	public:

		Heap() noexcept 
			: handle{ nullptr }, ptr{ nullptr }, size{ sizeof(T) }, destroy{ true }
		{
		}

		Heap(bool destroy) : Heap(), destroy{ destroy }
		{
		}

		~Heap()
		{
			if(destroy) dealloc();
		}

		bool alloc()
		{
			return alloc(sizeof(T));
		}

		bool alloc(uintptr_t size_bytes)
		{
			if(size_bytes == uintptr_t(-1))
				size_bytes = sizeof(T);

			if(ptr) dealloc();

			if(handle)
				handle = ::GetProcessHeap();

			size = size_bytes;

			ptr = reinterpret_cast<T *>(::HeapAlloc(handle, HEAP_ZERO_MEMORY, size));

			return ptr != nullptr;
		}

		bool realloc(uintptr_t size = uintptr_t(-1))
		{
			if(size == uintptr_t(-1))
				size = sizeof(T);

			auto lpMem = reinterpret_cast<T*>(::HeapReAlloc(handle, HEAP_ZERO_MEMORY, ptr, size));
			if(lpMem == nullptr)
				return false;

			this->size = size;
			ptr = lpMem;
			return true;
		}

		void dealloc()
		{
			if(ptr && handle)
				::HeapFree(handle, 0, ptr);

			ptr = nullptr;
			handle = nullptr;
			size = 0;
		}

		bool lock()
		{
			if(size == 0 || !ptr)
				return false;
			return ::HeapLock(handle);
		}

		bool unlock()
		{
			if(size == 0 || !ptr)
				return false;
			return ::HeapUnlock(handle);
		}

		bool null() const { return ptr == nullptr; }

		T *get() { return ptr; }
		const T *get() const { return ptr; }

		operator T *() { return ptr; }
		operator T *() const { return ptr; }
		operator const T *() const { return const_cast<T *>(ptr); }

		// Verify that a pointer points to valid memory (without caching):
		static bool IsPtr(void *ptr)
		{
			if(ptr != nullptr)
			{
				MEMORY_BASIC_INFORMATION mbi = { 0 };
				if(::VirtualQuery(ptr, &mbi, sizeof(mbi)))
				{
					return ((mbi.Protect & 0xE6) != 0 && (mbi.Protect & PAGE_GUARD) == 0);
				}
			}
			return false;
		}
	};

	/*
		class Heap
		{
			bool _zeroMemory;
			ULONG_PTR _size;
			HANDLE hHeap;
		public:
			LPVOID MemBlock;

		public:
			Heap() noexcept :
				hHeap(nullptr), MemBlock(nullptr),
				_size(0), _zeroMemory(false)
			{
			}

			Heap(ULONG_PTR size, bool zeroMemory = true)
				: hHeap(nullptr), MemBlock(nullptr), _size(size), _zeroMemory(zeroMemory)
			{
			}

			~Heap() { Free(); }

			bool Alloc()
			{
				hHeap = ::GetProcessHeap();
				MemBlock = ::HeapAlloc(hHeap, _zeroMemory ? HEAP_ZERO_MEMORY : 0, _size);
				return MemBlock != nullptr;
			}

			bool Alloc(ULONG_PTR size, bool zeroMemory)
			{
				Free();
				_size = size;
				_zeroMemory = zeroMemory;
				return Alloc();
			}

			void Free()
			{
				if(MemBlock && hHeap)
					::HeapFree(hHeap, 0, MemBlock);

				MemBlock = nullptr;
				hHeap = nullptr;
				_size = 0;
				_zeroMemory = true;
			}

			// Verify that a pointer points to valid memory (without caching):
			static bool IsPtr(void *ptr)
			{
				if(ptr != nullptr)
				{
					MEMORY_BASIC_INFORMATION mbi = { 0 };
					if(::VirtualQuery(ptr, &mbi, sizeof(mbi)))
					{
						return ((mbi.Protect & 0xE6) != 0 && (mbi.Protect & PAGE_GUARD) == 0);
					}
				}
				return false;
			}
		};
	*/

	//CGlobal
	class LongBinary
	{
	public:

		LongBinary()
		{
			m_hMemory = NULL;
			m_dwSize = 0L;
		}

		virtual ~LongBinary()
		{
			SetBinarySize(0);
		}

		operator HGLOBAL() { return m_hMemory; };

		DWORD GetBinarySize() { return m_dwSize; };

		void *LockMemory()
		{
			if(m_dwSize == 0 || !m_hMemory)
				return NULL;
			void *lpMem = NULL;
			lpMem = ::GlobalLock(m_hMemory);
			return lpMem;
		}

		void UnlockMemory()
		{
			if(m_dwSize == 0 || !m_hMemory)
				return;
			::GlobalUnlock(m_hMemory);
		}

		bool SetBinarySize(DWORD sizeNew, void *lpDataCopy = NULL)
		{
			DWORD sizeOld = m_hMemory != NULL ? (DWORD)::GlobalSize(m_hMemory) : 0;
			if(sizeOld == 0)
			{
				m_hMemory = ::GlobalAlloc(GMEM_MOVEABLE, sizeNew);
				m_dwSize = sizeNew;
			}
			else
			{
				if(sizeNew > sizeOld)
				{
					m_hMemory = ::GlobalReAlloc(m_hMemory, sizeNew, GMEM_MOVEABLE);
					m_dwSize = sizeNew;
				}
			}

			if(lpDataCopy && m_hMemory)
			{
				if(auto lpDest = ::GlobalLock(m_hMemory))
				{
					::memcpy(lpDest, lpDataCopy, sizeNew);
					::GlobalUnlock(m_hMemory);
				}
			}
			return true;
		}

	protected:
		HGLOBAL m_hMemory;
		DWORD m_dwSize;
	};

/*
	class Heap
	{//HEAP_REALLOC_IN_PLACE_ONLY
		static HANDLE _handle;
		static bool _init;
		// Get a handle to the default process heap.
		//NumberOfHeaps = GetProcessHeaps(0, NULL);
		//NumberOfHeaps = GetProcessHeaps(HeapsLength, aHeaps);
		/*hDefaultProcessHeap = GetProcessHeap();
		if(hDefaultProcessHeap == NULL)
		{
			_tprintf(TEXT("Failed to retrieve the default process heap with LastError %d.\n"),
					 GetLastError());
			return 1;
		}
		HANDLE hHeap = GetProcessHeap();
	public:
		//HeapReAlloc
		static void *alloc(std::size_t size, bool zero = false) noexcept(false)
		{
			DWORD dwFlags = zero ? HEAP_ZERO_MEMORY : 0U;
			if(!_init)
			{
				_handle = ::HeapCreate(0, 0, 0); // Private, expandable heap.
				_init = true;
			}

			if(_handle)
			{
				return ::HeapAlloc(_handle, dwFlags, size);
			}

			throw std::bad_alloc();
		}

		static void dealloc(void *ptr) noexcept
		{
			if(_handle)
			{
				(void)::HeapFree(_handle, 0, ptr);
			}
		}
	};

	void *CMyDllClass::operator new (size_t n, void *pv)
	{
	   ...
	   ...
	   return pv;
	}

	void CMyDllClass::operator delete (void *pv)
	{
		...
			HeapFree(GetProcessHeap(), 0, pv);
		...
	}

	void *operator new(std::size_t size) noexcept(false)
	{
		static HANDLE h = ::HeapCreate(0, 0, 0); // Private, expandable heap.
		if(h)
		{
			return ::HeapAlloc(h, 0, size);
		}
		throw std::bad_alloc();
	}

	// No corresponding global delete operator defined.
	*/

	/*
	class HeapAllocator
	{
		static HANDLE _handle;
		static bool _init;

	public:
		static void *alloc(std::size_t size) noexcept(false)
		{
			if(!_init)
			{
				_handle = ::HeapCreate(0, 0, 0); // Private, expandable heap.
				_init = true;
			}

			if(_handle)
			{
				return ::HeapAlloc(_handle, 0, size);
			}

			throw std::bad_alloc();
		}

		static void dealloc(void *ptr) noexcept
		{
			if(_handle)
			{
				(void)::HeapFree(_handle, 0, ptr);
			}
		}
	};

	HANDLE HeapAllocator::_handle = nullptr;
	bool HeapAllocator::_init = false;

	void *operator new(std::size_t size) noexcept(false)
	{
		return HeapAllocator::alloc(size);
	}

	void operator delete(void *ptr) noexcept
	{
		return HeapAllocator::dealloc(ptr);
	}
	*/
}