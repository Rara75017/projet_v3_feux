#pragma once

#include <cstdint>

namespace feux {

enum class Couleur { Rouge, Orange, Vert };
enum class Phase {
    ToutRougeAvantNordSud, VertNordSud, OrangeNordSud,
    ToutRougeAvantEstOuest, VertEstOuest, OrangeEstOuest
};

const char* nomCouleur(Couleur couleur) noexcept;
const char* nomPhase(Phase phase) noexcept;

// Copie de l'etat courant : modifier cette valeur ne modifie pas le controleur.
struct EtatCarrefour {
    std::uint64_t temps;
    Phase phase;
    Couleur nordSud;
    Couleur estOuest;
    int secondesRestantes;
    bool urgence;
};

}  // namespace feux
