from pathlib import Path
import os
import subprocess

import pytest
from tests.case_loader import load_cases

ROOT = Path(__file__).resolve().parents[1]
EXE = Path(os.environ.get(
    "FEUX_EXE",
    str(ROOT / "build" / ("feux_tricolores.exe" if os.name == "nt" else "feux_tricolores")),
)).resolve()
CASES = load_cases(ROOT / "tests.yaml")


def lancer(args):
    assert EXE.is_file(), f"Executable introuvable: {EXE}"
    try:
        return subprocess.run(
            [str(EXE), *args], capture_output=True, text=True,
            encoding="utf-8", timeout=5, check=False,
        )
    except (subprocess.TimeoutExpired, OSError) as error:
        pytest.fail(f"Lancement impossible ou delai depasse: {error}")


@pytest.mark.functional
@pytest.mark.parametrize("case", CASES, ids=lambda case: case["name"])
def test_cli(case):
    result = lancer(case["args"])
    assert result.returncode == case["expected_exit"], result.stderr
    # CRLF Windows est normalise par text=True. Aucun espace utile n'est retire.
    for actual, field in [(result.stdout, "expected_stdout"), (result.stderr, "expected_stderr")]:
        expected = case[field] + "\n" if case[field] else ""
        assert actual == expected
