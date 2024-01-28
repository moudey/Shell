#pragma once

#include <oleacc.h>

/*
sent to: +0x18, dwItemData start with 0xaa0df00d before its submenu being populated
// Looks like tagMSAAMENUINFO (MSAAMENUINFO0
// https://docs.microsoft.com/en-us/windows/desktop/api/oleacc/ns-oleacc-tagmsaamenuinfo
struct open_with_submenu
{
	INT32 magicNumber;	// 0d f0 0d aa (0xaa0df00d)
	INT32 length;		//
	WCHAR* text;		// The menu text
};

other menu items: +0x20

HBITMAP hBmp = *((HBITMAP*)(mii.itemData + 0x20));
*/

/*
* //windows 10 dwItemData
struct MenuItemData
{
	wchar_t*	text;		//
	uintptr_t	cch;
	uintptr_t	cch1;
	uint32_t	type;		// MF_POPUP or MF_SEPARATOR or MF_STRING
	uint32_t	iconIndex; 	//def -1
	HBITMAP		hbmpItem;
	HBITMAP		hbmpChecked;
	HBITMAP		hbmpUnchecked;
	uintptr_t	pos;		//def 1=top, 2=bottom
	uint32_t	fontSize;	//def 96=0x60
	uint32_t	state;		// 0 or 101 or 256=0x100
	uint32_t	dummy2;
	uint32_t	dummy3;
	uintptr_t	shared;		//all
	uint32_t	dummy5;
	uint16_t	dummy6;
	uint16_t	dummy7;
};
*/


namespace Nilesoft
{
	namespace Shell
	{
		struct open_with_submenu
		{
			DWORD magicNumber;	// 0d f0 0d aa (0xaa0df00d)
			DWORD length;		//
			WCHAR *text;		// The menu text
		};

		struct MenuItemData
		{
			wchar_t *text;		//
			size_t		cch;
			size_t		cch1;
			uint32_t	type;		// MF_POPUP or MF_SEPARATOR or MF_STRING
			uint32_t	iconIndex; 	//def -1
			HBITMAP		hbmpItem;
			HBITMAP		hbmpChecked;
			HBITMAP		hbmpUnchecked;
			size_t		pos;		//def 0, 1=top, 2=bottom
			uint32_t	dpi;		//def 96=0x60
			uint32_t	state;		// 0 or 0x101 or 256=0x100 // 0 explorer, 0x101 taskabr WM_KEYUP==0x0101
			uint32_t	dummy1;
			uint32_t	dummy2;
			uint32_t	dummy3;
			uint32_t	dummy4;
			uint32_t	dummy5;
			uint32_t	dummy6;
			uint32_t	dummy7;
			uint32_t	dummy8;
			uint32_t	dummy9;
			uint32_t	dummy10;

			HBITMAP get_image()
			{
			}
		};

		struct MenuItemData2
		{
			uint32_t	sig;		// 0x00
			uint32_t	length;		// 0x04
			wchar_t *	text;		// 0x08
			uint32_t	type;		// 0x0f xMF_POPUP or MF_SEPARATOR or MF_STRING
			HBITMAP		hbmpItem0x18; 	// 0x18
			HBITMAP		hbmpItem0x20;	// 0x20
			HBITMAP		hbmpChecked;
			HBITMAP		hbmpUnchecked;
			size_t		pos;		//def 0, 1=top, 2=bottom
			uint32_t	dpi;		//def 96=0x60
			uint32_t	state;		// 0 or 0x101 or 256=0x100 // 0 explorer, 0x101 taskabr WM_KEYUP==0x0101
		};

/*
		struct MenuStyle
		{
			//Renderer
			int Width{ -1 };		//default 158
			int Height{ -1 };		//
			uint32_t Background{};		//BackColor
			uint32_t BackgroundHover{};//BackColorSelected
			uint32_t Title{};			//ForegroundColor
			uint32_t TitleHover{};		//ForegroundColorSelected
			uint32_t Disable{};
			uint32_t Font{};
		};

		struct MenuPopup
		{
			bool	Image{ false };
			bool	Checked{ false };
			bool	HasDraw{ false };
			HMENU	Submenu{ nullptr };
			SIZE	Size{ 0, 0 };
			uint32_t	MaxWidth{ 0 };
		};
*/
		enum class ScaleType
		{
			DPI,
			PPI // This name is from debug symbol. Actually it means scale by window dpi, I don't know why it's called PPI.
		};

