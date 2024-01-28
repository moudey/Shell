#include <pch.h>

#include "Expression\Constants.h"
#include "Expression\Variable.h"

namespace Nilesoft
{
	namespace Shell
	{
		Expression *ArrayExpression::get(size_t index)
		{
			if(index < expressions.size())
				return expressions[index];
			return nullptr;
		}

		void ArrayExpression::push_back(Expression *e)
		{
			if(e)
			{
				e->Parent = this;
				expressions.push_back(e);
			}
		}

		void ArrayExpression::push_back(std::unique_ptr<Expression> &&e)
		{
			if(e)
			{
				e->Parent = this;
				expressions.push_back(e.release());
			}
		}

		ExpressionType ArrayExpression::Type() const
		{
			return ExpressionType::Array;
		}

		Expression *ArrayExpression::Copy()
		{
			std::unique_ptr<ArrayExpression> a(new ArrayExpression(Parent));
			for(auto e : expressions) a->push_back(e->Copy());
			return a.release();
		}

		Object ArrayExpression::Eval(Context *context)
		{
			auto array = new Object[expressions.size() + 1];
			Object result(array, true);
			*array++ = expressions.size();
			for(auto &e : expressions)
			{
				try{
					*array++ = context->Eval(e).move();
				}
				catch(...) {}
			}
			return result.move();
		}
	}
}