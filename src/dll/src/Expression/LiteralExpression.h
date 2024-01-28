#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class LiteralExpression : public Expression
		{
		public:
			LiteralExpression(const Object &value) : Value{ value } {}
			LiteralExpression(Object &&value) : Value{ std::move(value) } {}
			virtual ~LiteralExpression() = default;

		public:
			Object Value;
		};

		class StringExpression final : public LiteralExpression
		{
		public:
			StringExpression(const Object &value) : LiteralExpression(value) {}
			StringExpression(const string &value) : LiteralExpression(value) {}
			StringExpression(string &&value) : LiteralExpression(value.move()){}
			StringExpression(std::wstring const &value) : LiteralExpression(value) {}
			//StringExpression(std::wstring &&value) : LiteralExpression(std::move(value)) {}
			StringExpression(const wchar_t *value) : LiteralExpression(value) {}
			StringExpression(wchar_t value) : LiteralExpression(value) {}
			StringExpression(char value) : LiteralExpression(value) {}

			ExpressionType Type() const override
			{
				return ExpressionType::String;
			}

			Expression *Copy() override
			{
				return new StringExpression(Value);
			}

			Object Eval(Context *context) override;
		};

		class NumberExpression final : public LiteralExpression
		{
		public:
			template<typename T>
			NumberExpression(T value)
				: LiteralExpression(value){ }

			template<typename T>
			NumberExpression(T value, PrimitiveType inner)
				: LiteralExpression(value)
			{
				Value.Value.Inner = inner;
			}

			ExpressionType Type() const override
			{
				return ExpressionType::Number;
			}

			Expression *Copy() override
			{
				return new NumberExpression(Value);
			}

			Object Eval(Context *context) override;
		};

		class Array2Expression final : public LiteralExpression
		{
		public:
			Array2Expression(const Object &value) : LiteralExpression(value) {}
			Array2Expression(Object &&value) : LiteralExpression(value.move()) {}

			ExpressionType Type() const override
			{
				return ExpressionType::Literal;
			}

			Expression *Copy() override
			{
				return new NumberExpression(Value);
			}

			Object Eval(Context *context) override;
		};

		/*class ColorExpression final : public LiteralExpression
		{
		public:
			template<typename T>
			ColorExpression(T value)
				: LiteralExpression(value)
			{
			}

			ExpressionType Type() const override
			{
				return ExpressionType::Color;
			}

			Expression *Copy() override
			{
				return new NumberExpression(Value);
			}

			//virtual const Object &Eval() override;
			Object Eval(Context *context) override;
		};*/
	}
}