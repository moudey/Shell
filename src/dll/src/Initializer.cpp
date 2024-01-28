
#include <pch.h>
#include "Include\Hooker.h"
#include "Include/ContextMenu.h"

namespace Nilesoft
{
	namespace Shell
	{
		const Windows::Version *os = &Windows::Version::Instance();

		bool Initializer::init(HINSTANCE hInstance)
		{
			HInstance = hInstance;
			instance = this;

			try
			{
				com_initializer.initialize();

				application.Name = APP_NAME;
				application.FileName = APP_FILENAME_TITLE;
				application.Version = APP_VERSION;

				application.Path = Path::Module(hInstance).move();

				application.Root = Path::GetRoot(application.Path);
				application.Dirctory = Path::Parent(application.Path).move();

				// Nilesoft Shell Script (NSS)
				// Nilesoft Shell Object (NSO)
				application.ConfigPortable = Path::Join(application.Dirctory, L"\\shell.nss").move();
				application.Config = application.ConfigPortable;

				auto len = application.Path.length();
				if(len > 4)
				{
					string path = application.Path.substr(0, len - 3).move();
					application.Manager = (path + L"exe").move();
				}

				auto hwnd_foreground = ::GetForegroundWindow();
				if(os->IsWindowsVersionOrGreater(10, 0, 14393))
					dpi = DLL::User32<uint32_t>("GetDpiForWindow", hwnd_foreground);
				else
					dpi = DC(hwnd_foreground).GetDeviceCapsY();

				//dpi = ::GetDpiForWindow(::GetForegroundWindow());//win10 version 1607 //10.0.14393
				is_elevated = Security::Elevation::IsElevated();
				//WIC::init();
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
				return false;
			}
			return true;
		}

