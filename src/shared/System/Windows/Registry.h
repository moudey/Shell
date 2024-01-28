#pragma once

namespace Nilesoft
{

#define HKCR	HKEY_CLASSES_ROOT
#define HKCU	HKEY_CURRENT_USER
#define HKLM	HKEY_LOCAL_MACHINE
#define HKU		HKEY_USERS
#define HKPD	HKEY_PERFORMANCE_DATA
#define HKCC	HKEY_CURRENT_CONFIG
#define HKDD	HKEY_DYN_DATA
	/*
	// RAII-ish wrapper for HKEYs
	class HKEYWrapper
	{
		HKEY key;
		void Close()
		{
			if(key)
			{
				::RegCloseKey(key);
				key = NULL;
			}
		}
	public:
		HKEYWrapper() : key(NULL) { }
		~HKEYWrapper() { Close(); }

		operator HKEY const() { return key; }
		HKEY* operator&() { return &key; }
	};

	template<typename T>
	static LONG QueryFromDWORD(HKEY key, const wchar_t* value, T& dest)
	{
		DWORD v = 0;
		DWORD dataSize = sizeof(v);
		LONG result = RegGetValueW(key, nullptr, value, RRF_RT_REG_DWORD, nullptr, &v, &dataSize);
		if(result == ERROR_SUCCESS)
		{
			dest = static_cast<T> (v);
		}
		return result;
	}*/

	struct auto_regkey 
	{
		HKEY handle{};
		auto_regkey() {}
		auto_regkey(HKEY hkey) : handle{ hkey } {}
		~auto_regkey() { close(); }

		operator HKEY() const { return handle; }
		operator HKEY*() { return &handle; }
		explicit operator bool() const { return handle != nullptr; }

		HKEY get() const { return handle; }

		void close()
		{
			if(handle)
			{
				::RegCloseKey(handle);
				handle = nullptr;
			}
		}

		long open(LPCWSTR lpSubKey, HKEY *phkResult, REGSAM samDesired = 0) {
			return open(handle, lpSubKey, 0, samDesired, phkResult);
		}

		long delete_key(const wchar_t *lpSubKey, REGSAM samDesired = 0)
		{
			return delete_key(handle, lpSubKey, samDesired);
		}

		static long delete_key(HKEY hKey, const wchar_t *lpSubKey, REGSAM samDesired = 0)
		{
			return ::RegDeleteKeyExW(hKey, lpSubKey, samDesired, 0);
		}

		long set_value(const wchar_t *lpValueName, DWORD dwType, const uint8_t *lpData, DWORD cbData)
		{
			return set_value(handle, lpValueName, dwType, lpData, cbData);
		}

		long set_value(const wchar_t *lpValueName, const wchar_t *data, DWORD length, DWORD dwType = REG_SZ)
		{
			return set_value(lpValueName, dwType, reinterpret_cast<const uint8_t *>(data), length * sizeof(wchar_t));
		}

		long create_key(const wchar_t *lpSubKey, HKEY *phkResult, REGSAM samDesired = 0)
		{
			return create_key(handle, lpSubKey, nullptr,
							  0, samDesired, nullptr,
							  phkResult, nullptr);
		}

		static long open(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, HKEY *phkResult) {
			return ::RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
		}

		static long set_value(HKEY hKey, const wchar_t *lpValueName, DWORD dwType, const uint8_t *lpData, DWORD cbData)
		{
			return ::RegSetValueExW(hKey, lpValueName, 0, dwType, lpData, cbData);
		}

		static long create_key(HKEY hKey, const wchar_t *lpSubKey, wchar_t *lpClass,
							   DWORD dwOptions, REGSAM samDesired,
							   const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							   HKEY *phkResult, DWORD *lpdwDisposition)
		{
			return ::RegCreateKeyExW(hKey, lpSubKey, 0, lpClass, dwOptions,
									 samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
		}

		static long exists_key(HKEY hKey, const wchar_t *lpSubKey)
		{
			return ::RegQueryValueExW(hKey, lpSubKey, nullptr, 0, nullptr, nullptr);
		}
	};

