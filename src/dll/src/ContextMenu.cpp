#include <pch.h>
#include "Include/Theme.h"
#include "Include/ContextMenu.h"
#include "Include/stb_image_write.h"

using namespace Nilesoft::Diagnostics;
#include <mutex>

extern Logger &_log;

#pragma region

/*
void plutovg_surface_write_to_hdc(plutovg_surface_t *surface, int x, int y, HDC hdc)
{
	BYTE bitmapinfo[FIELD_OFFSET(BITMAPINFO, bmiColors) + (3 * sizeof(DWORD))];
	auto &bih = *(BITMAPINFOHEADER *)bitmapinfo;
	bih.biSize = sizeof(BITMAPINFOHEADER);

	bih.biWidth = surface->width;
	bih.biHeight = surface->height;

	bih.biPlanes = 1;
	bih.biBitCount = 32;

	bih.biCompression = BI_BITFIELDS;
	bih.biSizeImage = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	auto *pMasks = (DWORD *)(&bitmapinfo[bih.biSize]);
	pMasks[0] = 0xff0000; // Red
	pMasks[1] = 0x00ff00; // Green
	pMasks[2] = 0x0000ff; // Blue

	::StretchDIBits(hdc,
				  x,
				  y + surface->height,
				  surface->width,
				  -surface->height,
				  0,
				  0,
				  surface->width,
				  surface->height,
				  surface->data,
				  (BITMAPINFO *)&bih,
				  DIB_RGB_COLORS,
				  SRCCOPY);
}
*/

struct DWM
{
	enum class BackdropType : int
	{
		Default = 0,
		None = 1,
		Mica = 2,
		Acrylic = 3,
		Tabbed = 4,
	};

	enum class Corner : int
	{
		Default = 0,
		None = 1,
		Round = 2,
		RoundSmall = 3,
		Last = 4,
	};

	HWND handle{};
	DWM(HWND hWnd = nullptr) : handle(hWnd) {}

	HRESULT RemoveCorner()
	{
		return SetCorner(Corner::None);
	}

	HRESULT SetCorner(Corner value = Corner::Round)
	{
		const auto DWMWA_WINDOW_CORNER_PREFERENCE = 33U;
		return SetAttribute(DWMWA_WINDOW_CORNER_PREFERENCE, value);
	}

	HRESULT SetBorderColor(COLORREF color)
	{
		const auto DWMWA_BORDER_COLOR = 34U;
		return SetAttribute(DWMWA_BORDER_COLOR, color);
	}

	/// <summary>
	/// Enable or disable immersive dark mode.
	/// Requires Windows build 19041 or higher.
	/// </summary>
	HRESULT SetImmersiveDarkMode(BOOL state = TRUE)
	{
		const auto DWMWA_IMMERSIVE_DARK_MODE = 20U;
		return SetAttribute(DWMWA_IMMERSIVE_DARK_MODE, state);
	}

	/// <summary>
	/// Set backdrop type on target window
	/// Requires Windows build 22523 or higher.
	/// </summary>
	HRESULT SetBackdropType(BackdropType backdropType)
	{
		const auto DWMWA_SYSTEMBACKDROP_TYPE = 38;
		return SetAttribute(DWMWA_SYSTEMBACKDROP_TYPE, backdropType);
	}

	/// <summary>
	/// Enable or Disable Mica on target window
	/// Supported on Windows builds from 22000 to 22523. It doesn't work on 22523, use <see cref="SetBackdropType(IntPtr, DWM_SYSTEMBACKDROP_TYPE)"/> instead.
	/// </summary>
	HRESULT SetMica(BOOL state = true)
	{
		const auto DWMWA_MICA = 1029U;
		return SetAttribute(DWMWA_MICA, state);
	}

	template<typename T>
	HRESULT SetAttribute(DWORD dwAttribute, T pvAttribute)
	{
		return ::DwmSetWindowAttribute(handle, dwAttribute, (const void *)&pvAttribute, sizeof(T));
	}

	auto ExtendFrameIntoClientArea(MARGINS margins = { -1 })
	{
		return ::DwmExtendFrameIntoClientArea(handle, &margins);
	}
};

inline HMENU GET_HMENU(HWND hWnd) { return SendMSG<HMENU>(hWnd, MN_GETHMENU, 0, 0); }

auto ver = &Windows::Version::Instance();

#pragma endregion

namespace Nilesoft
{
	//std::mutex mtx; // mutex for critical section
	namespace Shell
	{
		//D2D_DC d2d;

		//std::mutex _mutex; // mutex for critical section
		//std::lock_guard<std::mutex> lock(_mutex);

		inline static MenuItemInfo *get_item(uint32_t id, const std::vector<MenuItemInfo *> &list)
		{
			for(auto item : list)
			{
				if(item->wID == id) return item;
			}
			return nullptr;
		}

		inline static MenuItemInfo *get_item(uint32_t id, HMENU hMenu, const std::vector<MenuItemInfo *> &list)
		{
			for(auto item : list)
			{
				if(item->handle == hMenu)
				{
					if(item->wID == id)
					{
						return item;
					}
				}
			}
			return nullptr;
		}

		ContextMenu::ContextMenu(HWND hWnd, HMENU hMenu, Point const &pt)
		{
			//d2d.create_render();
			//d2d.create_res();

			Window window = hWnd;

			hwnd.owner = hWnd;
			Processes[this] = true;

			dpi.val = Theme::GetDpi(pt, hwnd.owner);
			dpi.org = dpi.val;
			//static_cast<int>(std::ceil(640.f * dpi / 96.f))
			_hMenu_original = hMenu;
			_hMenu = ::CreatePopupMenu();

			_theme.dpi = &dpi;
			_context.dpi = &dpi;
			_context.wnd.owner = hwnd.owner;
			_context.hMenu = hMenu;
			_window = hwnd.owner;

			_cache = Initializer::instance->cache;
			if(Initializer::instance->dpi != dpi.val)
				_cache->reload(dpi.val);

			Initializer::instance->dpi = dpi.val;

			_tip.ctx = this;

			if(keyboard.get_keys_state(true))
			{
				// shift key is down "Extended Mode"
				//context.Extended = ::GetAsyncKeyState(VK_SHIFT) < 0;
				//_context.Extended = keyboard.key_shift();
			}

			_context.Keyboard = &keyboard;

			ThreadId = window.get_threadId(&ProcessId);

			GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
			if(::GetGUIThreadInfo(ThreadId, &gti))
			{
				hwnd.active = gti.hwndActive;
				hwnd.focus = gti.hwndFocus;
				_context.wnd.active = hwnd.active;
				_context.wnd.focus = gti.hwndFocus;
			}

			_context.wnd.active = hwnd.active;
			_context.wnd.focus = hwnd.focus;

			Monitor monitor(pt);
			if(monitor.info())
			{
				_rcMonitor = monitor.rcMonitor;
				_context.helper.is_primary_monitor = monitor.is_primary();
			}

			// current language
			languageId = ::GetThreadUILanguage();

			/*
			ctx->is_layoutRTL = flag.has(TPM_LAYOUTRTL) or (::GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
			MBF(L"%d", ctx->is_layoutRTL);
				DWORD pdwDefaultLayout = 0;
			GetProcessDefaultLayout(&pdwDefaultLayout) && pdwDefaultLayout == LAYOUT_RTL;
			auto is_middle_east_enabled = ::GetSystemMetrics(SM_MIDEASTENABLED);
			*/
			is_layoutRTL = (window.get_ex_style() & WS_EX_LAYOUTRTL) != 0;
		}