		enum ImmersiveContextMenuOptions
		{
			ICMO_USEPPI = 0x1,
			ICMO_OVERRIDECOMPATCHECK = 0x2,
			ICMO_FORCEMOUSESTYLING = 0x4,
			ICMO_USESYSTEMTHEME = 0x8,
			ICMO_ICMBRUSHAPPLIED = 0x10
		};

		enum ContextMenuPaddingType
		{
			CMPT_NONE = 0x0,
			CMPT_TOP_PADDING = 0x1,
			CMPT_BOTTOM_PADDING = 0x2,
			CMPT_TOUCH_INPUT = 0x4,
		};

		struct ContextMenuRenderingData;
		using CMRDArray = std::vector<std::unique_ptr<ContextMenuRenderingData>>;

#define SUBMENU 0x10 // For menuFlags
		struct ContextMenuRenderingData
		{
			std::wstring text;
			UINT menuFlags;
			HBITMAP hbmpItem;
			HBITMAP hbmpChecked;
			HBITMAP hbmpUnchecked;
			ContextMenuPaddingType cmpt;
			ScaleType scaleType;
			UINT dpi;
			bool useDarkTheme;
			bool useSystemPadding;
			bool forceAccelerators;
			CMRDArray *parentArray;
		}; 
		//IMEMENUITEMINFOW
		constexpr auto MF_NOITEM = 0xFFFFFFFFU;
		
		struct MenuItemInfo : public MENUITEMINFOW
		{
			static const uint32_t FMASK = MIIM_FTYPE | MIIM_STRING | MIIM_BITMAP |
				MIIM_DATA | MIIM_ID | MIIM_CHECKMARKS | MIIM_STATE | MIIM_SUBMENU;

			HMENU handle{};
			MenuItemInfo *owner{};
			NativeMenu *owner_static{};
			NativeMenu *owner_dynamic{};
			
			string path;

			struct
			{
				string text;
				string normalize;
				string raw;

				operator const wchar_t *() const { return text; }
				operator wchar_t *() const { return text; }
				explicit operator bool() const { return !text.empty(); }
				template<typename T = uint32_t>
				const T length() const { return text.length<T>(); }
				bool empty() const { return text.empty(); }
				bool equals(const wchar_t *str) const { return normalize.equals(str); }
			}title;


			string keys;
			std::vector<uint32_t> parent;
			std::vector<MenuItemInfo *> items;
			std::vector<NativeMenu *> native_items;

			std::vector<struct menuitem_t *> *sys_items=0;

			//Expression *tip{};
			uint32_t hash = 0;
			uint32_t id = 0;
			uint32_t length = 0;
			uint32_t index = 0;
			NativeMenuType type = NativeMenuType::Item;
			Position position = Position::Auto;
			Visibility visibility = Visibility::Enabled;
			SelectionMode mode = SelectionMode::Single;
			Privileges privileges = Privileges::None;
			ULONG_PTR itemdata = 0;
			Rect rect;
			bool populate{};
			bool dynamic{};
			bool destroy{};
			bool submenu_delete{};
			int separator{};
			int tab = -1;
			int column = 0;
			SIZE size = { -1,-1 };
			bool is_system{};
			struct MUID *ui{};

			struct {
				string val;
				int pos = 0;
				int def = -1;
			} indexof;

			struct {
				uint8_t type{};
				string text;
				uint16_t time = UINT16_MAX;
				explicit operator bool()const { return !text.empty(); }
			}tip;

			struct IMAGE
			{
				HBITMAP hbitmap{};
				SIZE size{};

				ImageImport import = ImageImport::None;
				bool inherited{};
				Expression *expr{};
				uint16_t display = 0;

				struct draw_t
				{
					enum {
						DT_NONE,
						DT_SHAPE,
						DT_GLYPH
					}type = DT_NONE;

					struct
					{
						bool solid = true;
						uint8_t radius = 0;
						SIZE size = { 16, 16 };
						long stroke = -1;
						Color color[2];
					} shape;

					struct glyph_t
					{
						wchar_t code[2] = {0, 0};
						Color color[2];
						SIZE size = { 0,0 };
						HFONT font = nullptr;
					} glyph;
				} draw;

