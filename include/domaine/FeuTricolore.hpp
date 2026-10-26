#pragma once

#include "domaine/EtatCarrefour.hpp"
#include <string>

namespace feux {

class FeuTricolore {
public:
    explicit FeuTricolore(std::string nom);

    const std::string& nom() const noexcept { return nom_; }
    Couleur couleur() const noexcept { return couleur_; }

    // Transitions normales : rouge -> vert -> orange -> rouge.
    void passerAuVert();
    void passerALOrange();
    // Le retour au rouge est aussi autorise pour un arret d'urgence.
    void passerAuRouge() noexcept;

private:
    std::string nom_;
    Couleur couleur_{Couleur::Rouge};
};

}  // namespace feux
