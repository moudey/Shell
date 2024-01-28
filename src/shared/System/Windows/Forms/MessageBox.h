#pragma once

namespace Nilesoft
{
	namespace Windows
	{
		namespace Forms
		{
			enum class DialogResult
			{
				None,
				OK,
				Cancel,
				Abort,
				Retry,
				Ignore,
				Yes,
				No
			};

			enum class MessageBoxButtons
			{
				OK,
				OKCancel,
				AbortRetryIgnore,
				YesNoCancel,
				YesNo,
				RetryCancel,
				CancelTryContinue
			};

			enum class MessageBoxIcon
			{
				None,
				Asterisk = MB_ICONASTERISK,
				Error = MB_ICONERROR,
				Exclamation = MB_ICONEXCLAMATION,
				Hand = MB_ICONHAND,
				Information = MB_ICONINFORMATION,
				Question = MB_ICONQUESTION,
				Stop = MB_ICONSTOP,
				Warning = MB_ICONWARNING
			};

			class MessageBox
			{
			public:
				static DialogResult Show(const wchar_t* text)
				{
					return Show(text, L"", MessageBoxIcon::None, MessageBoxButtons::OK);
				}

				static DialogResult Show(const wchar_t* text, const wchar_t* caption)
				{
					return Show(text, caption, MessageBoxIcon::None, MessageBoxButtons::OK);
				}

				static DialogResult Show(HWND hwnd, const wchar_t* text, const wchar_t* caption)
				{
					return Show(hwnd, text, caption, MessageBoxIcon::None, MessageBoxButtons::OK);
				}

				static DialogResult Show(const wchar_t* text, const wchar_t* caption, MessageBoxIcon icon)
				{
					return Show(nullptr, text, caption, icon, MessageBoxButtons::OK);
				}

				static DialogResult Show(HWND hwnd, const wchar_t* text, const wchar_t* caption, MessageBoxIcon icon)
				{
					return Show(hwnd, text, caption, icon, MessageBoxButtons::OK);
				}

				static DialogResult Show(const wchar_t* text, const wchar_t* caption, MessageBoxButtons button)
				{
					return Show(nullptr, text, caption, MessageBoxIcon::None, button);
				}

				static DialogResult Show(HWND hwnd, const wchar_t* text, const wchar_t* caption, MessageBoxButtons button)
				{
					return Show(hwnd, text, caption, MessageBoxIcon::None, button);
				}

				static DialogResult Show(const wchar_t* text, const wchar_t* caption, MessageBoxIcon icon, MessageBoxButtons button)
				{
					return Show(nullptr, text, caption, icon, button);
				}

				static DialogResult Show(HWND hwnd, const wchar_t* text, const wchar_t* caption, MessageBoxIcon icon, MessageBoxButtons button)
				{
					return (DialogResult)::MessageBoxW(hwnd, text, caption, (int)icon | (int)button);
				}
			};
		}
	}
}