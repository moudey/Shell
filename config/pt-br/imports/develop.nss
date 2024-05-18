menu(mode="multiple" title='&Desenvolvedor' vis=key.shift() sep=sep.bottom image=\uE26E)
{
	menu(mode="single" title='Editores' image=\uE17A)
	{
		item(title='Visual Studio Code' image=[\uE272, #22A7F2] cmd='code' args='"@sel.path"')
		separator
		item(type='file' mode="single" title='Bloco de Notas' image cmd='@sys.bin\notepad.exe' args='"@sel.path"')
	}

	menu(mode="multiple" title='.Net' image=\uE143)
	{
		item(title='Executar' cmd-line='/K dotnet run' image=\uE149)
		item(title='Visualizar' cmd-line='/K dotnet watch')
		item(title='Limpar' image=\uE0CE cmd-line='/K dotnet clean')
		separator
		item(title='Depuração da compilação' cmd-line='/K dotnet build')
		item(title='Versão da compilação' cmd-line='/K dotnet build -c release /p:DebugType=None')

		menu(mode="multiple" sep="both" title='Publicar' image=\ue11f)
		{
			$publish='dotnet publish -r win-x64 -c release --output publish /*/p:CopyOutputSymbolsToPublishDirectory=false*/'
			item(title='Publicar arquivo único' sep="after" cmd-line='/K @publish --no-self-contained /p:PublishSingleFile=true')
			item(title='Implantação dependente da estrutura' cmd-line='/K @publish')
			item(title='Executável dependente da estrutura' cmd-line='/K @publish --self-contained false')
			item(title='Implantação independente' cmd-line='/K @publish --self-contained true')
			item(title='Arquivo único' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=false')
			item(title='Aparado em arquivo único' cmd-line='/K @publish /p:PublishSingleFile=true /p:PublishTrimmed=true')
		}
		
		item(title='Migrações .Net EF adicionam InitialCreate' cmd-line='/K dotnet ef migrations add InitialCreate')
		item(title='Atualização do banco de dados .Net EF' cmd-line='/K dotnet ef database update')
		separator
		item(title='Ajuda .Net' image=\uE136 cmd-line='/k dotnet -h')
		item(title='Versão .Net' cmd-line='/k dotnet --info')
	}
}
