#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class StatementExpression : public Expression
		{
		public:
			std::vector<Expression *> Body;
			bool HasReturn = false;

		public:
			StatementExpression(Expression *parent = nullptr, bool has_return = false) 
			{
				Parent = parent; 
				HasReturn = has_return;
			}

			virtual ~StatementExpression() { for(auto e : Body) delete e; }

			ExpressionType Type() const override
			{
				return ExpressionType::Statement;
			}

			Expression *Copy() override
			{
				auto stmt = new StatementExpression(Parent);
				for(auto e : Body) stmt->push_back(e->Copy());
				return stmt;
			}

			virtual Object Eval(Context *context) override;

			bool size() const { return Body.size(); }
			bool empty() const { return Body.empty(); }
			
			void push_back(Expression *e) 
			{ 
				if(e) 
				{ 
					e->Parent = this; 
					Body.push_back(e);
				} 
			}
			
			void push_back(std::unique_ptr<Expression> &&e)
			{
				if(e)
				{
					e->Parent = this;
					Body.push_back(e.release());
				}
			}

			void push_back(const string &value)
			{
				if(!value.empty()) push_back(new StringExpression(value));
			}

			void push_back(string &&value)
			{
				if(!value.empty()) push_back(new StringExpression(value.move()));
			}
		};
	}
}