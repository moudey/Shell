menu(mode="multiple" title='&开发者菜单' sep=sep.bottom image=\uE26E)
{
	menu(mode="single" title='编辑' image=\uE17A)
	{
		item(title='Visual Studio Code' image=[\uE272, #22A7F2] cmd='code' args='"@sel.path"')
		separator
		item(type='file' mode="single" title='Windows 记事本' image cmd='@sys.bin\notepad.exe' args='"@sel.path"')
	}

	menu(mode="multiple" title='网络' image=\uE143)
	{
		item(title='开始运行' cmd-line='/K dotnet run' image=\uE149)
		item(title='监视' cmd-line='/K dotnet watch')
		item(title='清理' image=\uE0CE cmd-line='/K dotnet clean')
		separator
		item(title='建立调试' cmd-line='/K dotnet build')
		item(title='构建发布' cmd-line='/K dotnet build -c release /p:DebugType=None')

		menu(mode="multiple" sep="both" title='发布' image=\ue11f)
		{
			var { publish='dotnet publish -r win-x64 -c release --output publish /*/p:CopyOutputSymbolsToPublishDirectory=false*/' }
			item(title='发布sinale文件' sep="after" cmd-line='/K @publish --no-self-contained /p:PublishSingleFile=true')
			item(title='基于框架来部署的项目' cmd-line='/K @publish')
			item(title='依赖于框架的可执行程序项目' cmd-line='/K @publish --self-contained false')
			item(title='独自一体的部署项目' cmd-line='/K @publish --self-contained true')
			item(title='单个文件项目' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=false')
			item(title='单层修饰的项目' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=true')
		}
		
		item(title='ef migrations添加InitialCreate' cmd-line='/K dotnet ef migrations add InitialCreate')
		item(title='ef 数据库更新' cmd-line='/K dotnet ef database update')
		separator
		item(title='帮助' image=\uE136 cmd-line='/k dotnet -h')
		item(title='版本' cmd-line='/k dotnet --info')
	}
}