#include <pch.h>

namespace Nilesoft
{
	namespace Shell
	{
		Object UnaryExpression::Eval(Context *context)
		{
			Object _result, operand = context->Eval(Operand).move();
			if(!operand.is_null())
			{
				if(operand.is_string())
				{
					if(Operator == OperatorType::UnaryNot)
						_result = operand.Value.String.empty();
					else
						Logger::Debug(L"Invalid unary operator type.");
				}
				else switch(Operator)
				{
					case OperatorType::UnaryPlus:
						_result = +(operand.Value.Number);
						break;
					case OperatorType::UnaryMinus:
						_result = -(operand.Value.Number);
						break;
					case OperatorType::UnaryNegation:
						_result = ~operand.to_number<intptr_t>();
						break;
					case OperatorType::UnaryNot:
						_result = !operand.to_number<double>();
						break;
					default:
						Logger::Debug(L"Invalid unary operator type.");
						//throw Exception("Invalid unary operator type.");
						break;
				}
			}
			return _result.move();
		}

		Object BinaryExpression::Eval(Context *context)
		{
			Object _result;

			try
			{
				Object lhs = context->Eval(Left).move();

				if(this->Operator == OperatorType::LogicalAnd)
				{
					if(!lhs.to_bool())
					{
						_result = false;
						return _result.move();
					}
				}
				else if(this->Operator == OperatorType::LogicalOr)
				{
					if(lhs.to_bool())
					{
						_result = true;
						return _result.move();
					}
				}

				Object rhs = context->Eval(Right).move();

				if(!lhs.is_number() || !rhs.is_number())
				{
					switch(this->Operator)
					{
						case OperatorType::Plus:
							_result = (lhs.to_string() + rhs.to_string()).move();
							break;
						case OperatorType::EqualEqual:
							_result = lhs.to_string().equals(rhs.to_string());
							break;
						case OperatorType::NotEqual:
							_result = !lhs.to_string().equals(rhs.to_string());
							break;
						case OperatorType::LogicalOr:
							if(lhs.is_number())
								_result = lhs.to_number<size_t>() || rhs.to_string().length();
							else if(rhs.is_number())
								_result = lhs.to_string().length() || rhs.to_number<size_t>();
							else
								_result = lhs.to_string().length() || rhs.to_string().length();
							break;
						case OperatorType::LogicalAnd:
							if(lhs.is_number())
								_result = lhs.to_number<size_t>() && rhs.to_string().length();
							else if(rhs.is_number())
								_result = lhs.to_string().length() && rhs.to_number<size_t>();
							else
								_result = lhs.to_string().length() && rhs.to_string().length();
							break;
						case OperatorType::GreaterThan:
							if(lhs.is_number())
								_result = lhs.to_number<size_t>() > rhs.to_string().length();
							else if(rhs.is_number())
								_result = lhs.to_string().length() > rhs.to_number<size_t>();
							else
								_result = lhs.to_string().length() > rhs.to_string().length();
							break;
						case OperatorType::LessThan:
							if(lhs.is_number())
								_result = lhs.to_number<size_t>() < rhs.to_string().length();
							else if(rhs.is_number())
								_result = lhs.to_string().length() < rhs.to_number<size_t>();
							else
								_result = lhs.to_string().length() < rhs.to_string().length();
							break;
						case OperatorType::GreaterThanOrEqual:
							if(lhs.is_number())
								_result = lhs.to_number<size_t>() >= rhs.to_string().length();
							else if(rhs.is_number())
								_result = lhs.to_string().length() >= rhs.to_number<size_t>();
							else
								_result = lhs.to_string().length() >= rhs.to_string().length();
							break;
						case OperatorType::LessThanOrEqual:
							if(lhs.is_number())
								_result = lhs.to_number<size_t>() <= rhs.to_string().length();
							else if(rhs.is_number())
								_result = lhs.to_string().length() <= rhs.to_number<size_t>();
							else
								_result = lhs.to_string().length() <= rhs.to_string().length();
							break;
						default:
							Logger::Debug(L"Invalid binary operator type.");
							//		throw Exception("Invalid binary operator type.");
							break;
					}
				}
				else switch(this->Operator)
				{
					// Binary Arithmetic Operations
					case OperatorType::Plus:
						_result = lhs.to_number<double>() + rhs.to_number<double>();
						break;
					case OperatorType::Minus:
						_result = lhs.to_number<double>() - rhs.to_number<double>();
						break;
					case OperatorType::Multiply:
						_result = lhs.to_number<double>() * rhs.to_number<double>();
						break;
					case OperatorType::Divide:
						_result = lhs.to_number<double>() / rhs.to_number<double>();
						break;
					case OperatorType::Modulo:
						_result = lhs.to_number<intptr_t>() % rhs.to_number<intptr_t>();
						break;
						// Logical Operations
					case OperatorType::EqualEqual:
						_result = lhs.to_number<double>() == rhs.to_number<double>();
						break;
					case OperatorType::NotEqual:
						_result = lhs.to_number<double>() != rhs.to_number<double>();
						break;
					case OperatorType::GreaterThan:
						_result = lhs.to_number<double>() > rhs.to_number<double>();
						break;
					case OperatorType::LessThan:
						_result = lhs.to_number<double>() < rhs.to_number<double>();
						break;
					case OperatorType::GreaterThanOrEqual:
						_result = lhs.to_number<double>() >= rhs.to_number<double>();
						break;
					case OperatorType::LessThanOrEqual:
						_result = lhs.to_number<double>() <= rhs.to_number<double>();
						break;
					case OperatorType::LogicalOr:
						_result = lhs.to_number<double>() || rhs.to_number<double>();
						break;
					case OperatorType::LogicalAnd:
						_result = lhs.to_number<double>() && rhs.to_number<double>();
						break;
					// Bitwise Operations
					case OperatorType::BitwiseXor:
						_result = lhs.to_number<intptr_t>() ^ rhs.to_number<intptr_t>();
						break;
					case OperatorType::BitwiseAnd:
						_result = lhs.to_number<intptr_t>() & rhs.to_number<intptr_t>();
						break;
					case OperatorType::BitwiseNot:
					{
						auto r = lhs.to_number<intptr_t>();
						r &= ~rhs.to_number<intptr_t>();
						_result = r;
						break;
					}
					case OperatorType::BitwiseOr:
						_result = lhs.to_number<intptr_t>() | rhs.to_number<intptr_t>();
						break;
					case OperatorType::BitwiseLeftShift:
						_result = lhs.to_number<intptr_t>() << rhs.to_number<intptr_t>();
						break;
					case OperatorType::BitwiseRightShift:
						_result = lhs.to_number<intptr_t>() >> rhs.to_number<intptr_t>();
						break;
					default:
						Logger::Debug(L"Invalid binary operator type.");
						//	throw Exception("Invalid binary operator type.");
						break;
				}
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return _result.move();
		}
	}
}