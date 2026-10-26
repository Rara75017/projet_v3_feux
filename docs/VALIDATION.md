# Validation de la V3 - 26 octobre 2026

## Vérifications exécutées sous Linux

| Vérification | Résultat |
| --- | --- |
| Compilation Release CMake/Ninja/g++ | Réussite, avertissements traités comme erreurs |
| Tests C++ des composants | 38/38 réussis |
| Contrats CLI définis en YAML | 28/28 réussis |
| Séquences complètes et scénarios d'urgence | 21/21 réussis |
| Validation du lecteur YAML | 24/24 réussis |
| Total | 111/111 réussis |
| Rapports JUnit | `unit.xml` et `junit.xml` produits |
| Variante volontaire avec deux axes verts | Refusée par les tests C++ et Python |
| Analyse de syntaxe des scripts PowerShell | Dix scripts sans erreur détectée par Tree-sitter |

Environnement : Linux x86_64, g++ 13.3.0, Python 3.12, CMake 3.31.6,
Ninja 1.11.1.4, pytest 8.4.2 et PyYAML 6.0.3. Les dépendances du laboratoire
restent figées dans `requirements-lock.txt`.

Le défaut volontaire a été introduit dans une copie temporaire. Les sources livrées
utilisent bien le rouge Nord-Sud pendant le vert Est-Ouest. Aucun exécutable Linux
n'est livré à la place d'un programme Windows.

Les tests de séquence observent notamment des cycles asymétriques, les durées
minimales et maximales, les frontières de phase et une urgence à chaque position
du cycle par défaut. Le test d'exclusion vérifie une règle plus forte que l'absence
de deux verts : si un axe est vert ou orange, l'autre doit être rouge.

## Limite de cette validation

Les scripts PowerShell ont été relus et analysés syntaxiquement, sans exécution
sur Windows. Le Jenkinsfile, le packaging, le déploiement et le rollback doivent
encore être exécutés sur le poste Windows cible. Un contrôle de syntaxe ne prouve
pas le fonctionnement de ces intégrations.

Commence avec `scripts/run.ps1`, puis un build Jenkins avec `DEPLOY_LOCAL` désactivé.
Pour un essai de déploiement, utilise un dossier réservé à cette application.
Le verrou Python ne verrouille pas la version de g++ installée sur le poste et ne
garantit pas une compilation bit à bit identique.

## Reproduction des tests sous Linux

Depuis `projet_v3_feux`, avec g++ et Python accessibles :

```bash
python3 -m venv .venv-linux
.venv-linux/bin/python -m pip install -r requirements-lock.txt
export PATH="$PWD/.venv-linux/bin:$PATH"
cmake --preset release
cmake --build --preset release
mkdir -p reports
ctest --preset release --output-junit "$PWD/reports/unit.xml"
.venv-linux/bin/python -m pytest -q --junitxml=reports/junit.xml
```

Cette séquence vérifie le code et les tests. Elle ne produit pas la preuve de
livraison Windows `validated.json`, créée par les scripts PowerShell après succès.
