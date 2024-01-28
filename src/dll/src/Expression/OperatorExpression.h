#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class OperatorExpression abstract : public Expression
		{
		public:
			OperatorType Operator;

		public:
			OperatorExpression(OperatorType operatorType)
				: Operator{ operatorType }
			{
			}

			virtual ~OperatorExpression() = default;
		};

		class UnaryExpression final : public OperatorExpression
		{
		public:
			Expression *Operand = nullptr;

		public:
			UnaryExpression(OperatorType operatorType, Expression *operand =nullptr, Expression *parent = nullptr) :
				OperatorExpression(operatorType), Operand{ operand }
			{
				Parent = parent;
			}

			~UnaryExpression()
			{
				delete Operand;
			}

			ExpressionType Type() const override
			{
				return ExpressionType::UnaryOperator;
			}

			Expression *Copy() override
			{
				return new UnaryExpression(Operator, Operand->Copy());
			}

			Object Eval(Context *context) override;
		};

		class BinaryExpression final : public OperatorExpression
		{
		public:
			Expression *Left = nullptr;
			Expression *Right = nullptr;

		public:
			BinaryExpression(OperatorType op, Expression *lhs = nullptr, Expression *rhs = nullptr, Expression *parent = nullptr)
				: OperatorExpression(op), Left{ lhs }, Right{ rhs }
			{
				if(Left) Left->Parent = this;
				if(Right) Right->Parent = this;
				Parent = parent;
			}

			~BinaryExpression()
			{
				delete Left;
				delete Right;
			}

			ExpressionType Type() const override
			{
				return ExpressionType::BinaryOperator;
			}

			Expression *Copy() override
			{
				return new BinaryExpression(Operator, Left->Copy(), Right->Copy());
			}

			//virtual const Object &Eval() override;
			Object Eval(Context *context) override;
		};
	}
}