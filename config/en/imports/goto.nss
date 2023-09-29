menu(type='*' where=window.is_taskbar||sel.count mode=mode.multiple title=title.go_to sep=sep.both image=\uE14A)
{
	menu(title='Folder' image=\uE1F4)
	{
		item(title='Windows' image=inherit cmd=sys.dir)
		item(title='System' image=inherit cmd=sys.bin)
		item(title='Program Files' image=inherit cmd=sys.prog)
		item(title='Program Files x86' image=inherit cmd=sys.prog32)
		item(title='ProgramData' image=inherit cmd=sys.programdata)
		item(title='Applications' image=inherit cmd='shell:appsfolder')
		item(title='Users' image=inherit cmd=sys.users)
		separator
		//item(title='@user.name@@@sys.name' vis=label)
		item(title='Desktop' image=inherit cmd=user.desktop)
		item(title='Downloads' image=inherit cmd=user.downloads)
		item(title='Pictures' image=inherit cmd=user.pictures)
		item(title='Documents' image=inherit cmd=user.documents)
		item(title='Startmenu' image=inherit cmd=user.startmenu)
		item(title='Profile' image=inherit cmd=user.dir)
		item(title='AppData' image=inherit cmd=user.appdata)
		item(title='Temp' image=inherit cmd=user.temp)
	}
	item(title=title.control_panel image=\uE0F3 cmd='shell:::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}')
	item(title='All Control Panel Items' image=\uE0F3 cmd='shell:::{ED7BA470-8E54-465E-825C-99712043E01C}')
	item(title=title.run image=\uE14B cmd='shell:::{2559a1f3-21d7-11d4-bdaf-00c04f60b9f0}')
	menu(where=sys.ver.major >= 10 title=title.settings sep=sep.before image=\uE0F3)
	{
		// https://docs.microsoft.com/en-us/windows/uwp/launch-resume/launch-settings-app
		item(title='system' image=inherit cmd='ms-settings:')
		item(title='about' image=inherit cmd='ms-settings:about')
		item(title='your-info' image=inherit cmd='ms-settings:yourinfo')
		item(title='system-info' image=inherit cmd-line='/K systeminfo')
		item(title='search' cmd='search-ms:' image=inherit)
		item(title='usb' image=inherit cmd='ms-settings:usb')
		item(title='windows-update' image=inherit cmd='ms-settings:windowsupdate')
		item(title='windows-defender' image=inherit cmd='ms-settings:windowsdefender')
		menu(title='apps' image=inherit)
		{
			item(title='apps-features' image=inherit cmd='ms-settings:appsfeatures')
			item(title='default-apps' image=inherit cmd='ms-settings:defaultapps')
			item(title='optional-features' image=inherit cmd='ms-settings:optionalfeatures')
			item(title='startup' image=inherit cmd='ms-settings:startupapps')
		}
		menu(title='personalization' image=inherit)
		{
			item(title='personalization' image=inherit cmd='ms-settings:personalization')
			item(title='lockscreen' image=inherit cmd='ms-settings:lockscreen')
			item(title='background' image=inherit cmd='ms-settings:personalization-background')
			item(title='colors' image=inherit cmd='ms-settings:colors')
			item(title='themes' image=inherit cmd='ms-settings:themes')
			item(title='start' image=inherit cmd='ms-settings:personalization-start')
			item(title='taskbar' image=inherit cmd='ms-settings:taskbar')
		}
		menu(title='network' image=inherit)
		{
			item(title='status' image=inherit cmd='ms-settings:network-status')
			item(title='ethernet' image=inherit cmd='ms-settings:network-ethernet')
			item(title='connections' image=inherit cmd='shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}')
		}
	}
}