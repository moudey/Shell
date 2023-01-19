menu(type='*' where=(sel.count or wnd.is_taskbar or wnd.is_edit) title=title.terminal sep='top' image=icon.run_with_powershell)
{
	item(title=title.command_prompt admin=key.shift() image cmd='cmd.exe' args='/K TITLE Command Prompt &ver& PUSHD "@sel.dir"')
	item(title=title.windows_powershell admin=@key.shift() image cmd='powershell.exe' args='-noexit -command Set-Location -Path "@sel.dir\."')
	item(where=sys.ver.major >= 10 title=title.Windows_Terminal admin=key.shift() image='@package.path("WindowsTerminal")\WindowsTerminal.exe' cmd='wt.exe' arg='-d "@sel.path\."')
}