	class RegistryKey
	{
	/*public:
		static constexpr const wchar_t* KEYNAMES[7]
		{
			L"HKEY_CLASSES_ROOT",
			L"HKEY_CURRENT_USER",
			L"HKEY_LOCAL_MACHINE",
			L"HKEY_USERS",
			L"HKEY_PERFORMANCE_DATA",
			L"HKEY_CURRENT_CONFIG",
			L"HKEY_DYN_DATA"
		};

		static constexpr const wchar_t* KEYNAMES_SHORT[7]
		{
			L"HKCR",
			L"HKCU",
			L"HKLM",
			L"HKU",
			L"HKPD",
			L"HKCC",
			L"HKDD"
		};
		*/
	private:
		static bool is_system_key(HKEY hkey)
		{
			auto index = ULONG_PTR(hkey) & 0x0FFFFFFF;
			return (index >= 0 && index < 7);
			/*return hkey == HKEY_CLASSES_ROOT ||
				hkey == HKEY_CURRENT_USER ||
				hkey == HKEY_LOCAL_MACHINE ||
				hkey == HKEY_USERS ||
				hkey == HKEY_PERFORMANCE_DATA ||
				hkey == HKEY_CURRENT_CONFIG ||
				hkey == HKEY_DYN_DATA;*/
		}

		static bool is_valid(HKEY hKey)
		{
			if(is_system_key(hKey))
				return true;
			return (ULONG_PTR(hKey) & 0xFFFFFFF0) != 0x80000000;
		}
		
		static bool is_valid(HKEY hKey, const wchar_t* name)
		{
			if(hKey)
				return is_valid(hKey) && name && *name;
			return false;
		}
	
		static uint32_t get_key_access(bool writable)
		{
			return writable ? (KEY_WRITE | KEY_READ) : KEY_READ;
		}

	public:
		static const int MaxKeyLength = 255;
		static const int MaxValueLength = 16383;

		static RegistryKey FromHandle(HKEY handle, uint32_t view);
		static RegistryKey OpenBaseKey(HKEY hkey, bool writable = true, uint32_t view = 0);

	protected:

		HKEY m_hkey = nullptr;
		uint32_t m_view = 0;
		bool m_writable = false;
		bool m_systemkey = false;
		bool m_autoclose = false;
		int m_ref = 0;

		RegistryKey(HKEY hkeybase, bool writable, uint32_t view, bool systemkey, bool autoclose);

	public:
		~RegistryKey();
		RegistryKey(const nullptr_t&);
		//RegKey(const RegKey&)=delete;
		RegistryKey(const RegistryKey & key);
		RegistryKey(HKEY hkeybase, bool writable);
		RegistryKey(HKEY hkeybase, bool writable, uint32_t view);


		operator HKEY() const;
		operator bool() const { return m_hkey != nullptr; }

		RegistryKey & operator=(const nullptr_t&);

		bool operator ==(const nullptr_t&);
		bool operator !=(const nullptr_t&);

		bool operator ==(const RegistryKey & key);
		bool operator !=(const RegistryKey & key);

		RegistryKey CreateSubKey(const wchar_t* subkey, bool autoclose = false, uint32_t options = 0) const;
		RegistryKey OpenSubKey(const wchar_t* subkey, bool autoclose = false, bool writable = false, uint32_t view = 0xffffffff) const;
		bool OpenSubKey(HKEY root_key, const wchar_t *subkey, bool autoclose = false, bool writable = false, uint32_t view = 0xffffffff);

		//wchar_t* GetSTRING(const wchar_t* name, bool expand = false) const;
		uint32_t GetString(const wchar_t* name, wchar_t* * value, bool expand = false) const;
		uint32_t GetString(const wchar_t* name, wchar_t* value, uint32_t max_length, bool expand = false) const;

		Text::string ReadString(const wchar_t *name) const;
		bool ReadString(const wchar_t *name, Text::string &value) const;
		Text::string GetString(const wchar_t* name, bool expand = false) const;
	//	bool GetString(const wchar_t *name, Text::string &value, bool expand = false) const;


