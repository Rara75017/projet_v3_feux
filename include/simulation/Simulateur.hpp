#pragma once

#include "domaine/ControleurCarrefour.hpp"
#include "simulation/IAffichage.hpp"
#include "simulation/Scenario.hpp"

namespace feux {

class Simulateur {
public:
    // References injectees : les deux objets doivent vivre plus longtemps que lui.
    Simulateur(ControleurCarrefour& controleur, IAffichage& affichage);
    // Temps virtuel : aucun sleep. Les evenements precedant l'affichage sont
    // relatifs au debut de cet appel ; l'horloge du controleur reste cumulative.
    void executer(const Scenario& scenario);

private:
    ControleurCarrefour& controleur_;
    IAffichage& affichage_;
};

}  // namespace feux