		ContextMenu::~ContextMenu()
		{
			try
			{
				Processes.erase(this);
				//if(_cache)
				//	_cache->GC.clear();
				Uninitialize();
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
		}

		bool ContextMenu::prepare_new_items(PositionList &posList,
										  const std::vector<NativeMenu *> &list,
										  MenuItemInfo *owner,
										  menu_t *menu, bool moved)
		{
			if(list.empty())
				return false;

			int _this_index = 0;
			
			for(auto item : list)
			{
				try 
				{
					_context._this = nullptr;
					//std::lock_guard<std::mutex> lock(_mutex);
					//_context.variables.runtime = &item->owner->variables;
					_context.variables.local = &item->owner->variables;

					if(item->properties == 0)
					{
						if(item->is_separator())
						{
							auto mii = _gc.push(new MenuItemInfo(MIIM_ID | MIIM_FTYPE, MFT_SEPARATOR, -1));
							mii->owner = owner;
							mii->dynamic = true;
							mii->type = NativeMenuType::Separator;
							posList.Auto.push_back(mii);
						}
						continue;
					}

					auto not_sep = !item->is_separator();

					this_item _this{};
					_context._this = &_this;
					_this.level = (int)parent_level.size();

					_this.type = not_sep ? (item->is_menu() ? 2 : 1) : 0;
					_this.pos = _this_index++;

					if(!Selected.verify_types(item->fso))
						continue;
					
					/*
					if(Selected.Window.id >= WINDOW_TASKBAR && !Selected.Check(item->fso))
						continue;
					else if(Selected.Window.id <= WINDOW_TASKBAR && !item->fso.all_types)
					{
						if(item->fso.Types[FSO_TASKBAR] != Selected.Types[FSO_TASKBAR])
							continue;
					}*/

					if(item->where)
					{
						if(!_context.eval_bool(item->where))
							continue;
					}

					string value;

					if(!item->is_separator() && !moved)
					{
						if(item->owner != menu->parent)
						{
							if(_context.Eval(item->moveto, value, true) && !value.trim(L'/').empty())
							{
								auto mii = _gc.push();
								mii->dynamic = true;
								mii->owner_dynamic = item;
								if(mii->parse_parent(value))
									_moved_items.dynamics.push_back(mii);
								continue;
							}
						}
					}

					auto visibility = _context.parse_visibility(item->visibility);

					if(visibility == Visibility::Hidden)
						continue;

					_this.disabled = visibility == Visibility::Disabled;
					_this.vis = static_cast<int>(visibility);

					auto privileges = Privileges::None;
					auto mode = SelectionMode::Single;

					if(owner)
					{
						mode = owner->mode;
						privileges = owner->privileges;
					}

					mode = _context.parse_mode(item->mode, mode);

					if(Selected.Window.id > WINDOW_TASKBAR && !Selected.verify_mode(mode))
						continue;

					auto position = Position::Auto;
					string indexof;
					int indexof_pos = 0, indexof_def = -1;

					if(item->position)
					{
						Object obj = _context.Eval(item->position).move();

						if(obj.is_array(true))
						{
							auto ptr = obj.get_pointer();
							if((uint32_t)ptr[0] == IDENT_INDEXOF)
							{
								int ac = ptr[1];
								indexof = ptr[2].to_string().move();
								indexof_pos = ptr[3];
								if(ac == 3)
									indexof_def = (int)_context.parse_pos(ptr[4], Position::Auto);
								position = (Shell::Position)indexof.trim().hash();
							}
						}
						else if(!obj.is_null())
						{
							position = _context.parse_pos(obj, Position::Auto);
						}
					}

					//	auto position = item->parse_position(&_context);
					_this.pos = static_cast<int>(position);

					auto push_back = [&](MenuItemInfo *mii)
					{
						mii->position = position;
						mii->dynamic = true;

						switch(position)
						{
							case Position::Top:
								posList.Top.push_back(mii);
								break;
							case Position::Middle:
								posList.Middle.push_back(mii);
								break;
							case Position::Bottom:
								posList.Bottom.push_back(mii);
								break;
							case Position::Auto:
							case Position::None:
								posList.Auto.push_back(mii);
								break;
							default:
								posList.Custom.push_back(mii);
								break;
						}
					};

					if(item->is_separator())
					{
						auto mii = _gc.push(new MenuItemInfo(MIIM_ID | MIIM_FTYPE, MFT_SEPARATOR, -1));
						mii->type = NativeMenuType::Separator;
						mii->indexof.val = indexof.move();
						mii->indexof.pos = indexof_pos;
						mii->indexof.def = indexof_def;
						push_back(mii);
					}
					else
					{
						string title;
						try
						{
							if(!_context.Eval(item->title, title) || title.empty())
							{
								/*if(item->is_menu())
									is_container = true;
								else */if(!item->image.defined)
									continue;
							}
						}
						catch(...) 
						{
						}

						_this.title = title;
						_this.length = title.length<uint32_t>();

						FindPattern find;
						if(_context.Eval(item->find, value, true) && !value.empty())
						{
							if(find.split(value, L'|'))
							{
								auto found = 0;
								for(auto sel : Selected.Items)
								{
									string ext = sel->Extension.substr(1).move();
									if(!find(&sel->Title, sel->IsFile() ? &ext : nullptr, &sel->Path))
									{
										found = 0;
										break;
									}
									found++;
								}

								if(found == 0) continue;;
							}
						}

						auto mii = _gc.push(new MenuItemInfo(MIIM_STRING | MIIM_ID | MIIM_DATA | MIIM_STATE, 0, ident.get_id()));

						mii->owner = owner;
						mii->indexof.val = indexof.move();
						mii->indexof.pos = indexof_pos;
						mii->indexof.def = indexof_def;

						mii->set_title(title.move());
						
						mii->id = mii->hash;
						mii->hash = mii->hash;

						mii->privileges = privileges;

						_this.title_normalize = mii->title.normalize;
						_this.id = mii->hash;

						if(item->is_menu() && item->cmd->admin)
							mii->privileges = _context.parse_privileges(item->cmd);

						int checked = 0;

						if(item->is_item())
						{
							checked = _context.eval_number<int>(item->checked, 0);
							if(_settings.new_items.keys)
								_context.Eval(item->keys, mii->keys);
						}

						if(checked)
						{
							_this.checked = 1;
							mii->fState = MFS_CHECKED;
							if(checked == 2)
							{
								mii->fType |= MFT_RADIOCHECK;
								_this.checked = 2;
							}
						}

						mii->separator = (int)_context.parse_separator(item->separator);
						mii->column = _context.eval_number<int>(item->column, 0);
						mii->type = NativeMenuType::Item;
						mii->handle = menu->handle;
						mii->owner_dynamic = item;
						mii->mode = mode;
						mii->visibility = visibility;

						if(visibility != Visibility::Enabled)
							mii->fState |= MFS_DISABLED;

						if(mii->column > 0)
							mii->fType |= mii->column == 2 ? MFT_MENUBARBREAK : MFT_MENUBREAK;

						bool image_disabled_by_checked = mii->is_checked() && _theme.image.display == 0;

						if(!_cache->dynamic.image.disabled() && _settings.new_items.image && !image_disabled_by_checked)
						{
							bool parent_disabled = false;

							if(owner)
								parent_disabled = owner->image.import == ImageImport::Disabled;

							if(item->image.enabled() && !parent_disabled)
							{
								if(item->image.import == ImageImport::Inherit)
								{
									if(owner && owner->image.hbitmap)
									{
										//already resolved
										mii->image.inherit(&owner->image);
									}
								}
								else try
								{
									//use the resolved image location
									if(item->image.import == ImageImport::Image)
									{
										if(_context.Image(item->image.expr, mii, font.icon, false))
										{
											if(mii->image.import == ImageImport::Inherit)
											{
												if(owner)
												{
													if(owner->image.hbitmap)
														mii->image.inherit(&owner->image);
													else
														mii->image = owner->image;
												}
											}
										}
									}
									else if(item->image.import == ImageImport::Command)
									{
										auto cmd = item->cmd;
										if(!(cmd->command.expr || cmd->command.type != COMMAND_INVALID))
											cmd = item->commands.at(1);

										if(cmd)
										{
											string path;

											if(cmd->command.type == COMMAND_PROMPT)
												path = Environment::Variable(def_COMSPEC).move();
											else if(cmd->command.type == COMMAND_SHELL || cmd->command.type == COMMAND_EXPLORER)
												path = def_EXPLORER;
											else if(cmd->command.type == COMMAND_POWERSHELL)
												path = def_POWERSHELL;
											else if(cmd->command.type == COMMAND_PWSH)
											{
												path = def_PWSH;
												//auto len = ::SearchPathW(nullptr, def_PWSH, nullptr, MAX_PATH, path.buffer(MAX_PATH), nullptr);
												//if(len > 0) path.release(len); else path = def_PWSH;
											}
											else
											{
												_context.Eval(cmd->command.expr, path);
											}

											path.trim(str_trim);
											
											if(!path.empty())
											{
												Hash hash = path.hash();
												if(!hash.equals({ 0, IDENT_NONE, IDENT_DISABLED }))
												{
													//ask the cache for an existing image or add.
													//this will identify the image in the cache as "used" or "active".
													if(auto n = Path::Extension(path); n.length() == 0)
														path += L".exe";
													mii->image.hbitmap = Image::From(path.c_str(), _theme.image.size);
													mii->image.size = {(long) _theme.image.size ,(long)_theme.image.size };
													mii->image.import = ImageImport::Image;
													mii->image.inherited = false;
												}
											}
										}
									}

									if(_context.Image(item->images.select, mii, font.icon, true))
									{
										/*if(mii->image_select.import == ImageImport::Inherit)
										{
											if(owner)
											{
												if(owner->image_select.hbitmap)
												{
													mii->image_select.hbitmap = owner->image.hbitmap;
													mii->image_select.size = owner->image_select.size;
													mii->image_select.inherit = true;
													mii->image.inherit(&menu->owner->image);
												}
												else {
													mii->image = owner->image;
												}
											}
										}*/
									}
								}
								catch(...)
								{
								}
							}
						}

						auto is_container = false;
						if(item->is_menu() && item->expanded)
							is_container = _context.Eval(item->expanded).to_bool();

						if(is_container)
							prepare_new_items(posList, item->items, mii, menu);
						else
						{
							if(owner)
								mii->path = (owner->path + L'/' + owner->title.normalize).trim(L'/').move();
						
							//_log.info(mii->path);

							///else
							//	mii->path = mii->owner->path + L'/' + mii->owner->title.text;
							//mii->tip = item->tip;
							_context.eval_tip(item->tip, mii->tip.text, mii->tip.type, mii->tip.time);

							if(item->is_menu())
							{
								mii->set_popup_menu();
								//mii->destory = false;
								auto m_sub = &_menus[mii->hSubMenu];
								m_sub->type = m_sub->MFT_DYNAMIC;
								m_sub->destory = true;
								m_sub->id = mii->hash;
								m_sub->hash = mii->hash;
								m_sub->owner = mii;
								m_sub->dynamics = item->items;
								//m_sub->parent = item;
								m_sub->parent = item->owner;

								string path;
								if(!mii->path.empty())
									path = mii->path + L'/';

								path += mii->title.normalize;
								m_sub->path = path.trim(L'/').move();

								//_log.info(L"%s-%s", m_sub->path.c_str(), menu->path.c_str());
							}
							push_back(mii);
						}
					}
				}
				catch(...) 
				{
				}
			}
			return true;
		}

		void ContextMenu::prepare_system_item(menuitem_t *item, MenuItemInfo *mii, menu_t *menu)
		{
			auto fix_image_size = [](auto v1, auto v2)->long {
				double d = double(v1) / v2;
				return long(d * v2);
			};

			auto image_disabled = !_theme.image.enabled || _settings.modify_items.image == 0;

			auto ev_si = [=](NativeMenu *si, MenuItemInfo *mii, menuitem_t *item)
			{
				if(item->type == 0 && si->checked)
				{
					if(auto checked = _context.eval_number<int>(si->checked, 0); checked > 0)
					{
						_context._this->checked = 1;
						mii->fState = MFS_CHECKED;
						if(checked == 2)
						{
							mii->fType |= MFT_RADIOCHECK;
							_context._this->checked = 2;
						}
					}
				}

				bool image_disabled_by_checked = mii->is_checked() && _theme.image.display == 0;

				if(image_disabled_by_checked)
					mii->image.import = ImageImport::Disabled;
				else if(!image_disabled && si->images.normal)
					_context.Image(si->images.normal, mii, font.icon, false);

				if(mii->image.import == ImageImport::Disabled || image_disabled || image_disabled_by_checked)
					mii->image.destroy();
				else if(menu->owner && mii->image.import == ImageImport::Inherit)
					mii->image.inherit(&menu->owner->image);

				if(_settings.modify_items.separator)
				{
					mii->separator = (int)_context.parse_separator(si->separator);
					_context._this->sep = mii->separator;
				}

				auto position = Position::Auto;
				string indexof;
				int indexof_pos = 0, indexof_def = -1;

				if(si->position && _settings.modify_items.position)
				{
					Object obj = _context.Eval(si->position).move();

					if(obj.is_array(true))
					{
						auto ptr = obj.get_pointer();
						if((uint32_t)ptr[0] == IDENT_INDEXOF)
						{
							int ac = ptr[1];
							indexof = ptr[2].to_string().move();
							indexof_pos = ptr[3];
							if(ac == 3)
								indexof_def = (int)_context.parse_pos(ptr[4], Position::Auto);
							position = (Position)indexof.trim().hash();
						}
					}
					else if(!obj.is_null())
					{
						position = _context.parse_pos(obj, Position::Auto);
					}
				}

				_context._this->pos = static_cast<int>(position);

				mii->indexof.val = indexof.move();
				mii->indexof.pos = indexof_pos;
				mii->indexof.def = indexof_def;

				mii->position = position;

				if(_settings.modify_items.title)
				{
					string new_title;
					if(_context.Eval(si->title, new_title) && !new_title.empty())
					{
						mii->set_title(new_title.move());
					}
				}

				if(mii->is_item() && _settings.modify_items.keys)
					_context.Eval(si->keys, mii->keys, true);

				//mii->tip = item->tip;
				_context.eval_tip(si->tip, mii->tip.text, mii->tip.type, mii->tip.time);
			};


			mii->dwItemData = item->dwItemData;
			mii->handle = menu->handle;

			this_item _this; _context._this = &_this;

			_this.type = item->type;
			_this.pos = (int)item->position;
			_this.checked = item->checked ? (item->radio_check ? 2 : 1) : 0;
			_this.disabled = item->disabled;

			_this.system = true;
			_this.id = mii->id;
			_this.parent = menu->id;
			_this.level = (int)parent_level.size();

			mii->position = item->position;
			mii->visibility = item->visibility;

			if(item->is_separator())
			{
				mii->fType |= MFT_SEPARATOR;
				mii->wID = (uint32_t)-1;
				//mii->fix_separator_id();
			}
			else
			{
				mii->fMask |= MIIM_STATE;

				mii->wID = item->wid;

				if(mii->wID == 0 || mii->wID == (uint32_t)-1)
					mii->wID = ident.get_id();

				mii->id = item->uid();
				mii->hash = item->hash;

				if(item->disabled)
					mii->fState |= MFS_DISABLED;

				if(item->checked)
					mii->fState |= MFS_CHECKED;

				if(item->radio_check)
					mii->fType |= MFT_RADIOCHECK;

				bool is_auto_img = false;
				bool image_disabled_by_checked = item->checked && _theme.image.display == 0;

				if(!item->title.empty())
				{
					mii->fMask |= MIIM_STRING;

					mii->set_title(item->title);
					mii->length = item->length;
					mii->title.normalize = item->name;
					mii->keys = item->keys;

					if(!mii->ui)
						mii->ui = Initializer::get_muid(mii->hash);

					if(mii->ui)
					{
						mii->is_system = true;
						mii->id = mii->ui->id;
						mii->image.expr = mii->ui->image;
					}
					else
					{
						mii->id = mii->hash;
						mii->image.expr = _cache->get_image(mii->id);
					}

					_this.id = mii->id;
					_this.length = mii->length;
					_this.title = mii->title.text;
					_this.title_normalize = mii->title.normalize;
				}

				if(_settings.modify_items.image == 2 && !image_disabled_by_checked)
				{
					if(menu->is_main or item->is_toplevel or menu->id == IDENT_ID_VIEW)
					{
						if(Selected.Window.id != WINDOW_SYSMENU)
						{
							if(mii->image.expr or mii->ui)
							{
								is_auto_img = mii->image.expr;
								if(!is_auto_img && mii->ui)
									is_auto_img = bool(mii->ui->image_glyph[0] + mii->ui->image_glyph[1]);

								if(is_auto_img)
									_context.Image(nullptr, mii, font.icon, false);
								else
									mii->ui = nullptr;
							}
						}
					}
				}
				else
				{
					mii->ui = nullptr;
				}

				// titlebar menu
				if(menu->is_main && Selected.Window.id == WINDOW_SYSMENU)
				{
					bool ok = false;
					auto img = &mii->image.draw;
					if(mii->id == IDENT_ID_RESTORE)
					{
						ok = true;
						img->glyph.code[0] = L'\uE297';
						img->glyph.code[1] = 0;
					}
					else if(mii->id == IDENT_ID_CLOSE)
					{
						ok = true;
						img->glyph.code[0] = L'\uE29a';
						img->glyph.code[1] = 0;
					}
					else if(mii->id == IDENT_ID_MINIMIZE)
					{
						ok = true;
						img->glyph.code[0] = L'\uE298';
						img->glyph.code[1] = 0;
					}
					else if(mii->id == IDENT_ID_MAXIMIZE)
					{
						ok = true;
						img->glyph.code[0] = L'\uE299';
						img->glyph.code[1] = 0;
					}

					if(ok)
					{
						mii->image.import = ImageImport::Draw;
						img->type = img->DT_GLYPH;
						img->glyph.font = font.icon10;
						img->glyph.size = { (long)_theme.image.size, (long)_theme.image.size };
						img->glyph.color[0] = _theme.image.color[0];
						img->glyph.color[1] = _theme.image.color[1];
					}
				}

				if(item->image)
				{
					if(image_disabled || image_disabled_by_checked)
					{
						mii->image.destroy();
					}
					else if(!is_auto_img)
					{
						BITMAP bitmap{};
						if(::GetObjectW(item->image, sizeof(BITMAP), &bitmap) == sizeof(BITMAP))
						{
							SIZE size = { bitmap.bmWidth, bitmap.bmHeight };
							if(_theme.image.scale)
							{
								size = { static_cast<long>(_theme.image.size), static_cast<long>(_theme.image.size) };

								if(bitmap.bmWidth != size.cx or bitmap.bmHeight != size.cy)
								{
									auto def_size = size;

									size.cx = dpi.value<long>(bitmap.bmWidth);
									size.cy = dpi.value<long>(bitmap.bmHeight);

									if(size.cx > def_size.cx or size.cy > def_size.cy)
									{
										size.cx = fix_image_size(def_size.cx, size.cx);
										size.cy = fix_image_size(def_size.cy, size.cy);
									}
								}
							}

							uint8_t *bits{};
							if(auto hbitmap = WIC::ToBitmap32(item->image, size, &bits); hbitmap)
							{
								mii->image.hbitmap = hbitmap;
								mii->image.size = size;

								if(!bitmap.bmBits && bits)
								{
									auto p = (RGBA *)bits;
									if(p->a == 0 and (p->r > 0 || p->g > 0 || p->b > 0))
									{
										for(int i0 = 0; i0 < size.cx * size.cy; i0++)
										{
											if(p->a == 0) p->a = 255;
											p++;
										}
									}
								}
							}
						}
					}
				}

				if(item->is_menu())
				{
					mii->type = NativeMenuType::Menu;
					mii->fMask |= MIIM_SUBMENU;
					mii->hSubMenu = CreatePopupMenu();
					mii->sys_items = &item->items;

					auto m_sub = &_menus[mii->hSubMenu];
					m_sub->handle = mii->hSubMenu;
					m_sub->destory = true;
					
					m_sub->id = mii->id;
					m_sub->hash = mii->hash;

					if(mii->ui)
						m_sub->id = mii->ui->id;
					
					m_sub->owner = mii;
					m_sub->std_items = &item->items;

					string path;
					if(!mii->path.empty())
						path = mii->path + L'/';

					path += mii->title.normalize;
					m_sub->path = path.trim(L'/').move();
				}
			}

			for(auto si : item->native_items)
			{
				if(si->has_clsid)
					continue;
				ev_si(si, mii, item);
			}
		}

		bool ContextMenu::prepare_system_items(PositionList &list, menu_t *menu)
		{
			if(!menu || !menu->std_items)
				return false;

			//int _index = 0;
			for(auto item : *menu->std_items)
			{
				_context._this = nullptr;
				auto mii = _gc.push(new MenuItemInfo(MIIM_ID | MIIM_DATA | MIIM_FTYPE, 0, 0));
				prepare_system_item(item, mii, menu);
				list.push(mii);
			}

			return true;
		}

		bool ContextMenu::prepare_system_items2(PositionList &list, menu_t *menu)
		{
			if(!menu)
				return false;

			for(auto item : __movable_system_items)
			{		
				if(item->path.equals(menu->path))
				{
					auto mii = _gc.push(new MenuItemInfo(MIIM_ID | MIIM_DATA | MIIM_FTYPE, 0, 0));
					prepare_system_item(item, mii, menu);
					list.push(mii);
				}
			}

			return true;
		}

		int level = 0;
		//finalize
		LRESULT ContextMenu::OnInitMenuPopup(HMENU hMenu, [[maybe_unused]] uint32_t uPosition)
		{
			__trace(L"ContextMenu.InitMenuPopup begin");

			MENU m = hMenu;
			LRESULT ret = msg.invoke();
			
			auto menu = &_menus[hMenu];
			menu->handle = hMenu;
			menu->is_main = _hMenu == hMenu;
			current.hMenu = hMenu;
			current.menu = menu;

			if(menu->is_main)
			{
				level++;
			}
			else if(menu->id != 0)
			{
				level++;
				parent_level.push_back(menu->id);
			}
			/*else if(menu->id == 0 && !menu->is_dynamic())
			{
				return 0;
			}*/
			
			if(menu->is_main)
			{
				//__system_menuitems
				//if(!load_from_dllgetclassobject)
				//	_settings.dynamics.enabled = false;
			}

		//	_log.info(L"init %d %x", parent_level.size(), menu->id);

			std::vector<MenuItemInfo *> items;
			items.reserve(100);

			PositionList _system_items;
			PositionList _new_items;

			//dynamic items
			MenuItemInfo *owner = nullptr;
			if(menu->is_main)
			{
				menu->dynamics = _cache->dynamic.items;
				menu->std_items = &__system_menu_tree->items;
			}
			else
			{
				owner = menu->owner;
			}
			
			prepare_system_items(_system_items, menu);
			
			std::vector<MenuItemInfo *> bottom_items;

			if(_settings.new_items.enabled)
			{
				if(!(owner && owner->is_disabled()))
				{
					prepare_new_items(_new_items, menu->dynamics, owner, menu);

					// add moved dynamic items
					std::vector<NativeMenu*> mdi;
					for(auto item : _moved_items.dynamics)
					{
						if(item->is_parent(parent_level))
							mdi.push_back(item->owner_dynamic);
					}

					if(!mdi.empty())
						prepare_new_items(_new_items, mdi, owner, menu, true);
				}
				
				prepare_system_items2(_new_items, menu);

				// insert top items
				if(_new_items.size() > 0)
					items.insert(items.end(), _new_items.Top.begin(), _new_items.Top.end());
			}

			if(_system_items.size() > 0)
			{
				items.insert(items.end(), _system_items.Top.begin(), _system_items.Top.end());
				items.insert(items.end(), _system_items.Auto.begin(), _system_items.Auto.end());
				bottom_items.insert(bottom_items.end(), _system_items.Bottom.begin(), _system_items.Bottom.end());
			}

			if(_new_items.size() > 0)
			{
				size_t dynamic_pos = 0;
				
				// is main menu
				if(menu->is_main && Selected.Window.id > WINDOW_UI)
				{
					const int npos = -1;
					auto findpos = [items]() -> size_t
					{
						const int c = static_cast<int>(items.size());
						int pos = c / 2;
						int p = npos;

						if(pos > c)
							pos = c;

						for(int i = pos; i >= 0; i--)
						{
							if(items[i]->is_separator())
							{
								p = i;
								break;
							}
						}

						if(p == npos)
						{
							for(int i = pos; i < c; i++)
							{
								if(items[i]->is_separator())
								{
									p = i;
									break;
								}
							}
						}

						if(p != npos) pos = p;
						return static_cast<size_t>(pos);
					};

					// new menu items ii.fType = [0 || MFT_OWNERDRAW]
					auto _position = _context.parse_position(_cache->dynamic.position);
					switch(_position)
					{
						case Position::Auto:
						{
							if(Selected.Window.id <= WINDOW_EDIT)
							{
								dynamic_pos = items.size();
							}
							else 
							{
								bool rb = !Selected.Background && (Selected.Window.id == WINDOW_RECYCLEBIN);
								if(!Selected.Types[FSO_TASKBAR] && !rb)
								{
									dynamic_pos = items.size() == 0 ? 1 : findpos();
								}
							}
							break;
						}
						case Position::Top:
							dynamic_pos = 0;
							break;
						case Position::Middle:
							dynamic_pos = findpos();
							break;
						case Position::Bottom:
							dynamic_pos = items.size();
							break;
						default:
							dynamic_pos = static_cast<size_t>(_position);
							break;
					}
				}
				else
				{
					dynamic_pos = items.size();
				}

				if(dynamic_pos < 0 || dynamic_pos > items.size())
					dynamic_pos = items.size();

				items.insert(items.begin() + dynamic_pos, _new_items.Auto.begin(), _new_items.Auto.end());
				bottom_items.insert(bottom_items.end(), _new_items.Bottom.begin(), _new_items.Bottom.end());
			}

			items.insert(items.end(), bottom_items.begin(), bottom_items.end());

			auto middle = (items.size() / 2);

			if(_system_items.size() > 0)
				items.insert(items.begin() + middle, _system_items.Middle.begin(), _system_items.Middle.end());

			if(_new_items.size() > 0)
			{
				middle = (items.size() / 2);
				items.insert(items.begin() + middle, _new_items.Middle.begin(), _new_items.Middle.end());
			}

			auto add_custom_items = [&](std::vector<MenuItemInfo *> &custom_items)
			{
				for(auto item : custom_items)
				{
					if(item->indexof.val.empty())
					{
						auto pos = (int)item->position;
						if(pos < 0 || pos >(int)items.size())
							items.push_back(item);
						else
							items.insert(items.begin() + pos, item);
					}
					else
					{
						int i = 0; bool found = false;
						FindPattern find_ptattern;
						if(find_ptattern.split(item->indexof.val, L'|'))
						{
							for(const auto &fp : find_ptattern.matches)
							{
								i = 0;
								for(auto &it : items)
								{
									if(find_ptattern.find(fp, &it->title.normalize))
									{
										found = true;
										items.insert(items.begin() + i + item->indexof.pos, item);
										break;
									}
									i++;
								}
								
								if(found)
									break;
							}
							
							/*for(auto it : items)
							{
								if(find_ptattern(&it->title.normalize))
								{
									items.insert(items.begin() + i + item->indexof.pos, item);
									found = true;
									break;
								}
								i++;
							}*/
						}

						if(!found)
						{
							i = item->indexof.def;
							if(i == -2)
								items.insert(items.begin(), item);
							else if(i == -3)
								items.insert(items.begin() + (items.size() / 2), item);
							else
								items.push_back(item);
						}
					}
				}
			};

			add_custom_items(_new_items.Custom);
			add_custom_items(_system_items.Custom);

			// remove empty dynmic items 
			for(auto i = items.begin(); i < items.end(); i++)
			{
				auto item = *i;
				if(item->is_popup() && MENU::get_count(item->hSubMenu) == 0)
				{
					int cp = 0;

					for(auto x : __movable_system_items)
						cp += x->path.equals((item->path + L"/" + item->title.normalize).trim(L'/'));
					
					for(auto x : _moved_items.dynamics)
						cp += item->id == x->parent.front();

					if(cp == 0)
					{
						if(item->dynamic)
						{
							if(item->owner_dynamic->items.empty())
								items.erase(i--);
						}

						if(item->sys_items && item->sys_items->empty())
							items.erase(i--);
					}
				}

				if(!item->is_separator())
				{
					if(item->is_checked() && _theme.image.display == 0)
						item->image.destroy();

					menu->draw.checks += item->is_checked();
					menu->draw.images += item->has_image_or_draw();
					menu->draw.popups += item->is_popup();
				}
			}

			// remove duplicate separators
			while(!items.empty())
			{
				if(items.back()->is_separator())
					items.pop_back();
				else
				{
					if(items.back()->is_sep_after())
						items.back()->separator &= ~(int)Separator::Bottom;
					break;
				}
			}

			_items.reserve(items.size());
			_items_command.reserve(items.size());
			_items_popup.reserve(items.size());

			DC dc = hwnd.owner;
			dc.set_font(font.handle);

			//menu->draw = {};
			menu->draw.width = 0;
			menu->draw.height = 0;
			menu->draw.length = 0;

			long max_text_len = 0;
			int col = 0;

			// insert items to native menu
			int i = 0, x = 0;
			for(auto item : items)
			{
				item->remove_bitmap();
				item->add_ownerdraw();
				item->handle = hMenu;
				item->size = {};

				/*if(!item->is_separator() && item->title.empty() && menu->is_main)
				{
					if(!item->has_image_or_draw())
						continue;
				}*/
				if(!item->is_separator())
				{
					//item->wID += 100;
				}
				
				if(auto res = m.insert(item, i, true, x++); res)
				{
					if(item->is_separator())
					{
						menu->popup_height += item->size.cy +
							_theme.separator.margin.top + _theme.separator.margin.bottom +
							_theme.separator.size;
					}
					else
					{
						if(item->is_column() > 0)
						{
							col++;
							menu->has_col = true;
						}

						if(item->cch > 0)
						{
							Rect rc;

							if(item->column == 1)
								menu->draw.width = 0;

							dc.measure(item->title, item->title.length(), &rc, DT_NOCLIP | _theme.text.prefix);

							if(!item->keys.empty())
							{
								Rect rc_keys;
								dc.measure(item->keys, item->keys.length<uint32_t>(), &rc_keys, DT_NOCLIP | _theme.text.prefix);
								rc.right += rc_keys.width() + _theme.text.tap;
							}

							if(rc.right > max_text_len)
								max_text_len = rc.right;
							else
								rc.right = max_text_len;

							menu->draw.length = std::max<uint32_t>(rc.right, menu->draw.length);
							menu->draw.width = rc.right;

							if(menu->draw.checks || menu->draw.images)
							{
								// image siz
								item->size.cx += _theme.image.size;
								// padding left for text
								item->size.cx += _theme.image.gap;

								if(_theme.image.display == 2)
								{
									if(menu->draw.checks && menu->draw.images)
									{
										// image siz
										item->size.cx += _theme.image.size;
										// padding left for text
										item->size.cx += _theme.image.gap;
									}
								}
							}

							if(menu->draw.popups)
								item->size.cx += _theme.image.gap + symbol.chevron.size.cx;
							
							item->size.cx += rc.right;
							item->size.cy = rc.bottom;

							if(menu->draw.height < rc.bottom)
								menu->draw.height = rc.bottom;
						}

						menu->popup_height += item->size.cy +
							_theme.back.padding.top + _theme.back.padding.bottom + 
							_theme.back.margin.top + _theme.back.margin.bottom;

						_items.push_back(item);
					}

					if(item->dynamic && item->is_popup_or_item())
					{
						if(item->is_popup())
							_items_popup.push_back(item);
						else
							_items_command.push_back(item);
					}

					if(menu->is_main && item->is_popup())
						_main_popup.push_back(item);

					i += res;
				}
				else if(!item->is_separator())
				{
					//_log.warning(L"Not inserted %s", item->title.c_str());
				}
			}

			dc.reset_font();

			long image__size = _theme.image.size;
			if(menu->draw.height < image__size)
				menu->draw.height = image__size;

			if(menu->draw.height % 2)
				menu->draw.height++;

			menu->popup_height += _theme.border.padding.top + _theme.border.padding.bottom + _theme.border.size + _theme.border.size;
			

			MENUINFO mi = { sizeof(mi), MIM_STYLE | MIM_BACKGROUND | MIM_MAXHEIGHT };
			if(m.get(&mi))
			{
				if(mi.hbrBack)
				{
					if(hMenu == _hMenu)
						mi.hbrBack = _hbackground0;
					else
						::DeleteObject(mi.hbrBack);
				}

				Flag<uint32_t> style(mi.dwStyle);
				style.add(MNS_CHECKORBMP);
				style.add(MNS_NOCHECK);

				menu->popup_height += 100;
				auto h = _rcMonitor.height();
				if(menu->popup_height >= h)
				{
					menu->popup_height = 0;
					if(menu->is_main)
					{
						//mi.cyMax = h - 100;
						//style.add(MIM_MAXHEIGHT);
						//menu->popup_height = mi.cyMax;
					}
				}
				else {
					menu->popup_height = 0;
				}

				mi.hbrBack = composition ? GetStockBrush(BLACK_BRUSH) : ::CreateSolidBrush(_theme.background.color.to_BGR());
				m.set(&mi);
			}

			__trace(L"ContextMenu.InitMenuPopup end");

			return ret;
		}

		//if(CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, FRIENDLY_NAME_DOCUMENTS, -1, friendlyPath.c_str(), -1)
		LRESULT ContextMenu::OnUninitMenuPopup(HMENU hMenu)
		{
			auto menu = &_menus[hMenu];
			current.hMenu = hMenu;

			if(level > 0) level--;

			if(!parent_level.empty())
				parent_level.pop_back();

			_tip.hide();

			/*
			if(!menu->is_dynamic())
				menu->dynamics.clear();
			*/

			auto ret = msg.invoke();
			::DestroyMenu(hMenu);
			
			__trace(L"ContextMenu.UninitMenuPopup");
			
			current.hMenu = nullptr;
			menu->wnd = nullptr;
			return ret;
		}

		LRESULT ContextMenu::OnInitMenu([[maybe_unused]] HMENU hMenu)
		{
			//_hMenu = hMenu;
			//_context.hMenu = hMenu;

			_tip.create();
			//m_log->info(L"WM_INITMENU");
			//m_INITMENU = hMenu;
			return msg.invoke();
		}

		//VirtualFolder
		LRESULT ContextMenu::OnMenuSelect([[maybe_unused]] HMENU hMenu, [[maybe_unused]] uint32_t itemId, [[maybe_unused]] uint32_t flags)
		{
			//log->info(L"OnMenuSelect");
			current.hMenu = hMenu;
			_tip.hide();
			return msg.invoke();
		}

		//
		void ContextMenu::draw_string(HDC hdc, HFONT hFont, const Rect *rc, const Color &color, const wchar_t *text, int length, DWORD format, bool disable_BufferedPaint)
		{
			if(color.a == 0)
				return;

			BufferedPaint bp(hdc, rc);
			HDC hdcPaint = disable_BufferedPaint ? nullptr : bp.begin(color.a);

			if(!hdcPaint)
				hdcPaint = hdc;
			if(hdcPaint)
			{
				bp.clear();
				//::SetTextColor(hdcPaint, color);
				//::SetBkMode(hdcPaint, TRANSPARENT);
				auto hFontOld = ::SelectObject(hdcPaint, hFont);
				DTTOPTS dttOpts = { sizeof(DTTOPTS),  DTT_COMPOSITED | DTT_TEXTCOLOR, color.to_BGR() };
				//::SetTextAlign(hdcPaint, TA_BASELINE | TA_UPDATECP);
				::DrawThemeTextEx(_hTheme, hdcPaint, 0, 0, text, length, format, const_cast<Rect *>(rc), &dttOpts);
				::SelectObject(hdcPaint, hFontOld);
			}
		}

		void ContextMenu::draw_rect(DC *dc, const POINT &pt, const SIZE &size, const Color &color, const Color &border, int radius)
		{
			PlutoVG pluto(size.cx, size.cy);

			if(border.a > 0)
			{
				pluto.rect(.5, .5, size.cx - 1, size.cy - 1, radius, radius);
				if(color.a > 0)
					pluto.fill(color.to_RGB(), color.a, true);
				pluto.stroke_width(1)
					.stroke_fill(border.to_RGB(), border.a);
			}
			else
			{
				pluto.rect(0, 0, size.cx, size.cy, radius, radius)
					.fill(color.to_RGB(), color.a, true);
			}

			auto_gdi<HBITMAP> hbitmap(pluto.tobitmap());
			dc->draw_image(pt, size, hbitmap.get());
		}


		struct DRAWITEMSTATE
		{
			bool selected{};
			bool disabled{};
			bool grayed{};
			bool checked{};
			bool focus{};
			bool default${};
			bool hotlight{};
			bool inactive{};
			bool no_accel{};
			bool no_focus_rect{};

			DRAWITEMSTATE(uint32_t flags)
			{
				Flag<uint32_t> fs = flags;
				selected = fs.has(ODS_SELECTED);
				grayed = fs.has(ODS_GRAYED);
				disabled = fs.has(ODS_DISABLED) || grayed;
				checked = fs.has(ODS_SELECTED);
			}
		};

		LRESULT ContextMenu::OnDrawItem_D2D(DRAWITEMSTRUCT *di)
		{
			LRESULT lret = TRUE;

			auto hMenu = reinterpret_cast<HMENU>(di->hwndItem);
			auto rc = reinterpret_cast<const Rect *>(&di->rcItem);
			DC dc = di->hDC;
			Flag<uint32_t> faction = di->itemAction;
			Flag<uint32_t> fState = di->itemState;

			DRAWITEMSTATE state(di->itemState);

			auto draw_entire = faction.has(ODA_DRAWENTIRE);
			//state.selected = fState.has(ODS_SELECTED);
			//state.disabled = fState.has(ODS_DISABLED) || fState.has(ODS_GRAYED);

			Color back_color = _theme.back.color.nor;
			Color text_color = _theme.text.color.nor;

			_tip.hide(!draw_entire);

			D2D d2d2;
			d2d2.init_res();
			d2d2.begin(di->hDC, *rc);

			D2D1_SIZE_F size_f = { static_cast<float>(rc->width()), static_cast<float>(rc->height()) };

			//d2d->render->SetDpi(ctx->dpi.val, ctx->dpi.val);
			//d2d->render->SetTransform(D2D1::Matrix3x2F::Identity());

			d2d2.render->Clear(_theme.background.color);

			if(state.selected)
			{
				if(state.disabled)
				{
					back_color = _theme.back.color.sel_dis;
					text_color = _theme.text.color.sel_dis;
				}
				else
				{
					back_color = _theme.back.color.sel;
					text_color = _theme.text.color.sel;
				}
			}
			else if(state.disabled)
			{
				back_color = _theme.back.color.nor_dis;
				text_color = _theme.text.color.nor_dis;
			}

			if(di->itemID == MF_NOITEM)
			{
				auto rect = *rc;
				rect.left += _theme.separator.margin.left;
				rect.right -= _theme.separator.margin.right;
				//rect.top += _theme.separator.margin.top;		
				rect.top += _theme.separator.margin.top;
				rect.bottom = rect.top + _theme.separator.size;
				//dc.fill_rect(*rc, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);
				//draw_rect(&dc, rect.point(), { rect.width(), _theme.separator.size }, _theme.separator.color);
				//d2d2.brush->SetColor(D2D1::ColorF(0.f, 1.f, 0.f, 1.f));
				d2d2.brush->SetColor(_theme.separator.color);
				//d2d2.render->DrawLine(D2D1::Point2F((float)rect.left, 0), D2D1::Point2F((float)rect.right, 10),
				//					  d2d2.brush, 0.5f);

				D2D1_RECT_F rectF{};
				rectF.left = (float)rect.left;
				rectF.right = (float)rect.width();
				rectF.top = ((float)(rc->height() + _theme.separator.size) / 2.f) - _theme.separator.size;
				rectF.bottom = rectF.top + (float)_theme.separator.size;
				
				d2d2.render->FillRectangle(rectF, d2d2.brush);

				d2d2.end(true);
				dc.exclude_clip_rect(*rc);
				return lret;
			}

			auto menu = &_menus[hMenu];

			auto mii = get_item(di->itemID, hMenu, _items);

			if(!mii || (mii->title.empty() && !ident.equals(mii->wID)))
			{
				lret = msg.invoke();
				//DC dc_layer(dc.CreateCompatibleDC(), 1);
				dc.set_back_mode(true);
				dc.set_back(back_color);
				dc.set_text(text_color);
				//dc.fill_rect(di->rcItem, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);
				
				d2d2.end(true);
				dc.exclude_clip_rect(*rc);
				return lret;
			}

			auto is_label = mii->visibility == Visibility::Label;
			auto is_static = mii->visibility == Visibility::Static;
			auto is_static_or_label = is_static || is_label;

			if(draw_entire)
			{
				mii->index = MENU::get_index(hMenu, mii->wID);
				::GetMenuItemRect(0, hMenu, mii->index, &mii->rect);
				
				//dc.fill_rect(di->rcItem, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);
			}
			else
			{
				if(state.disabled && is_static_or_label)
				{
					d2d2.end(true);
					dc.exclude_clip_rect(*rc);
					return lret;
				}
			}

			if(!(state.disabled && is_static_or_label))
			{
				if(state.selected)
				{
					current.select_previtem = current.selectitem;
					current.selectitem = mii;
					if(mii->tip)
						current.tip = mii;
				}
				else
				{
				}
			}

			if(state.disabled)
			{
				if(is_static_or_label)
				{
					state.disabled = false;
					back_color = _theme.back.color.nor;
					text_color = _theme.text.color.nor;
				}
				else
				{
				}
			}

			const long image_size = _theme.image.size;
			auto rcblock = *rc;

			rcblock.top = _theme.back.margin.top;
			rcblock.bottom = rc->height() - _theme.back.margin.bottom;

			if(mii->cch > 0 || !menu->has_col)
			{
				rcblock.left += _theme.back.margin.left;
				rcblock.right -= _theme.back.margin.right;
			}
			else
			{
				rcblock.left += _theme.back.margin.left + dpi(3);
				rcblock.right -= _theme.back.margin.right + dpi(3);
			}

			//const auto width = rcblock.width();
			const auto height = rcblock.height();

			auto rcimg = rcblock;
			auto rcText = rcblock;

			rcimg.top = rcblock.top + ((height - image_size) / 2);
			rcimg.bottom = rcimg.top + image_size;

			if(mii->cch > 0 || !menu->has_col)
			{
				if(mii->cch == 0)
				{
				}
				else
				{
					rcimg.left = rcblock.left + _theme.back.padding.left;
					rcimg.right = rcimg.left + image_size;
				}
			}

			if(!is_static_or_label)
			{
				uint8_t op = back_color.a;

				Color border_color = _theme.back.border.nor;

				if(state.selected)
				{
					if(state.disabled && _theme.back.color.sel_dis.a > 0)
						op = _theme.back.color.sel_dis.a;
					else if(!state.disabled && _theme.back.color.sel_dis.a > 0)
						op = _theme.back.color.sel.a;

					border_color = state.disabled ? _theme.back.border.sel_dis : _theme.back.border.sel;
				}
				else
				{
					if(state.disabled && _theme.back.color.nor_dis.a > 0)
						op = _theme.back.color.nor_dis.a;
					else if(!state.disabled && _theme.back.color.nor.a > 0)
						op = _theme.back.color.nor.a;

					if(state.disabled)
						border_color = _theme.back.border.nor_dis;
				}

				//if(op > 0)
				{
					back_color.a = op;
					//draw_rect(&dc, rcblock.point(), { width, height }, back_color, border_color, _theme.back.radius);

					D2D1_RECT_F rectF = { 0, 0, size_f.width, size_f.height };

					if(state.selected)
					{
						d2d2.brush->SetColor(_theme.background.color);
						d2d2.render->FillRectangle(rectF, d2d2.brush);
					}

					d2d2.brush->SetColor(back_color);

					if(_theme.back.radius == 0)
					{
						d2d2.render->FillRectangle(rectF, d2d2.brush);
					}
					else 
					{
						auto radius = (float)_theme.back.radius;
						
						d2d2.render->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
						d2d2.render->FillRoundedRectangle({ rectF,radius, radius }, d2d2.brush);
					}
				}
			}

			auto has_checked_image = menu->draw.checks && menu->draw.images && (_theme.image.display >= 2);
			
			if(!mii->title.empty())
			{
				Color clrtext = text_color;

				if(!is_label && menu->draw.has_align())
				{
					rcText.left = rcblock.left + _theme.image.size + _theme.image.gap + _theme.back.padding.left;
					if(has_checked_image)
						rcText.left += _theme.image.size + _theme.image.gap;
				}
				else
				{
					rcText.left += _theme.back.padding.left;
				}

				rcText.right -= _theme.back.padding.right;

				if(mii->tab >= 0 && mii->is_popup())
					rcText.right -= _theme.image.size;

				auto txtfmt = DT_NOCLIP | DT_SINGLELINE | DT_VCENTER;

				if(_theme.text.prefix)
					txtfmt |= _theme.text.prefix;


				//rcText.top = rc->top - dpi(1);
				//rcText.bottom = rc->bottom;

				if(mii->tab <= 0 && mii->keys.empty())
				{
					//draw_string(dc, font.handle, &rcText, clrtext, mii->title, mii->title.length<int>(), (mii->tab < 0 ? DT_LEFT : DT_RIGHT) | txtfmt);
					///_log.info(L"%d %s", _theme.font.lfHeight, _theme.font.lfFaceName);
					auto tf = d2d2.createTextFormat(_theme.font.lfFaceName, std::abs(_theme.font.lfHeight));
					if(tf)
					{
						d2d2.render->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE::D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
						tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
						tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
						tf->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
						//_log.info(L"%d %s", _theme.font.lfHeight, _theme.font.lfFaceName);
						d2d2.brush->SetColor(clrtext);

						D2D1_RECT_F rect_ =
						{
							(float)rcText.left, (float)rcText.top,
							float(rcText.width()),
							float(rcText.height())
						};
						
					
						d2d2.render->DrawTextW(mii->title.normalize, mii->title.normalize.length<uint32_t>(),
											   tf,
											   rect_,
											   d2d2.brush);
						tf->Release();
					}
				}
				else
				{
					/*auto ds = [&](const string &left, const string &right)
					{
						if(!left.empty())
							draw_string(dc, font.handle, &rcText, clrtext, left, left.length<int>(), DT_LEFT | txtfmt);

						if(!right.empty())
						{
							Color c = clrtext;
							LOGFONTW lf{};
							std::memcpy(&lf, &_theme.font.lfHeight, sizeof lf);
							lf.lfHeight = long(lf.lfHeight * 0.80f);
							lf.lfWeight = FW_LIGHT;
							//lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
							auto_gdi<HFONT> r_hfont(::CreateFontIndirectW(&lf));
							if(menu->id == IDENT_ID_INSERT_UNICODE_CONTROL_CHARACTER)
								c.opacity(state.disabled ? 50 : 100);
							else
								c.opacity(state.disabled ? 30 : 50);

							draw_string(dc, r_hfont.get(), &rcText, c, right, right.length<int>(), DT_RIGHT | txtfmt);
						}
					};

					if(mii->keys.empty())
					{
						string left = mii->title.text.substr(0, mii->tab).trim_end().move();
						string right = mii->title.text.substr(mii->tab).trim_start().move();
						ds(left, right);
					}
					else
					{
						ds(mii->title.text, mii->keys);
					}*/
				}

			}

			if(state.selected && mii->tip)
				//	_tip.show(mii->tip, mii->rect);
				_tip.show(mii->tip.text, mii->tip.type, mii->tip.time, mii->rect);

			d2d2.end(true);

			// exlude menu item rectangle to prevent drawing by windows after us
			dc.exclude_clip_rect(*rc);

			return lret;
		}

		int ooo = 0;

		LRESULT ContextMenu::OnDrawItem(DRAWITEMSTRUCT *di)
		{
			LRESULT lret = TRUE;
			//current.selectitem = nullptr;
			if(di->itemID == 0x5ffffffe)
				return lret;

			bool render_d2d = false;
			if(render_d2d)
				return OnDrawItem_D2D(di);

			
			auto hMenu = reinterpret_cast<HMENU>(di->hwndItem);
			auto rc = reinterpret_cast<const Rect *>(&di->rcItem);

			Flag<uint32_t> faction = di->itemAction;
			Flag<uint32_t> fState = di->itemState;

			DRAWITEMSTATE state(di->itemState);

			auto draw_entire = faction.has(ODA_DRAWENTIRE);

			Color back_color = _theme.back.color.nor;
			Color text_color = _theme.text.color.nor;

			_tip.hide(!draw_entire);

			DC dc = di->hDC;

			dc.set_back_mode();
			
			if(state.selected)
			{
				if(state.disabled)
				{
					back_color = _theme.back.color.sel_dis;
					text_color = _theme.text.color.sel_dis;
				}
				else
				{
					back_color = _theme.back.color.sel;
					text_color = _theme.text.color.sel;
				}
			}
			else if(state.disabled)
			{
				back_color = _theme.back.color.nor_dis;
				text_color = _theme.text.color.nor_dis;
			}

			if(di->itemID == MF_NOITEM)
			{
				auto rect = *rc;
				rect.left += _theme.separator.margin.left;
				rect.right -= _theme.separator.margin.right;
				//rect.top += _theme.separator.margin.top;		
				rect.top += _theme.separator.margin.top;
				rect.bottom = rect.top + _theme.separator.size;
				//draw_rect(&dc, rc->point(), rc->size(), _theme.background.color);
				dc.fill_rect(*rc, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);
				draw_rect(&dc, rect.point(), { rect.width(), _theme.separator.size }, _theme.separator.color);
				return 1;
			}

			//dc.draw_fillrect({ rc->left, rc->top, rc->right, rc->bottom }, (composition ? 0x000000 : _theme.background.color));
			//dc.fill_rect(di->rcItem, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);

			auto menu = &_menus[hMenu];

			auto mii = get_item(di->itemID, hMenu, _items);

			if(!mii || (mii->title.empty() && !ident.equals(mii->wID)))
			{
				if(auto_gdi<HBITMAP> hbitmap(dc.createbitmap(rc->width(), rc->height())); hbitmap)
				{
					DC dcmem(dc.CreateCompatibleDC(), 1);
					dcmem.select_bitmap(hbitmap.get());

					auto old_hdc = di->hDC;
					di->hDC = dcmem;

					lret = msg.invoke();
					
					di->hDC = old_hdc;

					std::vector<COLORREF> pixels(rc->width() * rc->height());

					BITMAPINFOHEADER bmpInfo = { 0 };
					bmpInfo.biSize = sizeof(bmpInfo);
					bmpInfo.biWidth = rc->width();
					bmpInfo.biHeight = -int(rc->height());
					bmpInfo.biPlanes = 1;
					bmpInfo.biBitCount = 32;
					bmpInfo.biCompression = BI_RGB;

					::GetDIBits(dcmem, hbitmap.get(), 0, rc->height(), &pixels[0], (LPBITMAPINFO)&bmpInfo, DIB_RGB_COLORS);

					std::for_each(pixels.begin(), pixels.end(), [](COLORREF &pixel) {
						if(pixel != 0) // black pixels stay transparent
							pixel |= 0xFF000000; // set alpha channel to 100%
					});

					::SetDIBits(dcmem, hbitmap.get(), 0, rc->height(), &pixels[0], (LPBITMAPINFO)&bmpInfo, DIB_RGB_COLORS);
					dc.draw_image(rc->point(), rc->size(), dcmem);

					return lret;
				}

				dc.set_back_mode(true);
				dc.set_back(back_color);
				dc.set_text(text_color);
				dc.fill_rect(di->rcItem, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);

				lret = msg.invoke();

				return lret;
			}

			auto is_label = mii->visibility == Visibility::Label;
			auto is_static = mii->visibility == Visibility::Static;
			auto is_static_or_label = is_static || is_label;

			if(draw_entire)
			{
				mii->index = MENU::get_index(hMenu, mii->wID);
				::GetMenuItemRect(0, hMenu, mii->index, &mii->rect);
				dc.fill_rect(di->rcItem, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);
			}
			else
			{
				if(state.disabled && is_static_or_label)
				{
					dc.exclude_clip_rect(*rc);
					return true;
				}
				
				dc.fill_rect(di->rcItem, composition ? dc.stock_brush(BLACK_BRUSH) : _hbackground);
			}

			if(!(state.disabled && is_static_or_label))
			{
				if(state.selected)
				{
					current.select_previtem = current.selectitem;
					current.selectitem = mii;
					if(mii->tip)
						current.tip = mii;
				}
			}

			if(state.disabled)
			{
				if(is_static_or_label)
				{
					state.disabled = false;
					back_color = _theme.back.color.nor;
					text_color = _theme.text.color.nor;
				}
			}

			const long image_size = _theme.image.size;

			auto rcblock = *rc;
			
			rcblock.top += _theme.back.margin.top;
			rcblock.bottom -= _theme.back.margin.bottom;

			if(mii->cch > 0 || !menu->has_col)
			{
				rcblock.left += _theme.back.margin.left;
				rcblock.right -= _theme.back.margin.right;
			}
			else
			{
				rcblock.left += _theme.back.margin.left + dpi(3);
				rcblock.right -= _theme.back.margin.right + dpi(3);
			}

			const auto width = rcblock.width();
			const auto height = rcblock.height();

			auto rcimg = rcblock;
			auto rcText = rcblock;

			rcimg.top = rcblock.top + ((height - image_size) / 2);
			rcimg.bottom = rcimg.top + image_size;

			if(mii->cch > 0 || !menu->has_col)
			{
				if(mii->cch == 0)
				{
				}
				else 
				{
					rcimg.left = rcblock.left + _theme.back.padding.left;
					rcimg.right = rcimg.left + image_size;
				}
			}

			if(!is_static_or_label)
			{
				uint8_t op = back_color.a;

				Color border_color = _theme.back.border.nor;

				if(state.selected)
				{
					if(state.disabled && _theme.back.color.sel_dis.a > 0)
						op = _theme.back.color.sel_dis.a;
					else if(!state.disabled && _theme.back.color.sel_dis.a > 0)
						op = _theme.back.color.sel.a;

					border_color = state.disabled ? _theme.back.border.sel_dis : _theme.back.border.sel;
				}
				else
				{
					if(state.disabled && _theme.back.color.nor_dis.a > 0)
						op = _theme.back.color.nor_dis.a;
					else if(!state.disabled && _theme.back.color.nor.a > 0)
						op = _theme.back.color.nor.a;

					if(state.disabled)
						border_color = _theme.back.border.nor_dis;
				}
				
				//dc.draw_fill_rounded_rect(rcblock, _theme.back.radius+2, 0,0);
				//draw_rect(&dc, rcblock.point(), { width, height }, 0xff000000, {}, _theme.back.radius);
				//draw_rect(&dc, rcblock.point(), { width, height }, _theme.background.color, {}, _theme.back.radius);
				//if(op > 0)
				{
					back_color.a = op;
					draw_rect(&dc, rcblock.point(), { width, height }, back_color, border_color, _theme.back.radius);
				}
			}

			auto has_checked_image = menu->draw.checks && menu->draw.images && (_theme.image.display >= 2);

			if(!is_label)
			{
				auto rcchekhed = rcimg;
				if(has_checked_image)
				{
					auto offset = _theme.image.size + _theme.image.gap;
					rcimg.left += offset;
					rcimg.right += offset;
				}

				//Draw custom submenu arrow and checked
				if(mii->is_popup() || mii->is_checked())
				{
					auto sy = mii->is_popup() ? &symbol.chevron : mii->is_radiocheck() ? &symbol.bullet : &symbol.checked;
					auto hbitmap = state.selected ? sy->select : sy->normal;
					
					if(state.disabled)
						hbitmap = state.selected ? sy->select_disabled : sy->normal_disabled;

					if(mii->is_popup())
					{
						auto rect = rcblock;
						auto x = rect.right - (_theme.back.padding.right + symbol.chevron.size.cx);
						auto y = rcblock.top + ((height - symbol.chevron.size.cy) / 2);
						dc.draw_image({ x, y }, { symbol.chevron.size.cx, symbol.chevron.size.cy }, hbitmap, state.disabled ? 64 : 255);
					}
					else if(mii->is_checked() && (_theme.image.display != 1 || !mii->has_image_or_draw()))
					{
						long z = _theme.image.size;
						dc.draw_image(rcchekhed.point(), { z, z }, hbitmap, state.disabled ? 64 : 255);
					}
				}

				auto image = &mii->image;

				if(state.selected && mii->image_select.isvalid())
					image = &mii->image_select;

				if(image->hbitmap)
				{
					DC memDC(::CreateCompatibleDC(dc), 1);
					if(memDC)
					{
						if(memDC.select_bitmap(image->hbitmap))
						{
							Rect rcim = { rcimg.left + ((image_size - image->size.cx) / 2),
								(rcblock.top + (rcblock.bottom - image->size.cy)) / 2,
								image->size.cx, image->size.cy };
							
							//if(image->bitsPixel < 32)
							//	dc.bitblt({ rcim.left,rcim.top,size.cx,size.cy }, memDC, 0, 0);
							//else
							{
								bool is_16 = image->size.cx <= dpi(16) && image->size.cy <= dpi(16);
								if(image->import == ImageImport::SVG && is_16)
									dc.draw_image(rcim.point(), image->size, memDC, state.disabled ? 48 : 192);
								dc.draw_image(rcim.point(), image->size, memDC, state.disabled ? 64 : 255);
							}
							memDC.reset_bitmap();
						}
					}
				}
				else if(image->import == ImageImport::Draw)
				{
					auto draw = &image->draw;
					Color clr = text_color;
					SIZE size{};

					Color color_[2];
					color_[0] = text_color;

					if(draw->type == draw->DT_SHAPE)
					{
						auto shape = &draw->shape;
						size = shape->size;

						if(size.cx > 0 && size.cy > 0)
						{
							clr = shape->color[0];

							if(state.disabled) clr.opacities();

							if(size.cx > image_size) size.cx = image_size;
							if(size.cy > image_size) size.cy = image_size;

							if(shape->solid)
							{
								if(size.cx <= 3) size.cx = size.cx + 2;
								if(size.cy <= 3) size.cy = size.cy + 2;
							}
							else
							{
								if(size.cx == 1) size.cx = size.cx + 1;
								if(size.cy == 1) size.cy = size.cy + 1;
							}

							long left = rcimg.left + ((image_size - size.cx) / 2);
							if(mii->cch == 0)
							{
								left = rcimg.left + ((width - size.cx) / 2);
							}

							long top = rcimg.top + ((image_size - size.cy) / 2);
							draw_rect(&dc, { left, top}, size, clr);
						}
					}
					else if(draw->type == draw->DT_GLYPH)
					{
						if(mii->cch == 0)
						{
							rcimg = *rc;
							rcimg.left += 3;
							rcimg.right -= 3;
						}

						auto g = &draw->glyph;
						size = { g->size.cx, g->size.cy };
						if(size.cx > 0 && size.cy > 0 && g->font)
						{
							if(size.cx > image_size) size.cx = image_size;
							if(size.cy > image_size) size.cy = image_size;
							
							color_[0] = g->color[0];
							color_[1] = g->color[1];

							if(!color_[0])
							{
								if(_theme.image.color[0])
									color_[0] = _theme.image.color[0];
								else
								{
									if(state.disabled && (_theme.text.color.nor_dis))
										color_[0] = _theme.text.color.nor_dis;
									else
										color_[0] = text_color;
								}
							}

							if(!color_[1])
								color_[1] = _theme.image.color[0] ? _theme.image.color[1] : text_color;

							if(state.disabled)
							{
								color_[0].opacities();
								color_[1].opacities();
							}

							auto txtfmt = DT_NOCLIP | DT_SINGLELINE | DT_VCENTER;
							if(g->code[0])
								draw_string(dc, g->font, &rcimg, color_[0], &g->code[0], 1, DT_CENTER | txtfmt);

							if(g->code[1])
								draw_string(dc, g->font, &rcimg, color_[1], &g->code[1], 1, DT_CENTER | txtfmt);
						}
					}
				}
			}

			if(!mii->title.empty())
			{
				Color clrtext = text_color;

				if(!is_label && menu->draw.has_align())
				{
					rcText.left = rcblock.left + _theme.image.size + _theme.image.gap + _theme.back.padding.left;
					if(has_checked_image)
						rcText.left += _theme.image.size + _theme.image.gap;
				}
				else
				{
					rcText.left += _theme.back.padding.left;
				}

				rcText.right -= _theme.back.padding.right;

				if(mii->tab >= 0 && mii->is_popup())
					rcText.right -= _theme.image.size;

				auto txtfmt = DT_NOCLIP | DT_SINGLELINE | DT_VCENTER;

				if(_theme.text.prefix)
					txtfmt |= _theme.text.prefix;

				if(_hTheme)
				{
					rcText.top = rc->top - dpi(1);
					rcText.bottom = rc->bottom;

					if(mii->tab <= 0 && mii->keys.empty())
					{
						draw_string(dc, font.handle, &rcText, clrtext, mii->title, mii->title.length<int>(), (mii->tab < 0 ? DT_LEFT : DT_RIGHT) | txtfmt);
					}
					else
					{
						auto ds = [&](const string &left, const string &right)
						{
							if(!left.empty())
								draw_string(dc, font.handle, &rcText, clrtext, left, left.length<int>(), DT_LEFT | txtfmt);

							if(!right.empty())
							{
								Color c = clrtext;
								LOGFONTW lf{};
								std::memcpy(&lf, &_theme.font.lfHeight, sizeof lf);
								lf.lfHeight = long(lf.lfHeight * 0.80f);
								lf.lfWeight = FW_LIGHT;
								auto_gdi<HFONT> r_hfont(::CreateFontIndirectW(&lf));
								if(menu->id == IDENT_ID_INSERT_UNICODE_CONTROL_CHARACTER)
									c.opacity(state.disabled ? 50 : 100);
								else
									c.opacity(state.disabled ? 30 : 50);

								draw_string(dc, r_hfont.get(), &rcText, c, right, right.length<int>(), DT_RIGHT | txtfmt);
							}
						};

						if(mii->keys.empty())
						{
							string left = mii->title.text.substr(0, mii->tab).trim_end().move();
							string right = mii->title.text.substr(mii->tab).trim_start().move();
							ds(left, right);
						}
						else
						{
							ds(mii->title.text, mii->keys);
						}
					}
				}
			}
			
			// exlude menu item rectangle to prevent drawing by windows after us
			dc.exclude_clip_rect(*rc);

			if(state.selected && mii->tip)
				//	_tip.show(mii->tip, mii->rect);
				_tip.show(mii->tip.text, mii->tip.type, mii->tip.time, mii->rect);

			return TRUE;
		}

		LRESULT ContextMenu::OnMeasureItem(MEASUREITEMSTRUCT *mi)
		{
			LRESULT lret = 0;
			auto menu = current.menu;

			mi->itemHeight = 0;
			mi->itemWidth = 0;
			
			if(mi->itemID == 0x5ffffffe)
			{
				mi->itemWidth = 260;
				mi->itemHeight = 50;
			}
			else if(mi->itemID == MF_NOITEM)
			{
				mi->itemHeight = _theme.separator.margin.height() + _theme.separator.size;
			}
			else
			{
				mi->itemWidth = _theme.back.width();
				auto mii = get_item(mi->itemID, menu->handle, _items);
				if(mii)
				{
					if(mii->cch == 0)
					{
						if(!ident.equals(mi->itemID))
							lret = msg.invoke();
						else 
						{
							auto v = (uint32_t)_theme.view2;
							if(v < _theme.image.size)
								v = _theme.image.size;

							mi->itemWidth += v;
							if(mi->itemWidth >= (v*2))
							mi->itemWidth /= 2;
							mi->itemHeight += mii->is_spacer() ? dpi(10u) : v;
						}
					}
					else 
					{
						mi->itemHeight += mii->size.cy + _theme.back.height();
						if(mi->itemHeight % 2)
							mi->itemHeight++;

						mi->itemWidth += std::max<uint32_t>(menu->draw.length, mii->size.cx);

						// Remove extra space 'Submenu icon size'
						mi->itemWidth -= dpi.original(14);
					}
				}
			}

			if(_theme.layout.max_width > 0)
			{
				if(mi->itemWidth > _theme.layout.max_width)
					mi->itemWidth = _theme.layout.max_width;
			}

			if(_theme.layout.min_width > 0)
			{
				if(mi->itemWidth < _theme.layout.min_width)
					mi->itemWidth = _theme.layout.min_width;
			}

			return lret;
		}

		LRESULT ContextMenu::OnStart()
		{
			return msg.invoke();
		}

		LRESULT ContextMenu::OnEnd()
		{
			Selections::point = { 0, 0 };
			//InvokeCommand(selectid);
			current.zero();

			__trace(L"ContextMenu.End");

			auto ret = msg.invoke();
			_menus.clear();

			return ret;
		}

		LRESULT ContextMenu::OnTimer([[maybe_unused]] UINT_PTR nIDEvent, [[maybe_unused]] TIMERPROC Timerproc)
		{
			return msg.invoke();
		}

		void ContextMenu::init_cfg()
		{
			//SystemParametersInfoForDpi
			auto sets = &_cache->settings;
			//auto isW11OrGreater = ver->IsWindows11OrGreater();
			auto isHighContrast = Theme::IsHighContrast();

			Object obj;

			_context.theme = &_theme;
			_theme.dpi = &dpi;
			_context.font.icon = FontCache::Default;

			font.menu = {};
			Theme::GetFont(&font.menu, dpi.val);

		//	long zofont = std::abs(font.menu.lfHeight);

			font.menu.lfHeight = dpi(font.menu.lfHeight);

			_context.font.text = font.menu.lfFaceName;

			bool enableTransparency = false;
			bool systemUsesLightTheme = true;
			bool appsUseLightTheme = true;

			if(isHighContrast)
				_theme.system.mode = 2;
			else
			{
				Theme::Personalize(&systemUsesLightTheme, &appsUseLightTheme, &enableTransparency);
				_theme.system.mode = systemUsesLightTheme ? 0 : 1;
			}

			_theme.enableTransparency = enableTransparency;
			_theme.systemUsesLightTheme = systemUsesLightTheme;
			_theme.appsUseLightTheme = appsUseLightTheme;
			_theme.isHighContrast = isHighContrast;
			

			auto is_sys_dark = Selected.Window.isTaskbar() ? !systemUsesLightTheme : !appsUseLightTheme;// Theme::IsDarkMode(Selected.Window.isTaskbar());

			_theme.mode = is_sys_dark;

			auto is_dark = is_sys_dark;
			auto th = &sets->theme;

			if(th->dark)
			{
				obj = _context.Eval(th->dark).move();
				if(obj.not_default())
					is_dark = obj.to_bool();
				
				_theme.mode = is_dark;
			}

			if(th->name)
			{
				obj = _context.Eval(th->name).move();
				if(obj.is_number())
				{
					auto value = obj.to_number<ThemeType>();
					if(value >= ThemeType::Auto && value <= ThemeType::Custom)
					{
						_theme.Type = value;
					}
				}
				else if(obj.is_string())
				{
					Hash value = obj.to_string().trim().hash();
					if(value == IDENT_THEME_MODERN)
						_theme.Type = ThemeType::Modern;
					else if(value == IDENT_THEME_WHITE)
						_theme.Type = ThemeType::White;
					else if(value == IDENT_THEME_BLACK)
						_theme.Type = ThemeType::Black;
					else if(value == IDENT_THEME_CLASSIC)
						_theme.Type = ThemeType::Classic;
					else if(value == IDENT_THEME_SYSTEM)
						_theme.Type = ThemeType::System;
					else if(value == IDENT_THEME_AUTO)
						_theme.Type = ThemeType::Auto;
					else if(value == IDENT_THEME_HIGHCONTRAST)
						_theme.Type = ThemeType::HighContrast;
				}
			}

			_theme.enableTransparency = enableTransparency;
			_theme.systemUsesLightTheme = systemUsesLightTheme;
			_theme.appsUseLightTheme = appsUseLightTheme;
			_theme.isHighContrast = isHighContrast;

			_theme.system.transparency = enableTransparency;
			_theme.system.mode = systemUsesLightTheme ? 0 : 1;

			struct {
				int8_t effect = 0;
				Color tintcolor;
			} transparency;

			if(enableTransparency)
			{
				transparency.effect = 3;
			}

			if(_context.Eval(th->background.effect, obj))
			{
				//effect = none = 0, transparent = 1, blur = 2, acrylic = 3, mica = 4, tabbed = 5, add - to force
				auto ef = [&](Object &o)->bool
				{
					if(o.is_default())
					{
						transparency.effect = -1;
						return true;
					}
					else if(o.is_number())
					{
						transparency.effect = o;
						if(transparency.effect > 5)
							transparency.effect = 0;
						return true;
					}
					else if(o.is_string())
					{
						auto effect = o.Value.String.trim().hash();
						if(effect == IDENT_EFFECT_TRANSPARENT)
							transparency.effect = 1;
						else if(effect == IDENT_EFFECT_BLUR)
							transparency.effect = 2;
						else if(effect == IDENT_EFFECT_ACRYLIC)
							transparency.effect = 3;
						else if(effect == IDENT_NONE)
							transparency.effect = 0;
						else if(effect == IDENT_AUTO)
							transparency.effect = -1;
						return true;
					}

					return false;
				};

				if(!ef(obj) && obj.is_array(true))
				{
					auto ptr = obj.get_pointer();
					int ac = ptr[0];

					if(ef(ptr[1]))
					{
						if(ac >= 2)
							_context.to_color(ptr[2], &transparency.tintcolor);
						if(ac == 3 && ptr[3].not_default())
							transparency.tintcolor.opacity(ptr[3]);
					}
				}

				if(transparency.effect == -1)
				{
					enableTransparency = true;
					transparency.effect = 2;
					if(ver->IsWindows11OrGreater())
						transparency.effect = 3;
				}
			}

			if(!composition || isHighContrast)
			{
				transparency.effect = 0;
			}

			if(transparency.effect == 0)
				enableTransparency = false;
			else if(transparency.effect >= 2)
			{
				if(enableTransparency)
					_context.eval_color(th->background.tintcolor, &transparency.tintcolor);
				else
					transparency.effect = 0;
			}

			switch(_theme.Type)
			{
				case ThemeType::HighContrast:
					_theme.background.effect = 0;
					_theme = Theme::HighContrast();
					break;
				case ThemeType::White:
					_theme = Theme::White(transparency.effect);
					break;
				case ThemeType::Black:
					_theme = Theme::Black(transparency.effect);
					break;
				case ThemeType::Modern:
					_theme = is_dark ? Theme::Modern(ThemeType::Dark, 1, transparency.effect) : Theme::Modern(ThemeType::Light, 0, transparency.effect);
					
					if(enableTransparency and transparency.effect == 3)
					{
						if(!is_dark)
							_theme.background.tintcolor =  0xFFFFFF;
					}

					_settings.modify_items.position = 2;
					_settings.modify_items.image = 2;
					break;
				case ThemeType::Classic:
					_theme = is_dark ? Theme::Dark(false, enableTransparency) : Theme::Light(false, enableTransparency);
					break;
				case ThemeType::Auto:
				case ThemeType::System:
				default:
				{
					if(isHighContrast)
						_theme = Theme::HighContrast();
					else
					{
						_theme = is_dark ? Theme::Dark(false, enableTransparency) : Theme::Light(false, enableTransparency);
						//10240, 10586, 14393, 10593, 16299, 17134
						//17666 , 19042
						//ImmersiveStartDark::Menu;ImmersiveStart::Menu
						_hTheme = _theme.OpenThemeData(hwnd.active, is_dark ? L"DarkMode::Menu" : L"Menu", dpi.val);

						if(_hTheme && ver->IsWindows11OrGreater())
						{
							Color nor, sel, dis, dis_sel;

							// Special handling for Windows 11 Canary and Dev builds
							bool isCanaryOrDev = ver->IsWindows11CanaryOrDev();

							auto get_bk_clr = [&](Color &clr, int iPartId, int iStateId, int x = -1, int y = -1, int size = 9)->bool
							{
								DC dc = hwnd.owner;
								RECT rc = { 0, 0, size, size };
								auto res = ::DrawThemeBackground(_hTheme, dc, iPartId, iStateId, &rc, nullptr);
								if(SUCCEEDED(res))
								{
									x = x < 0 ? size / 2 : x;
									y = y < 0 ? size / 2 : y;
									clr.from(dc.get_pixel(x, y), 100);
									return true;
								}
								return false;
							};

							auto get_clr = [&](Color &clr, int iPartId, int iStateId, int iPropId)->bool
							{
								COLORREF ret = CLR_INVALID;
								if(SUCCEEDED(::GetThemeColor(_hTheme, iPartId, iStateId, iPropId, &ret)))
								{
									clr.from(ret, 100);
									return true;
								}
								return false;
							};

							// Use more reliable theme color detection for Canary builds
							if (isCanaryOrDev)
							{
								// Get text colors with fallbacks to system colors
								if (!get_clr(nor, MENU_POPUPITEM, MPI_NORMAL, TMT_TEXTCOLOR))
								{
									nor.from(::GetSysColor(COLOR_MENUTEXT), 100);
								}
								
								if (!get_clr(sel, MENU_POPUPITEM, MPI_HOT, TMT_TEXTCOLOR))
								{
									sel.from(::GetSysColor(COLOR_HIGHLIGHTTEXT), 100);
								}
								
								if (!get_clr(dis, MENU_POPUPITEM, MPI_DISABLED, TMT_TEXTCOLOR))
								{
									dis.from(::GetSysColor(COLOR_GRAYTEXT), 100);
								}
								
								if (!get_clr(dis_sel, MENU_POPUPITEM, MPI_DISABLEDHOT, TMT_TEXTCOLOR))
								{
									dis_sel.from(::GetSysColor(COLOR_GRAYTEXT), 100);
								}
							}
							else
							{
								// Standard theme color detection for non-Canary builds
								get_clr(nor, MENU_POPUPITEM, MPI_NORMAL, TMT_TEXTCOLOR);
								get_clr(sel, MENU_POPUPITEM, MPI_HOT, TMT_TEXTCOLOR);
								get_clr(dis, MENU_POPUPITEM, MPI_DISABLED, TMT_TEXTCOLOR);
								get_clr(dis_sel, MENU_POPUPITEM, MPI_DISABLEDHOT, TMT_TEXTCOLOR);
							}

							_theme.text.color = { nor, sel, dis, dis_sel };
							_theme.symbols.checked = { nor, sel, dis, dis_sel };
							_theme.symbols.bullet = { nor, sel, dis, dis_sel };
							_theme.symbols.chevron = { nor, sel, dis, dis_sel };

							// More reliable background color detection for Canary builds
							if (isCanaryOrDev)
							{
								if (!get_clr(_theme.background.color, MENU_POPUPBACKGROUND, MPI_NORMAL, TMT_FILLCOLOR))
								{
									if (!get_bk_clr(_theme.background.color, MENU_POPUPITEM, MPI_NORMAL))
									{
										_theme.background.color.from(::GetSysColor(COLOR_MENU), 100);
									}
								}
								
								if (!get_bk_clr(_theme.back.color.sel, MENU_POPUPITEM, MPI_HOT))
								{
									_theme.back.color.sel.from(::GetSysColor(COLOR_HIGHLIGHT), 100);
								}
								
								if (!get_bk_clr(_theme.back.color.nor_dis, MENU_POPUPITEM, MPI_DISABLED))
								{
									_theme.back.color.nor_dis.from(::GetSysColor(COLOR_MENU), 100);
								}
								
								if (!get_bk_clr(_theme.back.color.sel_dis, MENU_POPUPITEM, MPI_DISABLEDHOT))
								{
									_theme.back.color.sel_dis.from(::GetSysColor(COLOR_BTNFACE), 100);
								}
								
								if (!get_bk_clr(_theme.separator.color, MENU_POPUPSEPARATOR, 0, -1, -1, 3))
								{
									_theme.separator.color.from(::GetSysColor(COLOR_GRAYTEXT), 100);
								}
							}
							else
							{
								// Standard background color detection for non-Canary builds
								if(!get_clr(_theme.background.color, MENU_POPUPBACKGROUND, MPI_NORMAL, TMT_FILLCOLOR))
									get_bk_clr(_theme.background.color, MENU_POPUPITEM, MPI_NORMAL);

								get_bk_clr(_theme.back.color.sel, MENU_POPUPITEM, MPI_HOT);
								get_bk_clr(_theme.back.color.nor_dis, MENU_POPUPITEM, MPI_DISABLED);
								get_bk_clr(_theme.back.color.sel_dis, MENU_POPUPITEM, MPI_DISABLEDHOT);
								get_bk_clr(_theme.separator.color, MENU_POPUPSEPARATOR, 0, -1, -1, 3);
							}

							_theme.border.color = _theme.separator.color;// getbkclr(MENU_POPUPBORDERS, 0, 0, 4, 9);
							if(enableTransparency)
							{
								_theme.back.color.sel.opacity(50);
								_theme.back.color.sel_dis.opacity(50);
								_theme.back.color.nor_dis.opacity(25);
							}
							else
							{
								_theme.back.color.nor = _theme.background.color;
							}
							::GetThemeInt(_hTheme, MENU_POPUPSEPARATOR, 0, TMT_BORDERSIZE, (int *)&_theme.separator.size);
						}
					}
					break;
				}
			}

			_theme.system.transparency = enableTransparency;
			_theme.system.mode = systemUsesLightTheme ? 0 : 1;
			_theme.isHighContrast = isHighContrast;
			_theme.appsUseLightTheme = appsUseLightTheme;
			_theme.mode = is_dark;

			_theme.dpi = &dpi;
			_context.theme = &_theme;

			auto set_color = [=](Object &obj, Color *property)
			{
				_context.to_color(obj, property);
			};

			auto eval_color = [=](auto_expr *ep, Color *property)
			{
				if(ep) _context.eval_color(*ep, property);
			};

			auto eval_state = [=](Settings::COLOR *e, Theme::state_t *s)
			{
				_context.eval_color(e->value, &s->sel);
				if(e->value)
				{
					s->nor = s->sel;
					s->nor_dis = s->sel;
					s->sel_dis = s->sel;
				}
				_context.eval_color(e->normal, &s->nor);
				_context.eval_color(e->select, &s->sel);
				_context.eval_color(e->normal_disabled, &s->nor_dis);
				_context.eval_color(e->select_disabled, &s->sel_dis);
			};

			auto eval_color_array = [=](auto_expr *e, Color *clr1, Color *clr2)->void
			{
				Object obj;
				if(_context.Eval(*e, obj))
				{
					if(obj.is_array() && obj.Value.Pointer)
					{
						auto ptr = obj.get_pointer();
						set_color(ptr[1], clr1);
						set_color(ptr[2], clr2);
					}
					else if(obj.not_default())
					{
						set_color(obj, clr1);
					}
				}
			};

			auto eval_margin = [=](Settings::MARGIN *e, Margin *m)
			{
				Object obj;
				if(_context.eval_number(e->left, obj))
					m->left = obj;

				if(_context.eval_number(e->top, obj))
					m->top = obj;

				if(_context.eval_number(e->right, obj))
					m->right = obj;

				if(_context.eval_number(e->bottom, obj))
					m->bottom = obj;
			};

			auto get_rect = [](Object *o, Margin *m)
			{
				if(o)
				{
					if(o->is_array() && o->Value.Pointer)
					{
						auto ptr = o->get_pointer();
						int argc = ptr[0];
						if(argc == 2)
						{
							if(ptr[1].not_default())
							{
								m->left = ptr[1];
								m->right = m->left;
							}

							if(ptr[2].not_default())
							{
								m->top = ptr[2];
								m->bottom = m->top;
							}
						}
						else
						{
							if(ptr[1].not_default())
								m->left = ptr[1];
							if(ptr[2].not_default())
								m->top = ptr[2];
							if(ptr[3].not_default())
								m->right = ptr[3];
							if(ptr[4].not_default())
								m->bottom = ptr[4];
						}
					}
					else if(o->not_default())
					{
						long value = *o;
						if(value >= 0 && value <= 400)
							*m = { value, value, value, value };
					}
				}
			};

			if(!_hTheme)
				_hTheme = _theme.OpenThemeData(nullptr, L"Menu", dpi.val);

			if(_context.eval_number(sets->theme.image.enabled, obj))
			{
				_theme.image.enabled = obj.to_bool();
				
				if(!_theme.image.enabled)
				{
					_settings.modify_items.image = 0;
					_settings.new_items.image = false;
				}
			}

			// settings
			// static items
			if(_context.eval_number(sets->modify_items.enabled, obj))
				_settings.modify_items.enabled = obj.to_bool();

			if(_settings.modify_items.enabled)
			{
				if(_context.eval_number(sets->modify_items.title, obj))
					_settings.modify_items.title = obj.to_bool();

				if(_context.eval_number(sets->modify_items.visibility, obj))
					_settings.modify_items.visibility = obj.to_bool();

				if(_context.eval_number(sets->modify_items.parent, obj))
					_settings.modify_items.parent = obj.to_bool();

				if(_context.eval_number(sets->modify_items.separator, obj))
					_settings.modify_items.separator = obj.to_bool();

				if(_context.eval_number(sets->modify_items.keys, obj))
					_settings.modify_items.keys = obj.to_bool();

				if(_context.eval_number(sets->modify_items.position, obj))
					_settings.modify_items.position = obj.to_number<int>();

				if(_context.eval_number(sets->modify_items.remove.duplicate, obj))
					_settings.modify_items.remove.duplicate = obj.to_number<bool>();

				if(_context.eval_number(sets->modify_items.remove.disabled, obj))
					_settings.modify_items.remove.disabled = obj.to_number<bool>();

				if(_context.eval_number(sets->modify_items.remove.separator, obj))
					_settings.modify_items.remove.separator = obj.to_number<bool>();

				if(_context.eval_number(sets->modify_items.auto_image_group, obj) && obj.to_bool())
				{
					_settings.modify_items.position = 2;
					_settings.modify_items.image = 2;
				}

				if(_theme.image.enabled)
				{
					if(_context.eval_number(sets->modify_items.image, obj))
						_settings.modify_items.image = obj.to_number<int>();
				}
				else 
				{
					_settings.modify_items.image = 0;
				}
			}
			else 
			{
				_settings.modify_items.image = 0;
				_settings.modify_items.position = 0;
				_settings.modify_items.title = false;
				_settings.modify_items.visibility = false;
				_settings.modify_items.parent = false;
				_settings.modify_items.separator = false;
				_settings.modify_items.keys = false;
				_settings.modify_items.remove.duplicate = false;
				_settings.modify_items.remove.disabled = false;
				_settings.modify_items.remove.separator = false;
			}
			
			// new items
			if(_context.eval_number(sets->new_items.enabled, obj))
				_settings.new_items.enabled = obj.to_bool();

			if(_settings.new_items.enabled)
			{
				if(_theme.image.enabled)
				{
					if(_context.eval_number(sets->new_items.image, obj))
						_settings.new_items.image = obj.to_bool();
				}

				if(_context.eval_number(sets->new_items.keys, obj))
					_settings.new_items.keys = obj.to_bool();
			}

			// layout
			if(_context.Eval(th->layout.width, obj))
			{
				if(obj.not_default())
					_theme.layout.min_width = obj;
				else if(obj.is_array() && obj.Value.Pointer)
				{
					auto ptr = obj.get_pointer();
					int argc = ptr[0];
					if(argc >= 1)
					{
						if(ptr[1].not_default())
							_theme.layout.min_width = ptr[1];

						if(argc >= 2 && ptr[2].not_default())
							_theme.layout.max_width = ptr[2];
					}
				}

				if(_theme.layout.min_width < 0)
					_theme.layout.min_width = 0;
				if(_theme.layout.max_width < 0)
					_theme.layout.max_width = 0;
			}

			if(_context.eval_number(th->layout.rtl, obj))
			{
				_theme.layout.rtl = obj;
				is_layoutRTL = _theme.layout.rtl;
			}

			if(_context.eval_number(th->layout.popup.align, obj))
			{
				_theme.layout.popup.align = obj;
				if(_theme.layout.popup.align > 20)
					_theme.layout.popup.align = 20;
				else if(_theme.layout.popup.align < -20)
					_theme.layout.popup.align = -20;
			}

			// theme.background
			if(!_context.eval_color(th->background.color, &_theme.background.color))
			{
				if(enableTransparency)
					_theme.background.color.a = 0;
			}

			if(_context.eval_number(th->background.opacity, obj))
				_theme.background.color.a = _theme.opacity(obj);
			else if(enableTransparency)
				_theme.background.color.a = 0;

			if(_context.Eval(th->background.image, obj) )
			{
				if(!obj.is_default())
					_theme.background.image = obj.to_string().trim().move();
			}

			_theme.background.effect = transparency.effect;

			if(transparency.tintcolor)
				_theme.background.tintcolor = transparency.tintcolor;

			
			if(transparency.effect == 0)
				_theme.background.color.a = 0xFF;
			else
			{
				//_theme.back.color.nor.a = 0;
				//_theme.back.color.dis.a = 0;
			}

			_theme.background.opacity = _theme.background.color.a;

			//_theme.view = _theme.block.height();
			// theme.view
			if(_context.eval_number(th->view, obj))
			{
				auto val = VIEWMODE::get(obj);
				if(val >= 0)
				{
					_theme.back.padding.top = val;
					_theme.back.padding.bottom = val;
				}
			}

			_theme.back.color.nor.from(_theme.background.color);

			/*if(_theme.background.opacity < 0xFF)
			{
				auto op = _theme.background.opacity;
				_theme.back.color.nor.a = 0;
				_theme.back.color.dis.a = 0;
				_theme.back.color.sel.a = op;
				_theme.back.color.dis_sel.a = op / 2;
				_theme.back.opacity = op;
			}*/

			// theme.gradient
			if(_context.eval_number(th->gradient.enabled, obj) && obj != 0)
			{
				if(_context.Eval(th->gradient.linear, obj))
				{
					if(obj.is_array() && obj.Value.Pointer)
					{
						auto ptr = obj.get_pointer();
						_theme.gradient.linear[1] = ptr[1];
						_theme.gradient.linear[2] = ptr[2];
						_theme.gradient.linear[3] = ptr[3];
						_theme.gradient.linear[4] = ptr[4];

						if(_theme.gradient.linear[1] > 0.0 ||
						   _theme.gradient.linear[2] > 0.0 ||
						   _theme.gradient.linear[3] > 0.0 ||
						   _theme.gradient.linear[4] > 0.0)
						{
							_theme.gradient.linear[0] = 1.0;
							_theme.gradient.enabled = true;
						}
					}
				}
				else if(_context.Eval(th->gradient.radial, obj))
				{
					if(obj.is_array() && obj.Value.Pointer)
					{
						auto ptr = obj.get_pointer();
						uint32_t c = ptr[0];
						_theme.gradient.radial[1] = (c >= 1) ? (double)ptr[1] : 100;
						_theme.gradient.radial[2] = (c >= 2) ? (double)ptr[2] : 100;
						_theme.gradient.radial[3] = (c >= 3) ? (double)ptr[3] : 50;
						_theme.gradient.radial[4] = (c >= 4) ? (double)ptr[4] : _theme.gradient.radial[1];
						_theme.gradient.radial[5] = (c >= 5) ? (double)ptr[5] : _theme.gradient.radial[2];
					}
					else if(obj.not_default()) {
						_theme.gradient.radial[1] = 100;
						_theme.gradient.radial[2] = 100;
						_theme.gradient.radial[3] = obj;
						_theme.gradient.radial[4] = 100;
						_theme.gradient.radial[5] = 100;
						_theme.gradient.radial[0] = 1.0;
					}

					if(_theme.gradient.radial[1] > 0.0 ||
					   _theme.gradient.radial[2] > 0.0 ||
					   _theme.gradient.radial[3] > 0.0 ||
					   _theme.gradient.radial[4] > 0.0 ||
					   _theme.gradient.radial[5] > 0.0)
					{
						_theme.gradient.radial[0] = 1.0;
						_theme.gradient.enabled = true;
					}
				}

				if(_theme.gradient.enabled)
				{
					if(_context.Eval(th->gradient.stop, obj))
					{
						if(obj.is_array() && obj.Value.Pointer)
						{
							auto stop = [=](Object *obj, Theme::gradientstop_t *gs)->bool
							{
								if(obj->is_array() && obj->Value.Pointer)
								{
									auto ptr = obj->get_pointer();
									uint32_t ac = ptr[0];
									if(ac >= 1 && ac <= 3)
									{
										gs->offset = ptr[1];
										if(ac >= 2)
											_context.to_color(ptr[2], &gs->color);

										if(ac == 3 && ptr[3].not_default())
											gs->color.opacity(ptr[3]);

										return true;
									}
								}
								return false;
							};

							auto ptr = obj.get_pointer();
							uint32_t c = ptr[0];
							if(c > 0)
							{
								int ret = 0;
								for(auto i = 1u; i <= c; i++)
								{
									Theme::gradientstop_t gs;
									gs.color.opacity(100);
									if(stop(&ptr[i], &gs))
									{
										_theme.gradient.stpos.push_back(gs);
										ret++;
									}
								}
								_theme.gradient.enabled = ret > 0;
							}
						}
					}
				}
			}

			// theme.border
			if(_context.eval_number(th->border.size, obj))
			{
				uint8_t value = obj;
				_theme.border.size = value > 10 ? 10 : value;
			}

			if(_theme.border.size > 0)
			{
				if(_context.eval_number(th->border.enabled, obj) && !obj.to_bool())
					_theme.border.size = 0;

				if(_theme.border.size > 0)
				{
					_context.eval_color(th->border.color, &_theme.border.color);

					if(_context.eval_number(th->border.opacity, obj))
						_theme.border.color.a = _theme.opacity(obj);

					if(_theme.border.color.a == 0)
						_theme.border.size = 0;
				}
			}

			if(_context.eval_number(th->border.radius, obj))
				_theme.border.radius = _theme.radius(obj);

			if(_context.Eval(th->border.padding.value, obj))
				get_rect(&obj, &_theme.border.padding);

			eval_margin(&th->border.padding, &_theme.border.padding);

			// theme.shadow
			if(_context.eval_number(th->shadow.size, obj))
			{
				_theme.shadow.size = obj;
				if(_theme.shadow.size > 30)
					_theme.shadow.size = 30;
			}

			if(_theme.shadow.size > 0)
			{
				if(_context.eval_number(th->shadow.enabled, obj) && !obj.to_bool())
					_theme.shadow.size = 0;

				if(_theme.shadow.size > 0)
				{
					_context.eval_color(th->shadow.color, &_theme.shadow.color);

					if(_context.eval_number(th->shadow.opacity, obj))
						_theme.shadow.color.a = _theme.opacity(obj);

					if(_theme.shadow.color.a == 0)
						_theme.shadow.size = 0;
					else if(_context.eval_number(th->shadow.offset, obj))
					{
						uint8_t val = obj;
						_theme.shadow.offset = val > 30 ? 30 : val;
					}
				}
			}

			if(_theme.shadow.size == 0 || _theme.shadow.color.a == 0)
				_theme.shadow.enabled = false;

			// theme.item.text
			eval_state(&th->item.text.color, &_theme.text.color);

			// theme.item.back
			eval_state(&th->item.back, &_theme.back.color);

			if(_context.eval_number(th->item.opacity, obj))
				_theme.back.opacity = _theme.opacity(obj);

			// theme.item.boorder
			eval_state(&th->item.border, &_theme.back.border);

			if(_context.eval_number(th->item.radius, obj))
				_theme.back.radius = _theme.radius(obj);

			// theme.item.padding
			if(_context.Eval(th->item.padding.value, obj))
				get_rect(&obj, &_theme.back.padding);

			eval_margin(&th->item.padding, &_theme.back.padding);

			// theme.item.margin
			if(_context.Eval(th->item.margin.value, obj))
				get_rect(&obj, &_theme.back.margin);

			eval_margin(&th->item.margin, &_theme.back.margin);

			// theme.item.prefix		mnemonic-prefix
			if(_context.eval_number(th->item.prefix, obj) && obj.not_default())
			{
				int8_t val = obj;
				if(val == 1)
					_theme.text.prefix = 0;
				else if(val == 2)
					_theme.text.prefix = DT_NOPREFIX;
				else
					_theme.text.prefix = DT_HIDEPREFIX;
			}

			if(_theme.text.prefix == 0xFFFFFFFF)
				_theme.text.prefix = keyboard.key_shift() ? 0 : DT_HIDEPREFIX;

			// theme.image
			if(_theme.image.enabled)
			{
				if(_context.eval_number(th->image.gap, obj))
					_theme.image.gap = obj;

				if(_context.eval_number(th->image.scale, obj))
					_theme.image.scale = obj;
			}

			// theme.separator
			if(_context.eval_number(th->separator.size, obj))
			{
				uint8_t value = obj;
				_theme.separator.size = value > 40 ? 40 : value;
			}

			if(!_context.eval_color(th->separator.color, &_theme.separator.color))
			{
				if(_theme.background.opacity < 0xff)
				{
					if(_theme.separator.color.a == 0xff)
						_theme.separator.color.a = _theme.background.opacity + 5;
				}
			}

			if(_context.eval_number(th->separator.opacity, obj))
				_theme.separator.color.a = Color::ToByte(obj);

			if(_context.Eval(th->separator.margin.value, obj))
				get_rect(&obj, &_theme.separator.margin);

			eval_margin(&th->separator.margin, &_theme.separator.margin);

			_theme.set_symbols_as_text(_theme.mode == 1);

			// theme.symbol
			auto ev_sb = [&](auto_expr *expr, bool normal)
			{
				Object obj;
				if(_context.Eval(*expr, obj))
				{
					Color v;

					if(obj.is_array() && obj.Value.Pointer)
					{
						auto ptr = obj.get_pointer();
						if(ptr[1].not_default())
						{
							v = ptr[1].to_color();
							if(normal)
							{
								_theme.symbols.chevron.nor = v;
								_theme.symbols.checked.nor = v;
								_theme.symbols.bullet.nor = v;
							}
							else
							{
								_theme.symbols.chevron.sel = v;
								_theme.symbols.checked.sel = v;
								_theme.symbols.bullet.sel = v;
							}
						}

						if(ptr[2].not_default())
						{
							v = ptr[2].to_color();
							if(normal)
							{
								_theme.symbols.chevron.nor_dis = v;
								_theme.symbols.checked.nor_dis = v;
								_theme.symbols.bullet.nor_dis = v;
							}
							else
							{
								_theme.symbols.chevron.sel_dis = v;
								_theme.symbols.checked.sel_dis = v;
								_theme.symbols.bullet.sel_dis = v;
							}
						}
					}
					else if(obj.not_default())
					{
						v = obj.to_color();
						if(normal)
						{
							_theme.symbols.chevron.nor = v;
							_theme.symbols.checked.nor = v;
							_theme.symbols.bullet.nor = v;
						}
						else
						{
							_theme.symbols.chevron.sel = v;
							_theme.symbols.checked.sel = v;
							_theme.symbols.bullet.sel = v;
						}
					}
				}
			};

			ev_sb(&th->symbol.color.value, true);
			ev_sb(&th->symbol.color.normal, true);
			ev_sb(&th->symbol.color.select, false);

			auto ev_sb_dis = [&](auto_expr *expr, bool normal)
			{
				Object obj;
				if(_context.Eval(*expr, obj))
				{
					Color v;

					if(obj.is_array() && obj.Value.Pointer)
					{
						auto ptr = obj.get_pointer();
						if(ptr[1].not_default())
						{
							v = ptr[1].to_color();
							if(normal)
							{
								_theme.symbols.chevron.nor_dis = v;
								_theme.symbols.checked.nor_dis = v;
								_theme.symbols.bullet.nor_dis = v;
							}
							else
							{
								_theme.symbols.chevron.sel_dis = v;
								_theme.symbols.checked.sel_dis = v;
								_theme.symbols.bullet.sel_dis = v;
							}
						}
					}
					else if(obj.not_default())
					{
						v = obj.to_color();
						if(normal)
						{
							_theme.symbols.chevron.nor_dis = v;
							_theme.symbols.checked.nor_dis = v;
							_theme.symbols.bullet.nor_dis = v;
						}
						else
						{
							_theme.symbols.chevron.sel_dis = v;
							_theme.symbols.checked.sel_dis = v;
							_theme.symbols.bullet.sel_dis = v;
						}
					}
				}
			};

			ev_sb_dis(&th->symbol.color.normal_disabled, true);
			ev_sb_dis(&th->symbol.color.select_disabled, false);

			eval_state(&th->symbol.chevron, &_theme.symbols.chevron);
			eval_state(&th->symbol.bullet, &_theme.symbols.chevron);
			eval_state(&th->symbol.checkmark, &_theme.symbols.chevron);

			if(_context.eval_number(th->image.display, obj))
			{
				int v = obj;
				if(v < 0) v = 0;
				else if(v > 2) v = 2;
				_theme.image.display = v;
			}

			_tip.enabled = false;

			// tip
			if(_context.eval_number(sets->tip.enabled, obj))
				_tip.enabled = obj.to_bool();

			if(_tip.enabled)
			{
				eval_color_array(&sets->tip.normal, &_theme.tip.normal.back, &_theme.tip.normal.text);
				eval_color_array(&sets->tip.primary, &_theme.tip.primary.back, &_theme.tip.primary.text);
				eval_color_array(&sets->tip.info, &_theme.tip.info.back, &_theme.tip.info.text);
				eval_color_array(&sets->tip.success, &_theme.tip.success.back, &_theme.tip.success.text);
				eval_color_array(&sets->tip.warning, &_theme.tip.warning.back, &_theme.tip.warning.text);
				eval_color_array(&sets->tip.danger, &_theme.tip.danger.back, &_theme.tip.danger.text);

				_context.eval_color(sets->tip.border, &_theme.tip.border);

				if(_context.eval_number(sets->tip.width, obj))
				{
					uint16_t value = obj;
					if(value >= 200 && value <= 2000)
						_theme.tip.maxwidth = value;
				}

				//color.rgba(default, 255)
				if(_context.eval_number(sets->tip.opacity, obj))
					_theme.tip.opacity = Color::ToByte(obj);

				if(_context.eval_number(sets->tip.radius, obj))
					_theme.tip.radius = _theme.radius(obj);

				if(_context.eval_number(sets->tip.time, obj))
				{
					double value = obj;
					if(value >= 0.00 && value <= 10.00)
						_theme.tip.time = uint16_t(value * 1000);
				}

				if(_context.Eval(sets->tip.padding.value, obj))
					get_rect(&obj, &_theme.tip.padding);

				eval_margin(&sets->tip.padding, &_theme.tip.padding);
			}

			//New feature "showdelay" to change the menu show delay time and it is applied immediately without saving the value in the registry.
			//Gets or sets the time, in milliseconds, that the system waits before displaying a shortcut menu when the mouse cursor is over a submenu item.
			//New-Item -Path "HKCU:\Software\Control Panel\Desktop" -Name MenuShowDelay -Force -Value 200
			if(_context.eval_number(sets->showdelay, obj))
			{
				::SystemParametersInfoW(SPI_GETMENUSHOWDELAY, 0, &_showdelay[0], 0);
				_showdelay[1] = obj;

				if(_showdelay[1] > 4000)
					_showdelay[1] = 4000;

				if(_showdelay[1] != _showdelay[0])
				{
					::SystemParametersInfoW(SPI_SETMENUSHOWDELAY, _showdelay[1], nullptr, SPIF_SENDCHANGE);
				}
			}

			_theme.text.tap = dpi.value<int8_t>(_theme.text.tap);

			long font_size = -1;

			struct {
				Object name, size, weight, italic;
			}__font;

			// font
			if(th->font.value)
			{
				obj = _context.Eval(th->font.value).move();
				if(obj.is_array(true))
				{
					obj.get(1, __font.size);
					obj.get(2, __font.name);
					obj.get(3, __font.weight);
					obj.get(4, __font.italic);
				}
				else if(obj.is_number())
					__font.size = obj.move();
				else if(obj.is_string())
					__font.name = obj.move();
			}

			if(_context.Eval(th->font.name, obj) && !obj.is_empty())
				__font.name = obj.move();
			
			_context.eval_number(th->font.size, __font.size);

			_context.eval_number(th->font.weight, __font.weight);
			_context.eval_number(th->font.italic, __font.italic);

			_theme.font = {};

			NONCLIENTMETRICSW ncm = { sizeof(ncm) };

			_theme.font.lfQuality = DEFAULT_QUALITY;

			if(_theme.SystemParameters(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi.val))
			{
				std::memcpy(&_theme.font, &ncm.lfMenuFont, sizeof(ncm.lfMenuFont));
			}
			else if(S_OK != ::GetThemeSysFont(_hTheme, TMT_MENUFONT, &_theme.font))
			{
				_theme.font.lfHeight = 12;dpi.valuexx<long>(12);
				_theme.font.lfQuality = DEFAULT_QUALITY;
				string::Copy(_theme.font.lfFaceName, L"Segoe UI");
			}

			if(ver->IsWindows11OrGreater()) 
			{
				DWORD dwTextScaleFactor = 100, cbData;
				::RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Accessibility", 
							   L"TextScaleFactor", RRF_RT_DWORD, nullptr, &dwTextScaleFactor, &cbData);
				long scale = ((dpi.val + 24) * dwTextScaleFactor) / 100;
				_theme.font.lfHeight = 12 * scale / 100;
			}
					
			if(__font.name.is_string())
			{
				string value = __font.name.to_string().trim().move();
				if(!value.empty())
				{
					if(Font::Installed(value))
					{
						::memset(_theme.font.lfFaceName, 0, sizeof(_theme.font.lfFaceName));
						//::StringCchCopyW(_theme.font.lfFaceName, value.length(), value.c_str());
						string::Copy(_theme.font.lfFaceName, value.c_str(), value.length());
						FontNotFound = false;
					}
					else if(!FontNotFound)
					{
						_log.warning(L"Font name \"%s\" not found.", value.c_str());
						FontNotFound = true;
					}
				}
				_context.font.text = _theme.font.lfFaceName;
			}
			
			if(__font.size.not_default())
			{
				long value = __font.size;
				if(value < 6 )
					font_size = 6;
				else if(value > 200)
					font_size = 200;
				else
					font_size = value;
			}

			if(__font.weight.not_default())
			{
				uint32_t weight = __font.weight;
				if(weight <= 9 && weight >= 1)
					_theme.font.lfWeight = weight * 100;
			}

			if(__font.italic.not_default())
				_theme.font.lfItalic = __font.italic.to_bool();

			if(font_size >= 6)
			{
				//font_size = dpi(font_size);
				if(font_size != _theme.font.lfHeight)
				{
					auto of = std::abs(_theme.font.lfHeight);
					//auto di = font_size - of;
					_theme.font.lfHeight = font_size;
					dpi.val = (font_size * dpi.val) / of;
					//MBF(L"%d, %d, %d", dpi.val, (font_size * dpi.val) / of, di);
				}
			}
			
			if(_theme.image.enabled) 
			{
				if(th->image.color)
				{
					obj = _context.Eval(th->image.color).move();

					if(obj.is_color())
						set_color(obj, &_theme.image.color[0]);
					else if(obj.is_array() && obj.Value.Pointer)
					{
						auto ptr = obj.get_pointer();
						int argc = ptr[0];
						if(argc >= 1)
						{
							set_color(ptr[1], &_theme.image.color[0]);
							if(argc >= 2)
								set_color(ptr[2], &_theme.image.color[1]);
							if(argc >= 3)
								set_color(ptr[3], &_theme.image.color[2]);
						}
					}
				}

				if(th->image.glyph)
				{
					obj = _context.Eval(th->image.glyph).move();
					if(obj.is_string())
					{
						if(string glyph = obj.to_string().trim().move(); !glyph.empty())
							font.icon.Name = glyph.move();
					}
				}
			}

			//_theme.image.size = _theme.text.size;//16 //_theme.SystemMetrics<uint32_t>(SM_CXSMICON, 96/*dpi.val*/);
			
			_theme.scale();

			_theme.text.size = std::abs(_theme.font.lfHeight);
			_theme.image.size = _theme.text.size;


			if(font.icon.Name.empty())
			{
				font.icon.Name = FontCache::Default;
				font.icon.CharSet = SYMBOL_CHARSET;
			}

			_context.font.icon = font.icon.Name;

			font.icon.Size = _theme.image.size;
			font.icon.Quality = CLEARTYPE_QUALITY;
			font.icon.create();

			if(isHighContrast)
				_theme.mode = 2;

			if(_theme.font.lfHeight > 0)
				_theme.font.lfHeight *= -1;

			font.handle = ::CreateFontIndirectW(&_theme.font);

			font.icon10.Name = font.icon.Name;
			font.icon10.Size =int( _theme.image.size * 0.70);
			font.icon10.Quality = CLEARTYPE_QUALITY;
			font.icon10.create();

			//_log.info(L"%d %d %d", font.icon.Size, _theme.font.lfHeight, _theme.image.size);

			DC dc = hwnd.owner;
			dc.set_font(font.handle);
			TEXTMETRICW tm{};
			::GetTextMetricsW(dc, &tm);
			dc.reset_font();

			_theme.view2 = _theme.back.height() + tm.tmHeight;
			if(_theme.view2 % 2)
				_theme.view2++;

			// Erase background of entire client area.
			_hbackground = ::CreateSolidBrush(_theme.background.color.to_BGR());

			if(!composition)
				_theme.transparent = false;

			if(_theme.background.effect == 0)
			{
				if(!_theme.gradient.enabled)
					_theme.transparent = false;
			}
			
			std::string ll[]  
			{
				// Chevron Right
				"M7 16.82L6.17 16L12.17 10L6.17 3.99L7 3.17L13.82 10Z",
				// Chevron Left 
				"M12.99 16.82L13.82 16L7.82 10L13.82 3.99L12.99 3.17L6.17 10Z",
				// Checked Mark
				"M2.68 11.06C2.56 10.94 2.5 10.79 2.5 10.62C2.5 10.45 2.56 10.30 2.68 10.18C2.80 10.06 2.95 10 3.12 10C3.29 10 3.44 10.06 3.56 10.18L7.5 14.11L16.43 5.18C16.55 5.06 16.70 5 16.87 5C17.04 5 17.19 5.06 17.31 5.18C17.43 5.30 17.5 5.45 17.5 5.62C17.5 5.79 17.43 5.94 17.31 6.06L7.93 15.43C7.81 15.56 7.66 15.62 7.5 15.62C7.33 15.62 7.18 15.56 7.06 15.43Z",
				// Radio Bullet
				"M6.62 10L6.62 9.93C6.62 9.47 6.71 9.04 6.89 8.64C7.07 8.24 7.32 7.89 7.63 7.59C7.94 7.29 8.31 7.05 8.72 6.87C9.13 6.70 9.55 6.62 10 6.62C10.46 6.62 10.90 6.70 11.31 6.88C11.72 7.06 12.08 7.30 12.39 7.60C12.69 7.91 12.93 8.27 13.11 8.68C13.29 9.09 13.37 9.53 13.37 10C13.37 10.46 13.29 10.90 13.11 11.31C12.93 11.72 12.69 12.08 12.39 12.39C12.08 12.69 11.72 12.93 11.31 13.11C10.90 13.29 10.46 13.37 10 13.37C9.53 13.37 9.09 13.29 8.68 13.11C8.27 12.93 7.91 12.69 7.60 12.39C7.30 12.08 7.06 11.72 6.88 11.31C6.70 10.90 6.62 10.46 6.62 10Z"
			};

			std::string svg_begin = "<svg viewBox='0 0 20 20'><path d='";

			auto esvg = [=](std::string *data, Theme::state_t const &st, symbole_tag &sy, bool normal_only = false)
			{
				char fmt[30]{};
				::StringCchPrintfA(fmt, 30, "' fill='#%0.6x'/></svg>", st.nor.to_RGB());
				std::string sr = svg_begin + *data + fmt;
				if(PlutoVG plutovg(sr.c_str(), (int)sr.length(), _theme.image.size, _theme.image.size, dpi.val); plutovg)
				{
					sy.normal= plutovg.tobitmap();
					if(!normal_only)
					{
						sy.select = plutovg.tobitmap(st.sel);
						sy.normal_disabled = plutovg.tobitmap(st.nor_dis);
						sy.select_disabled = plutovg.tobitmap(st.sel_dis);
					}
				}
			};

			esvg(&ll[2], _theme.symbols.checked, symbol.checked);
			esvg(&ll[3], _theme.symbols.bullet, symbol.bullet);

			esvg(&ll[is_layoutRTL ? 1 : 0], _theme.symbols.chevron, symbol.chevron);
			
			if(symbol.chevron.normal)
			{
				symbol.chevron.size.cx = _theme.image.size;
				symbol.chevron.size.cy = _theme.image.size;

				auto popup = [&](HBITMAP &hbmp)
				{
					BITMAP bmp{};
					if(Bitmap::Info(hbmp, bmp))
					{
						auto w = bmp.bmWidth;
						auto h = bmp.bmHeight;
						auto b = (uint8_t *)bmp.bmBits;

						int bottom = 0;
						int left = w; 
						int right = 0;
						int top = h;

						for(int y = 0; y < h; y++)
						{
							for(int x = 0; x < w; x++)
							{
								auto a = (b + (x * 4))[3];
								if(a > 0)
								{
									if(x < left) left = x;
									if(x >= right) right = x + 1;
									if(y < top) top = y;
									if(y >= bottom) bottom = y + 1;
								}
							}
							b += (w * 4);
						}
	
						if(left < right && top < bottom)
						{
							SIZE trim = { right - left, bottom - top };
							if(auto hbitmap = dc.CreateDIBSection(trim.cx, trim.cy); hbitmap)
							{
								DC dcmem(dc.CreateCompatibleDC(), 1);
								dcmem.select_bitmap(hbitmap);
								dcmem.bitblt({ 0, 0, trim.cx, trim.cy }, hbmp, left, top);
								dcmem.restore_bitmap();
								::DeleteObject(hbmp);
								hbmp = hbitmap;
								symbol.chevron.size = trim;
							}
						}
					}
				};

				popup(symbol.chevron.normal);
				popup(symbol.chevron.normal_disabled);
				popup(symbol.chevron.select);
				popup(symbol.chevron.select_disabled);
			}
		}

