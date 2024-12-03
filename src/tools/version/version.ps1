# PowerShell script to perform Version patching for Shell 
# ToDo:
# - adapt $Build number in case of local machine builds
#powershell -executionpolicy bypass -File .\Test.ps1
#param([switch]$arg1 = $false,[string]$arg2 = "")

Set-StrictMode -Version Latest

$ScriptPath = $(if ($PSVersionTable.PSVersion.Major -ge 3) { $PSCommandPath } else { & { $MyInvocation.ScriptName } })
$ScriptDir = Split-Path -Path $ScriptPath -Parent

$location = (Get-Location)

Set-Location -Path $ScriptDir

function Debug-Output($msg)
{
	#~return ## disabled debug output
	if ($msg -ne $null) { 
      	Write-Host ""
		Write-Host "$msg"
	}
}

function file-copy($from, $to)
{
	if ($from -ne $null) { 
		if ($to -ne $null) { 
			Copy-Item -LiteralPath $from -Destination $to -Force
		}
	}
}

function replace-direct($content, $key, $val)
{
	if ($content -ne $null) { 
		(Get-Content $content) -replace $key, $val | Set-Content $content
	}
}

function replace($content, $key, $val)
{
	return $content -replace $key, $val
}

function update($src, $dest)
{
	Copy-Item -LiteralPath $src -Destination $dest -Force
	$content = (Get-Content $dest)
	$content = $(replace $content '\$AppCompany\$' "$AppCompany")
	$content = $(replace $content '\$AppName\$' "$AppName")
	$content = $(replace $content '\$AppFileName\$' "$AppFileName")
	$content = $(replace $content '\$AppFileExt\$' "$AppFileExt")
	$content = $(replace $content '\$AppProcess\$' "$AppProcess")
	
	$content = $(replace $content '\$MAJOR\$' "$Major")
	$content = $(replace $content '\$MINOR\$' "$Minor")
	$content = $(replace $content '\$BUILD\$' "$Build")
	$content = $(replace $content '\$MAINT\$' "$Revis")
	$content = $(replace $content '\$VERSION\$' "$Version")
	$content = $(replace $content '\$YEAR\$' "$YEAR")
	$content | Set-Content $dest
}

function update-regex($content, $key, $val)
{
	#$regex = [regex]( '(\$' + $key + ')([\s+]?=[\s+]?)(\d+)$')
	#$content = $content -replace $regex, ('$1 = ' + $val)
	
	$regex = [regex]( '(?<key>\$' + $key + ')([\s]+?=[\s]+?)(\d+)')
	$content = $content -replace $regex, ('${key}	= ' + $val)

	return $content;
}

try 
{
	$ChangeBuild	= 0
	$ChangeRevis	= 0
	
	$___Build__	= 19	# auto increase
	$___Revis__	= 0	# auto increase
	
	$AppCompany		= "Nilesoft"
	$AppName		= "Shell"
	$AppFileName	= "shell"
	$AppFileExt		= ".dll"
	$AppProcess		= "64-bit"

	$Major			= 1
	$Minor			= 9		#[int]$(Get-Date -format yy)
	$Build 			= $___Build__ #[int](Get-Content "build.txt")
	$Revis 			= $___Revis__ #[int]$(Get-Date -format Mdd)
	$YEAR 			= [int]$(Get-Date -format yyyy)
	
	if(!$Build) 
	{
		$Build = 0
	}

	# locally: increase build number and persit it
	if($ChangeBuild)
	{
		$Build = $Build + 1
	}
	
	if($ChangeRevis)
	{
		$Revis = $Revis + 1
	}
	
	#$Build | Set-Content "build.txt"
	$Version = "$Major.$Minor.$Build.$Revis"

	Debug-Output("Nilesoft Shell version number: $Version")

	update "Resource.h" "..\..\shared\Resource.h";
	#update "Shell.rc" "..\..\shared\Resource\Shell.rc";
	update "manifest.xml" "..\..\shared\Resource\manifest.xml";
	update "Shell.def" "..\..\dll\src\Shell.def";
	update "var.wxi" "..\..\setup\wix\var.wxi";
	
	$content = (Get-Content $ScriptPath)
	
	if($ChangeBuild)
	{
		$content = update-regex $content  '___Build__'  $BUILD
	}
	
	if($ChangeRevis)
	{
		$content = update-regex $content  '___Revis__'  $Revis
		#$key = '___Revis__'
		#$regex = [regex]( '(?<key>\$' + $key + ')([\s]+?=[\s]+?)(\d+)')
		#$content = $content -replace $regex, ('${key}	= ' + $Revis)
	}
	
	#$content = $content -replace $regex, ('$1 = ' + $BUILD)#  | Set-Content "Version.ps1"
	$content | Set-Content $ScriptPath
}
catch 
{
	$errorMessage = $_.Exception.Message
	Write-Warning "Exception caught: `"$errorMessage`"!"
	throw $_
}
finally
{
	Set-Location -Path $location
}
