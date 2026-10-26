# Exercices 17 à 20 - Objets et composants

Les commandes partent de `projet_v3_feux`, après le lancement de `scripts/run.ps1`.
Les sources fournies représentent la version corrigée. Les modifications proposées
ci-dessous servent à comprendre et à faire évoluer cette base.

## 17 - Créer un objet et respecter son contrat

Ouvre `include/domaine/FeuTricolore.hpp` puis son fichier `.cpp`. Repère le
constructeur, l'état privé et les méthodes publiques.

1. Quel est l'état d'un feu juste après sa construction ?
2. Quelle méthode réalise le passage du vert à l'orange ?
3. Que se passe-t-il si on demande l'orange depuis le rouge ?
4. Pourquoi `couleur_` n'est-elle pas publique ?

Lance les tests correspondants :

```powershell
.\.venv\Scripts\ctest.exe --preset release -R unit.feu_
```

**Correction.** Le feu démarre au rouge. `passerALOrange()` exige qu'il soit vert.
Depuis le rouge, cette méthode lance `std::logic_error` et laisse l'état inchangé.
L'attribut privé permet d'imposer ces règles à tous les appelants.

La création suivante construit deux objets différents du même type :

```cpp
feux::FeuTricolore premier{"Nord-Sud"};
feux::FeuTricolore second{"Est-Ouest"};
premier.passerAuVert();
// second reste rouge : chaque objet possede son propre etat.
```

Dans l'application complète, c'est le contrôleur qui possède ces objets et assure
leur coordination. Créer deux feux indépendants ne suffit pas à coordonner un carrefour.

## 18 - Comprendre la composition et les frontières temporelles

Lance ce plan personnalisé :

```powershell
.\build\feux_tricolores.exe --duree 17 --vert-ns 7 --vert-eo 3 --orange 2 --tout-rouge 1
```

Avant de regarder la trace, prédis les états aux temps 0, 1, 8, 10, 11, 14 et 16.

| Instant | Phase attendue | Nord-Sud | Est-Ouest |
| --- | --- | --- | --- |
| 0 | Tout rouge avant NS | Rouge | Rouge |
| 1 | Vert NS | Vert | Rouge |
| 8 | Orange NS | Orange | Rouge |
| 10 | Tout rouge avant EO | Rouge | Rouge |
| 11 | Vert EO | Rouge | Vert |
| 14 | Orange EO | Rouge | Orange |
| 16 | Nouveau cycle, tout rouge avant NS | Rouge | Rouge |

**Correction.** La durée du cycle vaut `7 + 3 + 2*2 + 2*1 = 16` secondes.
Le contrôleur demande les durées à son `PlanFeux`. À la fin d'un intervalle, il
applique la nouvelle phase et initialise son compteur. Une ligne à t=8 montre
donc déjà l'orange, pas la dernière seconde du vert.

Ajoute ensuite `--arret 3 --reprise 6`. À t=3, les deux axes deviennent rouges.
À t=6, la reprise recommence par un intervalle tout rouge ; NS devient vert à t=7.
La reprise remet le cycle à son début, sans remettre l'horloge globale à zéro.

## 19 - Remplacer une dépendance grâce à une interface

Ouvre `tests/unit/test_simulation.cpp`. La classe `Collecteur` implémente
`IAffichage` et stocke des observations en mémoire.

```cpp
class Collecteur final : public feux::IAffichage {
public:
    void debut() override { ++debuts; }
    void afficher(const feux::EtatCarrefour& etat) override {
        etats.push_back(etat);
    }
    int debuts{0};
    std::vector<feux::EtatCarrefour> etats;
};
```

Ce fragment utilise les en-têtes de la simulation et `<vector>`. Retrouve dans le
test `simulation_injection_affichage` les objets injectés au simulateur.

```powershell
.\.venv\Scripts\ctest.exe --preset release -R unit.simulation_injection_affichage
```

**Travail.** Ajoute un nouveau test qui exécute une simulation de 5 secondes et
vérifie que le collecteur reçoit 6 observations : t=0, 1, 2, 3, 4 et 5.

**Correction à ajouter dans `ajouterTestsSimulation`.**

```cpp
tests.emplace_back("simulation_six_observations", [] {
    ControleurCarrefour c;
    Collecteur collecteur;
    Simulateur simulateur{c, collecteur};
    simulateur.executer(Scenario{5});
    verifier(collecteur.etats.size() == 6, "Six observations attendues");
    verifier(collecteur.etats.back().nordSud == Couleur::Orange,
             "Orange attendu a t=5");
});
```

Ajoute aussi `simulation_six_observations` à la liste `unit_cases` de
`CMakeLists.txt`, puis relance compilation et tests. Il y aura 39 tests C++ et
112 tests au total si tu conserves ce nouveau cas.

Le simulateur n'a pas changé. Seul l'objet qui reçoit les observations a changé :
c'est l'intérêt de dépendre d'une interface. Compter des observations ne doit pas
être confondu avec mesurer une durée : l'instant final est lui aussi observé.

## 20 - Détecter une mauvaise coordination avant livraison

Travaille sur une copie ou une branche de test. Dans la méthode `appliquerPhase`
de `src/domaine/ControleurCarrefour.cpp`, repère uniquement ce bloc :

```cpp
case Phase::VertEstOuest:
    nordSud_.passerAuRouge();
    estOuest_.passerAuVert();
    break;
```

Remplace temporairement `nordSud_.passerAuRouge()` par `nordSud_.passerAuVert()`
dans ce bloc. Recompile puis lance les tests :

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\test.ps1
$LASTEXITCODE
```

**Attendu.** À la phase EO, les deux axes passent au vert. Le test C++
`carrefour_exclusion_axes` échoue, et les tests Python des traces détectent aussi
le défaut. Le script retourne un code non nul et ne produit aucune preuve de
réussite utilisable par le packaging. Jenkins bloque la livraison.

**Correction.** Rétablis le rouge Nord-Sud dans ce bloc, enregistre et relance
compilation et tests. Le résultat revient au vert.

Le composant `FeuTricolore` peut être correct alors que la coordination entre
plusieurs feux est incorrecte. Les tests d'objets isolés et les tests de leurs
interactions répondent à des questions différentes.