				void destroy()
				{
					if(hbitmap && !inherited)
						::DeleteObject(hbitmap);

					hbitmap = nullptr;
					inherited = false;
					import = ImageImport::None;
				}

				bool isvalid() const 
				{
					return hbitmap != nullptr || inherited || import != ImageImport::None;
				}

				void inherit(IMAGE *img)
				{
					hbitmap = img->hbitmap;
					size = img->size;
					inherited = true;
				}

			} image, image_select;

			MenuItemInfo()
			{
				cbSize = sizeof(MENUITEMINFOW);
				fMask = FMASK;
				fType = 0;
				fState = 0;
				wID = 0;
				hSubMenu = nullptr;
				hbmpChecked = nullptr;
				hbmpUnchecked = nullptr;
				dwItemData = 0;
				hbmpItem = nullptr;
				dwTypeData = nullptr;
				cch = 0;
			}

			MenuItemInfo(uint32_t fMask)
				: MenuItemInfo()
			{
				this->fMask = fMask;
			}

			MenuItemInfo(uint32_t fMask, uint32_t fType)
				: MenuItemInfo(fMask)
			{
				this->fType = fType;
			}

			MenuItemInfo(uint32_t fMask, uint32_t fType, int wID)
				: MenuItemInfo(fMask, fType)
			{
				this->wID = static_cast<uint32_t>(wID);
			}

			MenuItemInfo(uint32_t fMask, uint32_t fType, int wID, const string &title)
				: MenuItemInfo(fMask, fType, wID)
			{
				this->dwTypeData = title;
				this->cch = title.length<uint32_t>();
			}

			MenuItemInfo(const string &title, int wID)
				: MenuItemInfo(MIIM_STRING | MIIM_ID, 0, wID, title)
			{
			}

			~MenuItemInfo()
			{
				if(destroy)
					destroy_hSubMenu();
				image.destroy();
			}

			void delete_dwItemData()
			{
				if(Signed(dwItemData))
				{
					delete reinterpret_cast<MenuItemInfo *>(dwItemData);
					dwItemData = 0;
				}
				else
				{
					//if(HeapValidate(GetProcessHeap(), 0, (void *)dwItemData))
					{
						/*if(HeapFree(GetProcessHeap(), 0, (void *)dwItemData))
						{
							Logger::Info(L"%s", this->dwTypeData);
						}*/
					}
				}
			}

			bool is_id(std::initializer_list<uint32_t> ids) const
			{
				for(const auto &h : ids)
				{
					if(id == h) return true;
				}
				return false;
			}
			auto get_data() const { return reinterpret_cast<MenuItemInfo *>(dwItemData); }
			void set_data() { set_data(this); }

			template<typename T>
			void set_data(T data)
			{
				itemdata = dwItemData;
				dwItemData = reinterpret_cast<uintptr_t>(data);
			}

			void get_title()
			{
				dwTypeData = title.text.buffer(MAX_PATH);
				cch = MAX_PATH;
			}

			void set_title(const string &title)
			{
				this->title = title;
				this->dwTypeData = this->title.text;
				cch = title.length<uint32_t>();
			}

			void set_title(string &&title)
			{
				if(!title.empty())
				{
					this->title = title.move();
					this->dwTypeData = this->title.text;
					cch = this->title.text.length<uint32_t>();
					normalize();
				}
			}

			const string &normalize()
			{
				hash = normalize(title.text, &title.normalize, &tab, &length, &keys);
				return title.normalize;
			}

			MenuItemInfo *Separator()
			{
				fMask = MIIM_ID | MIIM_FTYPE;
				fType = MFT_SEPARATOR;
				wID = static_cast<uint32_t>(-1);
				type = NativeMenuType::Separator;
				return this;
			}

			void fix_separator_id()
			{
				if(is_separator() && wID != UINT32_MAX)
					wID = UINT32_MAX;
			}

			bool is_item() const
			{
				return !is_separator() && !is_popup();
			}

			bool is_popup() const
			{
				return hSubMenu != nullptr;
			}

			bool is_popup_or_item()const
			{
				return !is_separator() && !is_spacer() && !is_label();
			}

			bool is_separator() const
			{
				return (fType & MFT_SEPARATOR) == MFT_SEPARATOR;
			}

			bool is_sep_before() const
			{
				return separator & (int)Separator::Top;
			}

