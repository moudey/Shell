#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class Scope
		{
			//using pair = std::pair<uint32_t, Expression *>;
			std::unordered_map<uint32_t, class Expression *> _dispatch;
			
		public:
			Scope *Parent = nullptr;

			Scope() = default;
			~Scope() { clear(true); }

			void clear(bool destroy);
			auto find(uint32_t id);
			void set(uint32_t id, Expression *e);
			void set(uint32_t id, Object &value);
			void set(uint32_t id, double value);
			void set(uint32_t id, const string &value);
			void erase(uint32_t id);
			bool exists(uint32_t id, bool hierarchy = false);
			size_t size() const;
			Expression *at(uint32_t id);
			Expression *operator [](uint32_t id);

			Expression *to_expr(const Object &obj);
		};

		struct tagVariables 
		{
			Scope *global = nullptr;
			Scope *local = nullptr;
			Scope *runtime = nullptr;
		};
	}
}