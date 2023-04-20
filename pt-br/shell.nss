shell
{
	var
	{
		isw11=sys.is11
	}
	
	set
	{
		theme
		{
			name="modern"
			background
			{
				//opacity=95
				effect=0
			}
			
			image
			{
				align=2
			}
			
			item.prefix=auto
		}

		exclude.where = !process.is_explorer
		showdelay=200
	}

	images import 'imports/images.nss'

	static
	{	
		import 'imports/static.nss'
		item(where=this.title.length > 25 menu=title.more_options)
	}

	dynamic
	{
		menu(mode="multiple" title="Fixar/Desafixar" image=icon.pin) { }
		menu(mode="multiple" title=title.more_options image=icon.more_options) { }
		
		import 'imports/terminal.nss'
		import 'imports/file-manage.nss'
		import 'imports/develop.nss'
		import 'imports/goto.nss'
		import 'imports/taskbar.nss'
	}
}

