#pragma once
#include "Expression\Variable.h"
#include "Include\Theme.h"
#include <Resource.h>

namespace Nilesoft
{
	namespace Shell
	{
		/*static bool SHLoadIndirectString(const wchar_t *pszSource, wchar_t *pszOutBuf, uint32_t cchOutBuf)
		{
			return SUCCEEDED(DLL::Invoke<HRESULT>(L"shlwapi.dll", "SHLoadIndirectString",
												  pszSource, pszOutBuf, cchOutBuf, nullptr));
		}*/

		struct Package
		{
			string name;
			string path;
			string family;
			string id;
			string version;
		};

		class PackagesCache
		{
			inline static const string _local_settings = LR"(Software\Classes\Local Settings)";
			inline static const string _packages = _local_settings + LR"(\Software\Microsoft\Windows\CurrentVersion\AppModel\Repository\Packages)";
			inline static const string _mrt_cache = _local_settings + LR"(\MrtCache)";

		public:

			std::vector<Package> list;

			PackagesCache() = default;
			~PackagesCache() = default;

			bool load()
			{
				HKEY hkeyPackages = nullptr;
				TResult res = ::RegOpenKeyExW(HKEY_CURRENT_USER, _packages, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hkeyPackages);

				if(!res || !hkeyPackages)
					return false;

				res = ERROR_SUCCESS;
				for(int i = 0; res.get() != ERROR_NO_MORE_ITEMS; i++)
				{
					string name(MAX_PATH);
					DWORD cchName = MAX_PATH;
					res = ::RegEnumKeyExW(hkeyPackages, i, name.data(), &cchName, nullptr, nullptr, nullptr, nullptr);
					if(res)
					{
						Package pk;
						pk.name = name.release(cchName);

						HKEY hkeyPackage = nullptr;
						res = ::RegOpenKeyExW(hkeyPackages, name, 0, KEY_QUERY_VALUE, &hkeyPackage);
						if(res)
						{
							pk.id = name;// get_value(L"PackageID", hkeyPackage).move();
							//MBF(L"%s\n%s", pk.id.c_str(), name.c_str());
							if(!pk.id.empty())
							{
								pk.path = get_value(L"PackageRootFolder", hkeyPackage).move();
								//pk.name = get_value(L"DisplayName", hkeyPackage).move();
								get_name(&pk, hkeyPackage);

								auto pos = pk.id.index_of(L'_');
								if(pos != pk.id.npos)
								{
									string _family_ = pk.id.substr(0, pos).move();
									string v = pk.id.substr(pos + 1).move();

									pos = v.index_of(L'_');
									if(pos != v.npos)
										pk.version = v.substr(0, pos).move();

									pos = pk.id.last_index_of(L'_');
									if(pos != pk.id.npos)
									{
										_family_ += pk.id.substr(pos);
										pk.family = _family_.move();
									}
								}
								list.push_back(std::move(pk));
							}
							::RegCloseKey(hkeyPackage);
						}
					}
				}

				::RegCloseKey(hkeyPackages);
				hkeyPackages = nullptr;

				return true;
			}

			string get_value(const wchar_t *name, HKEY hkeyPackage)
			{
				if(hkeyPackage)
				{
					DWORD size = 0;
					DWORD type = REG_NONE;
					TResult res = ::RegQueryValueExW(hkeyPackage, name, 0, &type, nullptr, &size);
					if(res)
					{
						size_t length = size / sizeof(wchar_t);
						string buf(length);
						res = ::RegQueryValueExW(hkeyPackage, name, 0, &type, reinterpret_cast<LPBYTE>(buf.data()), &size);
						if(res)
						{
							return buf.release(length - 1).move();
						}
					}
				}
				return {};
			}

