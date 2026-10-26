"""Lecture YAML stricte : une faute de configuration ne devient pas un succes."""
from pathlib import Path

import yaml
from yaml.constructor import ConstructorError


class UniqueKeySafeLoader(yaml.SafeLoader):
    """SafeLoader avec refus des cles dupliquees, y compris dans un cas."""

    def construct_mapping(self, node, deep=False):
        seen = set()
        for key_node, _ in node.value:
            key = self.construct_object(key_node, deep=deep)
            if not isinstance(key, str):
                raise ConstructorError(
                    None, None, "Cle YAML textuelle attendue", key_node.start_mark
                )
            if key in seen:
                raise ConstructorError(
                    None, None, f"Cle YAML dupliquee: {key}", key_node.start_mark
                )
            seen.add(key)
        return super().construct_mapping(node, deep=deep)


def load_cases(path: Path):
    try:
        data = yaml.load(path.read_text(encoding="utf-8-sig"), Loader=UniqueKeySafeLoader)
    except (yaml.YAMLError, UnicodeError) as error:
        raise ValueError(f"{path.name}: YAML invalide: {error}") from error
    if not isinstance(data, dict) or set(data) != {"tests"}:
        raise ValueError("Une seule cle racine 'tests' est attendue")
    cases = data["tests"]
    if not isinstance(cases, list) or not cases:
        raise ValueError("Liste tests non vide attendue")
    names = set()
    required = {
        "name", "args", "expected_exit", "expected_stdout", "expected_stderr",
    }
    for case in cases:
        if not isinstance(case, dict) or set(case) != required:
            raise ValueError("Champs du cas de test invalides")
        name = case["name"]
        if not isinstance(name, str) or not name.strip() or name in names:
            raise ValueError("Nom de test vide ou duplique")
        names.add(name)
        args = case["args"]
        if not isinstance(args, list) or not all(
            isinstance(arg, str) and "\0" not in arg for arg in args
        ):
            raise ValueError(f"{name}: args doit contenir du texte sans NUL")
        code = case["expected_exit"]
        if type(code) is not int or not 0 <= code <= 255:
            raise ValueError(f"{name}: expected_exit doit etre un entier de 0 a 255")
        for field in ("expected_stdout", "expected_stderr"):
            if not isinstance(case[field], str):
                raise ValueError(f"{name}: {field} doit etre du texte")
    return cases
