menu(type='*' where=window.is_taskbar||sel.count mode=mode.multiple title=title.go_to sep=sep.both image=\uE14A)
{
	menu(title='Klasör' image=\uE1F4)
	{
		item(title='Windows' image=inherit cmd=sys.dir)
		item(title='Sistem' image=inherit cmd=sys.bin)
		item(title='Program Dosyaları' image=inherit cmd=sys.prog)
		item(title='Program Dosyaları x86' image=inherit cmd=sys.prog32)
		item(title='ProgramData' image=inherit cmd=sys.programdata)
		item(title='Uygulamalar' image=inherit cmd='shell:appsfolder')
		item(title='Kullanıcılar' image=inherit cmd=sys.users)
		separator
		//item(title='@user.name@@@sys.name' vis=label)
		item(title='Masaüstü' image=inherit cmd=user.desktop)
		item(title='İndirilenler' image=inherit cmd=user.downloads)
		item(title='Resimler' image=inherit cmd=user.pictures)
		item(title='Belgeler' image=inherit cmd=user.documents)
		item(title='Başlat Menüsü' image=inherit cmd=user.startmenu)
		item(title='Profil' image=inherit cmd=user.dir)
		item(title='AppData' image=inherit cmd=user.appdata)
		item(title='Temp' image=inherit cmd=user.temp)
	}
	item(title=title.control_panel image=\uE0F3 cmd='shell:::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}')
	item(title='Tüm Denetim Masası Öğeleri' image=\uE0F3 cmd='shell:::{ED7BA470-8E54-465E-825C-99712043E01C}')
	item(title=title.run image=\uE14B cmd='shell:::{2559a1f3-21d7-11d4-bdaf-00c04f60b9f0}')
	menu(where=sys.ver.major >= 10 title=title.settings sep=sep.before image=\uE0F3)
	{
		// https://docs.microsoft.com/en-us/windows/uwp/launch-resume/launch-settings-app
		item(title='sistem' image=inherit cmd='ms-settings:')
		item(title='hakkında' image=inherit cmd='ms-settings:about')
		item(title='bilgileriniz' image=inherit cmd='ms-settings:yourinfo')
		item(title='sistem bilgisi' image=inherit cmd-line='/K systeminfo')
		item(title='arama' cmd='search-ms:' image=inherit)
		item(title='usb' image=inherit cmd='ms-settings:usb')
		item(title='windows güncelleme' image=inherit cmd='ms-settings:windowsupdate')
		item(title='windows defender' image=inherit cmd='ms-settings:windowsdefender')
		menu(title='uygulamalar' image=inherit)
		{
			item(title='uygulamalar ve özellikler' image=inherit cmd='ms-settings:appsfeatures')
			item(title='varsayılan uygulamalar' image=inherit cmd='ms-settings:defaultapps')
			item(title='isteğe bağlı özellikler' image=inherit cmd='ms-settings:optionalfeatures')
			item(title='başlangıç' image=inherit cmd='ms-settings:startupapps')
		}
		menu(title='kişiselleştirme' image=inherit)
		{
			item(title='kişiselleştirme' image=inherit cmd='ms-settings:personalization')
			item(title='kilit ekranı' image=inherit cmd='ms-settings:lockscreen')
			item(title='arka plan' image=inherit cmd='ms-settings:personalization-background')
			item(title='renkler' image=inherit cmd='ms-settings:colors')
			item(title='temalar' image=inherit cmd='ms-settings:themes')
			item(title='başlat menüsü' image=inherit cmd='ms-settings:personalization-start')
			item(title='görev çubuğu' image=inherit cmd='ms-settings:taskbar')
		}
		menu(title='ağ' image=inherit)
		{
			item(title='durum' image=inherit cmd='ms-settings:network-status')
			item(title='ethernet' image=inherit cmd='ms-settings:network-ethernet')
			item(title='bağlantılar' image=inherit cmd='shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}')
		}
	}
}
