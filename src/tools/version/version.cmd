@echo off
::PowerShell.exe -ExecutionPolicy Unrestricted -FILE version.ps1
::PowerShell.exe -ExecutionPolicy Bypass -FILE version.ps1
cd %~dp0
PowerShell.exe -NoProfile -NonInteractive -NoLogo -ExecutionPolicy Bypass -FILE version.ps1