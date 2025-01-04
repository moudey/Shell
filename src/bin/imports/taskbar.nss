menu(type="taskbar" vis=key.shift() or key.lbutton() pos=0 title=app.name image=\uE249)
{
	item(title="config" image=\uE10A cmd='"@app.cfg"')
	item(title="manager" image=\uE0F3 admin cmd='"@app.exe"')
	item(title="directory" image=\uE0E8 cmd='"@app.dir"')
	item(title="version\t"+@app.ver vis=label col=1)
	item(title="docs" image=\uE1C4 cmd='https://nilesoft.org/docs')
	item(title="donate" image=\uE1A7 cmd='https://nilesoft.org/donate')
}
menu(where=@(this.count == 0) type="taskbar" image=icon.settings expanded=true)
{
	menu(title=loc.apps image=\uE254)
	{
		item(title=loc.paint image=\uE116 cmd='mspaint')
		item(title=loc.edge image cmd='@sys.prog32\Microsoft\Edge\Application\msedge.exe')
		item(title=loc.calculator image=\ue1e7 cmd='calc.exe')
		item(title=str.res('regedit.exe,-16') image cmd='regedit.exe')
	}
	menu(title=loc.windows image=\uE1FB)
	{
		item(title=loc.cascade_windows cmd=command.cascade_windows)
		item(title=loc.Show_windows_stacked cmd=command.Show_windows_stacked)
		item(title=loc.Show_windows_side_by_side cmd=command.Show_windows_side_by_side)
		sep
		item(title=loc.minimize_all_windows cmd=command.minimize_all_windows)
		item(title=loc.restore_all_windows cmd=command.restore_all_windows)
	}
	item(title=title.desktop image=icon.desktop cmd=command.toggle_desktop)
	item(title=title.settings image=icon.settings(auto, image.color1) cmd='ms-settings:')
	item(title=loc.task_manager sep=both image=icon.task_manager cmd='taskmgr.exe')
	item(title=loc.taskbar_Settings sep=both image=inherit cmd='ms-settings:taskbar')
	item(vis=key.shift() title=title.exit_explorer cmd=command.restart_explorer)
}