# Run in PowerShell on Windows x64 with Visual Studio 2022 C++ and CMake installed.
[CmdletBinding()]
param([string]$JucePath = '', [switch]$RunWindowTest)
$ErrorActionPreference = 'Stop'
if ($env:OS -ne 'Windows_NT') { throw 'This build script requires a Windows PC.' }
# Locate Visual Studio's bundled CMake when it is not already on PATH.
if (!(Get-Command cmake -ErrorAction SilentlyContinue)) {
 $VsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
 if (Test-Path $VsWhere) {
  $VsRoot = & $VsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
  if ($VsRoot) {
   $CmakeBin = Join-Path $VsRoot 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin'
   if (Test-Path (Join-Path $CmakeBin 'cmake.exe')) { $env:PATH = "$CmakeBin;$env:PATH" }
  }
 }
}
if (!(Get-Command cmake -ErrorAction SilentlyContinue)) { throw 'Install Visual Studio 2022 with Desktop development with C++, including CMake tools, then run this again.' }
function Invoke-Checked {
 param([string]$Program, [string[]]$Arguments)
 & $Program @Arguments
 if ($LASTEXITCODE -ne 0) { throw "$Program failed with exit code $LASTEXITCODE" }
}
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildPath = Join-Path $ProjectRoot 'build-windows-x64'
$Configure = @('-S', $ProjectRoot, '-B', $BuildPath, '-G', 'Visual Studio 17 2022', '-A', 'x64', '-DFM_BUILD_HOST_CHECK=ON')
if ($JucePath) { $Configure += "-DJUCE_PATH=$JucePath" }
Invoke-Checked 'cmake' $Configure
Invoke-Checked 'cmake' @('--build', $BuildPath, '--config', 'Release', '--parallel', '4')
Invoke-Checked 'ctest' @('--test-dir', $BuildPath, '-C', 'Release', '--output-on-failure')
$PluginPath = Join-Path $BuildPath 'FMDelay_artefacts/Release/VST3/FM DELAY.vst3'
if (!(Test-Path $PluginPath)) { throw 'VST3 bundle is missing' }
if ($RunWindowTest) {
 $HostCheck = Join-Path $BuildPath 'FMHostCheck_artefacts/Release/FMHostCheck.exe'
 foreach ($Program in @('0', '35', '36')) { Invoke-Checked $HostCheck @($PluginPath, $Program) }
}
$PackagePath = Join-Path $ProjectRoot 'dist/FM-Delay-Windows-x64'
New-Item -ItemType Directory -Force -Path $PackagePath | Out-Null
# Replace only the generated package copy; never touch an installed plugin.
$PackagedPlugin = Join-Path $PackagePath 'FM DELAY.vst3'
if (Test-Path $PackagedPlugin) { Remove-Item -Recurse -Force $PackagedPlugin }
Copy-Item -Recurse $PluginPath $PackagePath
Copy-Item (Join-Path $BuildPath 'FMDelay_artefacts/Release/Standalone/FM DELAY.exe') $PackagePath
foreach ($Document in @('README.md', 'PRESETS.md', 'QUALITY-AUDIT.md', 'COMPATIBILITY.md')) {
 Copy-Item (Join-Path $ProjectRoot $Document) $PackagePath
}
Compress-Archive -Path $PackagePath -DestinationPath (Join-Path $ProjectRoot 'dist/FM-Delay-Windows-x64.zip') -Force
Write-Host "Built and tested Windows x64 package: $PackagePath"
