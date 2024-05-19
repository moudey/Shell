menu(mode="multiple" title='&Entwickler' vis=key.shift() sep=sep.bottom image=\uE26E)
{
	menu(mode="single" title='Editoren' image=\uE17A)
	{
		item(title='Visual Studio Code' image=[\uE272, #22A7F2] cmd='code' args='"@sel.path"')
		separator
		item(type='file' mode="single" title='Windows Notepad' image cmd='@sys.bin\notepad.exe' args='"@sel.path"')
	}

	menu(mode="multiple" title='dotnet' image=\uE143)
	{
		item(title='Ausführen' cmd-line='/K dotnet run' image=\uE149)
		item(title='Dateiüberwachung' cmd-line='/K dotnet watch')
		item(title='Bereinigen' image=\uE0CE cmd-line='/K dotnet clean')
		separator
		item(title='Debug erstellen' cmd-line='/K dotnet build')
		item(title='Release erstellen' cmd-line='/K dotnet build -c release /p:DebugType=None')

		menu(mode="multiple" sep="both" title='Veröffentlichen' image=\ue11f)
		{
			$publish='dotnet publish -r win-x64 -c release --output publish /*/p:CopyOutputSymbolsToPublishDirectory=false*/'
			item(title='Einzelne Datei veröffentlichen' sep="after" cmd-line='/K @publish --no-self-contained /p:PublishSingleFile=true')
			item(title='Veröffentlichen' cmd-line='/K @publish')
			item(title='Veröffentlichen ohne .NET-Runtime' cmd-line='/K @publish --self-contained false')
			item(title='Veröffentlichen mit .NET-Runtime' cmd-line='/K @publish --self-contained true')
			item(title='Einzelne Datei' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=false')
			item(title='Einzelne Datei kürzen' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=true')
		}
		
		item(title='ef migrations add InitialCreate' cmd-line='/K dotnet ef migrations add InitialCreate')
		item(title='ef database update' cmd-line='/K dotnet ef database update')
		separator
		item(title='Hilfe' image=\uE136 cmd-line='/k dotnet -h')
		item(title='Version' cmd-line='/k dotnet --info')
	}
}
