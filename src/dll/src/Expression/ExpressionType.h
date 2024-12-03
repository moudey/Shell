#pragma once
namespace Nilesoft
{
	namespace Shell
	{
		enum class ExpressionType
		{
			Invalid = -1,
			None,
			Null,
			Literal,
			String,
			Number,
			Color,
			Array,
			Statement,
			BinaryOperator,
			UnaryOperator,
			Ternary,		//Operator (?:)
			
			Identifier,
			Function,
			Variable,
			RuntimeVariable,
			For,
			ForEach,
			Assign,			//An assignment operation, such as (a = b).
			StringExt
			/*
			Constant,		//A constant value.
			Add,			//An addition operation, such as a + b, without overflow checking, for numeric operands.
			AddAssign,		//An addition compound assignment operation, such as (a += b), without overflow checking, for numeric operands.
			And,			//A bitwise or logical AND operation, such as (a & b) in C# and (a And b) in Visual Basic.
			AndAlso,		//A conditional AND operation that evaluates the second operand only if the first operand evaluates to true. It corresponds to (a && b) in C#.
			AndAssign,		//A bitwise or logical AND compound assignment operation, such as (a &= b) in C#
			Call,			//A method call, such as in the obj.sampleMethod() expression.
			Coalesce,		//A node that represents a null coalescing operation, such as(a ? ? b) in C# or If(a, b) in Visual Basic.
			Conditional,	//A conditional operation, such as a > b ? a : b in C# or If(a > b, a, b) in Visual Basic.
			Divide,			//A division operation, such as(a / b), for numeric operands.
			DivideAssign, 	//An division compound assignment operation, such as(a /= b), for numeric operands.
			Equal, 			//A node that represents an equality comparison, such as(a == b) in C# or (a = b) in Visual Basic.
			ExclusiveOr, 	//A bitwise or logical XOR operation, such as(a ^b) in C# or (a Xor b) in Visual Basic.
			ExclusiveOrAssign, 	//A bitwise or logical XOR compound assignment operation, such as(a ^= b) in C#.
			GreaterThan, 		//A "greater than" comparison, such as(a > b).
			GreaterThanOrEqual,	//A "greater than or equal to" comparison, such as(a >= b).
			IsFalse, 		//A false condition value.
			IsTrue,			//A true condition value.
			LeftShift,		//A bitwise left - shift operation, such as(a << b).
			LeftShiftAssign, 	//A bitwise left - shift compound assignment, such as(a <<= b).
			LessThan, 		//A "less than" comparison, such as(a < b).
			LessThanOrEqual,//A "less than or equal to" comparison, such as(a <= b).
			Loop,			//A loop, such as for or while.
			MemberAccess,	//An operation that reads from a field or property, such as obj.SampleProperty.
			Modulo,			//An arithmetic remainder operation, such as(a %b) in C# or (a Mod b) in Visual Basic.
			ModuloAssign, 	//An arithmetic remainder compound assignment operation, such as(a %= b) in C#.
			Multiply,		//A multiplication operation, such as(a *b), without overflow checking, for numeric operands.
			MultiplyAssign,	//A multiplication compound assignment operation, such as(a *= b), without overflow checking, for numeric operands.
			MultiplyChecked,//An multiplication operation, such as(a *b), that has overflow checking, for numeric operands.
			Negate,			//An arithmetic negation operation, such as(-a).The object a should not be modified in place.
			Not,			//A bitwise complement or logical negation operation.In C#, it is equivalent to(~a) for integral types and to(!a) for Boolean values.In Visual Basic, it is equivalent to(Not a).The object a should not be modified in place.
			NotEqual,		//An inequality comparison, such as(a != b) in C# or (a <> b) in Visual Basic.
			OnesComplement,	//A ones complement operation, such as(~a) in C#.
			Or,				//A bitwise or logical OR operation, such as(a | b) in C# or (a Or b) in Visual Basic.
			OrAssign,		//A bitwise or logical OR compound assignment, such as(a |= b) in C#.
			OrElse,			//A short - circuiting conditional OR operation, such as(a || b) in C# or (a OrElse b) in Visual Basic.
			Power,			//A mathematical operation that raises a number to a power, such as(a ^b) in Visual Basic.
			PowerAssign,	//A compound assignment operation that raises a number to a power, such as(a ^= b) in Visual Basic.
			Quote,			//An expression that has a constant value of type Expression.A Quote node can contain references to parameters that are defined in the context of the expression it represents.
			RightShift,		//A bitwise right - shift operation, such as(a >> b).
			RightShiftAssign,//A bitwise right - shift compound assignment operation, such as(a >>= b).
			//RuntimeVariables,//A list of run - time variables.For more information, see RuntimeVariablesExpression.
			Subtract,		//A subtraction operation, such as(a - b), without overflow checking, for numeric operands.
			SubtractAssign,//A subtraction compound assignment operation, such as(a -= b), without overflow checking, for numeric operands.
			UnaryPlus		//A unary plus operation, such as(+a).The result of a predefined unary plus operation is the value of the operand, but user - defined implementations might have unusual results.
		*/
		};