			bool is_sep_after() const
			{
				return separator & (int)Separator::Bottom;
			}

			bool is_ownerdraw()const
			{
				return (fType & MFT_OWNERDRAW) == MFT_OWNERDRAW;
			}

			bool is_disabled()const
			{
				return (fState & MFS_DISABLED) && !(fType & MFT_SEPARATOR);
			}

			bool is_checked()const
			{
				return (fState & MFS_CHECKED) == MFS_CHECKED;
			}

			bool is_radiocheck()const
			{
				return (fType & MFT_RADIOCHECK) == MFT_RADIOCHECK;
			}

			int is_column()const
			{
				if((fType & MFT_MENUBARBREAK) == MFT_MENUBREAK)
					return 1;
				if((fType & MFT_MENUBARBREAK) == MFT_MENUBARBREAK)
					return 2;
				return 0;
			}

			bool remove_ownerdraw()
			{
				if(is_ownerdraw())
				{
					if(!is_fmask(MIIM_FTYPE))
						fMask |= MIIM_FTYPE;
					fType &= ~MFT_OWNERDRAW;
					return true;
				}
				return false;
			}

			bool remove_bitmap()
			{
				if(fMask & MIIM_BITMAP)
				{
					fMask &= ~MIIM_BITMAP;
					return true;
				}
				return false;
			}

			bool add_ownerdraw()
			{
				if(!is_ownerdraw())
				{
					fType |= MFT_OWNERDRAW;
					if(!is_fmask(MIIM_FTYPE)) fMask |= MIIM_FTYPE;
					return true;
				}
				return false;
			}

			bool is_fmask(uint32_t value) const
			{
				return (fMask & value) == value;
			}

			bool is_ftype(uint32_t value) const
			{
				return (fType & value) == value;
			}

			bool is_fstate(uint32_t value) const
			{
				return (fState & value) == value;
			}

			bool is_label() const { return visibility == Visibility::Label; }
			bool is_static() const { return visibility == Visibility::Static; }
			bool is_spacer() const { return is_static() && !has_image_or_draw(); }

			bool get(HMENU hMenu, int item, bool byPosition = true, bool btitle = false, bool bname = false)
			{
				if(btitle)
				{
					if(!is_fmask(MIIM_STRING))
						fMask |= MIIM_STRING;

					dwTypeData = title.text.buffer(MAX_PATH);
					cch = MAX_PATH;
				}

				auto ret = ::GetMenuItemInfoW(hMenu, item, byPosition, this);

				if(btitle)
				{
					title.text.release(cch);
					if(bname && !title.text.empty())
						normalize();
				}

				return ret;
			}

			bool set(HMENU hMenu, int item, bool byPosition = true)
			{
				if(!title.text.empty())
				{
					dwTypeData = title.text;
					cch = title.text.length<uint32_t>();
				}

				return ::GetMenuItemInfoW(hMenu, item, byPosition, this);
			}

			void set_popup_menu()
			{
				if(!is_popup())
					create_popup_menu();

				if(!is_fmask(MIIM_SUBMENU))
					fMask |= MIIM_SUBMENU;

				type = NativeMenuType::Menu;
			}

			HMENU create_popup_menu()
			{
				return this->hSubMenu = ::CreatePopupMenu();
			}

			bool has_image() const
			{
				return hbmpItem || hbmpUnchecked || hbmpChecked;
			}

			bool has_image_or_draw() const
			{
				return has_image() || image.hbitmap || image.inherited || image.import > ImageImport::Disabled;
			}

			HBITMAP get_image() { return FindImage(this); }

			bool is_signed() const
			{
				auto data = reinterpret_cast<uint32_t *>(dwItemData);
				return data && *data == sizeof(MENUITEMINFOW);
			}

			bool destroy_hSubMenu()
			{
				bool result = false;
				if(hSubMenu)
				{
					result = ::DestroyMenu(hSubMenu);
					hSubMenu = nullptr;
				}
				return result;
			}

			bool is_parent(const std::vector<uint32_t> & level)
			{
				if(parent.size() != level.size())
					return false;

				if(parent.back() != level.back())
					return false;

				for(auto i = 0u; i < parent.size(); i++)
				{
					if(parent[i] != level[i])
						return false;
				}

				/*for(auto i = parent.size() - 1; i > 0; i--)
				{
					if(parent[i] != level[i])
						return false;
				}*/
				return true;
			}