		bool ContextMenu::is_excluded() 
		{
			auto initializer = Initializer::instance;
			auto sets = &_cache->settings;

			if(_context.eval_bool(sets->exclude.value))
				return true;

			Object obj;

			auto is_exc = [=](const Object &obj, const string &name)
			{
				if(name.empty() || obj.is_null())
					return false;

				if(obj.is_array())
				{
					auto ptr = obj.ptr();
					int ac = ptr[0];
					for(auto i = 0; i < ac; i++)
					{
						if(name.equals(ptr[i + 1].to_string(), name))
							return true;
					}
				}
				else if(name.equals(obj.to_string()))
					return true;

				return false;
			};

			if(_context.Eval(sets->exclude.process, obj))
			{
				if(is_exc(obj, initializer->process.name))
					return true;
			}

			if(_context.Eval(sets->exclude.window, obj))
			{
				auto hWnd = hwnd.owner;
				auto hWnd_owner = ::GetAncestor(hWnd, GA_ROOTOWNER);
				string name;
				if(auto h = hWnd_owner ? hWnd_owner : hWnd; h)
					name = Window::class_name(h).move();

				if(is_exc(obj, name))
					return true;
			}

			return false;
		}

		HMENU ContextMenu::MenuHandle() const { return _hMenu; }

