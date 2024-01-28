#include <pch.h>

namespace Nilesoft
{
	namespace Shell
	{
		void Scope::clear(bool destroy)
		{
			if(destroy)
			{
				for(auto &it : _dispatch) delete it.second;
			}
			_dispatch.clear();
		}

		auto Scope::find(uint32_t id)
		{
			return _dispatch.find(id);
		}

		Expression *Scope::to_expr(const Object &obj)
		{
			if(obj.is_string())
				return new StringExpression(obj.to_string());
			else if(obj.is_color())
				return new NumberExpression(obj.to_number(), PrimitiveType::Color);
			else if(obj.is_number())
				return new NumberExpression(obj.to_number<double>());
			else if(obj.is_array(true))
			{
				std::unique_ptr<ArrayExpression> array(new ArrayExpression);
				auto ptr = obj.get_pointer();
				uint32_t argc = ptr[0];
				for(auto i = 0u; i < argc; i++)
					array->push_back(to_expr(ptr[i + 1]));
				return array.release();
			}
			return new NullExpression;
		}

		void Scope::set(uint32_t id, Expression *e)
		{
			auto pair = _dispatch.find(id);
			if(pair != _dispatch.end())
			{
				if(e == nullptr)
				{
					delete std::exchange(pair->second, nullptr);
					_dispatch.erase(pair);
				}
				else if(pair->second != e)
				{
					delete pair->second;
					pair->second = e;
				}
			}
			else if(e != nullptr)
			{
				_dispatch[id] = e;
			}
		}

		void Scope::set(uint32_t id, Object &value)
		{
			auto pair = _dispatch.find(id);
			if(pair != _dispatch.end())
				delete pair->second;
			_dispatch.insert_or_assign(id, to_expr(value));
		}

		void Scope::set(uint32_t id, double value)
		{
			auto pair = _dispatch.find(id);
			if(pair == _dispatch.end())
				_dispatch[id] = new NumberExpression(value);
			else if(pair->second->Type() == ExpressionType::Number)
			{
				auto e = (NumberExpression *)pair->second;
				e->Value = value;
			}
			else
			{
				delete pair->second;
				pair->second = new NumberExpression(value);
			}
		}

		void Scope::set(uint32_t id, const string &value)
		{
			auto pair = _dispatch.find(id);
			if(pair == _dispatch.end())
				_dispatch[id] = new StringExpression(value);
			else if(pair->second->Type() == ExpressionType::String)
			{
				auto e = (StringExpression *)pair->second;
				e->Value = value;
			}
			else
			{
				delete pair->second;
				pair->second = new StringExpression(value);
			}
		}

		void Scope::erase(uint32_t id)
		{
			auto pair = _dispatch.find(id);
			if(pair != _dispatch.end())
			{
				delete std::exchange(pair->second, nullptr);
				_dispatch.erase(pair);
			}
		}

		bool Scope::exists(uint32_t id, bool hierarchy)
		{
			bool result = _dispatch.find(id) != _dispatch.end();
			if(!hierarchy || result)
				return result;

			auto parent = Parent;
			while(parent)
			{
				result = parent->exists(id, hierarchy);
				if(result) break;
				parent = parent->Parent;
			}
			return result;
		}

		//bool Scope::exists(uint32_t id) { return _dispatch.find(id) != _dispatch.end(); }
		size_t Scope::size() const { return _dispatch.size(); }

		Expression *Scope::at(uint32_t id)
		{
			if(!_dispatch.empty())
			{
				if(auto v = _dispatch.find(id); v != _dispatch.end())
					return v->second;
			}
			return nullptr;
		}

		Expression *Scope::operator [](uint32_t id)
		{
			return at(id);
		}
	}
}