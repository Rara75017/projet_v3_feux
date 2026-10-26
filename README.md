# Version 3 - Feux tricolores en C++ orienté objet

Ce projet simule un carrefour à deux axes : **Nord-Sud** et **Est-Ouest**.
Il montre comment découper un programme C++ en composants, créer des objets,
protéger leur état et tester leurs interactions avec CMake, pytest et Jenkins.

Il s'agit d'une simulation pédagogique en console, avec des durées d'exemple.
Le programme ne commande aucun équipement routier. Le temps est virtuel :
20 secondes simulées sont calculées immédiatement, sans attendre 20 secondes.

## Premier lancement sous Windows 11

Extrais le kit puis ouvre **projet_v3_feux** dans VS Code. Les commandes suivantes
partent de ce dossier, dans PowerShell.

Prérequis : Git et un g++ Windows dans le PATH, par exemple MSYS2 UCRT64 ; Python
3.11 à 3.14 (3.12 x64 conseillé pour reproduire l'environnement vérifié) ; Windows
PowerShell 5.1. Le setup installe les versions fournies de CMake, Ninja, pytest et
PyYAML dans `.venv`, avec un accès à PyPI. Aucun binaire n'est fourni dans le ZIP.

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\run.ps1
.\build\feux_tricolores.exe --duree 20
```

La première commande prépare les outils, les vérifie, compile les trois composants
et l'application, puis lance les tests. Elle s'arrête en cas d'erreur.
Résultat attendu : **38 tests C++ + 73 tests pytest = 111 cas réussis**.
Le paramètre `Bypass` s'applique uniquement au processus lancé.

Si Python n'est accessible que par `py` :

```powershell
$pythonPath = py -3.12 -c "import sys; print(sys.executable)"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\run.ps1 -PythonExe $pythonPath
```

## Plusieurs composants, plusieurs responsabilités

| Composant compilé | Classes principales | Responsabilité |
| --- | --- | --- |
| `feux_domaine` | `FeuTricolore`, `PlanFeux`, `ControleurCarrefour` | Couleurs, transitions, durées et coordination des deux axes |
| `feux_simulation` | `Scenario`, `Simulateur`, interface `IAffichage` | Avancement du temps virtuel et événements |
| `feux_console` | `AffichageConsole`, `OptionsLigneCommande` | Affichage CSV et lecture des arguments |
| Application `feux_tricolores` | Assemblage dans `main.cpp` | Construction et connexion des objets |

Chaque composant a ses propres fichiers `.hpp` et `.cpp`, et sa cible CMake.
Les trois composants sont des bibliothèques statiques reliées à l'application.
Une classe décrit un type d'objet ; un composant regroupe des classes qui servent
la même responsabilité et se compile ici en une bibliothèque.

Dans `main.cpp`, les objets sont assemblés ainsi :

```cpp
feux::ControleurCarrefour controleur{options.plan};
feux::AffichageConsole affichage{std::cout};
feux::Simulateur simulateur{controleur, affichage};
simulateur.executer(options.scenario);
```

Le contrôleur **possède** ses deux feux. Le simulateur **utilise** le contrôleur et
un affichage fournis au constructeur. `AffichageConsole` **implémente** l'interface
`IAffichage`. Les tests fournissent à la place un collecteur en mémoire, sans modifier
le simulateur. Voir [l'architecture et les notions objet](docs/ARCHITECTURE.md).

## Fonctionnement du carrefour

Le cycle par défaut dure 14 secondes, puis recommence :

| Temps dans le cycle | Nord-Sud | Est-Ouest | Durée |
| --- | --- | --- | --- |
| 0 à moins de 1 s | Rouge | Rouge | 1 s |
| 1 à moins de 5 s | Vert | Rouge | 4 s |
| 5 à moins de 7 s | Orange | Rouge | 2 s |
| 7 à moins de 8 s | Rouge | Rouge | 1 s |
| 8 à moins de 12 s | Rouge | Vert | 4 s |
| 12 à moins de 14 s | Rouge | Orange | 2 s |

À 14 s, le cycle revient à sa première phase. Un axe vert **ou orange** exige que
l'autre reste rouge. Le démarrage et la reprise après urgence passent par un
intervalle tout rouge complet. Les couleurs ne peuvent pas être modifiées de
l'extérieur du contrôleur : `etat()` renvoie une copie des observations.

Le programme affiche un en-tête CSV, puis un état par seconde, de 0 à la durée
incluse. `--duree 20` produit donc **21 états**, plus l'en-tête.

```text
temps;phase;nord_sud;est_ouest;restant;urgence
0;TOUT_ROUGE_AVANT_NS;ROUGE;ROUGE;1;non
1;VERT_NS;VERT;ROUGE;4;non
2;VERT_NS;VERT;ROUGE;3;non
```

`restant` est le temps avant la prochaine transition. À la frontière d'une phase,
la ligne affiche déjà la nouvelle phase et sa durée complète.

## Scénarios et commandes

```powershell
# Observer le cycle complet et son redémarrage.
.\build\feux_tricolores.exe --duree 20

# Changer les durées des phases.
.\build\feux_tricolores.exe --duree 30 --vert-ns 6 --vert-eo 4 --orange 2 --tout-rouge 1

# Arrêt à t=3, reprise à t=6, puis un intervalle tout rouge avant le vert.
.\build\feux_tricolores.exe --duree 10 --arret 3 --reprise 6

# Afficher l'aide.
.\build\feux_tricolores.exe --help

# Écrire la trace pour la consulter ensuite.
.\build\feux_tricolores.exe --duree 20 | Set-Content -Encoding UTF8 .\reports\trace.csv
```

La simulation dure de 0 à 3600 secondes ; chaque phase dure de 1 à 3600 secondes.
Les événements doivent être dans l'intervalle simulé. Une reprise exige un arrêt
strictement antérieur. Les options dupliquées, les valeurs partielles comme
`12abc` et les options inconnues sont refusées.

L'arrêt d'urgence impose immédiatement deux rouges. L'horloge continue, mais le
cycle reste suspendu. La reprise commence un nouveau cycle à sa phase tout rouge,
puis donne le vert à Nord-Sud. Les événements d'un instant sont appliqués avant
l'affichage de la ligne correspondante.

| Code retour | Sens |
| --- | --- |
| 0 | Simulation ou aide réussie |
| 2 | Syntaxe des arguments incorrecte, entier illisible ou option inconnue |
| 3 | Durée ou scénario invalide |
| 1 | Erreur interne ou erreur d'écriture détectée |

## Tests et Jenkins

| Campagne | Nombre | Ce qu'elle vérifie |
| --- | --- | --- |
| C++ via CTest | 38 | Objets, transitions, coordination, urgence, injection de l'affichage et options |
| Contrats de CLI dans `tests.yaml` | 28 | Sorties exactes, erreurs et codes retour |
| Séquences complètes via pytest | 21 | Cycles, durées limites, exclusion des axes et urgence à chaque position du cycle |
| Validation du lecteur YAML | 24 | Fichier vide, champs incorrects, clés et noms dupliqués |

Les tests C++ utilisent un petit exécuteur pédagogique fourni, avec des
vérifications actives en Release. CTest lance les cas ; pytest lance le programme
et contrôle les traces. Les rapports sont `reports/unit.xml` et `reports/junit.xml`.

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\test.ps1
.\.venv\Scripts\ctest.exe --preset release -R unit.carrefour_exclusion_axes
.\.venv\Scripts\python.exe -m pytest tests/test_sequences.py -v
```

Le script complet force le bon binaire via `FEUX_EXE`, supprime les anciens rapports
et génère `reports/validated.json` uniquement si les deux campagnes réussissent.
Toute modification du code, des tests ou des scripts impose une nouvelle compilation
et une nouvelle campagne avant de livrer.

Pour Jenkins, le dépôt doit contenir **le contenu de projet_v3_feux à sa racine**.
Utilise `Pipeline script from SCM`, Script Path `Jenkinsfile`, le label `windows-lab`,
les plugins Pipeline/Git/JUnit et la variable `LAB_PYTHON` avec le chemin de Python.
Git et le compilateur Windows doivent être accessibles par le compte du nœud.
Le pipeline publie les deux rapports et archive `feux-tricolores-bN.zip` avec son SHA256.

La livraison exige un dépôt Git propre, un commit, une campagne complète réussie
et des empreintes inchangées. Utilise un nouveau nom de version à chaque livraison :

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\package.ps1 -Version feux-demo01
```

Garde `DEPLOY_LOCAL` désactivé pour le premier build. Si tu réutilises le déploiement
du laboratoire, choisis un dossier **dédié aux feux**, par exemple
`C:\jenkins-lab\deploy-feux`, comme `LAB_DEPLOY_ROOT`. Ne mélange pas les livraisons
de la calculatrice et des feux dans la même cible : les scripts de rollback n'attendent
pas le même nom d'exécutable. Un seul job doit déployer vers cette cible.
Le déploiement conserve le mécanisme pédagogique de pointeurs de la V2.

Les sources et tests ont été exécutés sous Linux. La syntaxe PowerShell a été
contrôlée ; les scripts Windows et le pipeline Jenkins restent à exécuter sur
ton poste. Voir [la validation](docs/VALIDATION.md).

## Apprendre en modifiant le projet

Commence par [les exercices 17 à 20](docs/EXERCICES_17_20.md) : instancier un feu,
comprendre le contrôleur, remplacer l'affichage, puis prouver qu'un défaut est détecté.
Les versions précédentes restent accessibles dans les autres dossiers du kit.
