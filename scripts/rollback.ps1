param([Parameter(Mandatory=$true)][string]$TargetRoot)
$ErrorActionPreference = 'Stop'
$TargetRoot = [IO.Path]::GetFullPath($TargetRoot)
$pointer = Join-Path $TargetRoot 'current.txt'
$previous = Join-Path $TargetRoot 'previous.txt'
if (-not (Test-Path $previous)) { throw 'Aucune version precedente.' }
$target = (Get-Content $previous -Raw).Trim()
$old = (Get-Content $pointer -Raw).Trim()
& "$PSScriptRoot\smoke.ps1" -Exe (Join-Path $target 'feux_tricolores.exe')
$next = Join-Path $TargetRoot 'current.next'
Set-Content $next $target -Encoding UTF8
Move-Item $next $pointer -Force
Set-Content $previous $old -Encoding UTF8
Write-Host "ROLLBACK PASS: $target"
exit 0