		enum class OperatorType : int
		{
			None,
			Invalid = None,
			Comma,
			Equal,		// =, *=, /=, %=, +=, -=, <<=, >>=, &=, ^=, |=	 Right-to-left
			MinusEqual,
			PlusEqual,
			
			Question,		// ?	Ternary conditional
			LogicalOr,		// ||	Logical or
			LogicalAnd,		// &&	Logical and
			BitwiseOr,		// |	Inclusive or
			BitwiseXor,		// ^	Exclusive or
			BitwiseAnd,		// &	Bitwise and

			// Equality
			EqualEqual,		// ==
			NotEqual,		// !=

			// Comparison Operations
			GreaterThan,
			GreaterThanOrEqual,
			LessThan,
			LessThanOrEqual,

			// Shift Operations
			BitwiseLeftShift,
			BitwiseRightShift,

			// Additive
			Plus,		// +
			Minus,	// -

			PlusPlus,
			MinusMinus,

			// Multiplicative
			Multiply,
			Divide,
			Modulo,
			//
			// Right-to-left 
			//
			BitwiseNot,		// ~
			LogicalNot,		// !

			// Unary Operations
			UnaryPlus,		// +a
			UnaryMinus,		// -a
			UnaryNot,		// !a
			UnaryNegation,	// ~a

			// Prefix increment and decrement
			PrefixInc,		// --a
			PrefixDec,		// ++a

			// Left-to-right 

			//Suffix / postfix increment and decrement a++,a--
			PostfixInc,		// a--
			PostfixDec,		// a++
			//a() 	Function call
			//a[] 	Subscript
			//.		Member access
			MemberAccess	// ., a[], a()
		};

		// PrecedenceLevels - These are precedences for the binary/ternary
// operators in the C99 grammar.  These have been named to relate
// with the C99 grammar productions.  Low precedences numbers bind
// more weakly than high numbers.
		/*enum class PrecedenceLevels
		{
			Unknown = 0,			// Not binary operator.
			Comma = 1,				// ,
			Assignment = 2,			// =, *=, /=, %=, +=, -=, <<=, >>=, &=, ^=, |=	 Right-to-left 
			Conditional = 3,		// ?	 Right-to-left 
			LogicalOr = 4,			// ||
			LogicalAnd = 5,			// &&
			BitwiseOr = 6,			// |
			BitwiseXor = 7,			// ^
			BitwiseAnd = 8,			// &
			Equality = 9,			// ==, !=
			Relational = 10,		//  >=, <=, >, <	Comparison
			Shift = 12,				// <<, >>
			Additive = 13,			// -, +
			Multiplicative = 14,	// *, /, %
			Unaray = 15,			// !a, ~a,  -a, +a	Right-to-left 
			MemberAccess = 16		// ., a[], a()
		};*/

		/*
static bool IsRightAssociative(SyntaxKind op)
		{
			switch (op)
			{
				case SyntaxKind.SimpleAssignmentExpression:
				case SyntaxKind.AddAssignmentExpression:
				case SyntaxKind.SubtractAssignmentExpression:
				case SyntaxKind.MultiplyAssignmentExpression:
				case SyntaxKind.DivideAssignmentExpression:
				case SyntaxKind.ModuloAssignmentExpression:
				case SyntaxKind.AndAssignmentExpression:
				case SyntaxKind.ExclusiveOrAssignmentExpression:
				case SyntaxKind.OrAssignmentExpression:
				case SyntaxKind.LeftShiftAssignmentExpression:
				case SyntaxKind.RightShiftAssignmentExpression:
				case SyntaxKind.CoalesceAssignmentExpression:
				case SyntaxKind.CoalesceExpression:
					return true;
				default:
					return false;
			}
		}
		enum Precedence : uint
		{
			Expression = 0, // Loosest possible precedence, used to accept all expressions
			Assignment = Expression,
			Lambda = Assignment, // "The => operator has the same precedence as assignment (=) and is right-associative."
			Conditional,
			Coalescing,
			ConditionalOr,
			ConditionalAnd,
			LogicalOr,
			LogicalXor,
			LogicalAnd,
			Equality,
			Relational,
			Shift,
			Additive,
			Mutiplicative,
			Switch,
			Range,
			Unary,
			Cast,
			PointerIndirection,
			AddressOf,
			Primary,
		}

		*/
	}
}