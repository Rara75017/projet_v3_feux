#include "domaine/FeuTricolore.hpp"

#include <stdexcept>
#include <utility>

namespace feux {

FeuTricolore::FeuTricolore(std::string nom) : nom_(std::move(nom)) {
    if (nom_.empty()) throw std::invalid_argument("Le feu doit avoir un nom");
}

void FeuTricolore::passerAuVert() {
    if (couleur_ != Couleur::Rouge) {
        throw std::logic_error("Le vert exige un feu rouge");
    }
    couleur_ = Couleur::Vert;
}

void FeuTricolore::passerALOrange() {
    if (couleur_ != Couleur::Vert) {
        throw std::logic_error("L'orange exige un feu vert");
    }
    couleur_ = Couleur::Orange;
}

void FeuTricolore::passerAuRouge() noexcept {
    couleur_ = Couleur::Rouge;
}

}  // namespace feux