		void ContextMenu::build_main_system_menuitems(menuitem_t *menu, bool is_root)
		{
			if(!menu || menu->items.empty())
				return;

			auto is_location = [=](string &location, string const &path)->auto
			{
				location.trim().trim(L'/');

				if(location.empty())
					return is_root;

				if(location.starts_with(L"**", false))
					location.remove(0, 1);
				else if(location.equals(L"*", false))
					return true;

				if(is_root || path.empty())
					return false;

				return path.equals(location);
			};

			auto items = &menu->items;

			for(auto si : _cache->statics)
			{
				if(si->has_clsid)
					continue;

				if(!Selected.verify_types(si->fso))
					continue;

				for(size_t i = 0; i < items->size(); i++)
				{
					bool removed = false;
					auto where_defined = false;
					auto item = items->at(i);
					try 
					{
						string location;
						this_item _this;

						_this.type = item->type;
						_this.pos = static_cast<int>(i);
						_this.checked = item->checked ? (item->radio_check ? 2 : 1) : 0;
						_this.disabled = item->disabled;
						_this.system = true;
						_this.id = item->uid();
						_this.length = item->length;

						_this.parent = menu->uid();
						//_this.level = (int)parent_level.size();

						if(!item->is_separator())
						{
							_this.length = item->length; ;// mii->title.length<uint32_t>();
							_this.title = item->title;
							_this.title_normalize = item->name;
						}

						_context._this = &_this;

						//if(!Selected.verify_types(si->fso))
						//	continue;

						if(si->mode)
						{
							auto mode = _context.parse_mode(si->mode);
							if(Selected.Window.id > WINDOW_TASKBAR && !Selected.verify_mode(mode))
								break;
						}

						if(si->location)
						{
							if(_context.Eval(si->location, location, true))
							{
								if(!is_location(location, item->path))
									goto skip;
							}
						}
						else if(!is_root)
							goto skip;

						if(si->where)
						{
							if(where_defined = _context.Eval(si->where).to_bool(); !where_defined)
								continue;
						}

						if(item->is_separator())
						{
							if(!where_defined || si->find)
								goto skip;
						}
						else
						{
							if(item->title.empty())
								goto skip;

							if(!si->find && !where_defined)
								goto skip;
							else
							{
								Object find = _context.Eval(si->find).move();
								if(find.is_null() || find.length() == 0)
								{
									if(!where_defined || (si->moveto && !is_root))
										goto skip;
								}
								else
								{
									string pattern = find.to_string().trim().tolower().move();
									FindPattern find_pattern;
									if(!find_pattern.split(pattern, L'|') && !where_defined)
										goto skip;

									if(!find_pattern.find(&item->name))
										goto skip;
								}
							}
						}

						if(si->visibility && _settings.modify_items.visibility)
						{
							item->visibility = _context.parse_visibility(si->visibility);

							if(item->visibility == Visibility::Hidden)
								removed= true;
							else if(!item->is_separator())
							{
								if(item->visibility == Visibility::Enabled)
									item->disabled = false;
								else if(item->visibility == Visibility::Disabled)
									item->disabled = true;
								_context._this->disabled = item->disabled;
							}
						}

						if(removed)
							items->erase(items->begin() + i--);
						else if(_settings.modify_items.parent)
						{
							string moveto;
							if(_context.Eval(si->moveto, moveto, true))
							{
								moveto.trim(L'/');

								if(!moveto.equals(item->path))
								{
									item->path = moveto.move();
									if(auto submenu = __map_system_menu[item->path.hash()]; submenu)
									{
										item->parent = submenu;
										submenu->items.push_back(item);
									}
									else
									{
										__movable_system_items.push_back(item);
									}
									items->erase(items->begin() + i--);
								}
							}
						}

						if(!removed)
							item->native_items.push_back(si);

						if(si->invoke && 0 == _context.parse_invoke(si->invoke))
						{
							if(!removed && item->is_menu())
								build_main_system_menuitems(item);
							break;
						}

						continue;

					skip:
						if(item->is_menu())
							build_main_system_menuitems(item);
					}
					catch(std::exception const& ex)
					{
						_log.error(L"%S", ex.what());
					}
				}
			}
		}