			bool parse_parent(string &value)
			{
				uint32_t h = 0;
				std::vector<string> ftree;
				if(value.split(ftree, L'/'))
				{
					for(auto &s : ftree)
					{
						h = MenuItemInfo::hashing(s);
						parent.push_back(h);
					}
				}
				return h != 0;
			};

			/*static bool parse_parent(string &value, std::vector<uint32_t> *parent)
			{
				uint32_t h = 0;
				std::vector<string> ftree;
				if(value.split(ftree, L'/'))
				{
					for(auto &s : ftree)
					{
						h = MenuItemInfo::hashing(s);
						parent->push_back(h);
					}
				}
				return h != 0;
			};
			*/
			static bool Signed(uintptr_t itemData)
			{
				auto data = reinterpret_cast<uint32_t *>(itemData);
				return data && *data == sizeof(MENUITEMINFOW);
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
			*/


			// iswpunct !"#$%&'()*+,-./ 
			// iswblank \x09 \x20
			static bool is_normalize(wchar_t c)
			{
				return !std::iswpunct(c) and !std::iswcntrl(c) and !iswblank(c);
			}

			static uint32_t normalize(string &title, string *title_normalize = {}, int *tab = {}, uint32_t *length = {}, string *keys = {})
			{
				uint32_t hash = 0;
				if(title_normalize)
					*title_normalize = {};
				
				if(!title.empty())
				{
					Hash h;
					wchar_t c = 0;
					string tmp = title;

					tmp.trim_end(L'.').trim_cntrl();

					bool last_punct = false;
					int itab = -1;
					
					for(int i = 0; i < tmp.length<int>(); i++)
					{
						c = tmp[i];
						if(c == 0 || c == L'\t' || c == L'\b')
						{
							itab = i;
							break;
						}
						else if(c == L'&')
						{
							if(tmp[i + 1] != L'&')
								continue;
							i++;
						}
						
						if(c == L'_' || tmp.length() == 1 || is_normalize(c))
						{
							if(c == L'_')
							{
								if(!last_punct) h.hash(c);
								last_punct = true;
							}
							else 
							{
								h.hash(c);
								last_punct = false;
							}
						}
						else if(!last_punct)
						{
							if(i + 1 < tmp.length<int>())
								h.hash(L'_');
							last_punct = true;
						}

						if(length) (*length)++;

						if(title_normalize)
							title_normalize->append(c);
					}

					hash = h;

					if(itab >= 0)
					{
						title = title.substr(0, itab).trim_end(L"\t\b").move();

						if(tab)
							*tab = itab;

						if(itab > 0 && keys)
							*keys = tmp.substr(itab).trim().move();
					}
				}
				return hash;
			}

			static uint32_t hashing(const string &value, uint32_t hash = 0)
			{
				if(value.empty())
					return 0;

				Hash h(hash == 0 ? Hash::offset_basis : hash);
				wchar_t c = 0;
				string tmp = value.c_str();
				tmp.trim_end(L'.').trim_cntrl();
				bool last_punct = false;
				for(auto i = 0u; i < value.length(); i++)
				{
					c = tmp[i];
					if(c == 0 or c == L'\t' or c == L'\b')
						break;
					else if(c == L'&')
					{
						if(tmp[i + 1] != L'&')
							continue;
						i++;
					}

					if(c == L'_' || tmp.length() == 1 || is_normalize(c))
					{
						if(c == L'_')
						{
							if(!last_punct) h.hash(c);
							last_punct = true;
						}
						else
						{
							h.hash(c);
							last_punct = false;
						}
					}
					else if(!last_punct)
					{
						if(i + 1 < tmp.length())
							h.hash(L'_');
						last_punct = true;
					}
				}

				return h;
			}

			static HBITMAP FindImage(MENUITEMINFOW *mii)
			{
				if(mii->hbmpItem)
				{
					if(mii->hbmpItem <= HBMMENU_POPUP_MINIMIZE && mii->hbmpItem >= HBMMENU_CALLBACK)
						return nullptr;
					return mii->hbmpItem;
				}
				else if(mii->hbmpUnchecked)
				{
					return mii->hbmpUnchecked;
				}
				else if(mii->hbmpChecked)
				{
					return mii->hbmpChecked;
				}
				else if(mii->dwItemData)
				{
					HBITMAP result{};
					auto verify = [&](int index)
					{
						result = *reinterpret_cast<HBITMAP *>(mii->dwItemData + index);
						if(result && ::GetObjectType(result) == OBJ_BITMAP)
						{
							BITMAP bitmap{ };
							if(::GetObjectW(result, sizeof(BITMAP), &bitmap) == sizeof(BITMAP))
							{
								auto bmWidthBytes = ((bitmap.bmWidth * bitmap.bmBitsPixel + 31) / 32) * 4;
								if(bmWidthBytes == bitmap.bmWidthBytes)
								{
									if(bitmap.bmBitsPixel == 32 || bitmap.bmBitsPixel == 24 ||
									   bitmap.bmBitsPixel == 16 || bitmap.bmBitsPixel == 8)
									{
										if(bitmap.bmWidth >= 4 && bitmap.bmWidth <= 64)
										{
											if(bitmap.bmHeight >= 4 && bitmap.bmHeight <= 64)
												return true;
										}
									}
								}
							}
						}
						return false;
					};

					//uniqueID
					if(!IS_INTRESOURCE(mii->dwItemData))
						//if((dwItemData >> 16) != 0)
					{
						__try
						{
							// 0x018 send to/library/givaccess
							// 0x220 open with
							auto msaa = reinterpret_cast<uint32_t *>(mii->dwItemData);
							if(*msaa == MSAA_MENU_SIG)
							{
								//0x18 send to/library/givaccess/open with(search item)
								if(verify(0x018) || verify(0x220))
									return result;
							}
							else
							{
								// 0x00 New menu
								// 0x20 Open with(Choose another app)/others
								if(verify(0x000) || verify(0x020))
									return result;
							}
						}
						__except(EXCEPTION_EXECUTE_HANDLER)
						{
						}
					}
				}
				return nullptr;
			}
		};

