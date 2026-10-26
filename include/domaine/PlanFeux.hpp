#pragma once

#include "domaine/EtatCarrefour.hpp"

namespace feux {

// Objet valeur : valide a la construction, sans setter.
class PlanFeux {
public:
    explicit PlanFeux(int vertNordSud = 4, int vertEstOuest = 4,
                      int orange = 2, int toutRouge = 1);
    int duree(Phase phase) const;
    int dureeCycle() const noexcept;

private:
    int vertNordSud_;
    int vertEstOuest_;
    int orange_;
    int toutRouge_;
};

}  // namespace feux