		Initializer::~Initializer()
		{
			try
			{
				//hooker.destroy();
				this->uninit();
				WIC::release();
				D2D::destroy_Factory();
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
		}

		bool Initializer::init()
		{
			try
			{
				if(!cache)
				{
					cache = new CACHE;

					// Determine the required bitmap size.
					cache->glyph.name = FontCache::Default;

					auto cur_dir = Path::CurrentDirectory();
					Path::CurrentDirectory(application.Dirctory);
					Parser parser;

					load_mui();

					cache->Packages.load();

					if(!parser.Load())
					{
						Status.Error = true;
						uninit();
						Path::CurrentDirectory(cur_dir);
						// Release ownership of the critical section.
						return false;
					}

					// load images cache
					for(auto &id : MAP_MUID)
					{
						auto uid = &MAP_MUID[id.first];
						for(auto &si : cache->images)
						{
							if(si.equals(uid->id))
							{
								uid->image = si.value;
								break;
							}
						}
					}

					Path::CurrentDirectory(cur_dir);
					cache->fonts.init(HInstance);
					ContextMenu::FontNotFound = false;
				}

				LastError = {};
				Status.Loaded = true;
				Status.Error = false;
				Status.Refresh = false;
				Status.Disabled = false;

				//::SHChange_Notify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
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

		bool Initializer::uninit()
		{
			__try
			{
				if(cache)
				{
					delete cache;
					cache = nullptr;
				}

				MAP_MUID.clear();
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}

			Status.Loaded = false;
			Status.Refresh = false;
			//Status->Unload = false;
			return true;
		}

		bool Initializer::query(int ch)
		{
			__try
			{
				if(Status.Disabled)
				{
					uninit();
					return false;
				}

				if(Status.Error && ch < 2)
					return false;

				if(Status.Refresh)
					uninit();
				
				return init();
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return false;
		}

		//determine config file changed
		bool Initializer::config_has_changed()
		{
			bool res = false;
			try
			{
				auto_handle hConfig = ::CreateFileW(application.Config, GENERIC_READ, FILE_SHARE_READ,
													nullptr, OPEN_EXISTING, 0, nullptr);
				if(hConfig)
				{
					FILETIME ft_lastWriteTime{};
					auto last_write_time = reinterpret_cast<uintptr_t *>(&ft_lastWriteTime);
					if(::GetFileTime(hConfig, nullptr, nullptr, &ft_lastWriteTime) && *last_write_time != 0)
					{
						if(*last_write_time != _last_write_time)
						{
							res = _last_write_time > 0;
							_last_write_time = *last_write_time;
						}
					}
				}
			}
			catch(...)
			{
#ifdef _DEBUG
				Logger::Exception(__func__);
#endif
			}
			return res;
		}

		bool Initializer::has_error(bool detect_changes)
		{
			if(Status.Error)
			{
				return detect_changes ? !config_has_changed() : true;
			}
			return false;
		}

		/*
		static bool isWhiteSpace(wchar_t ch) {
			// this is surprisingly faster than the equivalent if statement
			switch(ch) {
				case L'\u0009': case L'\u000A': case L'\u000B': case L'\u000C': case L'\u000D':
				case L'\u0020': case L'\u0085': case L'\u00A0': case L'\u1680': case L'\u2000':
				case L'\u2001': case L'\u2002': case L'\u2003': case L'\u2004': case L'\u2005':
				case L'\u2006': case L'\u2007': case L'\u2008': case L'\u2009': case L'\u200A':
				case L'\u2028': case L'\u2029': case L'\u202F': case L'\u205F': case L'\u3000':
					return true;
				default:
					return false;
			}
		}

		static string TrimAllWithLexerLoop(const string &s) 
		{
			auto length = s.length();
			auto buffer = s;
			size_t dstIdx = 0;
			for(int index = 0; index < s.length(); index++) 
			{
				auto ch = s[index];
				switch(ch) 
				{
					case L'\u0009': case L'\u000A': case L'\u000B': case L'\u000C': case L'\u000D':
					case L'\u0020': case L'\u0085': case L'\u00A0': case L'\u1680': case L'\u2000':
					case L'\u2001': case L'\u2002': case L'\u2003': case L'\u2004': case L'\u2005':
					case L'\u2006': case L'\u2007': case L'\u2008': case L'\u2009': case L'\u200A':
					case L'\u2028': case L'\u2029': case L'\u202F': case L'\u205F': case L'\u3000':
						length--;
						continue;
					default:
						break;
				}
				buffer[dstIdx++] = ch;
			}
			buffer.release(length);
			return buffer.move();
		}


		static string TrimControlCharacter(const string &s)
		{
			auto length = s.length();
			auto buffer = s;
			size_t dstIdx = 0;
			for(int i = 0; i < s.length(); i++)
			{
				auto c = s[i];
				if(std::iswcntrl(c))
					length--;
				else
					buffer[dstIdx++] = c;
			}
			buffer.release(length);
			return buffer.move();
		}
void remove_control_characters(std::string& s) {
	s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return std::iscntrl(c); }), s.end());
}
		*/
		/*
		static uint32_t hashing(const string &value, uint32_t hash = 0)
		{
			if(value.empty())
				return 0;

			Hash h(hash == 0 ? Hash::offset_basis : hash);
			wchar_t c = 0;
			string tmp = value;
			//tmp.trim_end(L'.').trim_cntrl();

			for(auto i = 0u; i < tmp.length(); i++)
			{
				c = tmp[i];
				if(c == 0 or c == L'\t' or c == L'\b')
					break;
				else if(c == L'&')
				{
					if(tmp[i + 1] == L'&')
						i++;
					else continue;
				}

				h.hash(c == L' ' ? L'_' : c);
			}
			return h;
		}
		*/
		void Initializer::load_mui()
		{
			/*
			string nn0 = L"Очистить &корзину";
			string nn, xx;
			auto hModule = ::LoadLibraryExW(L"C:\\Windows\\System32\\ru-RU\\shell32.dll.mui", nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
			auto len = ::LoadStringW(hModule, 10564, nn.buffer(MAX_PATH), MAX_PATH);
			if(len > 0)
			{
				nn.release(len);
				string dd = nn0;
				auto h = MenuItemInfo::normalize(nn0, &xx);
				MBF(L"%X\n%d\n%d", h, xx.length(), dd.trim_cntrl().length());
			}
			*/

			/*auto hh = ::LoadLibraryExW(L"D:\\mui\\w11\\ru-RU\\AppResolver.dll.mui", nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
			if(hh)
			{
				string title;
				auto len = ::LoadString(hh, 8505, title.buffer(MAX_PATH), MAX_PATH);
				if(len > 0)
				{
					string ss = L"Перейти к расположению файла"; // 30064DE5
					//string m;
					//for(auto i = 0u; i < ss.length(); i++) m.append_format(L"%0.6X ", ss[i]);
					
					title.release(len);
					MB(title);
				//	auto hash = hashing(title);
				//	auto hash2 = hashing(ss);
				//	MBF(L"30064DE5\n%X\n%X\n%d", title);
					//	Logger::Log(L"%d\t\t%X\t[%s]\t\t[%s]\n", item->res_id, item->hash, item->title.c_str(), item->title_normalize.c_str());
					//get_image(item);
				}
			}*/
			
			/*auto get_image = [=](IDMUI *ui)
			{
				for(auto &i : cache->images)
				{
					//auto id = ui->hash == 0 ? ui->id : ui->hash;
					if(i.first == ui->id)
					{
						ui->image = i.second;
						break;
					}
				}
			};*/

			auto ldstr = [&, this](const wchar_t *dll,
								std::initializer_list<MUID> list)
			{
				auto hModule = ::GetModuleHandleW(dll);
				bool load_as_dynamic = false;
				if(!hModule)
				{
					hModule = ::LoadLibraryExW(dll, nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
					load_as_dynamic = true;
				}

				if(hModule)
				{
					for(auto &it : list)
					{
						auto uid = &MAP_MUID[it.id];
						if(uid->title.empty())
						{
							*uid = it;
							auto len = ::LoadStringW(hModule, uid->res_id, uid->title.buffer(MAX_PATH), MAX_PATH);
							if(len > 0)
							{
								uid->title.release(len);
								uid->set_hash();
								//get_image(uid);
							}
						}
					}
				}
				else
				{
					for(auto &it : list)
					{
						auto uid = &MAP_MUID[it.id];
						*uid = it;
						//get_image(uid);
					}
				}

				if(hModule && load_as_dynamic)
					::FreeLibrary(hModule);
			};

			auto parse_menu = [&](HMODULE hModule, uint32_t menu_id,
								 std::initializer_list<MUID> list)
			{
				if(hModule)
				{
					auto hMenu = ::LoadMenuW(hModule, MAKEINTRESOURCEW(menu_id));
					if(hMenu)
					{
						for(auto &it : list)
						{
							auto uid = &MAP_MUID[it.id];
							if(uid->title.empty())
							{
								*uid = it;
								MENUITEMINFOW mii{ sizeof(mii), MIIM_STRING };
								mii.dwTypeData = uid->title.buffer(MAX_PATH);
								mii.cch = MAX_PATH;
								if(::GetMenuItemInfoW(hMenu, uid->res_id, false, &mii) && mii.cch > 0)
								{
									uid->title.release(mii.cch);
									uid->set_hash();
									//get_image(uid);
								}
							}
						}
						::DestroyMenu(hMenu);
					}
					else for(auto &it : list)
					{
						auto uid = &MAP_MUID[it.id];
						*uid = it;
						//get_image(uid);
					}
				}
				else for(auto &it : list)
				{
					auto uid = &MAP_MUID[it.id];
					*uid = it;
					//get_image(uid);
				}
			};

			auto ldmenu = [&](const wchar_t *dll, uint32_t menu_id,
								 std::initializer_list<MUID> list)
			{
				HMODULE lib{}, hModule = ::GetModuleHandleW(dll);
				if(!hModule)
				{
					lib = ::LoadLibraryExW(dll, nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
					hModule = lib;
				}

				parse_menu(hModule, menu_id, list);

				if(lib) ::FreeLibrary(lib);
			};

			struct menu_res {
				uint32_t id{};
				std::initializer_list<MUID> items;
			};

			auto ldmenu2 = [&](const wchar_t *dll, std::initializer_list<menu_res> list)
			{
				HMODULE lib{}, hModule = ::GetModuleHandleW(dll);
				if(!hModule)
				{
					lib = ::LoadLibraryExW(dll, nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
					hModule = lib;
				}

				if(hModule)
				{
					for(auto &it : list)
					{
						parse_menu(hModule, it.id, it.items);
					}
				}
				else for(auto &it : list)
				{
					for(auto &ui : it.items)
					{
						auto uid = &MAP_MUID[ui.id];
						if(uid->title.empty())
						{
							*uid = ui;
						//get_image(uid);
						}
					}
				}

				if(lib) ::FreeLibrary(lib);
			};

			auto add_uid = [](std::initializer_list<MUID> list) {
				for(auto &&uid : list)
					MAP_MUID.emplace(uid.id, uid);
			};

			auto isw11 = [](uint32_t val1, uint32_t val2)->uint32_t { return os->IsWindows11OrGreater() ? val1 : val2; };
			auto isw8 = [](uint32_t val1, uint32_t val2)->uint32_t { return os->IsWindows8OrGreater() ? val1 : val2; };

			auto user32 = L"user32.dll";
			auto shell32 = L"shell32.dll";
			auto explorerframe = L"explorerframe.dll";
			auto explorer = L"explorer.exe";
			
			ldstr(L"acppage.dll", {
				{IDENT_ID_TROUBLESHOOT_COMPATIBILITY, 2022},
			 });

			if(os->Major >= 10 && os->Build > 10240)
			{
				ldstr(L"ntshrui.dll", {
				   {IDENT_ID_GIVE_ACCESS_TO, 103},
				   {IDENT_ID_SHARE_WITH, 104},
				   {IDENT_ID_SHARE, 107},
				});
			}
			else 
			{
				ldstr(L"ntshrui.dll", {
				   {IDENT_ID_SHARE_WITH, 103},
				   {IDENT_ID_SHARE, 107},
				});

				add_uid({
					{ IDENT_ID_GIVE_ACCESS_TO, 0}
				});
			}

			ldstr(L"appresolver.dll", {
				{ IDENT_ID_OPEN, 8550 },
				{ IDENT_ID_RUN_AS_ADMINISTRATOR, 8551},
				{IDENT_ID_RUN_AS_DIFFERENT_USER, 8558},
				//{ IDENT_ID_OPEN_FILE_LOCATION, 8552, { L'\uE0E8', 0 } },
			});

			ldstr(L"windows.storage.dll", {
				{IDENT_ID_OPEN_IN_NEW_WINDOW, 8517},
				{IDENT_ID_OPEN_IN_NEW_TAB, 8519},
			});

			ldmenu2(shell32, 
			{ 
				{208, {
					{IDENT_ID_OPEN_WITH, 1},
					//{ IDENT_ID_CANCEL, 0 },
				}},
				{	210, {
					{ IDENT_ID_CREATE_SHORTCUT, 16},
					{ IDENT_ID_CUT, 24},
					{ IDENT_ID_COPY, 25},
					{ IDENT_ID_PASTE, 26},
					{ IDENT_ID_RENAME, 18},
					{ IDENT_ID_DELETE, 17},
				}},
				{	211, {
					{ IDENT_ID_PROPERTIES, 19 } 
				}},
				{	215, {
					{ IDENT_ID_ARRANGE_BY, 28678},
					{ IDENT_ID_VIEW, 28674},
					{ IDENT_ID_GROUP_BY, 28676},
					{ IDENT_ID_SORT_BY, 28673},
					{ IDENT_ID_REFRESH, 28931},
					{ IDENT_ID_CUSTOMIZE_THIS_FOLDER, 28722},
					{ IDENT_ID_PASTE_SHORTCUT, 28700},
					{ IDENT_ID_UNDO, 28699},
					{ IDENT_ID_REDO, 28704}
				}},
				/*{216, {
				    { IDENT_ID_PROPERTIES, 28691},
					{ IDENT_ID_COPY_TO_FOLDER, 28702},
					{ IDENT_ID_MOVE_TO_FOLDER, 28703},
				}},*/
				{217, { 
					{ IDENT_ID_EXTRA_LARGE_ICONS, 28749},
					{ IDENT_ID_LARGE_ICONS, 28751},
					{ IDENT_ID_MEDIUM_ICONS, 28750},
					{ IDENT_ID_SMALL_ICONS, 28752},
					{ IDENT_ID_LIST, 28753},
					{ IDENT_ID_DETAILS, 28747},
					{ IDENT_ID_TILES, 28748},
					{ IDENT_ID_CONTENT, 28754},
					{ IDENT_ID_EXPAND_ALL_GROUPS , 28755},
					{ IDENT_ID_COLLAPSE_ALL_GROUPS , 28756},
					{ IDENT_ID_COPY_TO_FOLDER, 28702},
					{ IDENT_ID_MOVE_TO_FOLDER, 28703},
					{ IDENT_ID_SELECT_ALL, isw11(28707, 28705)},
					{ IDENT_ID_INVERT_SELECTION, isw11(28708,28706)}
				}},
				{	218, {
					{ IDENT_ID_SHOW_DESKTOP_ICONS, 29698}
				}},
				{	223, {
					{IDENT_ID_SEND_TO, 79},
				}},
				{	225, {
					{ IDENT_ID_MAP_NETWORK_DRIVE, 14},
					{IDENT_ID_DISCONNECT_NETWORK_DRIVE, 9},
				}}, 
				{	226, {
					{ IDENT_ID_RESTORE, 139},
				}},
				{	228, {
					{ IDENT_ID_ADD_A_NETWORK_LOCATION, 1},
				}},
				{	247, {
					{ IDENT_ID_FORMAT, 8},
					{ IDENT_ID_DISCONNECT, 9 },
					{ IDENT_ID_EJECT, 10},
					{ IDENT_ID_ERASE_THIS_DISC, 3},
				}},
				{	275, {
					{ IDENT_ID_SHOW_THIS_PC, 4},
					{ IDENT_ID_SHOW_NETWORK, 5},
					{ IDENT_ID_SHOW_LIBRARIES, 3},
					//{ IDENT_ID_SHOW_ALL_FOLDERS, isw8(1, 0), {} },
					//{ IDENT_ID_EXPAND_TO_CURRENT_FOLDER, isw8(2, 1),/*, {L'\uE1B1', 0}*/ },
				}},
				/*{	280, {
					{ IDENT_ID_OPEN_FILE_LOCATION, 0, { L'\uE0E8', 0 } },
				}},
				{	394, {
					{ IDENT_ID_EDIT, 400, { L'\uE0A1', 0 } }
				}},
				{	360, {
					{IDENT_ID_OPEN, 25},
					{IDENT_ID_RUN_AS_ADMINISTRATOR, 32, {L'\uE057', L'\uE058'}},
				}},*/
				{	502, {
					{ IDENT_ID_REMOVE_PROPERTIES, 2 }
				}},
				{	200, {
					{IDENT_ID_COPY_HERE, 1},
					{ IDENT_ID_MOVE_HERE, 2},
					{IDENT_ID_CREATE_SHORTCUTS_HERE, 3},
					{IDENT_ID_CANCEL, 0},
				}},
				{262, {
					{IDENT_ID_INCLUDE_IN_LIBRARY, 0}
				}},
			});

			ldmenu2(explorerframe, { {
				3400, {
					{ IDENT_ID_EXPAND, 8},
					{ IDENT_ID_COLLAPSE, 9}
				}},
				{260,{
						{IDENT_ID_SELECT, 0, {}, {}, L"Select"} //shell32/menu/260/0  | 49927
					}
				},
				{266,{
						{IDENT_ID_GO_TO, 33104}
					}
				}
			});

			ldmenu2(explorer, {
				{ 205,{
					{IDENT_ID_ADJUST_DATE_TIME, 408, {L'\uE1F2'}},
					{IDENT_ID_CUSTOMIZE_NOTIFICATION_ICONS, 421, {L'\uE190'}},
					{IDENT_ID_CORTANA,445, {L'\ue170'}},
					{IDENT_ID_NEWS_AND_INTERESTS,621},
					{IDENT_ID_SHOW_CORTANA_BUTTON ,449, {L'\ue170'}},
					{IDENT_ID_SHOW_TASK_VIEW_BUTTON,430, {L'\uE204'}},
					{IDENT_ID_SHOW_PEOPLE_ON_THE_TASKBAR,435, {L'\uE106'}},
					{IDENT_ID_SHOW_PEN_BUTTON,437},
					{IDENT_ID_SHOW_TOUCH_KEYBOARD_BUTTON,436, {L'\ue18c'}},
					{IDENT_ID_SHOW_TOUCHPAD_BUTTON,438, {L'\uE1EB'}},
					{IDENT_ID_CASCADE_WINDOWS,403},
					{IDENT_ID_SHOW_WINDOWS_STACKED,405},
					{IDENT_ID_SHOW_WINDOWS_SIDE_BY_SIDE,404},
					{IDENT_ID_SHOW_THE_DESKTOP,407, {0, 0}},
					{IDENT_ID_TASK_MANAGER,420},
					{IDENT_ID_LOCK_THE_TASKBAR,424},
					{IDENT_ID_LOCK_ALL_TASKBARS,425},
					{IDENT_ID_TASKBAR_SETTINGS, 413},
					{IDENT_ID_EXIT_EXPLORER, 518, {L'\uE0D0'}}
				}},
				{12000,{
					{IDENT_ID_RESTORE_ALL_WINDOWS, 65493},
					{IDENT_ID_MINIMIZE_ALL_WINDOWS, 65492},
					{IDENT_ID_CLOSE_ALL_WINDOWS, 65491},
				}}
			});

			ldmenu2(user32, {
				{
					1, {{ IDENT_ID_INSERT_UNICODE_CONTROL_CHARACTER, 32787, {L'\uE14e'} }}
				},
				{ 
					16, {
					{ IDENT_ID_MINIMIZE, 61472 },
					{ IDENT_ID_MAXIMIZE, 61488 },
					{ IDENT_ID_MOVE, 61456 },
					{ IDENT_ID_SIZE, 61440 },
				}}
			});

			ldmenu2(L"wpdshext.dll", { 
				{20, {
					{IDENT_ID_IMPORT_PICTURES_AND_VIDEOS, 21}
				}},
				{30, {
					{ IDENT_ID_FORMAT, 31 }
				}}
			});

			ldstr(user32, {
				{IDENT_ID_RECONVERSION, 705, {L'\uE096'}},
			});

			//IDENT_ID_FILE_EXPLORER OPTIONS 22985

			ldstr(shell32, {
				{IDENT_ID_ADD_TO_PLAYLIST, 37427},
				{IDENT_ID_AUTOPLAY, 31384, {L'\ue1f9'}},
				{IDENT_ID_CAST_TO_DEVICE, 31289},
				{IDENT_ID_COMMAND_PROMPT, 22022},
				{IDENT_ID_CONTROL_PANEL, 4161, {L'\uE0F3'}},
				{IDENT_ID_COPY_TO, 30339},
				{IDENT_ID_MOVE_TO, 30340},
				{IDENT_ID_COPY_TO_FOLDER, 30304},
				{IDENT_ID_MOVE_TO_FOLDER, 30305},
				{IDENT_ID_COPY_AS_PATH, 30328},
				{IDENT_ID_COPY_PATH, 30329},
				{IDENT_ID_DESKTOP, 4162},
				{IDENT_ID_DEVICE_MANAGER, 24225},
				{IDENT_ID_EMPTY_RECYCLE_BIN, 10564},
				{IDENT_ID_EXTRACT_ALL, 37514},
				{IDENT_ID_EXTRACT_TO, 37516},
				{IDENT_ID_FILE_EXPLORER, 12352},
				{IDENT_ID_FILE_EXPLORER_OPTIONS, 22985},
				{IDENT_ID_RESTORE_DEFAULT_LIBRARIES, 34645},
				{IDENT_ID_MANAGE, 37602},
				{IDENT_ID_MAP_AS_DRIVE, 37440},
				{IDENT_ID_MOUNT, 31472},
				{IDENT_ID_NEW, 30315},
				{IDENT_ID_OPEN_AUTOPLAY, 8507},
				{IDENT_ID_OPEN_COMMAND_PROMPT, 37415},
				{IDENT_ID_OPEN_COMMAND_WINDOW_HERE, 8506},
				{IDENT_ID_OPEN_FILE_LOCATION, 1033 },
				{IDENT_ID_OPEN_FOLDER_LOCATION, 1040},
				{IDENT_ID_OPEN_IN_NEW_PROCESS, 8518},
				{IDENT_ID_OPEN_NEW_TAB, 37616},
				{IDENT_ID_OPEN_NEW_WINDOW, 37411},
				{IDENT_ID_OPEN_POWERSHELL_WINDOW_HERE, 8508},
				{IDENT_ID_OPEN_WINDOWS_POWERSHELL, 37446},
				{IDENT_ID_OPTIONS, 37459},
				{IDENT_ID_PIN_TO_QUICK_ACCESS, 51377},
				{IDENT_ID_PIN_CURRENT_FOLDER_TO_QUICK_ACCESS, 51388},
				{IDENT_ID_PIN_TO_START, 51201},
				{IDENT_ID_ADD_TO_FAVORITES, 51389},		//37408  windows 11 22H2
				{IDENT_ID_REMOVE_FROM_FAVORITES, 51390},//windows 11 22H2
				{IDENT_ID_REMOVE_FROM_RECENT, 51391},
				{IDENT_ID_PIN_TO_TASKBAR, 5386},
				{IDENT_ID_PLAY, 31283, {L'\uE1A5'}},
				{IDENT_ID_PREVIEW, 51260, {L'\uE275'}},
				{IDENT_ID_PRINT, 31250, {L'\uE0A8', 0}},
				{IDENT_ID_REMOVE_FROM_QUICK_ACCESS, 51381},
				{IDENT_ID_ROTATE_LEFT, 37610},
				{IDENT_ID_ROTATE_RIGHT, 37612},
				{IDENT_ID_RUN, 12710},
				{IDENT_ID_RUN_AS_ANOTHER_USER, 37419},
				{IDENT_ID_SEARCH, 12708, {L'\uE187'}},
				{IDENT_ID_EDIT, 37398},
				//{IDENT_ID_TROUBLESHOOT_COMPATIBILITY, 37425},
				{IDENT_ID_UNPIN_FROM_QUICK_ACCESS,51379},
				{IDENT_ID_UNPIN_FROM_START, 51394},
				{IDENT_ID_UNPIN_FROM_TASKBAR, 5387},
				{IDENT_ID_COMPRESSED, 31462},
				{IDENT_ID_CLEANUP, 37494},
				{IDENT_ID_NEW_FOLDER, 16859},
				{IDENT_ID_NEW_ITEM, 37400},
				{IDENT_ID_AUTO_ARRANGE_ICONS, 31157},//28785
				{IDENT_ID_ALIGN_ICONS_TO_GRID, 31158},// 28788
				{IDENT_ID_MORE_OPTIONS, 31153},
				{IDENT_ID_INSTALL, 10210},
				{IDENT_ID_CONFIGURE, 10209, { L'\uE0F3'}},
				{IDENT_ID_SELECT_ALL, 31276},
				{IDENT_ID_SELECT_NONE, 37378},
				{IDENT_ID_ALL_CONTROL_PANEL_ITEMS, 32012 },
				{IDENT_ID_IMPORT_PICTURES_AND_VIDEOS, 31285, {L'\uE14F'} },
				{IDENT_ID_CLOSE, 31450},
				{IDENT_ID_PERMANENTLY_DELETE, 37394}
			});

			//window 7
			if(!os->IsWindows8OrGreater())
			{
				ldstr(shell32, {
					{IDENT_ID_PIN_TO_START_MENU, 5381},
					{IDENT_ID_UNPIN_FROM_START_MENU, 5382}
				});
			}
			else 
			{
				add_uid({
					{ IDENT_ID_PIN_TO_START_MENU, 0},
					{ IDENT_ID_UNPIN_FROM_START_MENU, 0}
				});
			}
			
			ldstr(explorerframe, {
				{IDENT_ID_FOLDER_OPTIONS, 1030},
				//{IDENT_ID_SHARE, 49928},
				//{IDENT_ID_SHARE_WITH, 49930},
				{IDENT_ID_EXPAND_GROUP, 41220},
				{IDENT_ID_COLLAPSE_GROUP, 41221},
				{IDENT_ID_SELECT, 49927} //shell32/menu/260/0
			});

			/*ldstr(L"unregmp2.exe", {
				{IDENT_ID_ADD_TO_WINDOWS_MEDIA_PLAYER_LIST, 9800},
				{IDENT_ID_PLAY_WITH_WINDOWS_MEDIA_PLAYER, 9801},
			});*/

			ldstr(L"display.dll", { {IDENT_ID_DISPLAY_SETTINGS, 4} });
			ldstr(L"themecpl.dll", { {IDENT_ID_PERSONALIZE, 10} });
			ldstr(L"regedit.exe", { {IDENT_ID_MERGE, 310, {L'\uE142'} } });
			ldstr(L"powershell.exe", { {IDENT_ID_RUN_WITH_POWERSHELL, 108} });

			ldstr(L"stobject.dll", {
				{IDENT_ID_SET_AS_DESKTOP_BACKGROUND, 417},
				{IDENT_ID_NEXT_DESKTOP_BACKGROUND, 416}
			 });

			ldstr(L"fvecpl.dll", {
				{IDENT_ID_TURN_ON_BITLOCKER, 1140},
				{IDENT_ID_TURN_OFF_BITLOCKER, 1141},
			});

			ldstr(L"twext.dll", {
				{IDENT_ID_RESTORE_PREVIOUS_VERSIONS, 1037},
				//{IDENT_ID_RESTORE, 1033},
			});

			ldstr(L"twinui.dll", {
				{IDENT_ID_SETTINGS, 5621},
				{IDENT_ID_STORE, 10669, {L'\uE1B9'}},
				{IDENT_ID_POWER_OPTIONS, 5950},
				//{IDENT_ID_AUTOPLAY, 9914},
				//{IDENT_ID_REFRESH, 10620},
				{IDENT_ID_REMOVE, 11454, {L'\uE256'}},
				{IDENT_ID_ACCOUNT, 11452, {L'\uE106'}},
				{IDENT_ID_COPY_TO_CLIPBOARD, 13648},
				{IDENT_ID_WINDOWS_POWERSHELL, 10928}
			});

			ldstr(L"twinui.pcshell.dll", {
				{IDENT_ID_WINDOWS_POWERSHELL, 10928, {}, {}, L"Windows PowerShell"},
				{IDENT_ID_WINDOWS_TERMINAL, 10944},
				{IDENT_ID_TERMINAL, 10960, {}, {}, L"Terminal" }
			});

			ldstr(L"wpdshext.dll", {
				{ IDENT_ID_OPEN_AS_PORTABLE, 349 }
			});

			//Import_pictures_and_videos
			ldstr(L"isoburn.exe", {
				{IDENT_ID_BURN_DISC_IMAGE, 351}
			});

			ldstr(explorer, {
				{IDENT_ID_WINDOWS_TERMINAL, 22016, {}, {}, L"Windows Terminal"},
				{IDENT_ID_WINDOWS, 11104},
				{IDENT_ID_TASKBAR, 518},
				{IDENT_ID_FILE_EXPLORER, 6020},
				{IDENT_ID_SHOW_OPEN_WINDOWS, 850},
				//{IDENT_ID_HIDE, 1003},
				//{IDENT_ID_SHOW, 1004}
			});


			add_uid({
				{ IDENT_ID_SET_AS_DESKTOP_WALLPAPER, 0},
				{ IDENT_ID_SHOW_FILE_EXTENSIONS, 0},
				{ IDENT_ID_SHOW_HIDDEN_FILES,  0},
				{ IDENT_ID_MAKE_AVAILABLE_OFFLINE, 0},
				{ IDENT_ID_MAKE_AVAILABLE_ONLINE, 0},
				{ IDENT_ID_SHIELD, 0,}
			});

			//select id.windows_terminal
			//terminal
		}

		/*HRESULT Initializer::Modern(int enabled)
		{
			return S_FALSE;

			if(!ver->IsWindows11OrGreater())
				return S_FALSE;

			IID treatAs{};

			//REGDB_E_READREGDB
			auto hres = ::CoGetTreatAsClass(IID_FileExplorerContextMenu, &treatAs);

			if(enabled == 1)
			{
				if(treatAs != IID_ContextMenu)
					hres = ::CoTreatAsClass(IID_FileExplorerContextMenu, IID_ContextMenu);
			}
			else if(enabled == 2)
			{
				if(treatAs != IID_FileExplorerContextMenu)
					hres = ::CoTreatAsClass(IID_FileExplorerContextMenu, IID_FileExplorerContextMenu);
			}
			else if(S_OK == hres)
			{
				if(((treatAs == IID_ContextMenu) || (treatAs == IID_FileExplorerContextMenu)))
					hres = ::CoTreatAsClass(IID_FileExplorerContextMenu, GUID_NULL);
			}

			return hres;
		}*/


		// enabled/reload
		// disabled
		// mix
		bool Initializer::OnState(bool istaskbar)
		{
			bool reloaded = false;

			int changed = 0;

			Keyboard kb;
			auto count = kb.get_keys_state();

			if(istaskbar && kb.is_lbutton())
				return false;

			if(kb.is_rbutton())
				count--;

			if(kb.is_contextmenu())
				count -= 2;

			if(count == 0 || count > 2)
				return changed;

			auto kwin = kb.key_win();
			auto kctrl = kb.key_ctrl();
			auto kshift = kb.key_shift();

			//int key = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;

			if(count == 1)
			{
				if(kctrl || kb.is_lbutton()) // reload/enabled shell
				{
					//Modern(1);
					Status.Disabled = false;
					Status.Refresh = true;
					changed = 1;
				}
				else if(kwin) // modern + shell
				{
					//if(Status.Unload)
					//	Status.Unload = false;
					//Modern(0);
					Status.Refresh = false;
					changed = 1;
				}
			}
			else if(count == 2)
			{
				if(kctrl && kwin) // disabled shell/modern
				{
					//Modern(2);
					Status.Disabled = true;
					Status.Refresh = false;
					changed = 1;
				}
				else if(kshift && kctrl) // reload/enabled shell
				{
					Status.Disabled = false;
					Status.Refresh = true;
					changed = 2;
				}
			}

			if(changed > 0)
			{
				if(Status.Disabled)
					Initializer::instance->uninit();
				else if(Status.Refresh)
				{
					Initializer::instance->query(changed);
					reloaded = true;
				}
			}
			return reloaded;
		}
	}
}

/* explorer.exe

535, 	"Cascade all windows"
536, 	"Show all windows side by side"
537, 	"Minimize all windows"
538, 	"Show all windows stacked"
850, 	"&Show open windows"
25000, 	"News and interests"
*/

/* user32.dll
710, 	"Windows"
800, 	"OK"
801, 	"Cancel"
802, 	"&Abort"
803, 	"&Retry"
804, 	"&Ignore"
805, 	"&Yes"
806, 	"&No"
807, 	"&Close"
808, 	"Help"
809, 	"&Try Again"
810, 	"&Continue"
*/

/* shell32.dll
1361, 	"Start menu"
518, 	"Taskbar"
578, 	"Start"
511, 	"Pin to %1"
1362, 	"Unpin from %1"

22000, 	"Desktop"
22001, 	"Task Manager"
22002, 	"Disk Management"
22016, 	"Windows Terminal
602, 	"Power"
850, 	"&Show open windows"
857, 	"Show desktop"
10113, 	"Show desktop"
900, 	"Task View"
11107, 	"Close"
11104, 	"Windows"

4145, 	"Move"
4146, 	"Copy"
4147, 	"Delete"
4148, 	"Rename"
4149, 	"Link"
4150, 	"Apply Properties"
4151, 	"New"
4153, 	"Shortcut"
4161, 	"Control Panel"
4162, 	"Desktop"
4163, 	"Undo %s"
4164, 	"&Undo %s\tCtrl+Z"
4165, 	"&Undo\tCtrl+Z"
4166, 	" Search Control Panel"
4167, 	"Normal window"
4168, 	"Minimized"
4169, 	"Maximized"
4170, 	"&Redo\tCtrl+Y"
4191, 	"&Redo %s\tCtrl+Y"
4219, 	"Microsoft Windows"
4233, 	"Application"
4249, 	"&More..."
4640, 	"Runs the selected command with elevation."
5376, 	"Open wit&h"
5377, 	"Open wit&h..."
5378, 	"&Choose another app"
5381, 	"&Choose default app"
5382, 	"&Search the Microsoft Store"
5384, 	"Adds this item to the Start Menu"
5385, 	"Removes this item from the Start Menu"
5386, 	"Pin to tas&kbar"
5387, 	"Unpin from tas&kbar"
5388, 	"Pins this application to the taskbar."
5389, 	"Unpins this application from the taskbar."
5953, 	"Moving..."
5954, 	"Copying..."
8506, 	"Open command &window here"
8507, 	"Open AutoPla&y..."
8508, 	"Open Power&Shell window here"
8518, 	"Open in new &process"
8768, 	"Read-only"
8769, 	"Hidden"
8770, 	"System"
8771, 	"Compressed"
8772, 	"Encrypted"
8773, 	"Offline"
8997, 	"Application"
9015, 	"Browse"
9016, 	"Open with..."
9338, 	"Folders"
9740, 	"Close"
10208, 	"Open with Control Panel"
10209, 	"C&onfigure"
10210, 	"&Install"
10530, 	"..."
10564, 	"Empty Recycle &Bin"
12352, 	"File Explorer"
12389, 	"(Empty)"
12390, 	"More"
12560, 	"Burn to Disc"
12708, 	"Search"
12710, 	"Run"
12850, 	"Open"
12851, 	"Close"
12852, 	"Execute"
12853, 	"Application"
16532, 	"General"
16534, 	"Properties"
16857, 	"New File"
16859, 	"New Folder"
17536, 	"Microsoft Windows"
21763, 	"Administrator"
21782, 	"Programs"
24225, 	"Device Manager"
24743, 	"Task Manager"
29697, 	"&Copy here"
29698, 	"&Move here"
29699, 	"Create &shortcuts here"
30304, 	"Copy To &folder..."
30305, 	"Mo&ve To folder..."
30312, 	"Se&nd to"
30315, 	"Ne&w"
30316, 	"New %s"
30317, 	"&Folder"
30318, 	"&Shortcut"
30328, 	"Copy &as path"
30329, 	"Copy path"
30339, 	"Copy to"
30340, 	"Move to"
30381, 	"&Copy"
30382, 	"&Move"
30396, 	"New folder"
30397, 	"New shortcut"
30464, 	"Start menu"
30608, 	"Pin to Start list"
30609, 	"Control Panel"
31153, 	"More options"
31236, 	"New folder"
31242, 	"Rename"
31244, 	"Cut"
31246, 	"Copy"
31250, 	"Print"
31252, 	"Delete"
31254, 	"Remove properties"
31259, 	"Properties"
31261, 	"Undo"
31263, 	"Redo"
31276, 	"Select all"
31278, 	"Play all"
31283, 	"Play"
31355, 	"Burn to disc"
31359, 	"Erase this disc"
31380, 	"Paste"
31382, 	"Eject"
31384, 	"AutoPlay"
31389, 	"Close session"
31456, 	"New library"
31472, 	"Mount"
32788, 	"Expand Folders"
32789, 	"Collapse Folders"
33010, 	"Computer"
33011, 	"Share"
33224, 	"&Yes"
33225, 	"OK"
33226, 	"&Skip"
33228, 	"Cancel"
33229, 	"&Continue"
33230, 	"&Delete"
33231, 	"T&ry Again"
33232, 	"&No"
33248, 	"Programs and Features"
33280, 	"Shutdown"
33281, 	"Restart"
33553, 	"Delete"
33555, 	"Properties"
33560, 	"Cut"
33561, 	"Copy"
33562, 	"Paste"
33563, 	"Undo"
33568, 	"Redo"
34593, 	"&Include in library"
34645, 	"&Restore default libraries"
37376, 	"Paste shortcut"
37378, 	"Select none"
37380, 	"Invert selection"
37392, 	"Delete"
37398, 	"Edit"
37400, 	"New item"
37394, 	"Permanently delete"
37408, 	"&Add to favorites"
37411, 	"Open &new window"
37413, 	"Open new window in new &process"
37415, 	"Open command &prompt"
37417, 	"Run as administrator"
37419, 	"Run as another user"
37423, 	"Pin to taskbar"
37425, 	"Troubleshoot compatibility"
37427, 	"Add to playlist"
37444, 	"Open command prompt as &administrator"
37446, 	"Open Windows Powe&rShell"
37448, 	"Open Windows PowerShell as &administrator"
37459, 	"Options"
37460, 	"Sort by"
37462, 	"Group by"
37468, 	"Ascending"
37470, 	"Descending"
37472, 	"Open location"
37488, 	"Format"
37490, 	"Optimize"
37494, 	"Cleanup"
37514, 	"Extract all"
37516, 	"Extract to"
37584, 	"Show all folders"
37586, 	"Expand to open folder"
37590, 	"Show libraries"
37592, 	"Always show availability status"
37594, 	"Show This PC"
37596, 	"Show Network"
37602, 	"Manage"
37610, 	"Rotate left"
37612, 	"Rotate right"
37616, 	"Open new &tab"
38240, "Show more details..."
38325, 	"Add to Favorites"
49917, 	"Show Folders"
49918, 	"Show Libraries"
50944, 	"Run as di&fferent user"
51201, 	"&Pin to Start"
51260, 	"Preview"
51377, 	"Pin to Quick access"
51378, 	"Quick access"
51379, 	"Unpin from Quick access"
51381, 	"Remove from Quick access"
51384, 	"Pinned"
51388, 	"Pin current folder to Quick access"
51394, 	"Un&pin from Start"
51395, 	"Pin to Start &list"
51396, 	"Unpin from Start &list"
51729, 	"Theme"
51730, 	"Light"
51731, 	"Dark"
51732, 	"Windows Default"
51523, 	"Sign out"
61954, 	"&Shut Down"
61955, 	"Cancel"
61958, 	"&Sign out now"
61959, 	"Sign out &later"
61963, 	"&Restart Now"
61964, 	"Restart &Later"
*/

/*
//explorerframe.dll
41220, 	"Expand Group"
41221, 	"Collapse Group"
14416, "Set as background
49921, 	"Home"
49922, 	"Clipboard"
49927, 	"Select"
49929, 	"Send"
49926, 	"Open"
49945, 	"System"
49937, 	"Show/hide"
49947, 	"Manage"
49951, 	"Search"
49942, 	"Computer
49954, 	"Options"
49960, 	"Extract"
49986, 	"Run"
50003, 	"File Explorer"
50195, 	"New"
50215, 	"Extract To"
50217, 	"Cut (Ctrl+X)"
50218, 	"Paste (Ctrl+V)"
50219, 	"Copy (Ctrl+C)"
50220, 	"Rename (F2)"
50221, 	"New folder (Ctrl+Shift+N)"
50222, 	"Properties (Alt+Enter)"
50223, 	"Select all (Ctrl+A)"
50228, 	"Delete (Ctrl+D)"
50229, 	"Undo (Ctrl+Z)"
50230, 	"Redo (Ctrl+Y)"
50231, 	"Permanently Delete (Shift+Delete)"
50232, 	"Open New Window (Ctrl+N)"
50292, 	"All File Types"
50290, 	"Filter"
50291, 	"Filters"
*/

/* windows.storage.dll
51378, 	"Quick access"
4130, 	"File"
4131, 	"Folder"
4144, 	"Operation"
4145, 	"Move"
4146, 	"Copy"
4147, 	"Delete"
4148, 	"Rename"
4151, 	"New"
4161, 	"Control Panel"
4163, 	"Undo %s"
4164, 	"&Undo %s\tCtrl+Z"
4188, 	"Redo %s"
4191, 	"&Redo %s\tCtrl+Y"
8496, 	"&Open"
8497, 	"&Print"
8498, 	"P&lay"
8499, 	"P&review"
8502, 	"E&xplore"
8503, 	"S&earch..."
8505, 	"Run as &administrator"
8516, 	"&Edit"
8517, 	"Op&en in new window"
8519, 	"Open in new ta&b"
8768, 	"Read-only"
8769, 	"Hidden"
8770, 	"System"
8771, 	"Compressed"
8772, 	"Encrypted"
8773, 	"Offline"
8774, 	"Pinned"
8775, 	"Unpinned"
9012, 	"Computer"
*/
