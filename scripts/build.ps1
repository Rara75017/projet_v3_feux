$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root
. "$PSScriptRoot\common.ps1"

foreach ($path in @('reports\validated.json', 'build\inputs.sha256',
                    'build\feux_tricolores.exe', 'build\feux_unit_tests.exe')) {
    if (Test-Path -LiteralPath $path) { Remove-Item -LiteralPath $path }
}
$toolsPath = Join-Path $root '.venv\Scripts'
$cmake = Join-Path $toolsPath 'cmake.exe'
if (-not (Test-Path -LiteralPath $cmake)) {
    throw 'CMake absent. Executer scripts/setup.ps1.'
}
Get-Command g++ -CommandType Application -ErrorAction Stop | Out-Null
$env:Path = "$toolsPath;$env:Path"
$before = Get-LabFingerprint -Root $root

& $cmake --preset release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $cmake --build --preset release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
foreach ($path in @('build\feux_tricolores.exe', 'build\feux_unit_tests.exe')) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Executable absent: $path" }
}
if ((Get-LabFingerprint -Root $root) -ne $before) {
    throw 'Les entrees du projet ont change pendant la compilation. Recommencer.'
}
Set-Content '.\build\inputs.sha256' $before -Encoding ASCII
Write-Host 'BUILD PASS'
exit 0