		struct MUID
		{
			uint32_t id = 0;
			uint32_t res_id = 0;

			wchar_t image_glyph[2]{ 0, 0 };
			const wchar_t *image_res{};

			string title_fallback;
			string title;
			string title_normalize;
			uint32_t hash = 0;
			Expression *image{};

			std::vector<uint32_t> ids;

			void set_hash()
			{
				if(!title.empty())
					hash = MenuItemInfo::normalize(title, &title_normalize);
			}

			string get_title() const 
			{
				if(!title.empty())
					return title;
				else if(!title_fallback.empty())
					return title_fallback;
				return {};
			}

			string get_name() const
			{/*
				string name;
				if(!title.empty())
					name = title;
				else if(!title_fallback.empty())
					name = title_fallback;

				return name.move();
				*/
				return title_normalize;
			}
		};

		class MENU //: public MENUITEMINFO
		{
			HMENU _handle;
			int _count;

		public:
			MENU(HMENU handle) : _handle{ handle }, _count{ 0 }
			{
			}

			operator HMENU() const { return _handle; }
			operator bool() const { return _handle != nullptr; }

			auto empty() const { return  _count == 0; }
			auto count() const { return  _count; }

			auto get_count()
			{
				_count = get_count(_handle);
				return  _count;
			}

			bool insert(MenuItemInfo *mii, uint32_t uitem, bool byPosition = true)
			{
				if(::InsertMenuItemW(_handle, uitem, byPosition, reinterpret_cast<MENUITEMINFOW*>(mii)))
				{
					_count++;
					return true;
				}
				return false;
			}

			bool insert(MenuItemInfo *mii, int uitem, bool byPosition = true)
			{
				if(insert(_handle, mii, uitem, byPosition))
				{
					_count++;
					return true;
				}
				return false;
			}

			bool is_last_sep = false;

			int insert(MenuItemInfo *mii, int uitem, bool byPosition, int index)
			{
				int inserted = 0;

				if(mii->is_separator())
				{
					if(is_last_sep || index == 0)
					{
						return 0;
					}
				}

				auto pos = uitem >= 0 ? uitem : _count;
				if(this->insert(mii, pos, byPosition))
				{

					if(mii->is_sep_before() && !is_last_sep && index != 0)
						inserted += this->insert_separator(pos + inserted, mii->is_ownerdraw());

					inserted++;

					if(mii->is_sep_after())
					{
						inserted += this->insert_separator(pos + inserted, mii->is_ownerdraw());
						is_last_sep = true;
					}
					else
					{
						is_last_sep = false;
					}

					if(mii->is_separator())
						is_last_sep = true;
				}

				return inserted;
			}

