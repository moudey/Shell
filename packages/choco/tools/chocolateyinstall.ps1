$ErrorActionPreference = 'Stop' # stop on all errors
$url        = 'https://nilesoft.org/download/shell/1.8/setup.exe'
$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  fileType      = 'exe'
  url           = $url
  softwareName  = 'Nilesoft Shell'
  checksum      = 'c87da931586ab14ce957e99971ef8b344edfb1b0caa273e87c1be752bb924447'
  checksumType  = 'sha256'
  silentArgs   = '/VERYSILENT /NORESTART' # Inno Setup
  validExitCodes= @(0)
}
Install-ChocolateyPackage @packageArgs