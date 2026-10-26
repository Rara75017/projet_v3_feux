#include "simulation/Simulateur.hpp"

namespace feux {

Simulateur::Simulateur(ControleurCarrefour& controleur, IAffichage& affichage)
    : controleur_(controleur), affichage_(affichage) {}

void Simulateur::executer(const Scenario& scenario) {
    affichage_.debut();
    for (int instant = 0; instant <= scenario.duree(); ++instant) {
        if (instant > 0) controleur_.avancerUneSeconde();
        if (scenario.arret() && instant == *scenario.arret()) {
            controleur_.arreterUrgence();
        }
        if (scenario.reprise() && instant == *scenario.reprise()) {
            controleur_.reprendre();
        }
        affichage_.afficher(controleur_.etat());
    }
}

}  // namespace feux
