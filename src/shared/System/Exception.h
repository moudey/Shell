
#pragma once
/*
#include<string>
#include<exception>
#include <cstdarg>
*/
namespace Nilesoft
{
	class Exception : public std::exception
	{
	protected:
		std::wstring msg{};

	public:
		Exception(const std::string &message)
			: std::exception(message.c_str())
		{
		}

		Exception(std::wstring const &message)
			: msg{ message }
		{
		}

		Exception(const wchar_t *message)
			: msg{ message }
		{
		}

		Exception(const wchar_t *format, ...)
		{
			if(format != nullptr)
			{
				va_list args;
				va_start(args, format);
				msg.resize(std::vswprintf(nullptr, 0, format, args), '\0');
				std::vswprintf(&msg[0], msg.size() + 1, format, args);
				va_end(args);
			}
		}

		virtual std::wstring const &Message() const
		{
			return msg;
		}

		static DWORD Filter(DWORD exceptionCode, EXCEPTION_POINTERS *ei)
		{
			(ei);
			// we handle all exceptions
			DWORD dwResult = EXCEPTION_EXECUTE_HANDLER;
			switch(exceptionCode)
			{
				case EXCEPTION_ACCESS_VIOLATION:
				case EXCEPTION_INVALID_HANDLE:
				case EXCEPTION_STACK_OVERFLOW:
				case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				case EXCEPTION_BREAKPOINT:
				case EXCEPTION_DATATYPE_MISALIGNMENT:
				case EXCEPTION_FLT_DENORMAL_OPERAND:
				case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				case EXCEPTION_FLT_INEXACT_RESULT:
				case EXCEPTION_FLT_INVALID_OPERATION:
				case EXCEPTION_FLT_OVERFLOW:
				case EXCEPTION_FLT_STACK_CHECK:
				case EXCEPTION_FLT_UNDERFLOW:
				case EXCEPTION_INT_DIVIDE_BY_ZERO:
				case EXCEPTION_INT_OVERFLOW:
				case EXCEPTION_PRIV_INSTRUCTION:
				case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				case EXCEPTION_GUARD_PAGE:
				case EXCEPTION_INVALID_DISPOSITION:
				case EXCEPTION_ILLEGAL_INSTRUCTION:

				case STATUS_HEAP_CORRUPTION:
				case STATUS_STACK_BUFFER_OVERRUN:
				{
					dwResult = EXCEPTION_EXECUTE_HANDLER;
					break;
				}
			}
			return dwResult;
		}

		static DWORD Filter(DWORD exceptionCode)
		{
			return Filter(exceptionCode, nullptr);
		}
	};
}

