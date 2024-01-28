#pragma once

#include <functional>

namespace Nilesoft
{
	namespace UI
	{
		constexpr auto WC_Window = L"Nilesoft.Shell.Window";

		class Control
		{
		protected:
			HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
			HFONT _hfont = nullptr;

		public:
			typedef int(__stdcall *_Event) (Control *sender, int msg, UINT_PTR wparam, LONG_PTR lparam);

			Control *Parent = nullptr;
			HWND Handle = nullptr;
			string Text;
			string Class;
			int Id = 0;
			int Style = 0;
			string Tooltip;
			std::vector<Control *> Controls;
			RECT Rect{};

			_Event OnDraw = nullptr;

			struct
			{
				int Width = 0;
				int Hieght = 0;
			} Size;

			struct
			{
				int Top = 0;
				int Left = 0;
			} Loaction;

			struct
			{
				COLORREF Text = 0xFFFFFF;
				COLORREF Background = 0x000000;

				struct
				{
					COLORREF Text = 0xFFFFFF;
					COLORREF Background = 0x333333;

				} Select;

				struct
				{
					COLORREF Text = 0xFFFFFF;
					COLORREF Background = 0x666666;

				} Hover;
			} Color;

			struct
			{
				struct
				{
					COLORREF Text = 0xFFFFFF;
					COLORREF Background = (COLORREF)-1;

					struct
					{
						COLORREF Text = 0xFFFFFF;
						COLORREF Background = (COLORREF)-1;

					} Select;

					struct
					{
						COLORREF Text = 0xFFFFFF;
						COLORREF Background = (COLORREF)-1;
					} Hover;
				} Color;

				struct
				{
					int Style = 0;
					int white = 600;
					int Align = DT_CENTER | DT_VCENTER;
					string Name;

				} Format;

				HFONT Handle = nullptr;

			} Font;

			virtual ~Control()
			{
				while(!Controls.empty())
				{
					delete Controls.back();
					Controls.pop_back();
				}

				if(_hfont)
					::DeleteObject(_hfont);
			}

			Control()
			{
				Font.Handle = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
				LOGFONT lf{};
				if(::GetObject(Font.Handle, sizeof(LOGFONT), &lf))
				{
					if(lf.lfQuality < ANTIALIASED_QUALITY)
					{
						lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
						_hfont = CreateFontIndirectW(&lf);
					}
				}
			}

			Control(Control *parent) : Control()
			{
				Parent = parent;
			}

			Control(const wchar_t *lpclass, const wchar_t *text, const RECT &rect, int id = 0, 
					Control *parent = nullptr, int style = 0, HFONT hfont = nullptr, const wchar_t *tooltip = nullptr)
				: Control()
			{
				Class = lpclass;
				Text = text;
				Rect = rect;
				Id = id;
				Parent = parent;
				Style = style | WS_VISIBLE;
				Size.Width = Rect.right == -1 ? 100 : Rect.right;
				Size.Hieght = Rect.bottom == -1 ? 32 : Rect.bottom;
				Loaction.Left = Rect.left;
				Loaction.Top = Rect.top;

				if(hfont)
					Font.Handle = hfont;
				else if(_hfont)
				{
					if(_hfont != Font.Handle)
						Font.Handle = _hfont;
				}

				if(tooltip)
					Tooltip = tooltip;

				Init();
			}

			size_t Add(std::initializer_list<Control *> controls)
			{
				for(auto c : controls)
				{
					c->Parent = this;
					Controls.push_back(c);
				}
				return Controls.size();
			}

			void Show() { ::ShowWindow(Handle, SW_NORMAL); Update(); }
			void Hidden() { ::ShowWindow(Handle, SW_HIDE); }
			void Update() { ::UpdateWindow(Handle); }

			Control *FindByHandle(HWND handle)
			{
				for(auto c : Controls) if(handle == c->Handle) return c;
				return nullptr;
			}

			Control *FindById(int id)
			{
				for(auto c : Controls) if(id == c->Id) return c;
				return nullptr;
			}

			bool Init()
			{
				Handle = ::CreateWindowExW(0, Class, Text, Style,
										 (int)Rect.left, (int)Rect.top, (int)Rect.right, (int)Rect.bottom,
										 Parent ? Parent->Handle : nullptr, (HMENU)(UINT_PTR)Id, hInstance, nullptr);
				if(Handle && Parent)
				{
					if(Font.Handle)
						::SendMessageW(Handle, WM_SETFONT, (WPARAM)Font.Handle, MAKELPARAM(TRUE, 0));
					Parent->Add({ this });
					SetToolTip();
				}

				return Handle != nullptr;
			}

