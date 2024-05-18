menu(where=sel.count>0 type='file|dir|drive|namespace|back' mode="multiple" title='Explorador de Arquivos' image=\uE253)
{
	menu(separator="after" title=title.copy_path image=icon.copy_path)
	{
		item(where=sel.count > 1 title='Copiar (@sel.count) items selected' cmd=command.copy(sel(false, "\n")))
		item(mode="single" title=@sel.path tip=sel.path cmd=command.copy(sel.path))
		item(mode="single" type='file' separator="before" find='.lnk' title='Abrir local do arquivo')
		separator
		item(mode="single" where=@sel.parent.len>3 title=sel.parent cmd=@command.copy(sel.parent))
		separator
		item(mode="single" type='file|dir|back.dir' title=sel.file.name cmd=command.copy(sel.file.name))
		item(mode="single" type='file' where=sel.file.len != sel.file.title.len title=@sel.file.title cmd=command.copy(sel.file.title))
		item(mode="single" type='file' where=sel.file.ext.len>0 title=sel.file.ext cmd=command.copy(sel.file.ext))
	}

	item(mode="single" type="file" title="Alterar extensão" image=\uE0B5 cmd=if(input("Alterar extensão", "Tipo de extensão"),
		io.rename(sel.path, path.join(sel.dir, sel.file.title + "." + input.result))))

	menu(separator="after" image=\uE290 title=title.select)
	{
		item(title="Todos" image=icon.select_all cmd=command.select_all)
		item(title="Invertido" image=icon.invert_selection cmd=command.invert_selection)
		item(title="Nenhum" image=icon.select_none cmd=command.select_none)
	}

	item(type='file|dir|back.dir|drive' title='Tornar-se proprietário' image=[\uE194,#f00] admin
		cmd args='/K takeown /f "@sel.path" @if(sel.type==1,null,"/r /d y") && icacls "@sel.path" /grant *S-1-5-32-544:F @if(sel.type==1,"/c /l","/t /c /l /q")')
	separator
	menu(title="Mostrar/Ocultar" image=icon.show_hidden_files)
	{
		item(title="Arquivos de sistema" image=inherit cmd='@command.togglehidden')
		item(title="Extensões de arquivos" image=icon.show_file_extensions cmd='@command.toggleext')
	}

	menu(type='file|dir|back.dir' mode="single" title='Atributos' image=icon.properties)
	{
		$atrr = io.attributes(sel.path)
		item(title='Oculto' checked=io.attribute.hidden(atrr)
			cmd args='/c ATTRIB @if(io.attribute.hidden(atrr),"-","+")H "@sel.path"' window=hidden)

		item(title='Sistema' checked=io.attribute.system(atrr)
			cmd args='/c ATTRIB @if(io.attribute.system(atrr),"-","+")S "@sel.path"' window=hidden)

		item(title='Somente leitura' checked=io.attribute.readonly(atrr)
			cmd args='/c ATTRIB @if(io.attribute.readonly(atrr),"-","+")R "@sel.path"' window=hidden)

		item(title='Arquivar' checked=io.attribute.archive(atrr)
			cmd args='/c ATTRIB @if(io.attribute.archive(atrr),"-","+")A "@sel.path"' window=hidden)
		separator
		item(title="Criado" keys=io.dt.created(sel.path, 'y/m/d') cmd=io.dt.created(sel.path,2000,1,1) vis=label)
		item(title="Modificado" keys=io.dt.modified(sel.path, 'y/m/d') cmd=io.dt.modified(sel.path,2000,1,1) vis=label)
		item(title="Acessado" keys=io.dt.accessed(sel.path, 'y/m/d') cmd=io.dt.accessed(sel.path,2000,1,1) vis=label)
	}

	menu(mode="single" type='file' find='.dll|.ocx' separator="before" title='Servidor de Registro' image=\uea86)
	{
		item(title='Registrar' admin cmd='regsvr32.exe' args='@sel.path.quote' invoke="multiple")
		item(title='Remover Registro' admin cmd='regsvr32.exe' args='/u @sel.path.quote' invoke="multiple")
	}

	menu(mode="single" type='back' expanded=true)
	{
		menu(separator="before" title='Nova Pasta' image=icon.new_folder)
		{
			item(title='Data Hora' cmd=io.dir.create(sys.datetime("dmyHMSs")))
			item(title='Guid' cmd=io.dir.create(str.guid))
		}

		menu(title='Novo Arquivo' image=icon.new_file)
		{
			$dt = sys.datetime("ymdHMSs")
			item(title='TXT' cmd=io.file.create('@(dt).txt', 'Olá Mundo!'))
			item(title='XML' cmd=io.file.create('@(dt).xml', '<root>Olá Mundo!</root>'))
			item(title='JSON' cmd=io.file.create('@(dt).json', '[]'))
			item(title='HTML' cmd=io.file.create('@(dt).html', "<html>\n\t<head>\n\t</head>\n\t<body>Olá Mundo!\n\t</body>\n</html>"))
		}
	}

	item(where=!wnd.is_desktop title=title.folder_options image=icon.folder_options cmd=command.folder_options)
}