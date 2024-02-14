#pragma once

#include "System\Text\string.h"
#include "System\Drawing\Color.h"

namespace Nilesoft
{
	using namespace Text;
	/*template<class T>
	struct is_c_str : std::integral_constant<bool, false> {};

	template<>
	struct is_c_str<wchar_t *> : std::integral_constant<bool, true> {};

	template<>
	struct is_c_str<const wchar_t *> : std::integral_constant<bool, true> {};
	*/

	class Object
	{
		using pointer_t = Object*;
		using array_t = std::vector<Object>;

	public:
		struct
		{
			double Number{ };
			string String{ };
			pointer_t Pointer = nullptr;
			PrimitiveType Type = PrimitiveType::Null;
			PrimitiveType Inner = PrimitiveType::Null;//inner, nest,nested, Implicit
		} Value{};

		Object() = default;
		Object(Object const &other) = default;// noexcept { *this = other; }
		
		template<typename T, std::enable_if_t<std::_Is_any_of_v<T, Object, string>, bool> = true>
		Object(T &&other) noexcept { *this = std::move(other); }

		template<typename T>
		Object(T const &other) noexcept
		{
			if constexpr(std::is_null_pointer_v<T>)
				Value.Type = PrimitiveType::Null;
			else if constexpr(is_string_or_char_type_v<T>)
				*this = other;
			else if constexpr(is_numeric_type_v<T>)
				*this = other;
			else if constexpr(std::is_pointer_v<T>)
				*this = other;
		}

		Object(Object *other, bool array = false)
		{
			*this = other;
			if(array)
				Value.Inner = PrimitiveType::Array;
		}

		Object(const Drawing::Color& color)
		{
			Value.Type = PrimitiveType::Number;
			Value.Inner = PrimitiveType::Color;
			Value.Number = color.to_ABGR();
		}

		virtual ~Object() { clear(); }

		operator nullptr_t() const { return nullptr; }
		explicit operator bool() const { return !is_null(); }

		template<typename T>
		operator T *() const { return is_pointer() ? (T *)Value.Pointer : nullptr; }

		template<typename T, std::enable_if_t<is_string_type_v<T>, bool> = true>
		operator T() const
		{
			if constexpr(is_string_class_type_v<T>)
				return to_string().move();
			return to_string().c_str()();
		}

		template<typename T, std::enable_if_t<is_numeric_type_v<T>, bool> = true>
		operator T() const
		{
			if(is_char_type_v<T>)
			{
				if(is_string()) return (T)to_string()[0];
			}

			if(is_string() && std::is_same_v<T, bool>)
				return !Value.String.empty();

			return is_number() ? static_cast<T>(Value.Number) : static_cast<T>(0);
		}

		template<typename T>
		friend T &operator <<(T &out, const Object &obj)
		{
			if(obj.is_string())
				out << obj.Value.String.c_str();
			else
				out << obj.Value.Number;
			return out;
		}

		void clear()
		{
			if(Value.Pointer)
			{
				if(Value.Inner == PrimitiveType::Array)
					delete[] Value.Pointer;
				else
					delete Value.Pointer;
			}

			Value.Pointer = nullptr;
			Value.Type = PrimitiveType::Null;
			Value.Inner = PrimitiveType::Null;
		}

		Object& type(PrimitiveType type, PrimitiveType inner = PrimitiveType::Null)
		{
			clear();
			Value.Type = type;
			Value.Inner = inner;
			return *this;
		}

		Object &inner(PrimitiveType type)
		{
			Value.Inner = type;
			return *this;
		}

		Object &&move() noexcept
		{
			return std::move(*this);
		}

		template<typename T, std::enable_if_t<std::_Is_any_of_v<T, Object, string>, bool> = true>
		Object &operator =(T &&rhs) noexcept
		{
			if constexpr(std::is_same_v<T, Object>)
			{
				if(&rhs != this)
				{
					type(rhs.Value.Type, rhs.Value.Inner);

					Value.String = rhs.Value.String.move();
					Value.Number = rhs.Value.Number;
					Value.Pointer = rhs.Value.Pointer;

					rhs.Value.Number = {};
					rhs.Value.Pointer = nullptr;
					rhs.Value.Type = PrimitiveType::Null;
					rhs.Value.Inner = PrimitiveType::Null;
				}
			}
			else if(&rhs != &this->Value.String)
			{
				type(PrimitiveType::String);
				Value.String = rhs.move();
			}
			return *this;
		}

