param([Parameter(Mandatory=$true)][string]$Version)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root
. "$PSScriptRoot\common.ps1"
if ($Version -notmatch '^[A-Za-z0-9][A-Za-z0-9._-]*$') {
    throw 'Version invalide.'
}
$zip = ".\dist\feux-tricolores-$Version.zip"
if ((Test-Path -LiteralPath $zip) -or (Test-Path -LiteralPath "$zip.sha256")) {
    throw 'Livraison deja existante. Choisir une nouvelle version.'
}
if (-not (Test-Path '.\reports\validated.json')) {
    throw 'Preuve de tests absente. Executer scripts/build.ps1 puis scripts/test.ps1.'
}
$proof = Get-Content '.\reports\validated.json' -Raw | ConvertFrom-Json
if ((Get-LabFingerprint -Root $root) -ne $proof.inputs_sha256 -or
    (Get-FileHash '.\build\feux_tricolores.exe' -Algorithm SHA256).Hash -ne $proof.binary_sha256 -or
    (Get-FileHash '.\reports\unit.xml' -Algorithm SHA256).Hash -ne $proof.unit_report_sha256 -or
    (Get-FileHash '.\reports\junit.xml' -Algorithm SHA256).Hash -ne $proof.functional_report_sha256) {
    throw 'Code, binaire ou rapports modifies depuis les tests. Refaire compilation et tests.'
}
$commit = & git rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'Commit Git introuvable.' }
$dirty = & git status --porcelain '--untracked-files=normal'
if ($LASTEXITCODE -ne 0) { throw 'Etat Git illisible.' }
if ($dirty) { throw 'Enregistrer les fichiers dans Git avant la livraison.' }
$stage = '.\dist\staging'
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory -Force $stage | Out-Null
Copy-Item '.\build\feux_tricolores.exe' $stage
Copy-Item '.\reports\unit.xml', '.\reports\junit.xml', '.\reports\validated.json' $stage
$binary = Join-Path $stage 'feux_tricolores.exe'
$metadata = [ordered]@{
    version = $Version
    commit = "$commit".Trim()
    created_utc = [DateTime]::UtcNow.ToString('o')
    sha256 = (Get-FileHash $binary -Algorithm SHA256).Hash
    inputs_sha256 = $proof.inputs_sha256
    tested_utc = $proof.tested_utc
}
$metadata | ConvertTo-Json | Set-Content `
    (Join-Path $stage 'metadata.json') -Encoding UTF8
'Usage: feux_tricolores.exe --duree 20 [--arret 3 --reprise 6]' | Set-Content `
    (Join-Path $stage 'README.txt') -Encoding ASCII
Compress-Archive -Path "$stage\*" -DestinationPath $zip
$hash = (Get-FileHash $zip -Algorithm SHA256).Hash
"$hash  feux-tricolores-$Version.zip" | Set-Content `
    "$zip.sha256" -Encoding ASCII
Write-Host "Package: $zip"
exit 0