			void get_name(Package *pk, HKEY hkeyPackage)
			{
				pk->name = get_value(L"DisplayName", hkeyPackage).move();

				if(pk->name.length() > 3)
				{
					if(pk->name.starts_with(L"@{", false) &&
					   pk->name.back(L'}', false))
					{
						wchar_t displayName[MAX_PATH]{};
						if(S_OK != ::SHLoadIndirectString(pk->name, displayName, MAX_PATH, nullptr) || pk->name.equals(displayName))
						{
							displayName[0] = 0;
							HKEY hkeyPackages_mrt = nullptr;
							TResult res = ::RegOpenKeyExW(HKEY_CURRENT_USER, _mrt_cache, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hkeyPackages_mrt);
							if(res)
							{
								for(int i = 0; res.get() != ERROR_NO_MORE_ITEMS; i++)
								{
									string name(MAX_PATH);
									DWORD cchName = MAX_PATH;
									res = ::RegEnumKeyExW(hkeyPackages_mrt, i, name.data(), &cchName, nullptr, nullptr, nullptr, nullptr);
									if(res)
									{
										name.release(cchName);
										if(name.contains(pk->id))
										{
											auto hkeyLang = get_langKey(hkeyPackages_mrt, name);
											if(hkeyLang)
											{
												for(i = 0; res.get() != ERROR_NO_MORE_ITEMS; i++)
												{
													wchar_t valueName[MAX_PATH]{};
													DWORD cbDisplayName = MAX_PATH;
													displayName[0] = 0;
													DWORD cchValueName = MAX_PATH;
													DWORD dtype = 0;
													res = ::RegEnumValueW(hkeyLang, i, valueName, &cchValueName, nullptr, &dtype, reinterpret_cast<LPBYTE>(displayName), &cbDisplayName);
													if(res && pk->name.equals(valueName))
													{
														pk->name = displayName;
														break;
													}
												}
												::RegCloseKey(hkeyLang);
											}
											break;
										}
									}
								}
								::RegCloseKey(hkeyPackages_mrt);
							}
						}
						pk->name = displayName;
					}
				}
			}

			HKEY get_langKey(HKEY hkey, const wchar_t *subkey = nullptr)
			{
				HKEY result = nullptr;
				wchar_t name[MAX_PATH]{};
				DWORD cchName = MAX_PATH;
				HKEY hKeyLangList = nullptr;

				TResult res = ::RegOpenKeyExW(hkey, subkey, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hKeyLangList);
				if(res)
				{
					res = ::RegEnumKeyExW(hKeyLangList, 0, name, &cchName, nullptr, nullptr, nullptr, nullptr);
					if(res)
					{
						HKEY hKeyLang = nullptr;
						res = ::RegOpenKeyExW(hKeyLangList, name, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hKeyLang);
						if(res)
						{
							name[0] = {};
							cchName = MAX_PATH;
							res = ::RegEnumKeyExW(hKeyLang, 0, name, &cchName, nullptr, nullptr, nullptr, nullptr);
							if(res)
								::RegOpenKeyExW(hKeyLang, name, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &result);
							::RegCloseKey(hKeyLang);
						}
					}
					::RegCloseKey(hKeyLangList);
				}
				return result;
			}

			const Package *find(const wchar_t *name) const
			{
				for(auto &pk : list)
					if(pk.id.contains(name))
						return &pk;
				return nullptr;
			}

			bool exists(const wchar_t *name) const
			{
				return find(name);
			};
		};

		class FontCache
		{
			std::unordered_map<uint32_t, Font*> fonts;
			HANDLE _handle{};

		public:
			static constexpr auto  Default = L"Nilesoft.Shell";
			static constexpr auto  SegoeFluentIcons = L"Segoe Fluent Icons";
			static constexpr auto  SegoeMDL2Assets = L"Segoe MDL2 Assets";

		public:
			uint32_t _dpi = 96;
			//HFONT shell_font{};
			//HFONT default_font{};
			uint32_t default_id = 0;

			FontCache()
			{
			}

			~FontCache()
			{
				clear();
				if(_handle)
					::RemoveFontMemResourceEx(_handle);
			}

			void init(HINSTANCE hinstance, uint32_t dpi = 96)
			{
				clear();
				_dpi = dpi;
				auto hRes = ::FindResourceW(hinstance, L"FONTICON", RT_RCDATA);
				if(!hRes)
				{
					Logger::Warning(L"FONTICON not found");
					return;
				}

				auto hResData = ::LoadResource(hinstance, hRes);
				if(hResData)
				{
					auto lpFileView = ::LockResource(hResData);
					if(lpFileView)
					{
						auto cjSize = ::SizeofResource(hinstance, hRes);
						DWORD numFonts = 0;
						_handle = ::AddFontMemResourceEx(lpFileView, cjSize, nullptr, &numFonts);
						if(_handle)
						{
							//shell_font = add(Default, 16);
						}
						UnlockResource(lpFileView);
					}
					::FreeResource(hResData);
				}
			}