		template<typename T>
		Object &operator =(T const &rhs) noexcept
		{
			if constexpr(std::is_null_pointer_v<T>)
			{
				type(PrimitiveType::Null);
			}
			else if constexpr(is_string_or_char_type_v<T>)
			{
				type(PrimitiveType::String);
				Value.String = rhs;
			}
			else if constexpr(is_numeric_type_v<T>)
			{
				type(PrimitiveType::Number);
				Value.Number = double(rhs);
			}
			else if constexpr(std::is_pointer_v<T> || std::is_same_v<T, pointer_t>)
			{
				if(Value.Pointer != (pointer_t)rhs)
				{
					type(PrimitiveType::Pointer);
					Value.Pointer = (pointer_t)rhs;
				}
			}
			else if constexpr(std::_Is_any_of_v<T, Object, const Object>)
			{
				if(&rhs != this)
				{
					type(rhs.Value.Type, rhs.Value.Inner);
					Value.String = rhs.Value.String;
					Value.Number = rhs.Value.Number;
					Value.Pointer = rhs.Value.Pointer;
				}
			}
			else// wchar[N]
			{
				type(PrimitiveType::String);
				Value.String = (string)rhs;
			}

			return *this;
		}

		template<typename T = uint32_t>
		T to_number() const { return static_cast<T>(is_number() ? Value.Number : 0); }
		
		//0xAARRGGBB
		//0xBBGGRRAA
		Drawing::Color to_color() const
		{
			constexpr auto IDENT_DEFAULT = 0x0885548AU;
			constexpr auto IDENT_AUTO = 0x7C94415EU;
			if(Value.Type == PrimitiveType::Number)
			{
				auto val = static_cast<uint32_t>(Value.Number);
				if(val != IDENT_DEFAULT && val != IDENT_AUTO)
				{
					return val;
					/*if(Value.Inner == PrimitiveType::Color)
						return val;
					return Nilesoft::Drawing::Color::Swap(val);
					Nilesoft::Drawing::Color color(val);
					if(Value.Inner == PrimitiveType::Color)
						return color;// RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr));
					else
						return color;// RGB(GetRValue(clr), GetGValue(clr), GetBValue(clr));
					*/
				}
			}
			return {};
		}

