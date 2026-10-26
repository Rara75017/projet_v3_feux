# Fonctions partagees. Aucun exit ici : ce fichier est charge par les scripts.
Set-StrictMode -Version Latest

function Get-LabFingerprint {
    param([Parameter(Mandatory=$true)][string]$Root)
    $files = @()
    foreach ($directory in @('src', 'include', 'tests', 'scripts')) {
        $files += Get-ChildItem -LiteralPath (Join-Path $Root $directory) -Recurse -File |
            Where-Object { $_.Extension -in @('.cpp', '.hpp', '.py', '.ps1') }
    }
    foreach ($name in @('CMakeLists.txt', 'CMakePresets.json', 'tests.yaml',
                        'pytest.ini', 'requirements-lock.txt')) {
        $files += Get-Item -LiteralPath (Join-Path $Root $name)
    }
    $lines = foreach ($file in ($files | Sort-Object FullName)) {
        $relative = $file.FullName.Substring($Root.Length).TrimStart([char[]]'\/')
        $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
        $relative.Replace('\', '/') + ':' + $hash
    }
    $algorithm = [Security.Cryptography.SHA256]::Create()
    try {
        $bytes = [Text.Encoding]::UTF8.GetBytes(($lines -join [char]10))
        return [BitConverter]::ToString($algorithm.ComputeHash($bytes)).Replace('-', '')
    } finally {
        $algorithm.Dispose()
    }
}
