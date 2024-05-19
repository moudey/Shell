menu(type='*' where=window.is_taskbar||sel.count mode=mode.multiple title=title.go_to sep=sep.both image=\uE14A)
{
	menu(title='Pasta' image=\uE1F4)
	{
		item(title='Windows' image=inherit cmd=sys.dir)
		item(title='System32' image=inherit cmd=sys.bin)
		item(title='Arquivos de Programas' image=inherit cmd=sys.prog)
		item(title='Arquivos de Programas x86' image=inherit cmd=sys.prog32)
		item(title='ProgramData' image=inherit cmd=sys.programdata)
		item(title='Aplicativos' image=inherit cmd='shell:appsfolder')
		item(title='Usuários' image=inherit cmd=sys.users)
		separator
		//item(title='@user.name@@@sys.name' vis=label)
		item(title='Área de Trabalho' image=inherit cmd=user.desktop)
		item(title='Downloads' image=inherit cmd=user.downloads)
		item(title='Imagens' image=inherit cmd=user.pictures)
		item(title='Documentos' image=inherit cmd=user.documents)
		item(title='Menu Iniciar' image=inherit cmd=user.startmenu)
		item(title='Perfil' image=inherit cmd=user.dir)
		item(title='AppData' image=inherit cmd=user.appdata)
		item(title='Temp' image=inherit cmd=user.temp)
	}
	item(title=title.control_panel image=\uE0F3 cmd='shell:::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}')
	item(title='Todas as Tarefas' image=\uE0F3 cmd='shell:::{ED7BA470-8E54-465E-825C-99712043E01C}')
	item(title=title.run image=\uE14B cmd='shell:::{2559a1f3-21d7-11d4-bdaf-00c04f60b9f0}')
	menu(where=sys.ver.major >= 10 title=title.settings sep=sep.before image=\uE0F3)
	{
		// https://docs.microsoft.com/en-us/windows/uwp/launch-resume/launch-settings-app
		item(title='Sistema' image=inherit cmd='ms-settings:')
		item(title='Sobre' image=inherit cmd='ms-settings:about')
		item(title='Suas informações' image=inherit cmd='ms-settings:yourinfo')
		item(title='Informações do sistema' image=inherit cmd-line='/K systeminfo')
		item(title='Pesquisa' cmd='search-ms:' image=inherit)
		item(title='USB' image=inherit cmd='ms-settings:usb')
		item(title='Windows Update' image=inherit cmd='ms-settings:windowsupdate')
		item(title='Segurança do Windows' image=inherit cmd='ms-settings:windowsdefender')
		menu(title='Aplicativos' image=inherit)
		{
			item(title='Aplicativos instalados' image=inherit cmd='ms-settings:appsfeatures')
			item(title='Aplicativos padrão' image=inherit cmd='ms-settings:defaultapps')
			item(title='Recursos opcionais' image=inherit cmd='ms-settings:optionalfeatures')
			item(title='Inicialização' image=inherit cmd='ms-settings:startupapps')
		}
		menu(title='Personalização' image=inherit)
		{
			item(title='Personalização' image=inherit cmd='ms-settings:personalization')
			item(title='Tela de bloqueio' image=inherit cmd='ms-settings:lockscreen')
			item(title='Tela de fundo' image=inherit cmd='ms-settings:personalization-background')
			item(title='Cores' image=inherit cmd='ms-settings:colors')
			item(title='Temas' image=inherit cmd='ms-settings:themes')
			item(title='Iniciar' image=inherit cmd='ms-settings:personalization-start')
			item(title='Barra de Tarefas' image=inherit cmd='ms-settings:taskbar')
		}
		menu(title='Rede e Internet' image=inherit)
		{
			item(title='Status' image=inherit cmd='ms-settings:network-status')
			item(title='Ethernet' image=inherit cmd='ms-settings:network-ethernet')
			item(title='Conexões de Rede' image=inherit cmd='shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}')
		}
	}
}