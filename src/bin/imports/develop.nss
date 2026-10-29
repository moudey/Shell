menu(mode="multiple" title=loc.develop sep=sep.bottom image=\uE26E)
{
	menu(mode="single" title=loc.editors image=\uE17A)
	{
		item(title='Visual Studio Code' image=[\uE272, #22A7F2] cmd='code' args='"@sel.path"')
		separator
		item(type='file' mode="single" title=loc.windows_notepad image cmd='@sys.bin\notepad.exe' args='"@sel.path"')
	}

	menu(mode="multiple" title='dotnet' image=\uE143)
	{
		item(title=loc.dotnet_run cmd-line='/K dotnet run' image=\uE149)
		item(title=loc.dotnet_watch cmd-line='/K dotnet watch')
		item(title=loc.dotnet_clean image=\uE0CE cmd-line='/K dotnet clean')
		separator
		item(title=loc.dotnet_build_debug cmd-line='/K dotnet build')
		item(title=loc.dotnet_build_release cmd-line='/K dotnet build -c release /p:DebugType=None')

		menu(mode="multiple" sep="both" title=loc.dotnet_publish image=\ue11f)
		{
			$publish='dotnet publish -r win-x64 -c release --output publish /*/p:CopyOutputSymbolsToPublishDirectory=false*/'
			item(title=loc.dotnet_publish_single_file sep="after" cmd-line='/K @publish --no-self-contained /p:PublishSingleFile=true')
			item(title=loc.dotnet_framework_dependent_deployment cmd-line='/K @publish')
			item(title=loc.dotnet_framework_dependent_executable cmd-line='/K @publish --self-contained false')
			item(title=loc.dotnet_self_contained_deployment cmd-line='/K @publish --self-contained true')
			item(title=loc.dotnet_single_file cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=false')
			item(title=loc.dotnet_single_file_trimmed cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=true')
		}
		
		item(title='ef migrations add InitialCreate' cmd-line='/K dotnet ef migrations add InitialCreate')
		item(title='ef database update' cmd-line='/K dotnet ef database update')
		separator
		item(title=loc.dotnet_help image=\uE136 cmd-line='/k dotnet -h')
		item(title=loc.dotnet_info cmd-line='/k dotnet --info')
	}

	item(type="file" title="reshack" sep="top" image cmd='D:\config\Programs\dev\petools\resource\reshack\reshack.exe' args=sel(1))
	item(type="file" title="peview" image cmd='D:\config\Programs\dev\petools\peview.exe' args=sel(1))
	
	item(type="dir|dir.back" title="Web server" vis=key.shift() cmd='D:\config\Programs\dev\web\HTTPServer\WebServer.exe' args='-open -path:"@sel.path"')
}