		void ContextMenu::build_system_menuitems(HMENU hMenu, menuitem_t *menu, bool is_root)
		{
			::SendMessageW(hwnd.owner, WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(hMenu), 0xFFFFFFFF);

			auto itmes_count = ::GetMenuItemCount(hMenu);

			menu->items.reserve(itmes_count);

			for(int i = 0; i < itmes_count; i++)
			{
				int found_duplicate = 0;
				string title;
				MENUITEMINFOW mii{ sizeof(MENUITEMINFOW) };
				mii.fMask = MenuItemInfo::FMASK;
				mii.dwTypeData = title.buffer((1024));
				mii.cch = 1024;

				if(::GetMenuItemInfoW(hMenu, i, true, &mii))
				{
					std::unique_ptr<menuitem_t> item(new menuitem_t);
					auto itemPtr = item.get();
					item->parent = menu;
					item->wid = mii.wID;
					item->dwItemData = mii.dwItemData;
					item->is_toplevel = is_root;

					if(is_root)
						;
					else
					{
						string n = menu->name;
						if(!menu->path.empty())
							item->path += menu->path + L'/';
						item->path += n.tolower();
					}

					if(mii.fType & MFT_SEPARATOR)
					{
						if(_settings.modify_items.remove.separator)
							continue;
						item->type = 2;
					}
					else
					{
						item->type = mii.hSubMenu != nullptr;
						item->disabled = mii.fState & MFS_DISABLED;
						item->checked = mii.fState & MFS_CHECKED;
						item->radio_check = mii.fType & MFT_RADIOCHECK;
						item->image = MenuItemInfo::FindImage(&mii);

						if(item->disabled && _settings.modify_items.remove.disabled)
							continue;
						
						if(mii.cch > 0)
						{
							item->title = title.release(mii.cch).move();
							item->hash = MenuItemInfo::normalize(item->title, &item->name, &item->tab, &item->length, &item->keys);

							item->ui = Initializer::get_muid(item->hash);

							if(!item->is_menu() && is_root && item->disabled)
							{
								if(item->uid() == IDENT_ID_EMPTY_RECYCLE_BIN)
								{
									item->disabled = false;
									SHQUERYRBINFO sqrbi = { sizeof(SHQUERYRBINFO) };
									DLL::Invoke<HRESULT>(L"shell32.dll", "SHQueryRecycleBinW", nullptr, &sqrbi);
									if((sqrbi.i64Size + sqrbi.i64NumItems) == 0)
										item->disabled = true;
								}
							}

							if(_settings.modify_items.remove.duplicate)
							{
								uint32_t indexof = 0;
								for(auto im : menu->items)
								{
									if(im->hash == item->hash)
									{
										if(im->type == item->type)
										{
											found_duplicate = 1;
											if(im->disabled)
											{
												if(!item->disabled)
												{
													found_duplicate = 2;
													menu->items[indexof] = item.release();
												}
											}
											break;
										}
									}
								}
								indexof++;
							}
							
							if(found_duplicate == 1)
								continue;
						}
						else if(mii.fType & MFT_BITMAP)
						{
							//_log.info(L"MFT_BITMAP");
						}

						if(mii.hSubMenu)
							build_system_menuitems(mii.hSubMenu, itemPtr, false);
					}
					
					if(found_duplicate != 2)
					{
						if(item->is_menu())
						{
							string sub_path = item->name;
							if(!item->path.empty())
								sub_path = item->path + L'/' + sub_path;
							__map_system_menu[sub_path.hash()] = itemPtr;
						}

						menu->items.push_back(item.release());
					}
				}
			}
		}

