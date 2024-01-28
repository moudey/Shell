//#pragma once
//
//namespace Core
//{
//	struct Int8
//	{
//	public:
//		constexpr static int MinValue = (-127 - 1);
//		constexpr static int MaxValue = 127;
//	};
//
//	struct uint32_t8
//	{
//	public:
//		constexpr static unsigned int MaxValue = 0xff;
//	};
//
//	typedef struct Int16
//	{
//	public:
//		constexpr static int MinValue = (-32767 - 1);
//		constexpr static int MaxValue = 32767;
//	}Short;
//
//	typedef struct uint32_t16
//	{
//	public:
//		constexpr static unsigned int MaxValue = 0xffff;
//	}UShort;
//
//	struct Int32
//	{
//	public:
//		constexpr static int MinValue = (-2147483647 - 1);
//		constexpr static int MaxValue = 2147483647;
//	};
//
//	struct uint32_t32
//	{
//	public:
//		constexpr static unsigned int MaxValue = 0xffffffff;
//	};
//
//	struct Long
//	{
//	public:
//		constexpr static long MinValue = (-2147483647 - 1);
//		constexpr static long MaxValue = 2147483647;
//	};
//
//	struct ULong
//	{
//	public:
//		constexpr static unsigned long MaxValue = 0xffffffff;
//	};
//
//	struct Int64
//	{
//	public:
//		constexpr static __int64 MinValue = (-9223372036854775807 - 1);
//		constexpr static __int64 MaxValue = 9223372036854775807;
//	};
//
//	struct uint64_t
//	{
//	public:
//		constexpr static unsigned __int64 MaxValue = 0xffffffffffffffff;
//	};
//
//	struct Int
//	{
//	private:
//		size_t value;
//
//	public:
//		Int(void) : value(0) {}
//		Int(size_t val) : value(val) {}
//		Int(short val) : value(val) {}
//		Int(int val) : value(val) {}
//		Int(long val) : value(val) {}
//		Int(__int64 val) : value(val) {}
//
//		Int(unsigned short val) : value(val) {}
//		Int(unsigned int val) : value(val) {}
//		Int(unsigned long val) : value(val) {}
//		//	Int(unsigned __int64 val) : value(val) {}
//
//		operator size_t(void) { return value; }
//		operator short(void) { return short(value); }
//		operator int(void) { return int(value); }
//		operator long(void) { return long(value); }
//		operator __int64(void) { return __int64(value); }
//
//		operator unsigned short(void) { return unsigned short(value); }
//		operator unsigned int(void) { return unsigned int(value); }
//		operator unsigned long(void) { return unsigned long(value); }
//		//	operator unsigned __int64(void) { return unsigned __int64(value); }
//
//		template<typename T>
//		inline bool operator==(T val) { return this->value == val; }
//		template<typename T>
//		inline bool operator!=(T val) { return this->value != val; }
//
//		template<typename T>
//		inline const Int& operator=(T val) { this->value = val; return(*this); }
//
//		template<typename T>
//		inline const Int& operator+=(int val) { this->value += val; return(*this); }
//
//		template<typename T>
//		inline const Int& operator-(T val) { this->value -= val; return(*this); }
//
//		template<typename T>
//		inline const Int& operator-=(T val) { this->value -= val; return(*this); }
//
//		friend Int operator +(const Int& left, const Int& right) { return left.value + right.value; }
//
//		template<typename T>
//		friend Int operator +(const Int& left, const T& right) { return left.value + right; }
//
//		template<typename T>
//		friend Int operator +(const T& left, const Int& right) { return left + right.value; }
//
//		template<typename T>
//		friend Int operator +(const T& left, const T& right) { return left + right; }
//	};
//}