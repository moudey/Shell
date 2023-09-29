$ErrorActionPreference = 'Stop' # stop on all errors
$url        = 'https://nilesoft.org/download/shell/1.9/setup.exe'
$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  fileType      = 'exe'
  url           = $url
  softwareName  = 'Nilesoft Shell'
  checksum      = '4df2b30fc6b9d6d7c95c7e5070fbeb305c7d1b30ef3c135bb9e2838c10114fb6'
  checksumType  = 'sha256'
  silentArgs   = '/VERYSILENT /NORESTART' # Inno Setup
  validExitCodes= @(0)
}
Install-ChocolateyPackage @packageArgs