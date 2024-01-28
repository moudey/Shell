#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class ArrayExpression : public Expression
		{
		private:
			std::vector<Expression *> expressions;

		public:
			ArrayExpression(Expression *parent = nullptr) { Parent = parent; }
			virtual ~ArrayExpression() { for(auto e : expressions) delete e; }

			ExpressionType Type() const override;
			Expression *Copy() override;
			Object Eval(Context *context) override;

			Expression *get(size_t index);
			bool size() const { return expressions.size(); }
			bool empty() const { return expressions.empty(); }
			void push_back(Expression *e);
			void push_back(std::unique_ptr<Expression> &&e);
		};
	}
}