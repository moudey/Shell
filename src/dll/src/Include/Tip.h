#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		struct Tip
		{
			static constexpr auto IDT_SHOW = 0x100; //the identifier of the timer for show
			static constexpr auto IDT_HIDE = 0x101; //the identifier of the timer for hide

			static constexpr auto TIMEIN = 1000;
			static constexpr auto TIMEOUT = 1000 * 60;
			static constexpr auto MAXWIDTH = 640;
			static constexpr auto MAXHEIGHT = 100;

			HWND handle{};
			Rect rect;
			string text;
			uint8_t type = 0;
			uint16_t time = UINT16_MAX;

			bool enabled = true;
			bool visible{};
			HFONT hfont{};
			class ContextMenu *ctx{};
			Expression *e{};

			Tip()
			{
			}

			~Tip()
			{
				destroy();
			}

			bool create()
			{
				if(!handle)
				{
					handle = ::CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE, WC_Shell_Window,
											   L"Tip", WS_POPUP,
											   0, 0, 1, 1, nullptr, nullptr, nullptr, nullptr);
					if(handle)
					{
						::SetWindowSubclass(handle, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
													   UINT_PTR, DWORD_PTR dwRefData)->LRESULT
						{
							if(auto _this = reinterpret_cast<Tip *>(dwRefData); _this)
								return _this->Proc(hWnd, uMsg, wParam, lParam);
							return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
						}, 0, reinterpret_cast<DWORD_PTR>(this));
					}
				}
				return handle;
			}

			bool show();
			bool show(Expression *e, const Rect &rc);
			bool show(const string &text, uint8_t type, uint16_t time, const Rect &rc);
			bool hide(bool cancel = false);
			LRESULT Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

			void destroy()
			{
				killTimer();
				if(handle)
					::DestroyWindow(handle);
				handle = nullptr;
				visible = false;
			}

		private:
			void killTimer()
			{
				if(handle)
				{
					::KillTimer(handle, IDT_SHOW);
					::KillTimer(handle, IDT_HIDE);
				}
			}
		};
	}
}