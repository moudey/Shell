menu(type='*' where=(sel.count or wnd.is_taskbar or wnd.is_edit) title='终端' sep='top' image=icon.run_with_powershell)
{
	var{tip_run_admin=["\xE1A7 Pressione SHIFT para executar " + this.title + " como administrador", tip.warning, 1.0]}
	item(title=title.command_prompt tip=tip_run_admin admin=key.shift() image cmd='cmd.exe' args='/K TITLE Prompt de Comando &ver& PUSHD "@sel.dir"')
	item(title='Windows 终端' admin=@key.shift() tip=tip_run_admin image cmd='powershell.exe' args="-noexit -command Set-Location -Path '" + @sel.dir + "\\.'")
	
}