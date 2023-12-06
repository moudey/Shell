menu(type='*' where=window.is_taskbar||sel.count mode=mode.multiple title=title.go_to sep=sep.both image=\uE14A)
{
	menu(title='폴더' image=\uE1F4)
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
		item(title='바탕 화면' image=inherit cmd=user.desktop)
		item(title='다운로드' image=inherit cmd=user.downloads)
		item(title='사진' image=inherit cmd=user.pictures)
		item(title='문서' image=inherit cmd=user.documents)
		item(title='시작 메뉴' image=inherit cmd=user.startmenu)
		item(title='프로필' image=inherit cmd=user.dir)
		item(title='AppData' image=inherit cmd=user.appdata)
		item(title='Temp' image=inherit cmd=user.temp)
	}
	item(title=title.control_panel image=\uE0F3 cmd='shell:::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}')
	item(title='모든 제어판 항목' image=\uE0F3 cmd='shell:::{ED7BA470-8E54-465E-825C-99712043E01C}')
	item(title=title.run image=\uE14B cmd='shell:::{2559a1f3-21d7-11d4-bdaf-00c04f60b9f0}')
	menu(where=sys.ver.major >= 10 title=title.settings sep=sep.before image=\uE0F3)
	{
		// https://docs.microsoft.com/en-us/windows/uwp/launch-resume/launch-settings-app
		item(title='시스템' image=inherit cmd='ms-settings:')
		item(title='정보' image=inherit cmd='ms-settings:about')
		item(title='내 정보' image=inherit cmd='ms-settings:yourinfo')
		item(title='시스템 정보' image=inherit cmd-line='/K systeminfo')
		item(title='검색' cmd='search-ms:' image=inherit)
		item(title='usb' image=inherit cmd='ms-settings:usb')
		item(title='windows 업데이트' image=inherit cmd='ms-settings:windowsupdate')
		item(title='windows-defender' image=inherit cmd='ms-settings:windowsdefender')
		menu(title='앱' image=inherit)
		{
			item(title='앱 기능' image=inherit cmd='ms-settings:appsfeatures')
			item(title='기본 앱' image=inherit cmd='ms-settings:defaultapps')
			item(title='선택 기능' image=inherit cmd='ms-settings:optionalfeatures')
			item(title='시작' image=inherit cmd='ms-settings:startup')
		}
		menu(title='개인 설정' image=inherit)
		{
			item(title='개인 설정' image=inherit cmd='ms-settings:personalization')
			item(title='잠금 화면' image=inherit cmd='ms-settings:lockscreen')
			item(title='배경' image=inherit cmd='ms-settings:personalization-background')
			item(title='색상' image=inherit cmd='ms-settings:colors')
			item(title='테마' image=inherit cmd='ms-settings:themes')
			item(title='시작' image=inherit cmd='ms-settings:personalization-start')
			item(title='작업 표시줄' image=inherit cmd='ms-settings:taskbar')
		}
		menu(title='네트워크' image=inherit)
		{
			item(title='상태' image=inherit cmd='ms-settings:network-status')
			item(title='이더넷' image=inherit cmd='ms-settings:network-ethernet')
			item(title='연결' image=inherit cmd='shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}')
		}
	}
}