			bool SetToolTip()
			{
				if(Tooltip.empty())
					return false;

				// Create a tooltip.
				auto hwndTT = ::CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASS, nullptr,
											   WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
											   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
											   Handle, nullptr, hInstance, nullptr);
				if(hwndTT)
				{
					::SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
					// Set up "tool" information. In this case, the "tool" is the entire parent window.
					TOOLINFO ti = { 0 };
					ti.cbSize = sizeof(TOOLINFO);
					ti.uFlags = TTF_SUBCLASS;
					ti.hwnd = Handle;
					ti.hinst = hInstance;
					ti.lpszText = Tooltip;
					::GetClientRect(Handle, &ti.rect);
					// Associate the tooltip with the "tool" window.
					::SendMessageW(hwndTT, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&ti));
					return true;
				}
				return false;
			}

			LRESULT SendMSG(UINT msg, UINT_PTR wparam, LONG_PTR lparam = 0)
			{
				return ::SendMessageW(Handle, msg, wparam, lparam);
			}

			LRESULT SendCommand(int id, LPARAM lparam = 0)
			{
				return SendMSG(WM_COMMAND, (UINT_PTR)id, lparam);
			}

			void SetColor(std::initializer_list<Control *> controls, COLORREF text, COLORREF back = -1, COLORREF text_select = -1, COLORREF back_select = -1)
			{
				for(auto c : controls)
				{
					c->Color.Text = text;
					c->Color.Background = back;

					c->Color.Select.Text = text_select;
					c->Color.Select.Background = back_select;
				}
			}

