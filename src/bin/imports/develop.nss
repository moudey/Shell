menu(mode="multiple" title='&Geliştir' vis=key.shift() sep=sep.bottom image=\uE26E)
{
	menu(mode="single" title='editörler' image=\uE17A)
	{
		item(title='Visual Studio Kodu' image=[\uE272, #22A7F2] cmd='code' args='"@sel.path"')
		separator
		item(type='file' mode="single" title='Windows not defteri' image cmd='@sys.bin\notepad.exe' args='"@sel.path"')
	}

	menu(mode="multiple" title='dotnet' image=\uE143)
	{
		item(title='çalıştır' cmd-line='/K dotnet run' image=\uE149)
		item(title='izle' cmd-line='/K dotnet watch')
		item(title='temizle' image=\uE0CE cmd-line='/K dotnet clean')
		separator
		item(title='kur debug' cmd-line='/K dotnet build')
		item(title='kur yayınla' cmd-line='/K dotnet build -c release /p:DebugType=None')

		menu(mode="multiple" sep="both" title='yayınla' image=\ue11f)
		{
			$publish='dotnet publish -r win-x64 -c release --output publish /*/p:CopyOutputSymbolsToPublishDirectory=false*/'
			item(title='tek dosya yayınla' sep="after" cmd-line='/K @publish --no-self-contained /p:PublishSingleFile=true')
			item(title='çerçeve bağımlı dağıtım' cmd-line='/K @publish')
			item(title='çerçeve bağımlı çalıştırılabilir' cmd-line='/K @publish --self-contained false')
			item(title='bağımsız dağıtım' cmd-line='/K @publish --self-contained true')
			item(title='tek dosya' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=false')
			item(title='kesilmiş tek dosya' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=true')
		}
		
		item(title='ef geçişler ekle InitialCreate' cmd-line='/K dotnet ef migrations add InitialCreate')
		item(title='ef veritabanı güncelle' cmd-line='/K dotnet ef database update')
		separator
		item(title='yardım' image=\uE136 cmd-line='/k dotnet -h')
		item(title='sürüm' cmd-line='/k dotnet --info')
	}
}
