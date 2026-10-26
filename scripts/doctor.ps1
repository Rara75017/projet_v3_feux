$ErrorActionPreference = 'Stop'
Set-Location (Split-Path $PSScriptRoot -Parent)

foreach ($name in @('g++', 'git')) {
    $command = Get-Command $name -CommandType Application -ErrorAction Stop
    Write-Host "$name : $($command.Source)"
    & $command.Source --version
    if ($LASTEXITCODE -ne 0) { throw "$name ne fonctionne pas." }
}
$target = & g++ -dumpmachine
if ($LASTEXITCODE -ne 0 -or "$target" -notmatch 'mingw') {
    throw 'Compilateur Windows MinGW attendu, par exemple MSYS2 UCRT64.'
}
foreach ($name in @('python.exe', 'cmake.exe', 'ctest.exe', 'ninja.exe')) {
    $path = Join-Path '.\.venv\Scripts' $name
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Outil absent: $path. Executer scripts/setup.ps1."
    }
    & $path --version
    if ($LASTEXITCODE -ne 0) { throw "Outil inutilisable: $path" }
}
& '.\.venv\Scripts\python.exe' -m pip check
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host 'DOCTOR PASS'
exit 0