		bool ContextMenu::Initialize()
		{
			try
			{
				if(!Initializer::Inited()) return false;

				__trace(L"ContextMenu init");

				auto initializer = Initializer::instance;

				_context.Selections = &Selected;
				_context.Application = &initializer->application;
				_context.Cache = initializer->cache;

				Selected.Window.handle = hwnd.owner;
				Selected.Window.hInstance = _window.instance();

				if(!Selected.QueryShellWindow())
				{
					__trace(L"QueryShellWindow");
					return false;
				}

				//if(is_excluded())
				//	return false;

				auto sets = &_cache->settings;
				
				/*if(auto h = hWnd_owner ? hWnd_owner : hWnd; h)
					_result = Window::class_name(h);
				*/
				if(Selected.Window.id == WINDOW_UI)
				{
					if(Selected.Window.hash != WC__STATIC)
					{
						if(hwnd.active == hwnd.focus)
						{
							//return false;
						}
					}
				}

				if(!initializer->query())
				{
					__trace(L"initializer query");
					return false;
				}

				_context.Cache = initializer->cache;
				_cache = initializer->cache;
				_context.variables.global = &_cache->variables.global;
				_context.variables.runtime = &_cache->variables.runtime;
				_context.variables.local = nullptr;

				switch(Selected.Window.id)
				{
					case WINDOW_TASKBAR:
						Selected.Types[FSO_TASKBAR] = TRUE;
						Selected.front = FSO_TASKBAR;
						Selected.Directory = Path::GetKnownFolder(FOLDERID_Desktop).move();
						break;
					default:
						if(!Selected.QuerySelected())
						{
							__trace(L"QuerySelected");
							//return false;
						}
						//::{2cc5ca98-6485-489a-920e-b3e88a6ccce3}
						//return false;
						break;
				}

				if(Selected.Directory.empty())
					Selected.Directory = Path::CurrentDirectory().move();

				_vis = _context.parse_visibility(_cache->dynamic.visibility);

				if(_vis == Visibility::Hidden)
					return false;

				if(!Selected.Preparing())
				{
					__trace(L"Selected.Preparing");
					return false;
				}

				if(is_excluded())
					return false;

				hInstance = _window.instance();

				composition.activated = ::IsCompositionActive();
				::DwmIsCompositionEnabled(reinterpret_cast<BOOL *>(&composition.DwmEnabled));
				
				init_cfg();
				
				if(!_windowSubclass.hook(hwnd.owner, WindowSubclassProc, CONTEXTMENUSUBCLASS, this))
				{
					__trace(L"WindowSubclass");
					return false;
				}
			
				Prop::Set(hwnd.owner, this);

				if(_winEventHook.hook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, Initializer::HInstance,
									  ContextMenu::WinEventProc, ProcessId, ThreadId))
				{
					HookMap[_winEventHook.get()] = this;
				}

				if(_context.eval_bool(sets->screenshot.enabled))
				{
					_keyboardHook.hook(WH_KEYBOARD, KeyboardProc, ThreadId);
					_context.Eval(sets->screenshot.directory, _screenshot, true);
				}

				hCursor = ::LoadCursorW(nullptr, IDC_ARROW);

				if(hwnd.active)
				{
					Prop::Set(hwnd.active, this);
				}

				//set_prop(hWnd, ctx);

				__trace(L"ContextMenu.Initialized");
				
				__system_menu_tree = new menuitem_t;
				__system_menu_tree->type = 10;
				__map_system_menu[0] = __system_menu_tree;

				if(0 == ::GetPropW(hwnd.owner, UxSubclass))
					build_system_menuitems(_hMenu_original, __system_menu_tree, true);

				if(_settings.modify_items.enabled)
					build_main_system_menuitems(__system_menu_tree, true);

				return true;
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return false;
		}

		using namespace Diagnostics;

		int ContextMenu::Uninitialize()
		{
			if(_uninitialized)
				return TRUE;

			int result = FALSE;
			try
			{
				if(_hMenu)
				{
					::DestroyMenu(_hMenu);
					_hMenu = nullptr;
				}

				_windowSubclass.unhook();

				if(_hTheme)
				{
					::CloseThemeData(_hTheme);
					_hTheme = nullptr;
				}

				if(_hbackground)
				{
					::DeleteObject(_hbackground);
					_hbackground = nullptr;
				}

				if(font.handle) {
					::DeleteObject(font.handle);
					font.handle = nullptr;
				}

				if(hCursor)
				{
					::DestroyCursor(hCursor);
					hCursor = nullptr;
				}

				if(_showdelay[0] != UINT32_MAX && _showdelay[1] != UINT32_MAX && _showdelay[0] != _showdelay[1])
				{
					::SystemParametersInfoW(SPI_SETMENUSHOWDELAY, _showdelay[0], nullptr, SPIF_SENDCHANGE);
					_showdelay[0] = UINT32_MAX;
				}

				_keyboardHook.unhook();
				
				if(_winEventHook)
				{
					HookMap.erase(_winEventHook.get());
					_winEventHook.unhook();
				}

				Prop::Remove(hwnd.active);
				Prop::Remove(hwnd.owner);

				_tip.destroy();

				delete __system_menu_tree;

				__trace(L"ContextMenu.Uninitialized");
				_uninitialized = true;
				result = TRUE;
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return result;
		}

		int ContextMenu::InvokeCommand(int id)
		{
			Uninitialize();

			invoke_item = nullptr;

			if(id != 0)
			{
				if(ident.equals(id)) 
				{
					for(auto item : _items_command)
					{
						if(item->wID == static_cast<uint32_t>(id))
						{
							invoke_item = item;
							break;
						}
					}

					id = 0;
					if(invoke_item && invoke_item->dynamic)
					{

						//Invoke(this);
						//_thread.create(Invoke, this);
						std::thread(&Invoke, this).detach();
						//thread(Invoke, this).detach();
						//thread::begin(Invoke, this);
						return id;
					}
				}

				if(keyboard.equals({ VK_LMENU, VK_LCONTROL, L'C' }))
				{
					for(auto item : _items)
					{
						if(item->is_item() && item->wID == static_cast<uint32_t>(id))
						{
							invoke_item = item;
							break;
						}
					}

					if(invoke_item)
						_log.write(L"%s CommandId %d\r\n", invoke_item->title.text.c_str(), id);
					else
						_log.write(L"CommandId %d\r\n", id);
				}
			}

			// self destroy
			delete this;

			return id;
		}

		static void set00(ContextMenu *cm)
		{
			auto ctx = &cm->_context;
			auto menu = cm->invoke_item->owner_dynamic;
			ctx->Selections = &cm->Selected;
			//ctx->variables.runtime = &menu->owner->variables;
			ctx->variables.local = &menu->owner->variables;
			ctx->Keyboard->get_keys_state(true);

			auto invoke_item = cm->invoke_item;

			this_item _this;
			_this.type = (int)invoke_item->type;
			_this.pos = static_cast<int>(invoke_item->position);
			_this.checked = invoke_item->is_checked() ? (invoke_item->is_radiocheck() ? 2 : 1) : 0;
			_this.disabled = invoke_item->is_disabled();
			_this.system = true;
			_this.id = invoke_item->hash;
			_this.length = invoke_item->length;
			_this.parent = invoke_item->id;
			_this.length = invoke_item->length;
			_this.title = invoke_item->title;
			_this.title_normalize = invoke_item->title.normalize;
			ctx->_this = &_this;
		}

		void __stdcall ContextMenu::Invoke(ContextMenu *cm)
		{
			if(!cm) return;

			__try
			{
				auto ctx = &cm->_context;
				auto menu = cm->invoke_item->owner_dynamic;
				ctx->Selections = &cm->Selected;
				//ctx->variables.runtime = &menu->owner->variables;
				ctx->variables.local = &menu->owner->variables;
				ctx->Keyboard->get_keys_state(true);
				
				set00(cm);

				if(cm->mouse_button)
				{
					ctx->Keyboard->add_key(cm->mouse_button);
				}

				for(auto cmd_prop : menu->commands)
				{
					ctx->invoked = 0;
					cm->Selected.Index = 0;
					__try
					{
						if(cmd_prop)
						{
							if(auto invoke = cm->_context.parse_invoke(cmd_prop->invoke); invoke)
							{
								for(size_t i = 0; i < ctx->Selections->Count(); i++)
								{
									ctx->Selections->Index = i;
									if(2 == cm->invoke(cmd_prop))
										__leave;

									if(ctx->Break)
									{
										ctx->Break = false;
										__leave;
									}
									
									if(invoke > 1)
										::Sleep(invoke);
									ctx->invoked++;
								}
							}
							else if(2 == cm->invoke(cmd_prop))
								__leave;
						}
					}
					except
					{
#ifdef _DEBUG
						Logger::Exception(__func__);
#endif
					}
				}
			}
			__finally
			{
				delete cm;
				//thread::end();
			}
		}

		uint32_t ContextMenu::invoke(CommandProperty *cmd_prop)
		{
			struct {
				int type = 0;
				string command;
				string arguments;
				string directory;
				string verb;
				uint32_t window = SW_SHOWNORMAL;
				uint32_t wait = 0;
				uint32_t cancel = 0;
				Privileges admin = Privileges::None;
			} cmd;

			try
			{
				bool cancel = false;
				cmd.type = cmd_prop->command.type;
				string value;

				cmd.admin = invoke_item->privileges;
				
				if(cmd_prop->admin)
					cmd.admin = _context.parse_privileges(cmd_prop);

				if(cmd.type == COMMAND_PROMPT)
					cmd.command = Environment::Variable(def_COMSPEC).move();
				else if(cmd.type == COMMAND_SHELL || cmd.type == COMMAND_EXPLORER)
					cmd.command = /*Environment::Expand(L"windir") +*/ def_EXPLORER;
				else if(cmd.type == COMMAND_POWERSHELL)
					cmd.command = def_POWERSHELL;
				else if(cmd.type == COMMAND_PWSH)
					cmd.command = def_PWSH;
				else if(_context.Eval(cmd_prop->command.expr, value))
				{
					cmd.command = value.trim().move();
					if(cmd.command.taged(L'"'))
						cmd.command.trim(L'"');
					if(cmd.command.taged(L'\''))
						cmd.command.trim(L'\'');
				}

				if(!cmd.command.empty())
				{
					if(cmd.admin == Privileges::Default)
						cmd.verb = L"runas";
					else if(_context.Eval(cmd_prop->verb, value))
						cmd.verb = value.trim().move();

					cmd.window = _context.parse_window(cmd_prop);
					auto wait = _context.parse_wait(cmd_prop);

					if(_context.Eval(cmd_prop->arguments, value))
						cmd.arguments = value.move();

					cmd.directory = Selected.Directory;

					Object obj;
					if(_context.Eval(cmd_prop->directory, obj))
					{
						if(obj.is_number() && obj.to_bool() && !cmd.command.empty())
							cmd.directory = Path::Parent(cmd.command).move();
						else if(obj.is_string())
						{
							cmd.directory = obj.to_string().move();
							Selected.Directory = cmd.directory;
						}
					}

					/*auto r = */ShellExec::Run(cmd.command, cmd.arguments, cmd.directory,
												cmd.verb, wait, cmd.window);
				}
				//	cancel = r != exit_code
				if(cancel) return 2;
				return TRUE;
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return FALSE;
		}
		
		bool Tip::show()
		{
			if(handle && enabled)
			{
				hide();
				if(time == UINT16_MAX)
					time = ctx ? ctx->_theme.tip.time : TIMEIN;
				::SetTimer(handle, IDT_SHOW, time, nullptr);
				return true;
			}
			return false;
		}

		bool Tip::show(Expression *e, const Rect &rc)
		{
			this->e = e;
			this->rect = rc;
			return show();
		}

		bool Tip::show(const string &text, uint8_t type, uint16_t time, const Rect &rc)
		{
			this->type = type;
			this->text = text;
			this->rect = rc;
			this->time = time;
			return show();
		}

		bool Tip::hide(bool cancel)
		{
			if(handle)
			{
				if(cancel)
				{
					killTimer();
				}

				if(handle && visible)
				{
					rect = {};
					text = {};
					visible = false;
					e = {};
					return ::ShowWindow(handle, SW_HIDE);
				}
			};
			return true;
		}

		LRESULT Tip::Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch(uMsg)
			{
				case WM_TIMER:
				{
					auto nIDEvent = static_cast<UINT_PTR>(wParam);
					if(nIDEvent == IDT_SHOW)
					{
						::KillTimer(hWnd, Tip::IDT_SHOW);

						if(!text.empty())
						{
							auto theme = &ctx->_theme;
							DC dc = hWnd;
							dc.set_font(ctx->font.handle);
							Rect rc = { 0, 0, ctx->dpi(theme->tip.maxwidth), ctx->dpi(Tip::MAXHEIGHT) };
							dc.measure(text, text.length<int>(), &rc, DT_EXPANDTABS | DT_WORDBREAK | DT_EDITCONTROL);

							SIZE size = {
								theme->tip.padding.width() + rc.width(),
								theme->tip.padding.height() + rc.height()
							};

							PlutoVG pluto(size.cx, size.cy);

							Color txtclr = theme->tip.normal.text;
							Color bgclr = theme->tip.normal.back;

							// 0x0D6EFD

							if(type == 1)
							{
								txtclr = theme->tip.primary.text;
								bgclr = theme->tip.primary.back;
							}
							if(type == 2)
							{
								txtclr = theme->tip.info.text;
								bgclr = theme->tip.info.back;
							}
							else if(type == 3)
							{
								txtclr = theme->tip.success.text;
								bgclr = theme->tip.success.back;
							}
							else if(type == 4)
							{
								txtclr = theme->tip.warning.text;
								bgclr = theme->tip.warning.back;
							}
							else if(type == 5)
							{
								txtclr = theme->tip.danger.text;
								bgclr = theme->tip.danger.back;
							}

							int radius = ctx->dpi(theme->tip.radius);

							pluto.rect(0, 0, size.cx, size.cy, radius).fill(0x000000, 32, false);
							pluto.rect(1, 1, size.cx - 2, size.cy - 2, radius);

							if(theme->tip.border)
								pluto.fill(theme->tip.border.to_RGB(), theme->tip.border.a);
							else
								pluto.fill(0x000000, 32, false);

							pluto.rect(2, 2, size.cx - 4, size.cy - 4, radius).clear();

							pluto.rect(2, 2, size.cx - 4, size.cy - 4, radius)
								.fill(bgclr.to_RGB(), theme->tip.opacity);
							
							auto_gdi<HBITMAP> bitmap(pluto.tobitmap());
							DC dc_layer(dc.CreateCompatibleDC(), 1);
							dc_layer.set_font(ctx->font.handle);

							dc_layer.select_bitmap(bitmap.get());
							auto top = theme->tip.padding.top - (ctx->dpi.scale() > 1.0 ? ctx->dpi(1.5) : 1);

							Rect rctxt = { theme->tip.padding.left, top,
								size.cx - theme->tip.padding.right,
								size.cy - theme->tip.padding.bottom };

							ctx->draw_string(dc_layer, ctx->font.handle,
											 &rctxt,
											 txtclr,
											 text, text.length<int>(),
											 (ctx->is_layoutRTL ? DT_RIGHT : DT_LEFT) | DT_EXPANDTABS | DT_WORDBREAK | DT_EDITCONTROL);

							long x = rect.left + (rect.width() / 2) - (size.cx / 2);
							long y = rect.top - size.cy - ctx->dpi(10);

							if(x < 0) x = 0;
							if(y < 0) y = rect.bottom + ctx->dpi(10);

							if((x + size.cx) > ctx->_rcMonitor.right)
								x = ctx->_rcMonitor.right - size.cx;

							if(::SetWindowPos(hWnd, HWND_TOPMOST, x, y, size.cx, size.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW))
							{
								// use the source image's alpha channel for blending
								BLENDFUNCTION blend{ AC_SRC_OVER , 0, 0xFF, AC_SRC_ALPHA };
								POINT ptZero{ };
								// paint the window (in the right location) with the alpha-blended bitmap
								::UpdateLayeredWindow(hWnd, 0, 0, &size, dc_layer, &ptZero, 0x000000, &blend, ULW_ALPHA);
								::UpdateWindow(hWnd);
								::SetTimer(hWnd, Tip::IDT_HIDE, Tip::TIMEOUT, nullptr);
								visible = true;
							}
						}
						else
						{
							hide();
						}
					}
					else if(nIDEvent == IDT_HIDE)
					{
						hide(true);
					}
					break;
				}
			}
			return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
		}

#pragma region Class Menu Proc

		static const SIZE FixedFrame = {
			::GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
			::GetSystemMetrics(SM_CYFIXEDFRAME) * 2
		};

		WND *ContextMenu::OnMenuCreate(HWND hWnd)
		{
			auto wnd = &_map[hWnd];

			wnd->ctx = this;
			wnd->handle = hWnd;
			wnd->set_prop();
			set_prop(hWnd);

			///	auto hMenu = (HMENU)::SendMessageW(hWnd, MN_GETHMENU, 0, 0);
			//	wnd->hMenu = hMenu;
			//	map_menu_wnd[hMenu] = { hMenu, hWnd };

			current.hWnd = hWnd;
			_level.push_back(wnd);

			Flag<ULONG_PTR> cs_style = ::GetClassLongPtrW(hWnd, GCL_STYLE);
			Flag<LONG_PTR> style = ::GetWindowLongPtrW(hWnd, GWL_STYLE);
			Flag<LONG_PTR> ex_style = ::GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

			auto cs_style_old = cs_style;
			auto style_old = style;
			auto ex_style_old = ex_style;

			//RECT r = { 0 };
			//AdjustWindowRectEx(&r, (DWORD)style.value, false, (DWORD)ex_style.value);

			cs_style.remove(CS_DROPSHADOW);
			style.remove(WS_BORDER);

			ex_style.remove(WS_EX_WINDOWEDGE);
			ex_style.remove(WS_EX_DLGMODALFRAME);

			if(composition)
				ex_style.add(WS_EX_COMPOSITED);

			//ex_style.add(WS_EX_LAYERED);
			//ex_style.add(WS_EX_NOREDIRECTIONBITMAP);

			if(!cs_style.equals(cs_style_old))
				::SetClassLongPtrW(hWnd, GCL_STYLE, cs_style);

			if(!style.equals(style_old))
				::SetWindowLongPtrW(hWnd, GWL_STYLE, style);

			if(!ex_style.equals(ex_style_old))
				::SetWindowLongPtrW(hWnd, GWL_EXSTYLE, ex_style);
						
			//::SetClassLongPtrW(hWnd, GCL_STYLE, 0);
			//::SetWindowLongPtrW(hWnd, GWL_STYLE, WS_POPUP);
			//::SetWindowLongPtrW(hWnd, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP);

			::PostMessageW(hWnd, WM_SETCURSOR, 0, 0);

			//int opacity = 100;
			//SetLayeredWindowAttributes(hWnd, 0, (255 * 100) / 100, LWA_ALPHA);
			//SetLayeredWindowAttributes(hWnd, 0x0ff00, 0, LWA_COLORKEY);
			if(composition)
			{
				AccentPolicy ap(hWnd);
				//ap.set(AccentPolicy::Disabled);
				//ap.set(ap.AcrylicBlurBehind, ap.AllowSetWindowRgn, _theme.background.color.to_ABGR());

				//if(_theme.transparent)
				Compositor::TransparentArea(hWnd);
			}

			BOOL ENABLED = TRUE;
			::DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_ENABLED, &ENABLED, sizeof(BOOL));
			::DwmSetWindowAttribute(hWnd, DWMWA_ALLOW_NCPAINT, &ENABLED, sizeof(BOOL));
			::DwmSetWindowAttribute(hWnd, DWMWA_NONCLIENT_RTL_LAYOUT, &ENABLED, sizeof(BOOL));

			WindowSubclass::Set(hWnd, MenuSubClassProc, 0, this);
			//::ShowWindowAsync(hWnd, SW_HIDE);
			return wnd;
		}

