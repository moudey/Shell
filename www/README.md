# View documentation on your local machine

**WARNING: This is not meant to be used on a production sever!!**

It's sole intention is to be able to see the documentation documents in a context like on the live website in order to
be able to see the full formatting. So it's mainly intended for documentation authors or editors.

## Local web server

### Get Cassini 4 (tiny, redistributable ASP.NET webserver)

With PowerShell (no administrative privileges required):

``` PowerShell
# Source URL
$url = "https://github.com/adriancs2/CassiniDev/releases/download/5f7df0d/CassiniDev4.zip"

# Destination file
$zip = ".\CassiniDev4.zip"

# Download the file
Invoke-WebRequest -Uri $url -OutFile $zip

# Unzip the file
Expand-Archive -Path $zip -DestinationPath '.\bin\CassiniDev4\'

# Delete archive
Remove-Item $zip

```

### Start the web server

With PowerShell:

``` PowerShell
# Set the port you want to listen on. 80 is the default for http://
$port = 80

# Run the webserver
.\bin\CassiniDev4\CassiniDev4.exe /port:$port /path:. /vpath:/ /nodirlist /portMode:Specific

echo "To see the documentation, open the following url in your browser:"
echo "http://localhost:$port/docs"

```

With Command Prompt:

``` Cmd
# Set the port you want to listen on. 80 is the default for http://
set port = 80

# Run the webserver
.\bin\CassiniDev4\CassiniDev4.exe /port:%port% /path:. /vpath:/ /nodirlist /portMode:Specific

echo To see the documentation, open the following url in your browser:
echo http://localhost:%port%/docs

```

### Stop the web server

In your tray icons section, you will find an icon showing above url as tooltip. Right-clicking on the icon opens a
context menu with the following options:

- Show
- Brows
- View Log
- Exit

Obviously, choose `Exit` to quit the web server. :-)

<!--

## Alternative: UltiDev Web Server Pro 

see: http://ultidev.com/Forums/yaf_postst688_Installation--Operations--Troubleshooting.aspx
and: http://ultidev.com/forums/yaf_postsm2939_Debugging-your-web-app-with-UWS--or-just-hosting-the-app-from-command-line.aspx#post2939

Download:
http://ultidev.com/download/RegisterAndDownload.aspx?ProductID=3245eb7e-5b39-4568-8cb9-05e32d4291e7
http://download.ultidev.com/Products/Cassini/Pro/Build21/UltiDev%20Web%20Server%20Setup.exe

```cmd
"%ProgramFiles%\UltiDev\Web Server\UWS.InteractiveServer.Clr4AnyCPU.exe" /port:80 /path:C:\Users\mdr\projects\nilesoft.shell\www /vdir: /defaultdoc:Default.aspx /dirbrowsing:no /bypass:no /browser:firefox.exe
"%ProgramFiles%\UltiDev\Web Server\UWS.InteractiveServer.Clr4AnyCPU.exe" /port:8080 /path:C:\Users\mdr\projects\nilesoft.shell\www /vdir: /defaultdoc:Default.aspx /dirbrowsing:no /bypass:no /browser:firefox.exe
```

-->