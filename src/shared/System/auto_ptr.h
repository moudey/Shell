#pragma once

namespace Nilesoft
{
	/*
	template <class T>
	class autoptr
	{
	public:
		T *ptr;
		bool del;

		autoptr() : ptr(nullptr), del(true) {}
		autoptr(T *p, bool _delete = true) : ptr(p), del(_delete) {}


		T *get() { return ptr; }
		T *set(T *v) { return ptr = v; }

		void stop() { del = false; }
		void start() { del = true; }
		bool null() { return !ptr; }

		~autoptr()
		{
			if(del)
				delete ptr;
			ptr = nullptr;
		}
	};

	template <class T>
	class autoarray
	{
		T *ptr;
	public:
		autoarray() : ptr(nullptr) {}
		autoarray(T *p) : ptr(p) {}

		T get(int i) { return ptr[i]; }
		T *set(int i, T v) { ptr[i] = v; return ptr; }

		T *get() { return ptr; }
		T *set(T *v) { return ptr = v; }
		bool null() { return !ptr; }

		~autoarray()
		{
			//if (ptr)
			{
				delete[] ptr;
				ptr = nullptr;
			}
		}
	};

	template <class T>
	class auto_ptr
	{
	public:
		T *ptr = nullptr;
		constexpr auto_ptr() noexcept {}
		constexpr auto_ptr(nullptr_t) noexcept {}
		//explicit
		auto_ptr(T *other) noexcept : ptr(other) {}
		auto_ptr(auto_ptr<T> &&rhs) noexcept
		{
			*this = std::move(rhs);
		}

		auto_ptr<T> &operator=(nullptr_t) noexcept
		{
			reset();
			return *this;
		}

		auto_ptr<T> &operator=(T *other) noexcept
		{
			reset(other);
			return *this;
		}

		auto_ptr<T> &operator=(auto_ptr<T> &&rhs) noexcept
		{
			if(ptr != rhs.ptr)
			{
				reset(rhs.release());
				rhs.ptr = nullptr;
			}
			return *this;
		}

		void swap(auto_ptr<T> &rhs) noexcept
		{
			auto temp = rhs.ptr;
			rhs.ptr = ptr;
			ptr = temp;
		}

		~auto_ptr() noexcept
		{
			reset();
		}

		T *operator->() const noexcept
		{
			return ptr;
		}

		T *get() const noexcept
		{
			return ptr;
		}

		T *get() noexcept
		{
			return ptr;
		}

		operator T *() noexcept
		{
			return ptr;
		}

		operator T *() const noexcept
		{
			return ptr;
		}

		explicit operator bool() const noexcept
		{
			return static_cast<bool>(ptr);
		}

		T *release()
		{
			auto _ptr = ptr;
			ptr = nullptr;
			return _ptr;
		}

		void reset(T *other = nullptr) noexcept
		{
			auto _ptr = ptr;
			ptr = other;
			delete _ptr;
		}

		auto_ptr(const auto_ptr<T> &) = delete;
		auto_ptr<T> &operator=(const auto_ptr<T> &) = delete;
	};

	template <class T>
	class auto_ptr<T[]>
	{
	public:
		T *ptr = nullptr;

		constexpr auto_ptr() noexcept {}
		constexpr auto_ptr(nullptr_t) noexcept {}
		explicit auto_ptr(T *other) noexcept : ptr(other) {}
		auto_ptr(auto_ptr<T[]> &&rhs) noexcept
		{
			*this = std::move(rhs);
		}

		auto_ptr<T[]> &operator=(nullptr_t) noexcept
		{
			reset();
			return *this;
		}

		auto_ptr<T[]> &operator=(T *other) noexcept
		{
			reset(other);
			return *this;
		}

		auto_ptr<T[]> &operator=(auto_ptr<T[]> &&rhs) noexcept
		{
			if(ptr != rhs.ptr)
			{
				reset(rhs.release());
				rhs.ptr = nullptr;
			}
			return *this;
		}

		void swap(auto_ptr<T[]> &rhs) noexcept
		{
			auto temp = rhs.ptr;
			rhs.ptr = ptr;
			ptr = temp;
		}

		~auto_ptr() noexcept
		{
			reset();
		}

		T *operator->() const noexcept
		{
			return ptr;
		}

		T *get() const noexcept
		{
			return ptr;
		}

		T *get() noexcept
		{
			return ptr;
		}

		explicit operator T *() noexcept
		{
			return ptr;
		}

		explicit operator T *() const noexcept
		{
			return ptr;
		}

		explicit operator T() const
		{
			return _ptr ? *_ptr : throw std::runtime_error("bad type");
		}

		explicit operator bool() const noexcept
		{
			return static_cast<bool>(ptr);
		}

		T &operator[](size_t index) const noexcept
		{
			return ptr[index];
		}

		T *release() noexcept
		{
			return ptr;
		}

		void reset(T *other = nullptr) noexcept
		{
			delete[] ptr;
			ptr = other;
		}

		auto_ptr(const auto_ptr<T[]> &) = delete;
		auto_ptr<T> &operator=(const auto_ptr<T[]> &) = delete;

	private:
		//template <class> friend class auto_ptr;
	};
*/
	typedef class auto_handle
	{
		HANDLE _handle;

	public:
		
		auto_handle() : _handle(INVALID_HANDLE_VALUE) {}
		auto_handle(HANDLE handle) : _handle(handle == nullptr ? INVALID_HANDLE_VALUE : handle) {}
		auto_handle(const auto_handle &other) = default;
		auto_handle(auto_handle &&other) noexcept = default;

		~auto_handle(void) { close(); }

		HANDLE release() noexcept
		{
			return std::exchange(_handle, INVALID_HANDLE_VALUE);
		}

		void reset(HANDLE other = INVALID_HANDLE_VALUE) noexcept
		{
			close();
			_handle = other;
		}

		bool valid() const { return (_handle && (_handle != INVALID_HANDLE_VALUE)); }
		void close() { if(valid()) ::CloseHandle(_handle); _handle = INVALID_HANDLE_VALUE; }

		template<typename T = HANDLE>
		T get() const { return static_cast<T>(_handle); }

		operator HANDLE(void) const { return _handle; }
		operator HINSTANCE(void) const { return (HINSTANCE)_handle; }
		explicit operator bool(void) const { return valid(); }

		auto_handle& operator=(auto_handle &&rhs) noexcept
		{
			if(&rhs != this || _handle != rhs._handle)
			{
				close();
				_handle = rhs.release();
			}
			return *this;
		}

		auto_handle &operator=(HANDLE handle)
		{
			if(handle != _handle)
				reset(handle);
			return *this;
		}
	}Handle;
}