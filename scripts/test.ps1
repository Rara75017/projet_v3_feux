$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root
. "$PSScriptRoot\common.ps1"

New-Item -ItemType Directory -Force reports | Out-Null
foreach ($path in @('reports\unit.xml', 'reports\junit.xml', 'reports\validated.json')) {
    if (Test-Path -LiteralPath $path) { Remove-Item -LiteralPath $path }
}
if (-not (Test-Path '.\build\inputs.sha256')) {
    throw 'Compilation valide absente. Executer scripts/build.ps1.'
}
$inputs = Get-LabFingerprint -Root $root
if ((Get-Content '.\build\inputs.sha256' -Raw).Trim() -ne $inputs) {
    throw 'Le code, les tests ou les scripts ont change. Recompiler avant de tester.'
}
$exe = (Resolve-Path '.\build\feux_tricolores.exe').Path
$before = (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash
$env:FEUX_EXE = $exe
$env:PYTEST_ADDOPTS = ''
$env:PYTEST_DISABLE_PLUGIN_AUTOLOAD = '1'

& '.\.venv\Scripts\ctest.exe' --preset release --output-junit "$root\reports\unit.xml"
$unitCode = $LASTEXITCODE
# Les deux campagnes sont executees pour conserver les diagnostics des deux niveaux.
& '.\.venv\Scripts\python.exe' -m pytest -v '--junitxml=reports/junit.xml'
$functionalCode = $LASTEXITCODE
foreach ($path in @('reports\unit.xml', 'reports\junit.xml')) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Rapport absent: $path" }
}
if ($unitCode -ne 0) { exit $unitCode }
if ($functionalCode -ne 0) { exit $functionalCode }
if ((Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash -ne $before -or
    (Get-LabFingerprint -Root $root) -ne $inputs) {
    throw 'Le binaire ou les entrees ont change pendant les tests. Recommencer.'
}
[ordered]@{
    binary_sha256 = $before
    inputs_sha256 = $inputs
    unit_report_sha256 = (Get-FileHash '.\reports\unit.xml' -Algorithm SHA256).Hash
    functional_report_sha256 = (Get-FileHash '.\reports\junit.xml' -Algorithm SHA256).Hash
    tested_utc = [DateTime]::UtcNow.ToString('o')
} | ConvertTo-Json | Set-Content '.\reports\validated.json' -Encoding UTF8
Write-Host 'TEST PASS'
exit 0
