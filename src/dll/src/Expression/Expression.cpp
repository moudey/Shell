#include <pch.h>

namespace Nilesoft
{
	namespace Shell
	{
		//Interpret | Eval
		//evaluate expressions
		//Evaluate an expression. Returns the result of the expression.

		bool Expression::IsIdent() const { return Type() == ExpressionType::Identifier; }
		bool Expression::IsVariable() const
		{
			return
				Type() == ExpressionType::Variable ||
				Type() == ExpressionType::RuntimeVariable;
		}

		bool Expression::IsAssign() const
		{
			return Type() == ExpressionType::Assign;
		}

		bool Expression::IsLiteral() const
		{
			return Type() == ExpressionType::String ||
				Type() == ExpressionType::Number;
		}

		LiteralExpression *Expression::Literal()
		{
			return reinterpret_cast<LiteralExpression *>(this);
		}

		IdentExpression *Expression::ident()
		{
			return reinterpret_cast<IdentExpression *>(this);
		}

		AssignExpression *Expression::assign()
		{
			return reinterpret_cast<AssignExpression *>(this);
		}

		FuncExpression *Expression::identifier()
		{
			return reinterpret_cast<FuncExpression *>(this);
		}

		Object StringExpression::Eval(Context *)
		{
			return Value;
		}

		Object NumberExpression::Eval(Context *)
		{
			return Value;
		}

		Object Array2Expression::Eval(Context *)
		{
			return Value;
		}
		
		Object StatementExpression::Eval(Context *context)
		{
			Object _result;
			auto const &size = Body.size();
			if(size == 0)
				_result.type(PrimitiveType::String);
			else
			{
				try 
				{
					if(!HasReturn)
					{
						for(auto e : Body)
						{
							try {
								context->Eval(e);
							}
							catch(...)
							{
							}
						}
						return _result;
					}

					if(size == 1)
						_result = context->Eval(Body[0]).move();
					else
					{
						string str;
						Object r;
						for(auto e : Body)
						{
							try {
								r = context->Eval(e).move();
								if(!r.is_null())
									str.append(r.to_string());
							}
							catch(...) {}
						}
						_result = str.move();
					}
				}
				catch(...) 
				{
				}
			}
			return _result.move();
		}

		Object VariableExpression::Eval(Context *context)
		{
			if(context && context->variables.global)
			{
				if(auto variable = context->get_variable(Id, this); variable)
				{
					Object obj = context->Eval(variable).move();
					return context->eval_object(Array, obj).move();
				}
			}
			return nullptr;
		}

		Object AssignExpression::Eval(Context *context)
		{
			Object obj = context->Eval(Right).move();
			context->set_variable2(Id, this, obj.move());
			return nullptr;
		}

		Object ForStatement::Eval(Context *context)
		{
			string body;

			if(this->Init)
			{
				std::vector<string> list;

				auto var = context->get_variable(this->Init, this)->Copy();
				int x = 0;

				for(int i = context->Eval(var).move();
					context->Eval(Condition).to_bool(); x++)
				{
					if(x > 1000) break;

					if(Body1)
					{
						Object o = context->Eval(Body1).move();
						list.push_back(o.to_string().move());
					}
					else for(auto e : Body2->Body)
					{
						try {
							Object o = context->Eval(e).move();
							if(o.is_number())
							{
								uint32_t id = o;
								if(id == IDENT_BREAK or context->Break)
								{
									context->Break = false;
									goto end;
								}
								else if(id == IDENT_CONTINUE or context->Continue)
								{
									context->Continue = false;
									goto skip;
								}
							}
						}
						catch(...)
						{
						}
					}

				skip:
					Scope.set(Init, ++i);
				}
			end:
				Scope.set(Init, var);
				if(Body1)
				{
					for(auto &s : list)
						body += s;
					return body.move();
				}
			}
			return nullptr;
		}
	}
}