			static HWND Create(const wchar_t *lpclass, const wchar_t *text, const RECT &rect, HWND parent = nullptr, int id = 0, int style = 0)
			{
				return ::CreateWindow(lpclass, text,
									  style,
									  (int)rect.left, (int)rect.top,
									  (int)rect.right, (int)rect.bottom,
									  parent, (HMENU)(UINT_PTR)id, reinterpret_cast<HINSTANCE>(&__ImageBase), nullptr);
			}
		};

		class Window : public Control
		{
		public:
			Window()
			{
			}

			Window(const wchar_t *text, const RECT &rect, int id = 0, Control *parent = nullptr, int style = 0)
				: Control(WC_Window, text, rect, id, parent, style | WS_POPUP/* | WS_CAPTION*/)
			{
			}

			Window(const wchar_t *text, int x, int y, int w, int h, int id = 0, Control *parent = nullptr, int style = 0)
				: Window(text, { x,y,w,h }, id, parent, style)
			{
			}
		};

		class Button : public Control
		{
		public:
			HBITMAP Image = nullptr;

		public:
			Button()
			{
			}

			Button(const wchar_t *text, const RECT &rec, int id, Control *parent = nullptr, int style = 0, HFONT hfont = nullptr,const wchar_t *tooltip = nullptr)
				: Control(WC_BUTTONW, text, rec, id, parent, style | WS_CHILD | WS_TABSTOP, hfont, tooltip)
			{
				if(Handle && (style & BS_OWNERDRAW))
					::SetWindowSubclass(Handle, _SubclassProc, Id, (LONG_PTR)this);
			}

			Button(const wchar_t *text, int x, int y, int w, int h, int id, Control *parent = nullptr, int style = 0, HFONT hfont = nullptr, const wchar_t *tooltip = nullptr)
				: Button(text, { x,y,w,h }, id, parent, style, hfont, tooltip)
			{
			}

			virtual ~Button()
			{
				if(Handle && (Style & BS_OWNERDRAW))
					::RemoveWindowSubclass(Handle, _SubclassProc, Id);
			}

		private:

			static int draw(Control *s, UINT msg, [[maybe_unused]]WPARAM wParam = 0, [[maybe_unused]] LPARAM lParam = 0)
			{
				HDC hdc = ::GetWindowDC(s->Handle);

				RECT rect{};
				::GetClientRect(s->Handle, &rect);

				auto color_txt = msg == WM_MOUSEHOVER || msg == WM_LBUTTONDOWN ? s->Color.Select.Text : s->Color.Text;
				auto color_bg = msg == WM_MOUSEHOVER || msg == WM_LBUTTONDOWN ? s->Color.Select.Background : s->Color.Background;

				// if(color_bg == -1)
			   //      color_bg = c->Parent->Color.Select.Background;

				if(color_bg != 0xFFFFFFFF)
				{
					if(msg == WM_SETFOCUS)
					{
						::RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 20, 20);
					}
					else
					{
						auto br = ::CreateSolidBrush(color_bg);
						::FillRect(hdc, &rect, br);
						::DeleteObject(br);
					}
				}

				if(!s->Text.empty())
				{
					::SetBkMode(hdc, TRANSPARENT);
					::SetTextColor(hdc, color_txt);

					if(s->Font.Handle)
						::SelectObject(hdc, s->Font.Handle);

					::DrawTextW(hdc, s->Text, -1, &rect, s->Font.Format.Align | DT_SINGLELINE);
				}

				::UpdateWindow(s->Handle);
				::InvalidateRect(s->Handle, &s->Rect, true);
				return 0;
			}

			static LRESULT CALLBACK _SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR/* uIdSubclass*/, DWORD_PTR dwRefData)
			{
				auto control = reinterpret_cast<Control*>(dwRefData);
				switch(uMsg)
				{
					//  case WM_ERASEBKGND:
					//      return 1;
					case WM_PAINT:
					{
						int ret = 0, process = 0;
						PAINTSTRUCT ps{};
						if(control->Style & BS_OWNERDRAW)
						{
							::BeginPaint(control->Handle, &ps);
						}

						if(control->OnDraw)
						{
							ret = control->OnDraw(control, uMsg, wParam, (LONG_PTR)&ps);
							process++;
							if(ret)
							{
								::EndPaint(control->Handle, &ps);
								return 0;
							}
						}
						else if(control->Style & BS_OWNERDRAW)
						{
							ret = draw(control, uMsg, wParam, lParam);
							process++;
						}

						if(control->Style & BS_OWNERDRAW)
							::EndPaint(control->Handle, &ps);

						//if(process)  return ret;
						break;
					}
					case WM_MOUSEMOVE:
					{
						TRACKMOUSEEVENT ev = {};
						ev.cbSize = sizeof(TRACKMOUSEEVENT);
						ev.dwFlags = TME_HOVER | TME_LEAVE;
						ev.hwndTrack = hWnd;
						ev.dwHoverTime = 10;
						::TrackMouseEvent(&ev);

						if(control->OnDraw)
						{
							if(control->OnDraw(control, uMsg, wParam, lParam))
								return 0;
						}

						//  if(control->Style & BS_OWNERDRAW)
						//       draw(control, WM_MOUSEHOVER, wParam, lParam);

						break;
					}
					//case WM_SETFOCUS:
					case WM_MOUSEHOVER:
					{
						if(control->OnDraw)
							control->OnDraw(control, uMsg, wParam, lParam);
						else if(control->Style & BS_OWNERDRAW)
							if(draw(control, WM_MOUSEHOVER, wParam, lParam))
								return 0;
						break;
					}
					//case WM_KILLFOCUS:
					case WM_MOUSELEAVE:
					{
						if(control->OnDraw)
							control->OnDraw(control, uMsg, wParam, lParam);
						else if(control->Style & BS_OWNERDRAW)
							if(draw(control, uMsg, wParam, lParam))
								return 0;
						break;
					}
				}
				return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
			}
		};

		class Label : public Control
		{
		public:
			Label()
			{
			}

			Label(const wchar_t *text, const RECT &rect, int id, Control *parent = nullptr, int style = 0)
				: Control(WC_STATIC, text, rect, id, parent, style | WS_CHILD)
			{
			}

			Label(const wchar_t *text, int x, int y, int w, int h, int id, Control *parent = nullptr, int style = 0)
				: Label(text, { x,y,w,h }, id, parent, style)
			{
			}

			virtual ~Label()
			{
			}
		};

		class App
		{
		public:
			HINSTANCE HInstance = nullptr;
			COLORREF Background = 0x000000;
			int Style = 0;
			HICON Icon = nullptr;
			WNDPROC WndProc = nullptr;

			App()
			{
			}

			App(HINSTANCE hInstance, WNDPROC wndProc, COLORREF background, HICON icon, int style = 0)
				: HInstance{ hInstance }, WndProc{ wndProc }, Background{ background }, Icon{ icon }, Style{ style }
			{
			}

			~App()
			{
			}

			bool Init()
			{
				WNDCLASSEXW wcex{};
				wcex.cbSize = sizeof(WNDCLASSEX);
				wcex.lpfnWndProc = WndProc;
				wcex.hInstance = HInstance;
				wcex.hbrBackground = ::CreateSolidBrush(Background);
				wcex.lpszClassName = WC_Window;
				wcex.hCursor = LoadCursorW(nullptr, IDC_HAND);
				wcex.hIcon = Icon;
				wcex.hIconSm = Icon;
				wcex.style = Style;
				return ::RegisterClassExW(&wcex);
			}

			int Run(Window *mainWindow)
			{
				if(mainWindow)
				{
					mainWindow->Show();
					MSG msg{};
					while(::GetMessageW(&msg, nullptr, 0, 0))
					{
						if(!::IsDialogMessageW(mainWindow->Handle, &msg))
						{
							::TranslateMessage(&msg);
							::DispatchMessageW(&msg);
						}
					}
					return (int)msg.wParam;
				}
				return 0;
			}
		};
	}
}