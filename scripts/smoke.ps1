param([Parameter(Mandatory=$true)][string]$Exe)
$ErrorActionPreference = 'Stop'
$Exe = (Resolve-Path $Exe).Path
$cases = @(
    @{ Args=@('--duree','1'); Last='1;VERT_NS;VERT;ROUGE;4;non' },
    @{ Args=@('--duree','1','--arret','0'); Last='1;URGENCE;ROUGE;ROUGE;0;oui' }
)
foreach ($case in $cases) {
    $arguments = $case.Args
    $actual = @(& $Exe @arguments)
    $code = $LASTEXITCODE
    if ($code -ne 0 -or $actual.Count -ne 3 -or
        $actual[0] -ne 'temps;phase;nord_sud;est_ouest;restant;urgence' -or
        $actual[-1] -ne $case.Last) {
        throw "Smoke test en echec: obtenu=$actual code=$code"
    }
}
Write-Host 'SMOKE PASS'
