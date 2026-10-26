#include "TestSupport.hpp"
#include "domaine/ControleurCarrefour.hpp"

namespace tests {
using namespace feux;

namespace {
void avancer(ControleurCarrefour& controleur, int secondes) {
    for (int i = 0; i < secondes; ++i) controleur.avancerUneSeconde();
}
}

void ajouterTestsDomaine(std::vector<Test>& tests) {
    tests.emplace_back("feu_initial_rouge", [] {
        FeuTricolore feu{"Test"};
        verifier(feu.couleur() == Couleur::Rouge && feu.nom() == "Test", "Etat initial");
    });
    tests.emplace_back("feu_nom_vide_refuse", [] {
        verifierException<std::invalid_argument>([] { FeuTricolore feu{""}; });
    });
    tests.emplace_back("feu_cycle", [] {
        FeuTricolore feu{"Test"};
        feu.passerAuVert();
        verifier(feu.couleur() == Couleur::Vert, "Vert attendu");
        feu.passerALOrange();
        verifier(feu.couleur() == Couleur::Orange, "Orange attendu");
        feu.passerAuRouge();
        verifier(feu.couleur() == Couleur::Rouge, "Rouge attendu");
    });
    tests.emplace_back("feu_orange_depuis_rouge_refuse", [] {
        FeuTricolore feu{"Test"};
        verifierException<std::logic_error>([&] { feu.passerALOrange(); });
        verifier(feu.couleur() == Couleur::Rouge, "Echec sans modification");
    });
    tests.emplace_back("feu_vert_depuis_orange_refuse", [] {
        FeuTricolore feu{"Test"};
        feu.passerAuVert(); feu.passerALOrange();
        verifierException<std::logic_error>([&] { feu.passerAuVert(); });
    });
    tests.emplace_back("feu_retour_rouge_force", [] {
        FeuTricolore feu{"Test"};
        feu.passerAuVert(); feu.passerAuRouge(); feu.passerAuRouge();
        verifier(feu.couleur() == Couleur::Rouge, "Rouge idempotent");
    });
    tests.emplace_back("plan_durees", [] {
        PlanFeux plan{3, 5, 2, 4};
        verifier(plan.duree(Phase::VertNordSud) == 3, "Duree NS");
        verifier(plan.duree(Phase::VertEstOuest) == 5, "Duree EO");
        verifier(plan.duree(Phase::OrangeNordSud) == 2, "Duree orange");
        verifier(plan.duree(Phase::ToutRougeAvantEstOuest) == 4, "Duree tout rouge");
        verifier(plan.dureeCycle() == 20, "Duree cycle");
    });
    tests.emplace_back("plan_zero_refuse", [] {
        verifierException<std::domain_error>([] { PlanFeux p{0, 1, 1, 1}; });
        verifierException<std::domain_error>([] { PlanFeux p{1, 0, 1, 1}; });
        verifierException<std::domain_error>([] { PlanFeux p{1, 1, 0, 1}; });
        verifierException<std::domain_error>([] { PlanFeux p{1, 1, 1, 0}; });
    });
    tests.emplace_back("plan_bornes", [] {
        verifierException<std::domain_error>([] { PlanFeux p{-1, 1, 1, 1}; });
        verifierException<std::domain_error>([] { PlanFeux p{1, 3601, 1, 1}; });
        verifier(PlanFeux{3600, 3600, 3600, 3600}.dureeCycle() == 21600, "Borne max");
    });
    tests.emplace_back("carrefour_initial", [] {
        ControleurCarrefour c;
        const auto e = c.etat();
        verifier(e.temps == 0 && e.secondesRestantes == 1, "Horloge initiale");
        verifier(e.nordSud == Couleur::Rouge && e.estOuest == Couleur::Rouge,
                 "Les deux axes doivent commencer au rouge");
    });
    tests.emplace_back("carrefour_frontiere_vert", [] {
        ControleurCarrefour c{PlanFeux{4, 4, 2, 2}};
        c.avancerUneSeconde();
        verifier(c.etat().nordSud == Couleur::Rouge, "Tout rouge non termine");
        c.avancerUneSeconde();
        verifier(c.etat().nordSud == Couleur::Vert, "Vert a la frontiere");
        verifier(c.etat().secondesRestantes == 4, "Duree du nouveau vert");
    });
    tests.emplace_back("carrefour_frontiere_orange", [] {
        ControleurCarrefour c;
        avancer(c, 4);
        verifier(c.etat().nordSud == Couleur::Vert, "Vert avant frontiere");
        c.avancerUneSeconde();
        verifier(c.etat().nordSud == Couleur::Orange, "Orange a t=5");
    });
    tests.emplace_back("carrefour_intervalle_tout_rouge", [] {
        ControleurCarrefour c;
        avancer(c, 7);
        auto e = c.etat();
        verifier(e.nordSud == Couleur::Rouge && e.estOuest == Couleur::Rouge,
                 "Tout rouge entre les deux axes");
        c.avancerUneSeconde();
        e = c.etat();
        verifier(e.nordSud == Couleur::Rouge && e.estOuest == Couleur::Vert,
                 "EO seulement apres tout rouge");
    });
    tests.emplace_back("carrefour_cycle_complet", [] {
        ControleurCarrefour c;
        avancer(c, 14);
        const auto e = c.etat();
        verifier(e.phase == Phase::ToutRougeAvantNordSud, "Retour debut du cycle");
        verifier(e.nordSud == Couleur::Rouge && e.estOuest == Couleur::Rouge, "Deux rouges");
        verifier(e.secondesRestantes == 1 && e.temps == 14, "Temps du cycle");
    });
    tests.emplace_back("carrefour_exclusion_axes", [] {
        for (const PlanFeux plan : {PlanFeux{}, PlanFeux{1, 1, 1, 1}, PlanFeux{3, 7, 2, 4}}) {
            ControleurCarrefour c{plan};
            for (int i = 0; i < 2000; ++i) {
                const auto e = c.etat();
                verifier(e.nordSud == Couleur::Rouge || e.estOuest == Couleur::Rouge,
                         "Un axe actif exige l'autre au rouge");
                verifier(e.secondesRestantes > 0, "Compteur de phase positif");
                c.avancerUneSeconde();
            }
        }
    });
    tests.emplace_back("carrefour_etat_est_une_copie", [] {
        ControleurCarrefour c;
        auto copie = c.etat();
        copie.nordSud = Couleur::Vert;
        verifier(copie.nordSud != c.etat().nordSud, "Encapsulation de l'etat");
    });
    tests.emplace_back("urgence_depuis_toutes_phases", [] {
        for (int t = 0; t < 14; ++t) {
            ControleurCarrefour c;
            avancer(c, t); c.arreterUrgence(); avancer(c, 20);
            const auto e = c.etat();
            verifier(e.urgence && e.secondesRestantes == 0, "Urgence maintenue");
            verifier(e.nordSud == Couleur::Rouge && e.estOuest == Couleur::Rouge,
                     "Arret simultane des axes");
            verifier(e.temps == static_cast<unsigned>(t + 20), "Le temps continue");
        }
    });
    tests.emplace_back("urgence_reprise_par_tout_rouge", [] {
        ControleurCarrefour c{PlanFeux{4, 4, 2, 2}};
        avancer(c, 3); c.arreterUrgence(); c.reprendre();
        verifier(!c.etat().urgence && c.etat().secondesRestantes == 2, "Reprise");
        c.avancerUneSeconde();
        verifier(c.etat().nordSud == Couleur::Rouge, "Tout rouge complet");
        c.avancerUneSeconde();
        verifier(c.etat().nordSud == Couleur::Vert, "Reprise NS");
    });
    tests.emplace_back("urgence_appels_idempotents", [] {
        ControleurCarrefour c;
        c.avancerUneSeconde();
        c.reprendre();
        verifier(c.etat().nordSud == Couleur::Vert, "Reprise hors urgence sans effet");
        c.arreterUrgence(); c.arreterUrgence();
        verifier(c.etat().urgence, "Arret repete sans effet indesirable");
    });
}
}  // namespace tests
