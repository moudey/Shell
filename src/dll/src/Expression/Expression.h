#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		/*
		Serialization and Unserialization  
		serialize 
		deserialize
		*/ 
		/*
		auto x = new VariableExpression("X");
		auto y = new VariableExpression("Y");

		auto expression = new OrExpression(
		  new AndExpression (new Constant(true), x),
		  new AndExpression (y, new NotExpression(x))
		);

		context.AssignX(x, true);
		context.AssignY(y, false);

		bool result = expression->Evaluate(context);
		*/

		class Expression abstract
		{
		protected:
			Expression() = default;

		public:
			virtual ~Expression() = default;

			Scope		Scope;
			Expression *Parent = nullptr;

			// Returns the type of expression.
			virtual ExpressionType Type() const = 0;
			virtual Expression *Copy() = 0;//Clone

			// Interpreter evaluate
			// Interpret evaluate
			//Interpret | Eval
			//evaluate expressions
			//Evaluate an expression. Returns the result of the expression.
			virtual Object Eval(Context *context) = 0;
			//const Object & Interpret(context);

			bool IsIdent() const;
			bool IsVariable() const;
			bool IsAssign() const;
			bool IsLiteral() const;

			class LiteralExpression *Literal();
			class IdentExpression *ident();
			class AssignExpression *assign();
			class FuncExpression *identifier();

			void set_parent(Expression *parent)
			{
				if(!Parent) Parent = parent;
			}
		};

		struct auto_expr
		{
			Expression *_e{};
			auto_expr() = default;
			auto_expr(Expression *e) : _e{ e } {}
			auto_expr(auto_expr &&other) noexcept { *this = std::move(other); }
			virtual ~auto_expr() { delete _e; }

			operator Expression *() { return _e; }
			operator Expression *() const { return _e; }

		//	operator const Expression *() { return _e; }
		//	operator const Expression *() const { return _e; }

			explicit operator bool() const { return _e != nullptr; }

			Expression *get() { return _e; }

			void reset(Expression *e = nullptr) { delete _e; _e = e; }
			Expression *release() { auto e = _e; _e = nullptr; return e; }
			auto_expr &&move() noexcept { return std::move(*this); }

			auto_expr &operator =(Expression *rhs) noexcept
			{
				if(_e != rhs)
				{
					delete _e;
					_e = rhs;
				}
				return *this;
			}
			auto_expr &operator =(auto_expr &&rhs) noexcept
			{
				if(_e != rhs._e)
				{
					delete _e;
					_e = rhs._e;
					rhs._e = {};
				}
				return *this;
			}
		};

		class NullExpression : public Expression
		{
			ExpressionType Type() const override { return ExpressionType::Null; }
			Expression *Copy() override { return new NullExpression(); }
			Object Eval(Context *) override { return nullptr; }
		};


		//(condition ? expressionTrue : expressionFalse)
		// (condition) ? expressionTrue : expressionFalse;
		//Conditional or Ternary Operator
		class TernaryExpression : public Expression
		{
		public:
			TernaryExpression() = default;
			TernaryExpression(Expression *condition) 
				: Condition{ condition } { }

			TernaryExpression(Expression *condition, Expression *true_stmt, Expression *false_stmt)
				: Condition{ condition }, True{ true_stmt }, False{ false_stmt }
			{
			}

			~TernaryExpression()
			{
				delete Condition;
				delete True;
				delete False;
			}

			ExpressionType Type() const override { return ExpressionType::Ternary; }
			Expression *Copy() override
			{
				auto ternary = new TernaryExpression(Condition->Copy());
				if(ternary->True)
					ternary->True = True->Copy();
				if(ternary->False)
					ternary->False = False->Copy();
				return ternary;
			}

			Object Eval(Context *context) override
			{
				Object condition = context->Eval(Condition).move();
				if(condition.to_bool())
					return context->Eval(True).move();
				return context->Eval(False).move();
			}

			Expression *Condition = nullptr;
			Expression *True = nullptr;
			Expression *False = nullptr;
		};
	}
}