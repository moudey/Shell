@echo off
title dija debug
cls
set TH=%time:~0,2%
set TM=%time:~3,2%
set TS=%time:~6,2%

set MM=%date:~4,2%
set DD=%date:~7,2%
set YYYY=%date:~10,4%

set PLATFORM=release\x64

set dt=%YYYY%%MM%%DD%%TH%%TM%%TS%
set build=%YYYY%%MM%%DD%

:: Validate administrative permissions...
:: https://stackoverflow.com/questions/4051883/batch-script-how-to-check-for-admin-rights
::echo Administrative permissions required for unregistering the Shell Extension.
:: Unregistering the Shell Extension.
::echo Proceeding with the Shell Extension unregistration.
echo.
echo Shell version $MAJOR$.$MINOR$ build $BUILD$	%date% %time%
echo.

cd /d %~dp0

IF EXIST "%PLATFORM%\shell.dll" (
	
	IF EXIST "shell.dll" (
		move "shell.dll" "deleted\shell-%dt%.dll" >nul
	)
	
	copy /Y "%PLATFORM%\shell.dll" "shell.dll" >nul
	
	IF EXIST "%PLATFORM%\shell.exe" (
		copy /Y "%PLATFORM%\shell.exe" "shell.exe" >nul
		timeout 1 >nul
	)

	shell.exe -restart
	
	IF EXIST "shell.log" (
		del /s /q "shell.log" >nul
	)
	
	timeout 2 >nul
	
	del deleted\*.dll>nul

	rem start %~dp0
)