		int GetInt(const wchar_t* name, int defualt_value = 0) const;
		bool GetInt(const wchar_t* name, int* value) const;

		uint32_t GetDWord(const wchar_t* name, uint32_t defualt_value = 0) const;
		uint64_t GetQWord(const wchar_t* name, uint64_t defualt_value = 0) const;

		bool GetValue(const wchar_t* name, uint32_t type, LPBYTE data, uint32_t * datasize) const;
		//int GetValue(const wchar_t* name, LPVOID *data, uint32_t* datasize) const;

		uint32_t ValueSize(const wchar_t* name) const;
		uint32_t ValueType(const wchar_t* name) const;
		uint32_t ValueCount() const;

		bool get_value(const wchar_t *name, uint32_t *type, uint32_t *cbdata = nullptr, void* data = nullptr) const;

		uint32_t SubKeyCount() const;
		//wchar_t** GetSubKeyNames() const;

		bool SetString(const wchar_t* name, const wchar_t* value, bool expand = false) const;
		bool SetString(const wchar_t* name, const wchar_t* value, size_t length, bool expand = false) const;
		bool SetString(const wchar_t* subkey, const wchar_t* name, const wchar_t* value, bool expand = false) const;
		bool SetString(const wchar_t* subkey, const wchar_t* name, const wchar_t* value, size_t length, bool expand = false) const;


		bool SetInt(const wchar_t* name, uint32_t value) const;
		bool SetInt(const wchar_t* subkey, const wchar_t* name, uint32_t value) const;
		bool SetInt64(const wchar_t* name, uint64_t value) const;
		bool SetInt64(const wchar_t* subkey, const wchar_t* name, uint64_t value) const;

		bool SetValue(const wchar_t* name, uint32_t type, LPBYTE data, size_t size) const;

		bool DeleteSubKeyTree(const wchar_t* subKey) const;
		bool DeleteSubKey(const wchar_t* subKey) const;
		bool DeleteValue(const wchar_t* name) const;

		bool ExistsValue(const wchar_t* name) const;
		bool Exists(const wchar_t* subKey) const;

		const HKEY Handle();
		void Flush() const;
		void Close();
		bool IsWritable() const;
		uint32_t View() const;
		void auto_close(bool set_auto_close = true);

	};

	class Registry
	{
		static bool is_valid(HKEY hkey)
		{
			auto index = ULONG_PTR(hkey) & 0x0FFFFFFF;
			if(index >= 0 && index < 7)
				return true;
			return (ULONG_PTR(hkey) & 0xFFFFFFF0) != 0x80000000;
		}

		static bool is_valid(HKEY hKey, const wchar_t* name)
		{
			return is_valid(hKey) && name &&* name;
		}

	public:
		static const RegistryKey ClassesRoot;
		static const RegistryKey CurrentUser;
		static const RegistryKey LocalMachine;

		static bool DeleteKeyValue(HKEY hkey, const wchar_t* subKey, const wchar_t* name);
		static bool SetKeyValue(HKEY hkey, const wchar_t* subKey, const wchar_t* name,
								   uint32_t type, LPBYTE value, size_t value_size);

		static bool SetKeyValue(HKEY hkey, const wchar_t* subKey, const wchar_t* name, int value);
		static bool SetKeyValue(HKEY hkey, const wchar_t* subKey, const wchar_t* name, uint32_t value);
		static bool SetKeyValue(HKEY hkey, const wchar_t* subKey, const wchar_t* name, uint64_t value);

		static bool SetKeyValue(HKEY hkey, const wchar_t* subKey, const wchar_t* name,
								  const wchar_t* value, size_t value_size, bool expand = false);

		static bool SetKeyValue(HKEY hkey, const wchar_t* subKey, const wchar_t* name,
								  const wchar_t* value, bool expand = false);

		static bool Exists(HKEY hKeyRoot, const wchar_t* subKey, uint32_t view = 0);
		static bool ExistsValue(HKEY hKeyRoot, const wchar_t* subKey, const wchar_t* name, uint32_t view = 0);
		static bool DeleteSubKey(HKEY hKeyRoot, const wchar_t* subKey);


	};
}