menu(type='*' where=(sel.count or wnd.is_taskbar or wnd.is_edit) title=title.terminal sep='top' image=icon.run_with_powershell)
{
	var{tip_run_admin=["\xE1A7 Pressione SHIFT para executar " + this.title + " como administrador", tip.warning, 1.0]}
	item(title=title.command_prompt tip=tip_run_admin admin=key.shift() image cmd='cmd.exe' args='/K TITLE Prompt de Comando &ver& PUSHD "@sel.dir"')
	item(title=title.windows_powershell admin=@key.shift() tip=tip_run_admin image cmd='powershell.exe' args='-noexit -command Set-Location -Path "@sel.dir\."')
	item(where=sys.ver.major >= 10 title=title.Windows_Terminal tip=tip_run_admin admin=key.shift() image='@package.path("WindowsTerminal")\WindowsTerminal.exe' cmd='wt.exe' arg='-d "@sel.path\."')
}