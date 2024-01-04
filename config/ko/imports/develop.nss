menu(mode="multiple" title='개발(&D)' vis=key.shift() sep=sep.bottom image=\uE26E)
{
	menu(mode="single" title='편집기' image=\uE17A)
	{
		item(title='Visual Studio Code' image=[\uE272, #22A7F2] cmd='code' args='"@sel.path"')
		separator
		item(type='file' mode="single" title='Windows 메모장' image cmd='@sys.bin\notepad.exe' args='"@sel.path"')
	}

	menu(mode="multiple" title='닷넷' image=\uE143)
	{
		item(title='실행' cmd-line='/K dotnet run' image=\uE149)
		item(title='감시' cmd-line='/K dotnet watch')
		item(title='지우기' image=\uE0CE cmd-line='/K dotnet clean')
		separator
		item(title='빌드 디버그' cmd-line='/K dotnet build')
		item(title='빌드 릴리즈' cmd-line='/K dotnet build -c release /p:DebugType=None')

		menu(mode="multiple" sep="both" title='publish' image=\ue11f)
		{
			$publish='dotnet publish -r win-x64 -c release --output publish /*/p:CopyOutputSymbolsToPublishDirectory=false*/'
			item(title='sinale 파일 게시' sep="after" cmd-line='/K @publish --no-self-contained /p:PublishSingleFile=true')
			item(title='프레임워크 종속 배포' cmd-line='/K @publish')
			item(title='프레임워크 종속 실행 파일' cmd-line='/K @publish --self-contained false')
			item(title='독립형 배포' cmd-line='/K @publish --self-contained true')
			item(title='단일 파일' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=false')
			item(title='단일 파일 트리밍' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=true')
		}
		
		item(title='ef migrations add InitialCreate' cmd-line='/K dotnet ef migrations add InitialCreate')
		item(title='ef database update' cmd-line='/K dotnet ef database update')
		separator
		item(title='도움말' image=\uE136 cmd-line='/k dotnet -h')
		item(title='버전' cmd-line='/k dotnet --info')
	}
}