			bool insert(const string &title, uint32_t id, int uitem, bool byPosition = true, HMENU hSubMenu = nullptr)
			{
				MenuItemInfo mii(MIIM_STRING | MIIM_ID, 0, id, title);
				if(hSubMenu)
				{
					mii.fMask |= MIIM_SUBMENU;
					mii.hSubMenu = hSubMenu;
				}

				return this->insert(&mii, uitem, byPosition);
			}

			bool insert_separator(int uitem, bool ownerdraw = false)
			{
				MenuItemInfo mii(MIIM_FTYPE | MIIM_ID, MFT_SEPARATOR, -1);
				if(ownerdraw) mii.fType |= MFT_OWNERDRAW;
				return this->insert(&mii, uitem, true);
			}

			bool remove(int uitem, bool byPosition = true)
			{
				if(remove(_handle, uitem, byPosition ? MF_BYPOSITION : MF_BYCOMMAND))
				{
					_count--;
					return true;
				}
				return false;
			}

			bool destroy(int uitem, bool byPosition = true)
			{
				if(destroy(_handle, uitem, byPosition ? MF_BYPOSITION : MF_BYCOMMAND))
				{
					_count--;
					return true;
				}
				return false;
			}

			bool destroy() const
			{
				return destroy(_handle);
			}

			HMENU get() const { return _handle; }

			bool set(MenuItemInfo *mii, int uitem, bool byPosition = true) const
			{
				return set(_handle, mii, uitem, byPosition);
			}

			bool get(MenuItemInfo *mii, int uitem, bool byPosition = true, bool btitle = false, bool bname = false)
			{
				return get(_handle, mii, uitem, byPosition, btitle, bname);
			}

			bool set(MENUINFO *mi) const
			{
				return set(_handle, mi);
			}

			bool get(MENUINFO *mi) const
			{
				return get(_handle, mi);
			}

			bool set_background(HBRUSH hBrush) const
			{
				MENUINFO mi = { sizeof(mi), MIM_BACKGROUND };
				mi.hbrBack = hBrush;
				return set(&mi);
			}

			string get_title(int uitem, bool byPosition = true) const
			{
				string title;
				MenuItemInfo mii(MIIM_STRING);
				mii.destroy = false;
				mii.dwTypeData = title.buffer(MAX_PATH);
				mii.cch = MAX_PATH;
				if(::GetMenuItemInfoW(_handle, uitem, byPosition, reinterpret_cast<MENUITEMINFOW *>(&mii)))
					return title.release(mii.cch).move();
				return nullptr;
			}

			int clean_separator()
			{
				int removed = 0;
				bool last_sep = false;
				for(auto index = get_count() - 1; index >= 0; index--)
				{
					MenuItemInfo mii = MIIM_FTYPE;
					mii.destroy = false;
					if(get(&mii, index))
					{
						if(mii.is_separator())
						{
							if(index == _count - 1 || last_sep || index == 0)
							{
								removed += this->destroy(index);
								continue;
							}
							last_sep = true;
						}
						else
						{
							last_sep = false;
						}
					}
				}
				return removed;
			}

			int clear(bool destroy = true)
			{
				int removed = 0;
				//if(get_count())
				{
					int loop = 0;
					while(get_count() > 0)
					{
						__try {
							if(loop++ > 1000) break;
							removed += destroy ? this->destroy(0) : this->remove(0);
						}
						__except(EXCEPTION_EXECUTE_HANDLER) {
							//Logger::Info(L"**********");
						}
					}
				}
				return removed;
			}

			static bool get(HMENU hMenu, MenuItemInfo *mii, int uitem, bool byPosition = true, bool btitle = false, bool bname = false)
			{
				if(btitle)
				{
					if((mii->fMask & MIIM_STRING) != MIIM_STRING)
						mii->fMask |= MIIM_STRING;

					mii->dwTypeData = mii->title.text.buffer(MAX_PATH);
					mii->cch = MAX_PATH;
				}

				auto ret = ::GetMenuItemInfoW(hMenu, uitem, byPosition, reinterpret_cast<MENUITEMINFOW *>(mii));

				if(btitle)
				{
					mii->title.text.release(mii->cch);
					if(bname && mii->cch > 0)
						mii->normalize();
				}

				return ret;
			}