			//icon.Fluent()
			/*void add_default(const string &name, long size)
			{
				if(name.empty())
				{
					default_font = add(SegoeFluentIcons, size);
					if(!default_font)
					{
						default_font = add(SegoeMDL2Assets, size);
					}
				}
				else
				{
					default_font = add(name, size);
				}
			}*/

			Font *at(uint32_t id) const
			{
				for(auto &font : fonts)
				{
					if(id == font.first) return font.second;
				}
				return nullptr;
			}

			Font *at(HFONT hfont) const
			{
				for(auto font : fonts)
				{
					if(hfont == font.second->get()) 
						return font.second;
				}
				return nullptr;
			}

			bool remove(HFONT hfont)
			{
				for(auto font = fonts.begin(); font != fonts.end(); font++)
				{
					if(hfont == font->second->get())
					{
						fonts.erase(font);
						break;
					}
				}
				return false;
			}

			HFONT add(const string &name, int size, int charset = DEFAULT_CHARSET)
			{
				size = Theme::DPI(size, _dpi);
				auto id = Hash::dohash(size, name.hash());
				auto font = at(id);
				if(!font)
				{
					font = new Font(name, size, CLEARTYPE_QUALITY, charset);
					if(font->get())
						fonts[id] = font;
					else
					{
						delete font;
						return nullptr;
					}
				}
				return font ? font->get() : nullptr;
			}

			HFONT add(const string &name, int size, uint32_t dpi, int charset = DEFAULT_CHARSET)
			{
				//size = Theme::DPI(size, _dpi);
				auto id = Hash::dohash(size + dpi, name.hash());
				auto font = at(id);
				if(!font)
				{
					font = new Font(name, size, CLEARTYPE_QUALITY, charset);
					if(font->get())
						fonts[id] = font;
					else
					{
						delete font;
						return nullptr;
					}
				}
				return font ? font->get() : nullptr;
			}

			void clear()
			{
				for(auto font : fonts)
					delete font.second;

				fonts.clear();
				//default_font = {};
				//shell_font = {};
				default_id = {};
			}

			size_t size() const { return fonts.size(); }
		};

		struct ImageCache
		{
			std::vector<uint32_t> id;
			auto_expr value;

			bool equals(uint32_t ident) const
			{
				for(auto &it : id)
				{
					if(it == ident) return true;
				}
				return false;
			}

			bool equals(std::vector<uint32_t> const &idents) const
			{
				for(auto& i : idents)
				{
					if(equals(i)) return true;
				}
				return false;
			}

			void add(uint32_t ident)
			{
				for(auto &it : id)
					if(it == ident) return;
				id.push_back(ident);
			}

			void add(std::vector<uint32_t> ids)
			{
				for(auto &it : ids) add(it);
			}
		};

		struct CACHE
		{
			Settings					settings;

			struct {
				Scope global;
				Scope runtime;
				Scope loc;
			} variables;

			std::vector<NativeMenu*>	statics;
			NativeMenu					dynamic;
			FontCache					fonts;
			PackagesCache				Packages;
			std::vector<ImageCache>		images;

			struct
			{
				uint32_t color = IDENT_DEFAULT;
				uint32_t back = IDENT_DEFAULT;
				string name = FontCache::Default;
			}
			glyph;


			CACHE()
			{
				//runtime_variables.Parent = &variables;
			}

			~CACHE()
			{
				while(!statics.empty())
				{
					delete statics.back();
					statics.pop_back();
				}

				images.clear();
				
				variables.global.clear(true);
				variables.runtime.clear(true);
				variables.loc.clear(true);

				Packages.list.clear();
			}

			void reload(uint32_t dpi = 96)
			{
				fonts.clear();
				fonts._dpi = dpi;
				fonts.add(glyph.name, Theme::SystemMetrics(SM_CXSMICON, dpi));
			}

			Expression *get_image(uint32_t id)
			{
				for(auto &si : images)
				{
					if(si.equals(id))
						return si.value;
				}
				return nullptr;
			}

			void add_image(std::vector<uint32_t> &&ids, Expression *e)
			{
				if(e)
				{
					for(auto &img : images)
					{
						if(img.equals(ids))
						{
							img.add(ids);
							img.value = e;
							return;
						}
					}

					images.emplace_back(std::move(ids), e);
				}
			}
		};
	}
}
