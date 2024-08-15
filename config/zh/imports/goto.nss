menu(type='*' where=window.is_taskbar||sel.count mode=mode.multiple title=title.go_to sep=sep.both image=\uE14A)
{
	menu(title='文件夹' image=\uE1F4)
	{
		item(title='Windows' image=inherit cmd=sys.dir)
		item(title='System32' image=inherit cmd=sys.bin)
		item(title='Program Files' image=inherit cmd=sys.prog)
		item(title='Program Files (x86)' image=inherit cmd=sys.prog32)
		item(title='ProgramData' image=inherit cmd=sys.programdata)
		item(title='Applications' image=inherit cmd='shell:appsfolder')
		item(title='用户' image=inherit cmd=sys.users)
		separator
		//item(title='@user.name@@@sys.name' vis=label)
		item(title='桌面' image=inherit cmd=user.desktop)
		item(title='下载' image=inherit cmd=user.downloads)
		item(title='图片' image=inherit cmd=user.pictures)
		item(title='文档' image=inherit cmd=user.documents)
		item(title='「开始」菜单' image=inherit cmd=user.startmenu)
		item(title='主文件夹' image=inherit cmd=user.dir)
		item(title='AppData' image=inherit cmd=user.appdata)
		item(title='临时文件夹' image=inherit cmd=user.temp)
	}
	item(title=title.control_panel image=\uE0F3 cmd='shell:::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}')
	item(title='所有任务' image=\uE0F3 cmd='shell:::{ED7BA470-8E54-465E-825C-99712043E01C}')
	item(title=title.run image=\uE14B cmd='shell:::{2559a1f3-21d7-11d4-bdaf-00c04f60b9f0}')
	menu(where=sys.ver.major >= 10 title=title.settings sep=sep.before image=\uE0F3)
	{
		// https://docs.microsoft.com/en-us/windows/uwp/launch-resume/launch-settings-app
		item(title='主页' image=inherit cmd='ms-settings:')
		item(title='系统信息' image=inherit cmd='ms-settings:about')
		item(title='账户信息' image=inherit cmd='ms-settings:yourinfo')
		item(title='详细系统信息' image=inherit cmd-line='/K systeminfo')
		item(title='搜索' cmd='search-ms:' image=inherit)
		item(title='USB' image=inherit cmd='ms-settings:usb')
		item(title='Windows 更新' image=inherit cmd='ms-settings:windowsupdate')
		item(title='Windows 安全中心' image=inherit cmd='ms-settings:windowsdefender')
		menu(title='应用' image=inherit)
		{
			item(title='安装的应用' image=inherit cmd='ms-settings:appsfeatures')
			item(title='默认应用' image=inherit cmd='ms-settings:defaultapps')
			item(title='可选功能' image=inherit cmd='ms-settings:optionalfeatures')
			item(title='启动' image=inherit cmd='ms-settings:startupapps')
		}
		menu(title='个性化' image=inherit)
		{
			item(title='个性化首页' image=inherit cmd='ms-settings:personalization')
			item(title='锁屏界面' image=inherit cmd='ms-settings:lockscreen')
			item(title='背景' image=inherit cmd='ms-settings:personalization-background')
			item(title='颜色' image=inherit cmd='ms-settings:colors')
			item(title='主题' image=inherit cmd='ms-settings:themes')
			item(title='开始' image=inherit cmd='ms-settings:personalization-start')
			item(title='任务栏' image=inherit cmd='ms-settings:taskbar')
		}
		menu(title='网络' image=inherit)
		{
			item(title='网络和 Internet' image=inherit cmd='ms-settings:network-status')
			item(title='以太网' image=inherit cmd='ms-settings:network-ethernet')
			item(title='网络连接' image=inherit cmd='shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}')
		}
	}
}