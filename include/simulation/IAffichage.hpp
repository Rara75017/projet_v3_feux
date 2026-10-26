#pragma once

#include "domaine/EtatCarrefour.hpp"

namespace feux {

// Le simulateur depend de ce contrat, pas d'une console precise.
class IAffichage {
public:
    virtual ~IAffichage() = default;
    virtual void debut() = 0;
    virtual void afficher(const EtatCarrefour& etat) = 0;
};

}  // namespace feux
