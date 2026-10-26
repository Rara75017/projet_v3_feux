param([string]$PythonExe = $env:LAB_PYTHON)
$ErrorActionPreference = 'Stop'
foreach ($step in @('setup', 'doctor', 'build', 'test')) {
    $arguments = @('-NoProfile', '-ExecutionPolicy', 'Bypass',
                   '-File', "$PSScriptRoot\$step.ps1")
    if ($step -eq 'setup' -and $PythonExe) {
        $arguments += @('-PythonExe', $PythonExe)
    }
    & powershell.exe @arguments
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
Write-Host 'LAB PASS - Consulter reports/unit.xml et reports/junit.xml.'
exit 0
