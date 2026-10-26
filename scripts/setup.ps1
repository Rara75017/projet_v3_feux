param([string]$PythonExe = $env:LAB_PYTHON)
$ErrorActionPreference = 'Stop'
Set-Location (Split-Path $PSScriptRoot -Parent)
if (-not $PythonExe) { $PythonExe = 'python' }

& $PythonExe -c "import sys; sys.exit(0 if (3, 11) <= sys.version_info[:2] < (3, 15) else 1)"
if ($LASTEXITCODE -ne 0) {
    throw 'Utiliser Python 3.11 a 3.14, de preference Python 3.12 x64.'
}
if (-not (Test-Path '.\requirements-lock.txt')) {
    throw 'requirements-lock.txt est absent du projet.'
}
if (-not (Test-Path '.\.venv\Scripts\python.exe')) {
    & $PythonExe -m venv .venv
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
& '.\.venv\Scripts\python.exe' -c "import sys; sys.exit(0 if (3, 11) <= sys.version_info[:2] < (3, 15) else 1)"
if ($LASTEXITCODE -ne 0) {
    throw 'Le venv utilise un Python incompatible. Recreer .venv avec Python 3.12.'
}
& '.\.venv\Scripts\python.exe' -m pip install -r requirements-lock.txt
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& '.\.venv\Scripts\python.exe' -m pip check
exit $LASTEXITCODE
