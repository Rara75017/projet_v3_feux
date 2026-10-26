#include "TestSupport.hpp"
#include "console/AffichageConsole.hpp"
#include "console/OptionsLigneCommande.hpp"
#include "domaine/ControleurCarrefour.hpp"
#include <sstream>

namespace tests {
using namespace feux;

void ajouterTestsConsole(std::vector<Test>& tests) {
    tests.emplace_back("console_trace_exacte", [] {
        std::ostringstream sortie;
        AffichageConsole affichage{sortie};
        affichage.debut();
        affichage.afficher(ControleurCarrefour{}.etat());
        verifier(sortie.str() ==
                 "temps;phase;nord_sud;est_ouest;restant;urgence\n"
                 "0;TOUT_ROUGE_AVANT_NS;ROUGE;ROUGE;1;non\n", "CSV initial");
    });
    tests.emplace_back("console_urgence", [] {
        ControleurCarrefour c;
        c.avancerUneSeconde(); c.arreterUrgence();
        std::ostringstream sortie; AffichageConsole affichage{sortie};
        affichage.afficher(c.etat());
        verifier(sortie.str() == "1;URGENCE;ROUGE;ROUGE;0;oui\n", "CSV urgence");
    });
    tests.emplace_back("console_erreur_ecriture", [] {
        std::ostringstream sortie;
        sortie.setstate(std::ios::badbit);
        AffichageConsole affichage{sortie};
        verifierException<std::runtime_error>([&] { affichage.debut(); });
    });
    tests.emplace_back("options_defaut", [] {
        const char* argv[] = {"feux_tricolores"};
        const auto o = OptionsLigneCommande::lire(1, argv);
        verifier(!o.aide && o.scenario.duree() == 20, "Options par defaut");
        verifier(o.plan.dureeCycle() == 14, "Plan par defaut");
    });
    tests.emplace_back("options_personnalisees", [] {
        const char* argv[] = {"feux", "--vert-ns", "3", "--vert-eo", "5",
                             "--orange", "1", "--tout-rouge", "2",
                             "--duree", "30", "--arret", "4", "--reprise", "9"};
        const auto o = OptionsLigneCommande::lire(15, argv);
        verifier(o.plan.dureeCycle() == 14 && o.scenario.duree() == 30, "Durees");
        verifier(o.scenario.arret() == 4 && o.scenario.reprise() == 9, "Evenements");
    });
    tests.emplace_back("options_help", [] {
        const char* argv[] = {"feux", "--help"};
        verifier(OptionsLigneCommande::lire(2, argv).aide, "Aide reconnue");
    });
    tests.emplace_back("options_inconnue_refusee", [] {
        const char* argv[] = {"feux", "--intrus", "1"};
        verifierException<std::invalid_argument>([&] { OptionsLigneCommande::lire(3, argv); });
    });
    tests.emplace_back("options_dupliquee_refusee", [] {
        const char* argv[] = {"feux", "--duree", "1", "--duree", "2"};
        verifierException<std::invalid_argument>([&] { OptionsLigneCommande::lire(5, argv); });
    });
    tests.emplace_back("options_conversion_stricte", [] {
        for (const char* mauvais : {"1abc", "+1", " 1", "", "1.5", "9999999999999999999"}) {
            const char* argv[] = {"feux", "--duree", mauvais};
            verifierException<std::invalid_argument>([&] { OptionsLigneCommande::lire(3, argv); });
        }
    });
    tests.emplace_back("options_valeur_absente", [] {
        const char* argv[] = {"feux", "--duree"};
        verifierException<std::invalid_argument>([&] { OptionsLigneCommande::lire(2, argv); });
    });
}
}  // namespace tests
