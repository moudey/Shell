#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class IdentExpression abstract: public Expression
		{
		public:
			Ident Id{};
			bool Brackets = false;
			Expression *Array = nullptr;

		public:
			IdentExpression(const Ident &ident) : Id{ ident }
			{
			}

			virtual ~IdentExpression() = default;
		};

		class VariableExpression final : public IdentExpression
		{
		public:

			VariableExpression(const Ident &ident, Expression *parent = nullptr)
				: IdentExpression(ident)
			{
				Parent = parent;
			}

			~VariableExpression() = default;

			ExpressionType Type() const
			{
				return ExpressionType::Variable;
			}

			Expression *Copy() override
			{
				auto e = new VariableExpression(Id, Parent);
				e->Array = Array ? Array->Copy() : nullptr;
				e->Brackets = Brackets;
				return e;
			}

			Object Eval(Context *context) override;
		};

		class AssignExpression final : public IdentExpression
		{
		public:
			Expression *Right = nullptr;

		public:

			AssignExpression(const Ident &ident, Expression *right, Expression *parent = nullptr)
				: IdentExpression(ident), Right{ right }
			{
				Parent = parent;
			}

			~AssignExpression() { delete Right; }

			ExpressionType Type() const
			{
				return ExpressionType::Assign;
			}

			Expression *Copy() override
			{
				return new AssignExpression(Id, Right->Copy(), Parent);
			}

			Object Eval(Context *context) override;
		};

		class ForStatement final : public Expression
		{
		public:
			ForStatement(Expression *parent = nullptr)
			{
				Parent = parent;
			}

			~ForStatement()
			{
				delete Condition;
				delete Iterator;
				delete Body1;
				delete Body2;
			}

			ExpressionType Type() const override { return ExpressionType::For; }

			Expression *Copy() override
			{
				auto stmt = new ForStatement();

				stmt->Init = Init;

				if(Condition)
					stmt->Condition = Condition->Copy();

				if(Iterator)
					stmt->Iterator = Iterator->Copy();

				if(Body1)
					stmt->Body1 = (StatementExpression *)Body1->Copy();

				if(Body2)
					stmt->Body2 = (StatementExpression *)Body2->Copy();

				return stmt;
			}

			Object Eval(Context *context) override;

			uint32_t Init = 0;
			Expression *Condition = nullptr;
			Expression *Iterator = nullptr;
			Expression *Body1 = nullptr;
			StatementExpression *Body2 = nullptr;
		};

		class FuncExpression final : public IdentExpression
		{
			Object _result;
			Context *context = nullptr;
			struct CACHE* cache = nullptr;
		
			bool eval = false;

		public:
			bool ischild = false;
			bool extented = false;

			std::vector<Expression *> Arguments{ };
			Shell::Scope Variables;

			FuncExpression *Child = nullptr;

		public:

			FuncExpression(const Ident &ident, Expression *parent = nullptr)
				: IdentExpression(ident)
			{
				Parent = parent;
			}

			~FuncExpression()
			{
				for(auto e : Arguments) delete e;
				
				delete Array;
				delete Child;
			}

			ExpressionType Type() const override
			{
				return ExpressionType::Identifier;
			}

			void SetChild(Expression* child)
			{
				if(child)
				{
					Child = (FuncExpression *)child;
					Child->Parent = this;
					Child->ischild = true;
				}
			}

			Expression *Copy() override
			{
				auto func = new FuncExpression(Id, Parent);
				for(auto e : Arguments)
					func->Arguments.push_back(e->Copy());
				
				if(Child)
				{
					func->Child = (FuncExpression *)Child->Copy();
					func->Child->ischild = true;
					func->Child->Parent = func;
					func->Array = Array->Copy();
				}

				return func;
			}

			Object Eval(Context *context) override;

			// Add argument
			void push_back(Expression *e)
			{
				e->Parent = this;
				Arguments.push_back(e);
			}

			void push_front(Expression *e)
			{
				e->Parent = this;
				if(Arguments.size() == 0)
					Arguments.push_back(e);
				else
					Arguments.insert(Arguments.begin(), e);
			}

			Object operator()(size_t arg);
			Expression *operator[](size_t arg);

			// Evaluate arguments
			Object eval_arg(size_t arg);

			template<typename T>
			T eval_arg(size_t arg, T const &default_value);

		private:
			void eval_sel();
			void eval_path();
			void eval_io();
			void eval_user();
			void eval_sys();
			void eval_app();
			void eval_str();
			void eval_for();
			void eval_foreach();
			void eval_reg();
			void eval_msg();
			void eval_icon();

			Object eval_key();
			Object eval_color();
			bool eval_var(Object &);
			bool eval_var(uint32_t id, Object &obj);
		};

		/*class IfExpression : public Expression
		{
			Context *context = nullptr;
			Object _result;

		private:
			IfExpression() = default;

		public:
			Expression *Expr = nullptr;
			Expression *Stmt = nullptr;
			Expression *ElseStmt = nullptr;

		public:

			IfExpression()
			{
			}

			~IfExpression()
			{
				delete Expr;
				delete Stmt;
				delete ElseStmt;
			}

			ExpressionType Type() const override
			{
				return ExpressionType::Identifier;
			}

			Expression *Copy() override
			{
				auto ifexpr = new IfExpression();
				if(Expr)
					ifexpr->Expr = Expr->Copy();
				if(Stmt)
					ifexpr->Stmt = Stmt->Copy();
				if(ElseStmt)
					ifexpr->ElseStmt = ElseStmt->Copy();
				return ifexpr;
			}

			Object Eval(Context *context) override;
		private:
		};*/
	}
}