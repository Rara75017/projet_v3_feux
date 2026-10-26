param(
    [Parameter(Mandatory=$true)][string]$Package,
    [Parameter(Mandatory=$true)][string]$TargetRoot
)
$ErrorActionPreference = 'Stop'
$Package = (Resolve-Path $Package).Path
$line = Get-Content "$Package.sha256" | Select-Object -First 1
$expected = ($line -split '\s+')[0]
$actual = (Get-FileHash $Package -Algorithm SHA256).Hash
if ($actual -ne $expected) { throw 'SHA256 du ZIP incorrect.' }
$TargetRoot = [IO.Path]::GetFullPath($TargetRoot)
New-Item -ItemType Directory -Force $TargetRoot | Out-Null
$release = Join-Path $TargetRoot ([IO.Path]::GetFileNameWithoutExtension($Package))
if (Test-Path $release) { throw 'Cette version existe deja.' }
Expand-Archive -Path $Package -DestinationPath $release
$meta = Get-Content (Join-Path $release 'metadata.json') -Raw |
    ConvertFrom-Json
$exe = Join-Path $release 'feux_tricolores.exe'
if ((Get-FileHash $exe -Algorithm SHA256).Hash -ne $meta.sha256) {
    throw 'SHA256 de executable incorrect.'
}
& "$PSScriptRoot\smoke.ps1" -Exe $exe
$pointer = Join-Path $TargetRoot 'current.txt'
$old = if (Test-Path $pointer) {
    (Get-Content $pointer -Raw).Trim()
} else { '' }
if ($old) {
    Set-Content (Join-Path $TargetRoot 'previous.txt') `
        $old -Encoding UTF8
}
$next = Join-Path $TargetRoot 'current.next'
Set-Content $next $release -Encoding UTF8
Move-Item $next $pointer -Force
Write-Host "DEPLOY PASS: $release"
exit 0