		uint32_t to_colora() const
		{
			constexpr auto IDENT_DEFAULT = 0x0885548AU;
			constexpr auto IDENT_AUTO = 0x7C94415EU;
			if(Value.Type == PrimitiveType::Number)
			{
				auto clr = static_cast<uint32_t>(Value.Number);
				if(clr != CLR_INVALID && clr != IDENT_DEFAULT && clr != IDENT_AUTO)
				{
					if(Value.Inner == PrimitiveType::Color)
						return RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr));
					else
						return RGB(GetRValue(clr), GetGValue(clr), GetBValue(clr));
				}
			}
			return IDENT_DEFAULT;
		}

		bool to_bool() const
		{
			switch(Value.Type)
			{
				case PrimitiveType::String:
					return !Value.String.empty();
				case PrimitiveType::Number:
					return static_cast<bool>(Value.Number);
				case PrimitiveType::Array:
				case PrimitiveType::Pointer:
					return Value.Pointer != nullptr;
			}
			return false;
		}

		virtual string to_string() const
		{
			switch(Value.Type)
			{
				case PrimitiveType::String:
					return Value.String;
				case PrimitiveType::Number:
				{
					if(is_float_point())
						return string().format(L"%g", Value.Number).move();
					else if(Value.Inner == PrimitiveType::Color)
					{
						auto clr = to_color();
						if(clr.a == 255 || clr.a == 0)
							return string().format(L"#%0.6X", clr.to_RGB()).move();
						return string().format(L"#%0.8X", clr.to_ARGB()).move();
					}
					else
						return string().format(sizeof(intptr_t) == sizeof(int32_t) ? L"%d" : L"%lld",
											   static_cast<intptr_t>(Value.Number)).move();
					//auto len = static_cast<size_t>(_scwprintf(L"%g", Value.Double));
					//std::wstring str(std::swprintf(nullptr, 0, L"%g", Value.Double), '\0');
					//string str(std::swprintf(nullptr, 0, L"%g", Value.Double));
					//std::swprintf(str.data(), str.Length() + 1, L"%g", Value.Double);
					//return str;
				}
			}
			return nullptr;
		}

		bool get_bool()
		{
			return static_cast<bool>(Value.Number);
		}

		string &get_string()
		{
			return Value.String;
		}

		template<typename T = Object>
		T *get_pointer() const
		{
			return reinterpret_cast<T *>(is_pointer() ? Value.Pointer : nullptr);
		}

		template<typename T = Object>
		T *ptr() const
		{
			return reinterpret_cast<T *>(is_pointer() ? Value.Pointer : nullptr);
		}

		/*template<typename T = Object>
		T get(uint32_t i, bool move = false) const
		{
			if(is_array(true))
			{
				if(auto ptr = get_pointer<T>(); ptr)
				{
					uint32_t ac = ptr[0];
					if(1 <= ac)
					{
						if(move)
							return ptr[i].move();
						return ptr[i];
					}
				}
			}
			return nullptr;
		}*/

		template<typename T = Object>
		bool get(uint32_t i, T& obj) const
		{
			if(is_array(true))
			{
				if(auto ptr = get_pointer<T>(); ptr)
				{
					if(uint32_t ac = ptr[0]; ac >= i)
					{
						obj = ptr[i];
						return false;
					}
				}
			}
			return false;
		}

		/*
		template<typename T = size_t>
		wchar_t &operator [](T index)
		{
			return m_data[static_cast<size_t>(index)];
		}
		*/

		const Object& operator [](size_t index) const
		{
			if(is_pointer(true))
				return Value.Pointer[index];
			return *this;
		}

		uint32_t length() const
		{
			switch(Value.Type)
			{
				case PrimitiveType::String:
					return Value.String.length<uint32_t>();
				case PrimitiveType::Number:
					return to_string().length<uint32_t>();
			//	case PrimitiveType::Pointer:
				//	return Value.Pointer != nullptr;
			}

			if(is_array(true))
				return Value.Pointer[0];
			return 0u;
		}

		bool is_empty() const
		{
			switch(Value.Type)
			{
				case PrimitiveType::String:
					return Value.String.empty();
				case PrimitiveType::Number:
					return !Value.Number;
				case PrimitiveType::Pointer:
					return !Value.Pointer;
			}
			return true;
		}

		PrimitiveType type() const
		{
			return Value.Type;
		}

		PrimitiveType inner() const
		{
			return Value.Inner;
		}

		bool is_null() const
		{
			if(Value.Type == PrimitiveType::Pointer && !Value.Pointer)
				return true;
			return(Value.Type == PrimitiveType::Null);
		}

		bool is_string() const
		{
			return (Value.Type == PrimitiveType::String);
		}

		bool is_pointer(bool validate = false) const
		{
			if(validate && !Value.Pointer)
				return false;
			return(Value.Type == PrimitiveType::Pointer);
		}

		bool is_array(bool validate = false) const
		{	
			if(validate && !Value.Pointer)
				return false;
			return(Value.Type == PrimitiveType::Pointer && 
				   Value.Inner == PrimitiveType::Array);
		}

		bool is_number() const
		{
			return (Value.Type == PrimitiveType::Number);
		}

		bool is_color() const {
			return (Value.Type == PrimitiveType::Number) && (Value.Inner == PrimitiveType::Color);
		}

		bool is_float_point() const
		{
			return is_number() ? (Value.Number != static_cast<intptr_t>(Value.Number)) : false;
		}

		bool equals(const Object &other) const
		{
			if(this->is_number() && other.is_number())
				return this->to_number<double>() == other.to_number<double>();

			if(this->is_string() && other.is_string())
				return this->to_string().equals(other.to_string(), false);
			return false;
		}

		bool not_default() const
		{
			if(!is_number()) return false;
			return !is_default(static_cast<uint32_t>(Value.Number));
		}

		bool is_default() const
		{
			if(is_null()) return true;
			return is_default(static_cast<uint32_t>(Value.Number));
		}

		static bool is_default(uint32_t value) 
		{
			auto IDENT_DEFAULT = 0x0885548AU;
			auto IDENT_AUTO = 0x7C94415EU;
			return /*value != 0xFFFFFFFF && */value == IDENT_DEFAULT || value == IDENT_AUTO;
		}

		string too(string sep = L"")
		{
			if(is_string() || is_number())
				return to_string().move();
			else if(is_array(true))
			{
				string r;
				auto ptr = get_pointer();
				uint32_t c = ptr[0];
				for(auto i = 0u; i < c; i++)
				{
					r += ptr[i + 1].too().move();
					if(i != c - 1)
						r += sep;
				}
				return r.move();
			}
			return nullptr;
		}

		Object &from(std::vector<string> const &list)
		{
			auto _array = new Object[list.size() + 1]{};
			_array[0] = (uint32_t)list.size();
			
			for(auto i = 0u; i< list.size(); i++)
			{
				try {
					_array[i + 1] = list[i];
				}
				catch(...) {}
			}

			Object result(_array, true);
			*this = result.move();

			return *this;
		}

		/*bool is_default() const {

			constexpr auto IDENT_DEFAULT = 0x0885548AU;
			auto val = static_cast<uint32_t>(Value.Number);
			return (is_null() || !is_number() || val == 0xFFFFFFFF || val == IDENT_DEFAULT);
		}

		bool is_auto() const
		{
			if(!is_number()) return false;
			auto IDENT_DEFAULT = 0x0885548AU;
			auto IDENT_AUTO = 0x7C94415EU;
			auto val = static_cast<uint32_t>(Value.Number);
			return (val == 0xFFFFFFFF || val == IDENT_DEFAULT || val == IDENT_AUTO);
		}*/
	};
}