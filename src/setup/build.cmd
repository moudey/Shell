@echo off

echo build setup...
echo.

cd wix
set arch=%~1
set bin=..\..\bin

if "%~1" == "x86" (
	set arch=x86
) else if "%~1" == "arm64" (
	set arch=arm64
) else (
	set arch=x64
)

echo %arch%

if exist %bin%\setup-%arch%.msi del %bin%\setup-%arch%.msi

echo WIX

wix.exe --version
echo.
wix.exe build -o %bin%\setup-%arch%.msi setup.wxs -arch %arch%

if exist %bin%\setup-%arch%.wixpdb del %bin%\setup-%arch%.wixpdb>nul
echo.
pause