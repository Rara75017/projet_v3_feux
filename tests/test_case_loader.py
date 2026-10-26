from copy import deepcopy

import pytest
import yaml

from tests.case_loader import load_cases

pytestmark = pytest.mark.configuration

VALID = {
    "name": "simulation", "args": ["--duree", "0"], "expected_exit": 0,
    "expected_stdout": "12", "expected_stderr": "",
}


def case_with(**changes):
    return {"tests": [{**deepcopy(VALID), **changes}]}


@pytest.mark.parametrize("data", [
    None,
    [],
    {"tests": [VALID], "typo": True},
    {"tests": []},
    {"tests": {}},
    {"tests": [None]},
    {"tests": [{key: value for key, value in VALID.items() if key != "args"}]},
    case_with(unexpected="typo"),
    case_with(name=""),
    case_with(name="   "),
    {"tests": [VALID, VALID]},
    case_with(args="7 5"),
    case_with(args=[7, "5"]),
    case_with(args=["\0", "5"]),
    case_with(expected_exit=True),
    case_with(expected_exit=-1),
    case_with(expected_exit=256),
    case_with(expected_stdout=12),
    case_with(expected_stderr=None),
], ids=[
    "empty", "root_list", "unknown_root_key", "no_cases", "cases_not_list",
    "case_not_object", "missing_field", "unknown_field", "empty_name",
    "blank_name", "duplicate_name", "args_not_list", "numeric_arg", "nul_arg",
    "bool_exit", "negative_exit", "exit_above_255", "numeric_stdout", "null_stderr",
])
def test_reject_invalid_schema(tmp_path, data):
    path = tmp_path / "cases.yaml"
    path.write_text(yaml.safe_dump(data), encoding="utf-8")
    with pytest.raises(ValueError):
        load_cases(path)


@pytest.mark.parametrize("content", [
    "tests: [",
    "tests: []\ntests: []\n",
    "tests:\n  - name: one\n    name: two\n",
    "true: []\n",
], ids=["syntax", "duplicate_root_key", "duplicate_case_key", "non_text_key"])
def test_reject_invalid_yaml(tmp_path, content):
    path = tmp_path / "cases.yaml"
    path.write_text(content, encoding="utf-8")
    with pytest.raises(ValueError, match="YAML invalide"):
        load_cases(path)


def test_valid_file_with_utf8_bom(tmp_path):
    path = tmp_path / "cases.yaml"
    path.write_text(yaml.safe_dump({"tests": [VALID]}), encoding="utf-8-sig")
    assert load_cases(path) == [VALID]
