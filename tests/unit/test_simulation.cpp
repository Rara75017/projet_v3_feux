#include "TestSupport.hpp"
#include "simulation/Simulateur.hpp"

namespace tests {
using namespace feux;

namespace {
class Collecteur final : public IAffichage {
public:
    void debut() override { ++debuts; }
    void afficher(const EtatCarrefour& etat) override { etats.push_back(etat); }
    int debuts{0};
    std::vector<EtatCarrefour> etats;
};
}

void ajouterTestsSimulation(std::vector<Test>& tests) {
    tests.emplace_back("scenario_bornes", [] {
        verifier(Scenario{0}.duree() == 0 && Scenario{3600}.duree() == 3600, "Bornes");
        verifierException<std::domain_error>([] { Scenario s{-1}; });
        verifierException<std::domain_error>([] { Scenario s{3601}; });
    });
    tests.emplace_back("scenario_reprise_sans_arret_refusee", [] {
        verifierException<std::domain_error>([] { Scenario s{20, std::nullopt, 4}; });
    });
    tests.emplace_back("scenario_ordre_evenements", [] {
        verifierException<std::domain_error>([] { Scenario s{20, 5, 5}; });
        verifierException<std::domain_error>([] { Scenario s{20, 5, 4}; });
        verifier(Scenario{20, 5, 6}.reprise() == 6, "Ordre valide");
    });
    tests.emplace_back("scenario_evenement_hors_duree", [] {
        verifierException<std::domain_error>([] { Scenario s{20, 21}; });
        verifierException<std::domain_error>([] { Scenario s{20, -1}; });
        verifierException<std::domain_error>([] { Scenario s{20, 1, 21}; });
    });
    tests.emplace_back("simulation_duree_zero", [] {
        ControleurCarrefour c; Collecteur affichage; Simulateur s{c, affichage};
        s.executer(Scenario{0});
        verifier(affichage.debuts == 1 && affichage.etats.size() == 1, "Instant initial seul");
        verifier(affichage.etats.front().temps == 0, "Temps initial");
    });
    tests.emplace_back("simulation_injection_affichage", [] {
        ControleurCarrefour c; Collecteur affichage; Simulateur s{c, affichage};
        s.executer(Scenario{20});
        verifier(affichage.debuts == 1 && affichage.etats.size() == 21, "21 instants");
        verifier(affichage.etats[5].nordSud == Couleur::Orange, "Objet recu par interface");
    });
    tests.emplace_back("simulation_arret_et_reprise", [] {
        ControleurCarrefour c; Collecteur affichage; Simulateur s{c, affichage};
        s.executer(Scenario{10, 3, 6});
        verifier(!affichage.etats[2].urgence && affichage.etats[3].urgence, "Frontiere arret");
        verifier(affichage.etats[5].urgence && !affichage.etats[6].urgence, "Frontiere reprise");
        verifier(affichage.etats[6].nordSud == Couleur::Rouge, "Rouge a la reprise");
        verifier(affichage.etats[7].nordSud == Couleur::Vert, "Vert apres reprise");
    });
    tests.emplace_back("simulation_arret_initial", [] {
        ControleurCarrefour c; Collecteur affichage; Simulateur s{c, affichage};
        s.executer(Scenario{2, 0});
        for (const auto& e : affichage.etats) {
            verifier(e.urgence && e.nordSud == Couleur::Rouge &&
                     e.estOuest == Couleur::Rouge, "Urgence des t=0");
        }
    });
    tests.emplace_back("simulation_evenement_final", [] {
        ControleurCarrefour c; Collecteur affichage; Simulateur s{c, affichage};
        s.executer(Scenario{1, 1});
        verifier(!affichage.etats[0].urgence && affichage.etats[1].urgence, "Dernier instant");
    });
}
}  // namespace tests
