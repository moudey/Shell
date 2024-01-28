#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		enum FSO : int32_t
		{
			FSO_TITLEBAR,
			FSO_EDIT,
			FSO_START,
			FSO_TASKBAR,

			FSO_DESKTOP,
			FSO_DIRECTORY,
			FSO_FILE,
			FSO_DRIVE,
			FSO_NAMESPACE,

			FSO_DRIVE_FIXED,
			FSO_DRIVE_VHD,
			FSO_DRIVE_USB,
			FSO_DRIVE_DVD,
			FSO_DRIVE_REMOVABLE,
			FSO_DRIVE_REMOTE,

			FSO_NAMESPACE_COMPUTER,
			FSO_NAMESPACE_RECYCLEBIN,

			FSO_BACK,
			FSO_BACK_DIRECTORY,
			FSO_BACK_NAMESPACE,
			FSO_BACK_NAMESPACE_COMPUTER,
			FSO_BACK_NAMESPACE_RECYCLEBIN,
			
			FSO_BACK_DRIVE,
			FSO_BACK_DRIVE_FIXED,
			FSO_BACK_DRIVE_USB,
			FSO_BACK_DRIVE_VHD,
			FSO_BACK_DRIVE_DVD,
			FSO_BACK_DRIVE_REMOVABLE,
			FSO_BACK_DRIVE_REMOTE,

			FSO_SIZE,
			FSO_NAMESPACE_LIBRARIES,
			FSO_COUNT,
			FSO_MAX
		};

		struct FSO_TYPE
		{
			int32_t type;
			const wchar_t *name;
		};

		const FSO_TYPE fs_types[FSO_SIZE] =
		{
			 {FSO_TITLEBAR,L"FSO_TITLEBAR"},
			 {FSO_EDIT,L"FSO_EDIT"},
			 {FSO_START,L"FSO_START"},
			 {FSO_TASKBAR,L"FSO_TASKBAR"},

			 {FSO_DESKTOP,L"FSO_DESKTOP" },
			 {FSO_DIRECTORY,  L"FSO_DIRECTORY"},
			 {FSO_FILE,  L"FSO_FILE"},

			 {FSO_DRIVE, L"FSO_DRIVE"},
			 {FSO_DRIVE_FIXED, L"FSO_DRIVE_FIXED"},
			 {FSO_DRIVE_VHD,L"FSO_DRIVE_VHD" },
			 {FSO_DRIVE_USB,L"FSO_DRIVE_USB"},
			 {FSO_DRIVE_DVD,L"FSO_DRIVE_DVD"},
			 {FSO_DRIVE_REMOVABLE,L"FSO_DRIVE_REMOVABLE"},
			 {FSO_DRIVE_REMOTE,L"FSO_DRIVE_REMOTE"},

			 {FSO_NAMESPACE,L"FSO_NAMESPACE"},
			 {FSO_NAMESPACE_COMPUTER,L"FSO_NAMESPACE_COMPUTER"},
			 {FSO_NAMESPACE_RECYCLEBIN,L"FSO_NAMESPACE_RECYCLEBIN"},

			 {FSO_BACK,L"FSO_BACK"},
			 {FSO_BACK_DIRECTORY,L"FSO_BACK_DIRECTORY"},
			 {FSO_BACK_NAMESPACE,L"FSO_BACK_NAMESPACE"},
			 {FSO_BACK_NAMESPACE_COMPUTER,L"FSO_BACK_NAMESPACE_COMPUTER"},
			 {FSO_BACK_NAMESPACE_RECYCLEBIN,L"FSO_BACK_NAMESPACE_RECYCLEBIN"},


			 {FSO_BACK_DRIVE,L"FSO_BACK_DRIVE"},
			 {FSO_BACK_DRIVE_FIXED,L"FSO_BACK_DRIVE_FIXED"},
			 {FSO_BACK_DRIVE_VHD,L"FSO_BACK_DRIVE_VHD"},
			 {FSO_BACK_DRIVE_USB,L"FSO_BACK_DRIVE_USB"},
			 {FSO_BACK_DRIVE_DVD,L"FSO_BACK_DRIVE_DVD"},
			 {FSO_BACK_DRIVE_REMOVABLE,L"FSO_BACK_DRIVE_REMOVABLE"},
			 {FSO_BACK_DRIVE_REMOTE,L"FSO_BACK_DRIVE_REMOTE"}
		};


		//FileSystemObject
		class FileSystemObjects
		{
		public:
			int32_t Types[FSO_MAX]{ FALSE };
			int32_t count = FSO_SIZE;
			int32_t exclude = 0;
			bool defined{};
			bool any_types{};
			static constexpr int32_t EXCLUDE = -1;

		public:
			FileSystemObjects() noexcept
			{
			}

			FileSystemObjects(BOOL any)
			{
				this->set(any);
			}

			~FileSystemObjects()
			{
			}

			FileSystemObjects &operator=(FileSystemObjects const &fso)
			{
				::memcpy(this, &fso, sizeof(FileSystemObjects));
				return *this;
			}

			void set(BOOL any = TRUE)
			{
				count = FSO_SIZE - 1;
				::memset(&Types, any, sizeof(Types));
			}

			bool excluded(int32_t t) const
			{
				return Types[t] == EXCLUDE;
			}

			bool excluded(std::initializer_list<uint32_t> list) const
			{
				for(auto t : list)
				{
					if(Types[t] == EXCLUDE)
						return true;
				}
				return false;
			}

			bool assgined(uint32_t t) const { return Types[t] > 0; }

			bool assgined(std::initializer_list<uint32_t> types) const
			{
				int32_t ret = 0;
				for(auto t : types) if(assgined(t)) ret++;
				return ret > 0;
			}

			bool is_all() const
			{
				for(int32_t t = 0; t < FSO_SIZE; t++)
					if(!assgined(t)) return false;
				return true;
			}

			bool is_edit() const { return assgined(FSO_EDIT); }
			bool is_start() const { return assgined(FSO_START); }
			bool is_taskbar() const { return assgined(FSO_TASKBAR); }
			bool is_desktop() const { return assgined(FSO_DESKTOP); }
			//is background
			bool is_back() const
			{
				return assgined({ 
					FSO_BACK,
					FSO_BACK_DIRECTORY,
					FSO_BACK_DRIVE, FSO_BACK_DRIVE_FIXED, FSO_BACK_DRIVE_VHD, FSO_BACK_DRIVE_USB, FSO_BACK_DRIVE_DVD, FSO_BACK_DRIVE_REMOVABLE,
					FSO_BACK_NAMESPACE,FSO_BACK_NAMESPACE, FSO_BACK_NAMESPACE_COMPUTER, FSO_BACK_NAMESPACE_RECYCLEBIN
				});
			}

			bool is_file() const { return assgined(FSO_FILE); }
			bool is_directory() const { return assgined(FSO_DIRECTORY); }

			bool is_drives() const
			{
				return assgined({ FSO_DRIVE,FSO_DRIVE_FIXED,FSO_DRIVE_USB,
							   FSO_DRIVE_DVD,FSO_DRIVE_REMOVABLE });
			}

			bool is_namespaces() const
			{
				return assgined({
					FSO_NAMESPACE, 
					FSO_NAMESPACE_COMPUTER, 
					FSO_NAMESPACE_RECYCLEBIN,
					FSO_BACK_NAMESPACE,
					FSO_BACK_NAMESPACE_COMPUTER, 
					FSO_BACK_NAMESPACE_RECYCLEBIN
				});
			}

			static bool is_type_back(uint32_t id)
			{
				switch(id)
				{
					case IDENT_TYPE_DIR:
					case IDENT_TYPE_DIRECTORY:
					case IDENT_TYPE_ROOT:
					case IDENT_TYPE_DRIVE:
					case IDENT_TYPE_FIXED:
					case IDENT_TYPE_USB:
					case IDENT_TYPE_VHD:
					case IDENT_TYPE_DVD:
					case IDENT_TYPE_REMOVABLE:
					case IDENT_TYPE_NAMESPACE:
						return true;
				}
				return false;
			}
		};
	}
}