		void ContextMenu::OnMenuShow(HWND hWnd, WND *wnd)
		{
			if(wnd == nullptr)
				wnd = WND::get_prop(hWnd);

			if(wnd)
			{
				wnd->show_layers();
				//Compositor::TransparentArea(hWnd);
				// must used sendmsg to invoke initmenupopup
				//if(ver->IsWindows11OrGreater())
				//	DWM(hWnd).RemoveCorner();
			}
		}

		HBITMAP crop_image(const RECT rectangle, const HBITMAP source_image)
		{
			return static_cast<HBITMAP>(CopyImage(source_image, IMAGE_BITMAP, rectangle.right - rectangle.left,
																rectangle.bottom - rectangle.top, LR_CREATEDIBSECTION));
		}

		void ContextMenu::screenshot()
		{
			if(!_level.empty()) try
			{
				auto wnd = _level.front();
				auto hwnd = wnd->handle;
				auto hdesktop = ::GetDesktopWindow();
				Rect rc_desktop = hdesktop;
				SIZE sz = { rc_desktop.right + 100, rc_desktop.bottom + 100 };

				DC dc = hwnd;
				uint8_t *bits{};
				auto_gdi<HBITMAP> hbitmap(dc.CreateDIBSection(sz.cx,sz.cy, &bits));

				if(!hbitmap)
					return;

				DC dc_dst(dc.CreateCompatibleDC(), 1);
				dc_dst.select_bitmap(hbitmap.get());

				Point pt0 = { sz.cx, sz.cy };
				Point pt1 = { 0, 0 };

				for(auto &l : _level)
				{
					Rect rc = l->handle;
					if(l->layer.hbitmap)
					{
						BITMAP bmp{}; GetObject(l->layer.hbitmap, sizeof(bmp), &bmp);
						dc_dst.draw_image(rc.point(), { bmp.bmWidth, bmp.bmHeight }, l->layer.hbitmap);
					}

					if(l->blurry.handle)
					{
						DC dc_blurry = l->blurry.handle;
						dc_dst.draw_image(rc.point(), rc.size(), dc_blurry);
					}

					dc_dst.draw_image(rc.point(50), rc.size(), l->hdc);

					pt0.x = std::min<long>(rc.left, pt0.x);
					pt0.y = std::min<long>(rc.top, pt0.y);
					pt1.x = std::max<long>(rc.right, pt1.x);
					pt1.y = std::max<long>(rc.bottom, pt1.y);
				}
				
				pt0.x = std::max<long>(0, pt0.x);
				pt0.y = std::max<long>(0, pt0.y);

				pt1.x = std::min<long>(sz.cx, pt1.x);
				pt1.y = std::min<long>(sz.cy, pt1.y);
				
				sz = { (pt1.x - pt0.x) + 100, (pt1.y - pt0.y) + 100 };

				bits = nullptr;
				auto_gdi<HBITMAP> hbitmap0(dc_dst.CreateDIBSection(sz.cx, sz.cy, &bits));
				if(bits)
				{
					DC dc0(dc_dst.CreateCompatibleDC(), 1);
					dc0.select_bitmap(hbitmap0.get());
					dc0.draw_image({ }, sz, dc_dst, pt0, sz);
				}
				
				auto w = sz.cx, h = sz.cy;
				
				auto p = bits;
				if(bits)
				{
					std::unique_ptr< unsigned char[]>flip(new unsigned char[w * h * 4]);

					auto src = p + w * h * 4 - w * 4;
					auto dst = flip.get();

					for(int y = 0; y < h; y++)
					{
						::memcpy(dst, src, static_cast<size_t>(w) * 4);
						src -= w * 4;
						dst += w * 4;
					}

					auto pi = (RGBA *)flip.get();
					for(int i = 0; i < w * h; i++)
					{
						pi[i].prem();
					}

					SYSTEMTIME lt = { };
					::GetLocalTime(&lt);

					string tf = string::TimeFormat(&lt, L"ymd_HMS").move();
					//string::NewGuid().c_str()
					// FOLDERID_Pictures, FOLDERID_SavedPictures
					string location = _screenshot;

					if(location.empty())
						location = IO::Path::GetKnownFolder(FOLDERID_Screenshots).move();
					
					if(location.empty())
						location = Initializer::instance->application.Dirctory;
					
					location = Path::Combine(location, L"screenshot_" + tf + L".png");

					plutovg_stbi_write_png(location, w, h, flip.get());
				}
			}
			catch(...) {
			}
		}

		bool ContextMenu::draw_layer(WND *wnd, SIZE size, int margin)
		{
			//Gradients - box, linear and radial
			if(!wnd || !wnd->ctx)
				return false;

			if(wnd->layer.hbitmap)
			{
				::DeleteObject(wnd->layer.hbitmap);
				wnd->layer.hbitmap = nullptr;
			}

			double radius = _theme.border.radius;
			//auto border_opacity = double(double(theme->frame.opacity) / 100.0);
			auto szborder = _theme.border.size;
			auto szborder2 = _theme.pow(szborder);

			double x = margin;
			double y = margin;
			double w = (size.cx - (margin + margin));
			double h = (size.cy - (margin + margin));

			PlutoVG pluto(size.cx, size.cy);

			bool need_clear = _theme.background.effect > 0;

			if(_theme.shadow.enabled)
			{
				uint8_t shadow_opacity = _theme.shadow.color.a;
				Rect shadow_rect = { 0, 0, size.cx, size.cy };

				auto clr = _theme.shadow.color.to_RGB();
				//uint8_t offset = 0;
				auto sz = _theme.shadow.size;

				uint8_t o = shadow_opacity * 20 / 100;
				need_clear = _theme.background.color.a < 0xFF;
				sz = sz > 30 ? 30 : sz;

				/*for(uint8_t i = 0; i < sz + 1; i++)
				{
					auto b = szborder + i;
					pluto.rect(x - b,
							   y - b + _theme.shadow.offset,
							   w + (b * 2),
							   h + (b * 2),
							   radius + b)
						.fill(clr, o);
				}*/

				sz += _theme.shadow.offset;

				for(uint8_t i = 0; i < sz + 1; i++)
				{
					auto b = szborder + i;
					pluto.rect((x + _theme.shadow.offset) - b,
							   (y + _theme.shadow.offset) - b + _theme.shadow.offset,
							   w + (b * 2) - _theme.shadow.offset - _theme.shadow.offset,
							   h + (b * 2) - _theme.shadow.offset - _theme.shadow.offset,
							   radius + (b / static_cast<double>(2)))
						.fill(clr, o);
				}

			}

			SIZE back = { wnd->width, wnd->height };
			Rect border_rect = { margin, margin, wnd->width, wnd->height };
			Rect back_rect = { margin + szborder , margin + szborder,
				wnd->width - szborder2, wnd->height - szborder2 };

			if(_theme.background.effect == 0)
			{
				auto xb = (szborder % 2) == 0 ? 0.0 : 0.5;
				xb += (szborder / 2.0);
				pluto.rect(margin + xb, margin + xb,
						   back.cx - xb - xb, back.cy - xb - xb, radius)
					//.fill(0x00ff00, theme->background.color.a, true)
					.fill(_theme.background.color.to_RGB(), _theme.background.color.a, true)
					.stroke_width(szborder)
					.stroke_fill(_theme.border.color.to_RGB(), _theme.border.color.a);
			}
			else
			{
				if(szborder > 0 && _theme.border.color.a > 0)
				{
					need_clear = true;// theme->border.color.a < 0xFF;
					pluto.rect(border_rect.left, border_rect.top, border_rect.right, border_rect.bottom, radius == 0 ? 0 : radius + szborder)
						.fill(_theme.border.color.to_RGB(), _theme.border.color.a);
				}

				if(need_clear)
					pluto.rect(back_rect.left, back_rect.top, back_rect.right, back_rect.bottom, radius).clear();

				if(_theme.background.opacity > 0)
				{
					pluto.save();
					pluto.rect(back_rect.left, back_rect.top, back_rect.right, back_rect.bottom, radius)
						//.set_operator(plutovg_operator_src)
						//.set_fill_rule(plutovg_fill_rule_non_zero)
						.fill(_theme.background.color.to_RGB(), _theme.background.opacity, true);
					pluto.restore();
				}
			}

			if(_theme.gradient.enabled)
			{
				auto to_size = [](double value, double size) ->double { return value * size / 100; };
				bool render = false;
				Gradient gradient;

				auto fxx = szborder;
				auto fyy = margin + szborder;

				w = back_rect.right;
				h = back_rect.bottom;

				if(_theme.gradient.linear[0] == 1.0)
				{
					double x1 = to_size(_theme.gradient.linear[1], w);
					double y1 = to_size(_theme.gradient.linear[2], h);
					double x2 = to_size(_theme.gradient.linear[3], w);
					double y2 = to_size(_theme.gradient.linear[4], h);

					if(x1 > 0) x1 += fxx * 2;
					if(x2 > 0) x2 += fxx * 2;

					if(y1 > 0) y1 += fyy * 2;
					if(y2 > 0) y2 += fyy * 2;
					
					render = x1 != 0.0 || y1 != 0.0 || x2 != 0.0 || y2 != 0.0;
					if(render)
						gradient.create_linear(x1, y1, x2, y2);
				}
				else if(_theme.gradient.radial[0] == 1.0)
				{
					double cx = to_size(_theme.gradient.radial[1], w / 2 + fxx);
					double cy = to_size(_theme.gradient.radial[2], h / 2 + fyy);
					double r  = to_size(_theme.gradient.radial[3], ((h >= w ? w : h) / 2));
					double fx = to_size(_theme.gradient.radial[4], w / 2 + fxx);
					double fy = to_size(_theme.gradient.radial[5], h / 2 + fyy);

					render = cx != 0.0 || cy != 0.0 || r != 0.0 || fx != 0.0 || fy != 0.0;
					if(render)
						gradient.create_radial(cx, cy, r, fx, fy, 0);
				}

				if(render)
				{
					for(auto &s : _theme.gradient.stpos)
						gradient.add_stop(s.offset, s.color.to_RGB(), s.color.a);
					
					pluto.rect(back_rect.left, back_rect.top, w, h, radius).fill(gradient);
				}
			}

			wnd->layer.hbitmap = pluto.tobitmap();
			return wnd->layer.hbitmap;
		}

		void ContextMenu::UpdateLayered(WND *wnd, bool update_blurry)
		{
			if(!wnd) return;

			int margin = 50;

			SIZE size = {
				wnd->width + margin + margin,
				wnd->height + margin + margin
			};

			if(wnd->layer.handle && draw_layer(wnd, size, margin))
			{
				DC dc = wnd->layer.handle;
				DC dc_layer(dc.CreateCompatibleDC(), 1);
				dc_layer.select_bitmap(wnd->layer.hbitmap);

				POINT ptZero{ };
				// use the source image's alpha channel for blending
				BLENDFUNCTION blend{ AC_SRC_OVER , 0, 0xFF, AC_SRC_ALPHA };
				// paint the window (in the right location) with the alpha-blended bitmap
				::UpdateLayeredWindow(wnd->layer.handle, nullptr, nullptr, &size, dc_layer, &ptZero, 0x000000, &blend, ULW_ALPHA);
				dc_layer.restore_bitmap();
			}

			if(wnd->blurry.handle)
			{
				if(update_blurry)
				{
					auto flags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_SHOWWINDOW;
					SetWindowPos(wnd->blurry.handle, nullptr, wnd->x, wnd->y, wnd->width, wnd->height, flags);
				}

				if(_theme.border.radius > 0)
				{
					auto border = _theme.border.size;
					auto w = wnd->width - border - border;
					auto h = wnd->height - border - border;
					wnd->Regoin(wnd->blurry.handle, 0, 0, w + 1, h + 1, int(_theme.border.radius / 1.5));
				}
			}
		}

		bool ContextMenu::CreateLayer(WND *wnd)
		{
			if(!wnd || !wnd->ctx) return false;

			DWM dwm;
			auto layer = &wnd->layer;
			auto blurry = &wnd->blurry;
			auto border = _theme.border.size;
			int margin = 50;

			SIZE size = {
				wnd->width + margin + margin,
				wnd->height + margin + margin
			};

			auto w = wnd->width - border - border;
			auto h = wnd->height - border - border;

			// Blurry is not useful in high contrast mode
			if(composition &&
			   _theme.background.effect >= 2 &&
			   _theme.background.opacity < 0xFF)
			{
				/*
				Enable Acrylic Effects in Hyper-V VM
				August 17, 2018 (revised September 8, 2018)
				[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Dwm]
				"ForceEffectMode"=dword:00000002
				*/

				//blurry->create(wnd->x + border - 1, wnd->y + border - 1, w + 2, h + 2, hwnd.focus, WS_EX_COMPOSITED);
				blurry->create(wnd->x + border, wnd->y + border, w, h, hwnd.focus, WS_EX_COMPOSITED /*| WS_EX_NOREDIRECTIONBITMAP*/);
				if(blurry->handle)
				{
					AccentPolicy accent(blurry->handle);
					// Blur is not useful in high contrast mode
					// Windows build version less than 16299 does not support acrylic effect
					// 1903 = 18362
					// 1803 = 17134
					// 1709 = 16299
					// 18362 	TintLuminosityOpacity
					if(_theme.background.effect >= 3)
					{
						if(ver->Major < 10 || (ver->Major == 10 && ver->Build < 17134))
							_theme.background.effect = 2;
					}

					uint32_t tintColor = _theme.background.tintcolor;

					if(_theme.background.effect == 2) // blur effect 
					{
						accent.state = accent.BlurBehind;
						if(_theme.border.radius > 0)
							accent.flags = accent.AllowSetWindowRgn;
					}
					else if(_theme.background.effect >= 3) // acrylic effect 
					{
						// Windows 10 build 17134
						accent.state = accent.AcrylicBlurBehind;
						accent.flags = accent.Luminosity;
						if(_theme.border.radius > 0)
							accent.flags |=accent.AllowSetWindowRgn;

						if(tintColor == 0)
							tintColor = 0x01000000;
					}

					dwm.handle = blurry->handle;
					//dwm.SetImmersiveDarkMode();
					//dwm.ExtendFrameIntoClientArea();

					Compositor::TransparentArea(blurry->handle);
					accent.color = tintColor;
					accent.set();
				}
			}

			layer->create(wnd->x - margin, wnd->y - margin, size.cx, size.cy, hwnd.focus, WS_EX_LAYERED);
			UpdateLayered(wnd, false);
			return wnd->show_layers();
		}

		struct ooo
		{
			int sel = 0;
			Rect rc{};
		}__o[6];
		int __sel = -1;
		Point __pt;
		int xx = 0;

		LRESULT __stdcall window_Subclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
															[[maybe_unused]] UINT_PTR uIdSubclass,
															[[maybe_unused]] DWORD_PTR dwRefData)
		{
			MESSAGE defSubclassProc = { hWnd, uMsg, wParam, lParam };
			switch(uMsg)
			{
				case WM_MOUSEMOVE:
					break;
				case WM_LBUTTONUP:
					break;
				case WM_RBUTTONUP:
					PostQuitMessage(WM_QUIT);
					break;
			}
			return defSubclassProc();
		}

		LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
		{
			if(nCode >= 0)
			{
				auto pMSG = (MSG *)lParam;

				//_log.info(L"0x%04x %s", pMSG->message, msg_map[pMSG->message]);
				
				switch(pMSG->message)
				{
					case WM_CREATE:
						break;
					case WM_MOUSEMOVE:
						//_log.info(L"%s", Window::class_name(GetCapture()).c_str());
						SendMessage(pMSG->hwnd, WM_MOUSEMOVE, pMSG->wParam, pMSG->lParam);
						break;
					case WM_MENUSELECT:
						// keep in mind menu handle and selected item identifier
						//m_hMenu = (HMENU)pMSG->lParam;
						//m_wMenuItemID = LOWORD(pMSG->wParam);
						break;
					case WM_LBUTTONUP:
					{
						// toggle check item
						// send command message to owner window
						//if(NULL != m_pWnd->GetSafeHwnd())
						//	m_pWnd->SendMessage(WM_COMMAND, MAKEWPARAM(m_wMenuItemID, 0), 0);

						// change to WM_NULL to prevent closing menu
						//pMSG->message = WM_NULL;
					}
					break;
					case WM_LBUTTONDBLCLK:
						// just set WM_NULL to get rid of all default processing 
						//pMSG->message = WM_NULL;
						break;
					case 485:  // HACK to handle popup menus
						//_log.info(L"485 HACK to handle popup menus");
						break;
				}
				//_log.info(L"%d, %04x", nCode, pMSG->message);
			}
			return ::CallNextHookEx(0, nCode, wParam, lParam);
		}

		HHOOK m_hHook = NULL;

