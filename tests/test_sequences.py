"""Verification de traces entieres, avec une table de phases independante du C++."""
import csv
import io

import pytest
from tests.test_cli import lancer

pytestmark = pytest.mark.functional


def lire_trace(args):
    result = lancer(args)
    assert result.returncode == 0, result.stderr
    assert result.stderr == ""
    reader = csv.DictReader(io.StringIO(result.stdout), delimiter=";")
    assert reader.fieldnames == ["temps", "phase", "nord_sud", "est_ouest", "restant", "urgence"]
    return list(reader)


def attendu(instant, ns=4, eo=4, orange=2, rouge=1):
    # Le contrat est une succession d'intervalles [debut, fin).
    phases = [
        (rouge, "TOUT_ROUGE_AVANT_NS", "ROUGE", "ROUGE"),
        (ns, "VERT_NS", "VERT", "ROUGE"),
        (orange, "ORANGE_NS", "ORANGE", "ROUGE"),
        (rouge, "TOUT_ROUGE_AVANT_EO", "ROUGE", "ROUGE"),
        (eo, "VERT_EO", "ROUGE", "VERT"),
        (orange, "ORANGE_EO", "ROUGE", "ORANGE"),
    ]
    position = instant % sum(duree for duree, *_ in phases)
    for duree, phase, nord_sud, est_ouest in phases:
        if position < duree:
            return phase, nord_sud, est_ouest, str(duree - position)
        position -= duree
    raise AssertionError("Table de reference incoherente")


def verifier_ligne(row, temps, phase, ns, eo, restant, urgence="non"):
    assert row == dict(zip(
        ["temps", "phase", "nord_sud", "est_ouest", "restant", "urgence"],
        [str(temps), phase, ns, eo, restant, urgence],
    ))
    assert row["nord_sud"] == "ROUGE" or row["est_ouest"] == "ROUGE"


@pytest.mark.parametrize("ns,eo,orange,rouge,duree", [
    (4, 4, 2, 1, 45), (1, 1, 1, 1, 60), (3, 7, 2, 4, 80),
    (5, 2, 3, 1, 60), (3600, 3600, 3600, 3600, 3600),
])
def test_sequences_et_exclusion(ns, eo, orange, rouge, duree):
    rows = lire_trace([
        "--vert-ns", str(ns), "--vert-eo", str(eo), "--orange", str(orange),
        "--tout-rouge", str(rouge), "--duree", str(duree),
    ])
    assert len(rows) == duree + 1
    for instant, row in enumerate(rows):
        verifier_ligne(row, instant, *attendu(instant, ns, eo, orange, rouge))


@pytest.mark.parametrize("arret", range(14))
def test_urgence_a_chaque_position_du_cycle(arret):
    reprise = arret + 2
    rows = lire_trace(["--duree", "20", "--arret", str(arret), "--reprise", str(reprise)])
    assert len(rows) == 21
    for instant, row in enumerate(rows):
        if arret <= instant < reprise:
            verifier_ligne(row, instant, "URGENCE", "ROUGE", "ROUGE", "0", "oui")
        else:
            reference = instant if instant < arret else instant - reprise
            verifier_ligne(row, instant, *attendu(reference))


def test_options_par_defaut():
    rows = lire_trace([])
    assert len(rows) == 21
    for instant, row in enumerate(rows):
        verifier_ligne(row, instant, *attendu(instant))


def test_reprise_respecte_toute_la_duree_rouge():
    rows = lire_trace(["--duree", "12", "--tout-rouge", "3", "--arret", "4", "--reprise", "6"])
    for t in (6, 7, 8):
        verifier_ligne(rows[t], t, "TOUT_ROUGE_AVANT_NS", "ROUGE", "ROUGE", str(9 - t))
    verifier_ligne(rows[9], 9, "VERT_NS", "VERT", "ROUGE", "4")