			static bool set(HMENU hMenu, MenuItemInfo *mii, int uitem, bool byPosition = true)
			{
				return ::SetMenuItemInfoW(hMenu, uitem, byPosition, reinterpret_cast<MENUITEMINFOW *>(mii));
			}

			static bool insert(HMENU hMenu, int position, MenuItemInfo *mii, Separator separator)
			{
				auto result = false;
				auto sep_top = separator == Separator::Top || separator == Separator::Both;
				auto sep_bottom = separator == Separator::Bottom || separator == Separator::Both;

				if(sep_top && insert_separator(hMenu, position))
					position++;

				result = insert(hMenu, mii, position, true);

				if(result && sep_bottom)
					insert_separator(hMenu, ++position);

				return result;
			}

			static int insert(HMENU hMenu, MenuItemInfo *mii, int uitem, bool byPosition, bool append_separator)
			{
				int inserted = 0;

				if(insert(hMenu, mii, uitem, byPosition))
				{
					if(append_separator)
					{
						if(mii->is_sep_before())
							inserted += insert_separator(hMenu, uitem + inserted);

						inserted++;

						if(mii->is_sep_after())
							inserted += insert_separator(hMenu, uitem + inserted);
					}
				}
				return inserted;
			}

			static bool insert_separator(HMENU hMenu, int position)
			{
				return insert(hMenu, { MIIM_FTYPE | MIIM_ID, MFT_SEPARATOR, -1 }, position, true);
			}

			static bool insert(HMENU hMenu, const MenuItemInfo &mii, int uitem, bool byPosition = true)
			{
				return insert(hMenu, const_cast<MenuItemInfo *>(&mii), uitem, byPosition);
			}

			static bool insert(HMENU hMenu, MenuItemInfo *mii, int uitem, bool byPosition = true)
			{
				return ::InsertMenuItemW(hMenu, uitem, byPosition, reinterpret_cast<MENUITEMINFOW *>(mii));
			}

			static bool remove(HMENU hMenu, int uitem, bool byPosition = true)
			{
				return ::RemoveMenu(hMenu, uitem, byPosition ? MF_BYPOSITION : MF_BYCOMMAND);
			}

			static bool destroy(HMENU hMenu)
			{
				return hMenu ? ::DestroyMenu(hMenu) : false;
			}

			static bool destroy(HMENU hMenu, int uitem, bool byPosition = true)
			{
				return ::DeleteMenu(hMenu, uitem, byPosition ? MF_BYPOSITION : MF_BYCOMMAND);
			}

			static MENU createPopupMenu()
			{
				return ::CreatePopupMenu();
			}

			static int get_count(HMENU hMenu)
			{
				auto c = ::GetMenuItemCount(hMenu);
				return c < 0 ? 0 : c;
			}

			static uint32_t get_index(HMENU hMenu, uint32_t id)
			{
				for(int i = 0; i < get_count(hMenu); i++)
				{
					MENUITEMINFOW mii = { sizeof mii, MIIM_ID };
					if(::GetMenuItemInfoW(hMenu, i, true, &mii))
						if(id == mii.wID) return i;
				}
				return MF_NOITEM;
			}

			static uint32_t get_id(HMENU hMenu, uint32_t index)
			{
				MENUITEMINFOW mii = { sizeof mii, MIIM_ID };
				if(::GetMenuItemInfoW(hMenu, index, true, &mii))
					return mii.wID;
				return MF_NOITEM;
			}

			static bool set(HMENU hMenu, MENUINFO *mi)
			{
				return ::SetMenuInfo(hMenu, mi);
			}

			static bool get(HMENU hMenu, MENUINFO *mi)
			{
				return ::GetMenuInfo(hMenu, mi);
			}

			int GetMenuItemPos(HMENU hMenu, UINT uID)
			{
				int iPos = -1;

				for(int i = 0; i < GetMenuItemCount(hMenu); i++)
				{
					if(GetMenuItemID(hMenu, i) == uID)
					{
						iPos = i;
						break;
					}
				}

				return iPos;
			}
		};
	}
}