		inline void fix_ugly_flicker()
		{
			LARGE_INTEGER freq, now0, now1;
			::QueryPerformanceFrequency(&freq); // hz

			// this absurd code makes Sleep() more accurate
			// - without it, Sleep() is not even +-10ms accurate
			// - with it, Sleep is around +-1.5 ms accurate
			TIMECAPS tc;

			::timeGetDevCaps(&tc, sizeof(tc));
			int ms_granularity = tc.wPeriodMin;
			::timeBeginPeriod(ms_granularity); // begin accurate Sleep() !

			::QueryPerformanceCounter(&now0);

			// ask DWM where the vertical blank falls
			DWM_TIMING_INFO dti;
			::memset(&dti, 0, sizeof(dti));
			dti.cbSize = sizeof(dti);

			::DwmGetCompositionTimingInfo(NULL, &dti);
			::QueryPerformanceCounter(&now1);

			// - DWM told us about SOME vertical blank
			//   - past or future, possibly many frames away
			// - convert that into the NEXT vertical blank
			__int64 period = (__int64)dti.qpcRefreshPeriod;
			__int64 dt = (__int64)dti.qpcVBlank - (__int64)now1.QuadPart;
			__int64 w, m;

			if(dt >= 0)
				w = dt / period;
			else // dt < 0
			{
				// reach back to previous period - so m represents consistent position within phase
				w = -1 + dt / period;
			}

			// uncomment this to see worst-case behavior
			// dt += (sint_64_t)(0.5 * period);
			m = dt - (period * w);
			assert(m >= 0);
			assert(m < period);
			double m_ms = 1000.0 * m / (double)freq.QuadPart;
			::Sleep((int)round(m_ms));
			::timeEndPeriod(ms_granularity);
		}
		int bbb = 0;
		int ixi = 0;
		LRESULT __stdcall ContextMenu::MenuSubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
														[[maybe_unused]] UINT_PTR uIdSubclass,
														DWORD_PTR dwRefData)
		{
			MESSAGE defSubclassProc = { hWnd, uMsg, wParam, lParam };

			auto ctx = reinterpret_cast<ContextMenu *>(dwRefData);
			if(ctx == nullptr)
			{
				if(auto gti = Window::GetGUIThread(hWnd); gti.hwndActive)
					ctx = Prop::Get(gti.hwndActive);

				if(ctx == nullptr)
					return defSubclassProc();
			}

			auto wnd = WND::get_prop(hWnd);
			if(!wnd)
				return defSubclassProc();

			if(!wnd->hdc)
			{
				ixi = 0;
				wnd->hdc = ::GetWindowDC(hWnd);
			}

			LRESULT lret = FALSE;
			auto theme = &ctx->_theme;
			//_log.info(L"%04x, %s", uMsg, msg_map[uMsg]);
			switch(uMsg)
			{
				//case MN_GETPPOPUPMENU:
				//case MN_SETHMENU:
				//	break;
				case WM_DESTROY:
				{
					//wnd->hidden();
					break;
				}
				case WM_NCDESTROY:
				{
					wnd->destroy();
					ctx->_map.erase(hWnd);
					if(!ctx->_level.empty())
						ctx->_level.pop_back();
					ctx->current.zero();
					break;
				}
				case WM_SIZE:
				case WM_MOVE:
				{
					//_log.info(L"%x", GetMenu(hWnd));
					break;
				}
				case MN_SIZEWINDOW: // after WM_INITMENUPOPUP
				{
					//_log.info(L"MN_SIZEWINDOW");
					if(wParam & MNSW_DRAWFRAME)
						wParam &= ~MNSW_DRAWFRAME;
					
					lret = defSubclassProc();
					return lret;
				}
				case MN_GETHMENU:
				{
					// returns the hmenu associated with this menu window.
					lret = defSubclassProc();
					if(!wnd->hMenu)
					{
						wnd->hMenu = reinterpret_cast<HMENU>(lret);
						if(::IsMenu(wnd->hMenu))
						{
							ctx->map_menu_wnd[wnd->hMenu] = { wnd->hMenu, hWnd };
						}
					}
					break;
				}
				case WM_NCCALCSIZE:
				{
					//_log.info(L"WM_NCCALCSIZE %x", wParam);
					xx = 0;
					__pt = { -1,-1 };
					//pncc->rgrc[0] is the new rectangle
					//pncc->rgrc[1] is the old rectangle
					//pncc->rgrc[2] is the client rectangle
					//lret = DefWindowProc(hWnd, WM_NCCALCSIZE, wParam, lParam);
					auto fCalcValidRects = static_cast<BOOL>(wParam);
					// Calculate new NCCALCSIZE_PARAMS based on custom NCA inset.
					if(fCalcValidRects)
					{
						[[maybe_unused]] auto nc = reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam);
						//if(++_WM_NCCALCSIZE == 1)
						{
							//nc->lppos->flags |= SWP_NOREDRAW| SWP_NOCOPYBITS;
							nc->rgrc[0].left += theme->border.size + theme->border.padding.left;
							nc->rgrc[0].top += theme->border.size + theme->border.padding.top;
							nc->rgrc[0].right += theme->border.size + theme->border.padding.right;
							nc->rgrc[0].bottom += theme->border.size + theme->border.padding.bottom;
							if(wnd->has_scroll)
							{
								nc->rgrc[0].top += ctx->dpi(10);
								nc->rgrc[0].bottom -= ctx->dpi(10);
							}
						}
					//	return WVR_VALIDRECTS;
					}
					else
					{
						//auto rect = reinterpret_cast<RECT*>(lParam);
						//r->top += 200;
						//const int cxBorder = 2;
						//const int cyBorder = 2;
						//InflateRect((LPRECT)lParam, -cxBorder, -cyBorder);
					}
					fix_ugly_flicker();
					//::DwmFlush();// wait till finished
					return lret;
				}
				//BOOL OnWindowPosChanging(HWND hwnd, LPWINDOWPOS pwp)
				case WM_WINDOWPOSCHANGING:
				{
					//_log.info(L"WM_WINDOWPOSCHANGING");
					//lret = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
					auto wp = reinterpret_cast<WINDOWPOS *>(lParam);
					Flag<uint32_t> flags = wp->flags;

					if(!flags.has(SWP_HIDEWINDOW))
					{
						if(!flags.has(SWP_NOMOVE))
						{
							if(wnd->y == wp->y && wnd->y > 0)
							{
								if(wnd->height > wp->cy)
								{
									//wp->y += (wnd->height - wp->cy);
									//wnd->y = wp->y;
									//wnd->height += wnd->height - wp->cy;
									//ctx->UpdateLayered(wnd, true);
								}
								//auto old_height = wnd->height;
								//auto old_y = wnd->width;
								//wp->y = 0;
								//_log.info(L"%d %d", wnd->height, wp->cy);
							}
						}

						auto bz = theme->pow(theme->border.size);
						if(!flags.has(SWP_NOSIZE))
						{
							// remove fixed frame
							wp->cx -= FixedFrame.cx;
							wp->cy -= FixedFrame.cy;

							wp->cx += bz + theme->border.padding.width();
							wp->cy += bz + theme->border.padding.height();

							if((wp->cy + 100) > ctx->_rcMonitor.height())
							{
								wnd->has_scroll = true;
								wp->cy -= ctx->dpi(10);
							}
							/*else
							{
								auto hMenu = (HMENU)::SendMessageW(hWnd, MN_GETHMENU, 0, 0);
								if(::IsMenu(hMenu))
								{
									auto menu = ctx->_menus[hMenu];
									if(menu.popup_height)
									{
										wnd->has_scroll = true;
									}
								}
							}*/
							auto old_height = wnd->height;
							//auto old_y = wnd->y;

							wnd->width = wp->cx;
							wnd->height = wp->cy;

							if(old_height > 0 && old_height != wp->cy)
								ctx->UpdateLayered(wnd, true);

							//if(wp->cy > 450)
								//wp->cy += 16;
							
						}
						else if(!flags.has(SWP_NOMOVE))
						{
							if(ctx->_level.size() == 1)
							{
								wp->x += 1;
								wp->y += 1;
							}
							else
							{
								auto border = &theme->border;
								auto align = theme->layout.popup.align != 0x7F;
								auto prev_window = ctx->_level[ctx->_level.size() - 2];
								auto swap_popup = wp->x < prev_window->x;
								auto fr = FixedFrame.cx / 2;
								auto x = bz + theme->border.padding.width();

								wp->x += swap_popup ? -x : x;

								if(align)
								{
									auto v = theme->layout.popup.align;
									if(swap_popup)
									{
										if(v < 0)
											wp->x += fr + (v * -1);
										else
											wp->x += fr - v;
									}
									else
										wp->x += v;
								}
								else if(swap_popup)
									wp->x += FixedFrame.cx;
								else
								{
									if(border->size >= 3)
										wp->x -= FixedFrame.cx + ((theme->border.padding.right + border->size) / 2);
									else
										wp->x -= fr;
								}
							}

							if(wnd->has_scroll)
							{
								wp->y = (ctx->_rcMonitor.height() - wnd->height) / 2;
							}
							else if((wnd->height + 100) > (ctx->_rcMonitor.height() / 2))
							{
								//wp->y -= 50;
							}
							else if((wp->y + wnd->height) > (ctx->_rcMonitor.height() - ctx->dpi(40)))
							{
								//wp->y -= ctx->dpi(40);
								//wp->y /= 2;
								//(y(237)+ cy(1227)) /2
							}

							//_log.write(L"y(%d), cy(%d)\r\n", wp->y , wnd->height);

							wnd->x = wp->x;
							wnd->y = wp->y;

							if(!wnd->layer.handle)
							{
								// fixme
								if(!ctx->composition)
									//if(!theme->transparent)
										//if(theme->background.effect == 0)
								{
									//int sh = theme->border.radius ? 1 : 0;
									auto border = theme->border.size;
									if(border > 0 || theme->border.radius > 0)
									{
										wnd->Regoin(hWnd, border, border,
													1 + wnd->width - border,
													1 + wnd->height - border,
													theme->border.radius);
									}
								}

								//if(IsWindowVisible(hWnd))
								// load layer when window is not visible
								ctx->CreateLayer(wnd);
							}
						}
					}
					//lret = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
					return lret;
				}
				case WM_WINDOWPOSCHANGED:
				{
					auto wp = reinterpret_cast<WINDOWPOS *>(lParam);
					Flag<uint32_t> flags = wp->flags;
					if(flags.has(SWP_HIDEWINDOW) || flags.has(SWP_SHOWWINDOW))
						;
					else if(!flags.has(SWP_NOZORDER))
					{
						//if(hWnd == ctx->_level.back()->handle)
						{
							/*auto s = ctx->_level.size();
							if(s > 1)
							{
								auto hRoot = ctx->_level[0]->handle;
								::DestroyWindow(hRoot);
							}*/
						}
					}
					return lret;
					//wnd->rect = { wp->x, wp->y, wp->cx, wp->cy };
					//return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
				}
				case WM_PAINT:
				{
					lret = defSubclassProc();
					//auto_gdi<HBRUSH> _hblack(CreateSolidBrush(0x000000));
					//::FillRect(wnd->hdc, &wnd->rect, _hblack.get());

					if(wnd->has_scroll)
					{
						auto_gdi<HBRUSH> hbblack(CreateSolidBrush(0x000000));

						int h = ctx->dpi(14);
						Rect rc = { 0, 0, wnd->width, h };
						::FillRect(wnd->hdc, rc, hbblack.get());
						
						rc = { 0, wnd->height - h, wnd->width, wnd->height };
						::FillRect(wnd->hdc, rc, hbblack.get());
						
						auto txtfmt = DT_NOCLIP | DT_SINGLELINE | DT_VCENTER| DT_CENTER;

						rc = { 0, 0, wnd->width, h };
						ctx->draw_string(wnd->hdc, ctx->font.icon, &rc, ctx->_theme.symbols.chevron.nor, L"\uE009", 1, txtfmt);

						rc = { 0, wnd->height - h, wnd->width, wnd->height };
						ctx->draw_string(wnd->hdc, ctx->font.icon, &rc, ctx->_theme.symbols.chevron.nor, L"\uE00A", 1, txtfmt);

						//::ExcludeClipRect(wnd->hdc, 0, 0, wnd->width, 20);
						//::ExcludeClipRect(wnd->hdc, 0, wnd->height - 20, wnd->width, wnd->height);
					}

					// exlude menu item rectangle to prevent drawing by windows after us
					//dc.exclude_clip_rect(*rc);
					return lret;
				}
				case WM_NCPAINT:
					//lret = defSubclassProc();
					return lret;
				case WM_ERASEBKGND:
				{
					if(++ixi == 0)
					{
						Rect r = hWnd;
						D2D d2d;
						
						d2d.begin(wnd->hdc, { 0, 0, r.width(), r.height() });
						
						//auto z = (float)theme->border.size*2;
						D2D1_RECT_F rect = { 0.0f, 0.0f, float(r.width()), float(r.height()) };

						//d2d.render->SetDpi(96.f, 96.f);
						d2d.render->SetTransform(D2D1::Matrix3x2F::Identity());
						//d2d.render->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.f));
						if(theme->border.radius > 0)
						{
							d2d.render->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
							auto radius = float(theme->border.radius);
							d2d.brush->SetColor(theme->background.color);
							d2d.render->FillRoundedRectangle({ rect, radius, radius }, d2d.brush);
						}
						else
						{
							d2d.brush->SetColor(theme->background.color);
							d2d.render->FillRectangle(rect, d2d.brush);
						}
						d2d.end(true);
					}
					lret = TRUE;
					//lret = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
					return lret;
				}
				case WM_SETCURSOR:
				{
					if(ctx->hCursor)
					{
						::SetCursor(ctx->hCursor);
						return TRUE;
					}
					break;
				}
				case MN_SELECTITEM:
				{
					auto cmdItem = static_cast<uint32_t>(wParam);
					if(cmdItem == MFMWFP_NOITEM)
					{
						if(lParam > 0)
						{
							//InvalidateRect(hWnd, &__o[lParam].rc, 0);

							//_log.info(L"### %d %x", cmdItem, lParam);
						}
					}
					else {
						__sel = -1;
					}

					ctx->current.hWnd = hWnd;
					ctx->current.selectitem_pos = cmdItem;
					lret = defSubclassProc();
				//cmdLast
					if(cmdItem != MFMWFP_NOITEM)
					{
						if(!wnd->hdc)
						wnd->hdc = GetWindowDC(hWnd);
						if(Keyboard::IsKeyDown(VK_RBUTTON))
						{
							//SendMessage(hWnd, MN_BUTTONDOWN, wParam, 0);
							//	EnableMenuItem(wnd->hMenu, cmdItem, true);
						}
						//if(ctx->current.select_previtem)
						//::InvalidateRect(hWnd, 0, 1);
					}
					//	wParam == MFMWFP_NOITEM
						/*pMenuBuilder->SetActiveMenu((HMENU)SendMessage(pcwps->hwnd, MN_GETMENU, 0, 0));
					// Check to see if it's the up or down arrow and if so...
					if ((pcwps->wParam == MENU_DOWN) || (pcwps->wParam == MENU_UP))
					  {
						// ...send a MN_BUTTONDOWN message to the menu window with that
						// arrow
						SendMessage(pcwps->hwnd, MN_BUTTONDOWN, pcwps->wParam, 0);
						buttonDown = true;
					  }
					else if (buttonDown)
					  {
						// ...or else send a MN_BUTTONUP message or WM_CONTEXTMENU is
						// not sent in the case of a WM_RBUTTONDOWN
						SendMessage(pcwps->hwnd, MN_BUTTONUP, pcwps->wParam, 0);
						buttonDown = false;
					  }
					*/
					return lret;
				}
				case WM_PRINT:
				case WM_PRINTCLIENT: //mouse
					//lret = defSubclassProc();
					return lret;
					/*
					 * wParam - the item to select. Must be a valid index or MFMWFP_NOITEM
					 * Returns the item flags of the wParam (0 if failure)
					 */
					 //case WM_SIZING:
	 //	RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT);
				case WM_KEYUP:
				case WM_SYSKEYUP:
				{
					//auto vkCode = LOWORD(wParam); // virtual-key code
					return defSubclassProc();
				}
				case WM_SYSKEYDOWN:
				case WM_KEYDOWN:
				{
					/*auto vkCode = LOWORD(wParam); // virtual-key code
					_log.info(L"%d", vkCode);
					if(vkCode == VK_HOME)
						SendMessageW(hWnd, MN_SELECTITEM, 0, 0);
					else if(vkCode == VK_END)
						SendMessageW(hWnd, MN_SELECTITEM, -1, 0);
					*/
					return defSubclassProc();
				}
				/*
				 * wParam is position (index) of item the button was clicked on.
				 * Must be a valid index or MFMWFP_NOITEM
				 */
				 /*
		  * Cancels all menus, unselects everything, destroys windows, and cleans
		  * everything up for this hierarchy. wParam is the command to send and
		  * lParam says if it is valid or not.
		  */
				case MN_OPENHIERARCHY:
				{
					lret = defSubclassProc();
					return lret;
				}
				case MN_CLOSEHIERARCHY:
					break;
				case MN_SHOWPOPUPWINDOW:
					break;
				case MN_CANCELMENUS:
					break;
					/*
					*wParam is position(index) of item the button was up clicked on.
					*/
				case MN_BUTTONDOWN:
				{
					if(lParam == 0 && ctx)
					{
						ctx->mouse_button = (::GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0x8000 ? VK_RBUTTON : VK_LBUTTON;
					}
					break;
				}
				case MN_BUTTONUP:
				{
					//auto hMenu = (HMENU)::SendMessageW(hWnd, MN_GETHMENU, 0, 0);
					//wParam is position(index) of item the button was up clicked on.
					MENUITEMINFOW mii = { sizeof(mii), MIIM_TYPE | MIIM_ID | MIIM_SUBMENU | MIIM_STATE };
					if(::GetMenuItemInfoW(wnd->hMenu/*_this->current.hMenu*/, (uint32_t)wParam, MF_BYPOSITION, &mii))
					{
						if(mii.hSubMenu == nullptr && !(mii.fState & MFS_DISABLED) && !(mii.fType & MFT_SEPARATOR))
						{
							ctx->selectid = mii.wID;
							ctx->current.selectid = mii.wID;
						}
					}

					int pvParam{};
					::SystemParametersInfoW(SPI_GETSELECTIONFADE, 0, &pvParam, 0);
					if(!pvParam)
					{
						// Fade out animation is disabled system-wide
						break;
					}
					// We need to prevent the system default menu fade out animation
					// and begin a re-implemented one
					// 
					// Windows does not show animation if the selection was done
					// with keyboard (i.e. Enter)

					::SystemParametersInfoW(SPI_SETSELECTIONFADE, 0, FALSE, 0);
					lret = defSubclassProc();
					::SystemParametersInfoW(SPI_SETSELECTIONFADE, 0, (PVOID)TRUE, 0);
					return lret;
				}
				break;
				case MN_DBLCLK:
					//return FALSE;
					break;
				case MN_FINDMENUWINDOWFROMPOINT://mouse
				{
					lret = defSubclassProc();
					//auto h = (HWND)ret;
					//m_log->info(L"%s", Window(h).classn_name().c_str());
					return lret;
				}
				case WM_MOUSEHWHEEL:
				case WM_MOUSEWHEEL:
				{
					int delta = GET_WHEEL_DELTA_WPARAM(wParam);
					if(delta > 0)
					{
						//Mouse Wheel Up
					}
					else
					{
						//Mouse Wheel Down
					}

					lret = defSubclassProc();
					//auto h = (HWND)ret;
					//m_log->info(L"%s", Window(h).classn_name().c_str());
					return lret;
				}
				case WM_CHAR:
				case WM_SYSCHAR:
				case WM_MENUCHAR:
					break;
				case WM_TIMER:
				{
					switch(wParam)
					{
						case IDSYS_MNSHOW:
							/*
							 * Open the window and kill the show timer.
							 *
							 * Cancel any toggle state we might have. We don't
							 * want to dismiss this on button up if shown from
							 * button down.
							 */
							break;

						case IDSYS_MNHIDE:
							break;

						case IDSYS_MNUP:
							break;
						case IDSYS_MNDOWN:
							wnd->scrolled = true;
							//if(pMenuState->fButtonDown) {
							//	xxxMNDoScroll(ppopupmenu, (UINT)wParam, FALSE);
							//}
							//else {
							//	_KillTimer(pwnd, (UINT)wParam);
							break;
						default:
							//_log.info(L"%x", wParam);
							break;
					}

					break;
				}
				case MN_SETHMENU:
					break;
					//mouse
				case MN_MOUSEMOVE:
					break;
				case WM_MOUSEACTIVATE:
				case WM_NCMOUSEMOVE:
				case WM_MOUSELEAVE:
				case WM_DEVICECHANGE:
				case WM_CREATE:
					break;
				case WM_NCHITTEST:
				case MN_SETTIMERTOOPENHIERARCHY:
				case MN_ENDMENU:
				case WM_UAHDESTROYWINDOW:
				/*case WM_UAHINITMENUPOPUP:
					//	return OnUAHDrawPopupMenuProc(hWnd, uMsg, wParam, lParam);
				case WM_UAHNCPAINTMENUPOPUP:
				case WM_UAHDRAWMENU:
					break;
				case WM_NCUAHDRAWFRAME:
					break;
				case WM_UAHMEASUREMENUITEM:
					break;
				case WM_UAHDRAWMENUITEM:*/
					break;
			/*	case WM_UAHINITMENUPOPUP:
				case WM_UAHDRAWMENU:
				case WM_UAHDRAWMENUITEM:
				case WM_UAHMEASUREMENUITEM:
				case WM_UAHNCPAINTMENUPOPUP:
						_log.info(L"%0.4x", uMsg);
						return 0;
					break;*/
					// not handle
				//	case WM_SHOWWINDOW:
				//		beep;
				default:
					break;
			}
			//_log.info(L"0x%0.4x\t%s", uMsg, msg_map[uMsg]);
			return defSubclassProc();
		}


		LRESULT __stdcall ContextMenu::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
		{
			int alt = static_cast<int> ((lParam >> 29) & 1);
			int key_down = static_cast<int> (!((lParam >> 30) & 1));
			int key_up = static_cast<int> (((lParam >> 31) & 1));
			int scancode = static_cast<int> ((lParam >> 16) & 0xFF);

			// Code < 0 is windows telling us 'don't process this message'.
			if(nCode != 0)  // do not process message 
				goto skip;

			//::GetWindow(GetActiveWindow(), GW_ENABLEDPOPUP))

			// If alt-F12 is pressed, release hook
			if(alt && key_down && (scancode == 88))
			{
			}

			if(key_up)
			{
				if(wParam == VK_SNAPSHOT)
				{
					::EnumThreadWindows(::GetCurrentThreadId(), [](HWND hWnd, LPARAM)->BOOL
					{
						if(auto ctx = ContextMenu::Prop::Get(hWnd); ctx)
						{
							if(Window::IsPopupMenu(hWnd))
							{
								ctx->screenshot();
								return FALSE;
							}
						}
						return TRUE;
					}, 0);
				}
			}

		skip:
			// NOTE: The first parameter is always ignored
			return WindowsHook::CallNext(nullptr, nCode, wParam, lParam);
		}

		void __stdcall ContextMenu::WinEventProc([[maybe_unused]] HWINEVENTHOOK hWinEventHook,
												 DWORD dwEvent, HWND hWnd, LONG idObject,
												 [[maybe_unused]] LONG idChild, [[maybe_unused]] DWORD idEventThread, DWORD)
		{
			if(dwEvent != EVENT_OBJECT_CREATE && dwEvent != EVENT_OBJECT_SHOW)
				return;

			if(idObject != OBJID_WINDOW || !::IsWindow(hWnd))
				return;
			//idChild == INDEXID_CONTAINER
			if(Window::IsPopupMenu(hWnd))
			{
				if(dwEvent == EVENT_OBJECT_CREATE)
				{
					if(auto ctx = HookMap[hWinEventHook]; ctx)
						ctx->OnMenuCreate(hWnd);
				}
				else if(dwEvent == EVENT_OBJECT_SHOW)
				{
					if(auto ctx = Prop::Get(hWnd); ctx)
						ctx->OnMenuShow(hWnd);
				}
			}
		}


#pragma endregion

		//WindowProc
		LRESULT __stdcall ContextMenu::WindowSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
														  UINT_PTR uIdSubclass,
														  DWORD_PTR dwRefData)
		{
			if(CONTEXTMENUSUBCLASS != uIdSubclass)
				return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);

			__try
			{
				auto ctx = reinterpret_cast<ContextMenu *>(dwRefData);
				if(ctx == nullptr)
				{
					ctx = Prop::Get(hWnd);
					if(ctx == nullptr)
						return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
				}

				ctx->msg.hWnd = hWnd;
				ctx->msg.uMsg = uMsg;
				ctx->msg.wParam = wParam;
				ctx->msg.lParam = lParam;

				switch(uMsg)
				{
					case WM_ENTERMENULOOP:
					{
						//_log.info(L"WM_ENTERMENULOOP");
						if(wParam == TRUE)
							return ctx->OnStart();
						break;
					}
					case WM_EXITMENULOOP:
					{
						if(wParam == TRUE)
							return ctx->OnEnd();
						break;
					}
					case WM_INITMENU:
					{
						//_log.info(L"WM_INITMENU");
						if(auto hMenu = reinterpret_cast<HMENU>(wParam); hMenu)
							return ctx->OnInitMenu(hMenu);
						break;
					}
					/*
					lParam
						The low-order word specifies the zero-based relative position of the menu item that opens the drop-down menu or submenu.
						The high-order word indicates whether the drop-down menu is the window menu. If the menu is the window menu, this parameter is TRUE; otherwise, it is FALSE.
					*/
					case WM_INITMENUPOPUP:
					{
						//_log.info(L"WM_INITMENUPOPUP");
						auto lp = HIWORD(lParam);
					//	if(lParam != 0xFFFFFFFF)
					//		_log.info(L"WM_INITMENUPOPUP %d %d %d", HIWORD(lParam), LOWORD(lParam), lParam);
						if(lp == FALSE)
						{
							//MENUINFO info{0};
							//info.cbSize = sizeof(info);
							//info.fMask = MIM_STYLE | MIM_MAXHEIGHT| MIM_MENUDATA;
							//GetMenuInfo(reinterpret_cast<HMENU>(wParam), &info);
							//_log.info(L"WM_INITMENUPOPUP %d 0x%08x 0x%08x", GetMenuItemCount(reinterpret_cast<HMENU>(wParam)), info.dwStyle, info.dwMenuData);
							if(auto hMenu = reinterpret_cast<HMENU>(wParam); hMenu)
								return ctx->OnInitMenuPopup(hMenu, LOWORD(lParam));
						}
						else if(lp == 100) // preinitmenupopup
						{
							lParam = MAKELPARAM(LOWORD(lParam), lp);
						}
						break;
					}
					//lParam
					//	The high - order word identifies the menu that was destroyed.Currently, this parameter can only be MF_SYSMENU(the window menu).
					case WM_UNINITMENUPOPUP:
					{
						if(HIWORD(lParam) == FALSE)
						{
							if(auto hMenu = reinterpret_cast<HMENU>(wParam); hMenu)
								return ctx->OnUninitMenuPopup(hMenu);
							return 0;
						}
						break;
					}
					case WM_MEASUREITEM:
					{
						//_log.info(L"WM_MEASUREITEM %x", wParam);
						auto mi = reinterpret_cast<MEASUREITEMSTRUCT *>(lParam);
						//Only process if this notification is actually for a menu.
						if(mi->CtlType == ODT_MENU/* && wParam == 0*/)
							return ctx->OnMeasureItem(mi);
						break;
					}
					case WM_DRAWITEM:
					{
						//_log.info(L"WM_DRAWITEM");
						auto di = reinterpret_cast<DRAWITEMSTRUCT *>(lParam);
						//Only process if this notification is actually for a menu.
						if(di->CtlType == ODT_MENU/* && wParam == 0*/)
							return ctx->OnDrawItem(di);
						break;
					}
					case WM_MENUSELECT:
						return ctx->OnMenuSelect(reinterpret_cast<HMENU>(lParam), LOWORD(wParam), HIWORD(wParam));
					case WM_ENTERIDLE:
						if(wParam == MSGF_MENU)
						{
							//_log.info(L"WM_ENTERIDLE %x %x", wParam, lParam);
							/*if(!ctx->HWNDMenu)
							{
								ctx->HWNDMenu = reinterpret_cast<HWND>(lParam);
							}*/
						}
						break;
					case WM_TIMER:
						return ctx->OnTimer(static_cast<UINT_PTR>(wParam), reinterpret_cast<TIMERPROC>(lParam));
					case WM_MOUSEMOVE:
					{
						/*if(ctx->_level.size() == 1)
						{
						//	_log.info(L"**** WM_MOUSEMOVE %x %x", WM_MOUSEMOVE, wParam, lParam);
						//	PostMessage(ctx->_level[0]->handle, WM_MOUSEMOVE, wParam, lParam);
						}*/
						break;
					}
					// handled by DefWindowProc
					case WM_UAHINITMENUPOPUP:
					{
						//auto uahmenu = (UAHMENU *)lParam;
						//uahmenu->dwFlags = 0x4000401;WS_EX_UISTATEACTIVE
						//_log.info(L"WM_UAHINITMENUPOPUP %d 0x%08x 0x%08x", GetMenuItemCount(uahmenu->hmenu), uahmenu->dwFlags, wParam);
						//if(auto hMenu = reinterpret_cast<HMENU>(wParam); hMenu)
						//return ctx->OnInitMenuPopup(uahmenu->hmenu, 0);
						break;
					}
					case WM_UAHMEASUREMENUITEM:
					{
						//_log.info(L"WM_UAHMEASUREMENUITEM %08x", wParam);
						/*auto mi = reinterpret_cast<UAHMEASUREMENUITEM *>(lParam);
						//Only process if this notification is actually for a menu.
						if(mi->mis.CtlType == ODT_MENU)
							return ctx->OnMeasureItem(&mi->mis);*/
						break;
					}
					case WM_UAHDRAWMENU:
					case WM_UAHDRAWMENUITEM:
					case WM_UAHNCPAINTMENUPOPUP:
						//_log.info(L"%0.4x", uMsg);
						
						break;
					case WM_CAPTURECHANGED:
						break;
					/*case 0x04a3:
					case 0x04ad:
					case 0x04ae:
					case 0x04af:
					case 0x04b0:
						return 0;*/
				//	default:
						//_log.info(L"%0.4x %s", uMsg, msg_map[uMsg]); break;
						/*	case WM_MENUCHAR:
							case WM_NEXTMENU:
							case WM_MENUCOMMAND:
							case WM_COMMAND:
							case WM_SYSCOMMAND:
							// modify async menu item
							case WM_ERASEBKGND:
							case WM_PRINTCLIENT:
							case WM_UAHDRAWMENU:
							case WM_UAHDRAWMENUITEM:
							case WM_UAHINITMENUPOPUP:
							case WM_UAHMEASUREMENUITEM:
							case WM_UAHNCPAINTMENUPOPUP
								break;
							*/
							//_log.info(L"msg = 0x%0.4x, wParam = 0x%0.8x, lParam = 0x%0.8x, WindProc, %s", uMsg, wParam, lParam, msg_map[uMsg]);
				}
				//_log.info(L"msg = 0x%04x, wParam = 0x%08x, lParam = 0x%08x, WindProc, %s", uMsg, wParam, lParam, msg_map[uMsg]);
			}
			__except(EXCEPTION_EXECUTE_HANDLER) {}
			return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
		}
	}
}
#pragma endregion

LRESULT LayerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_WINDOWPOSCHANGED)
	{
		auto wp = reinterpret_cast<WINDOWPOS *>(lParam);
		Flag<uint32_t> flags = wp->flags;
		if(flags.has(SWP_HIDEWINDOW))
		{
		}
		else if(flags.has(SWP_SHOWWINDOW))
		{
		}
		else
		{
			if(!flags.has(SWP_NOSIZE) || flags.has(SWP_FRAMECHANGED)) {
				//UpdateShadowShape(pwnd);
			}
			else if(!flags.has(SWP_NOMOVE)) {
				//MoveShadow(pwnd);
			}

			if(!flags.has(SWP_NOZORDER))
			{
				::SetWindowPos(hWnd, nullptr,
							   0, 0, 0, 0,
							   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
							   SWP_NOZORDER | SWP_NOOWNERZORDER |
							   SWP_NOACTIVATE | SWP_SHOWWINDOW);
			}
		}
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}