#include "domaine/PlanFeux.hpp"
#include <stdexcept>
#include <initializer_list>

namespace feux {

PlanFeux::PlanFeux(int vertNordSud, int vertEstOuest, int orange, int toutRouge)
    : vertNordSud_(vertNordSud), vertEstOuest_(vertEstOuest),
      orange_(orange), toutRouge_(toutRouge) {
    for (const int valeur : {vertNordSud_, vertEstOuest_, orange_, toutRouge_}) {
        if (valeur < 1 || valeur > 3600) {
            throw std::domain_error("Les durees des phases doivent etre entre 1 et 3600");
        }
    }
}

int PlanFeux::duree(Phase phase) const {
    switch (phase) {
        case Phase::VertNordSud: return vertNordSud_;
        case Phase::VertEstOuest: return vertEstOuest_;
        case Phase::OrangeNordSud:
        case Phase::OrangeEstOuest: return orange_;
        case Phase::ToutRougeAvantNordSud:
        case Phase::ToutRougeAvantEstOuest: return toutRouge_;
    }
    throw std::logic_error("Phase inconnue");
}

int PlanFeux::dureeCycle() const noexcept {
    return vertNordSud_ + vertEstOuest_ + 2 * orange_ + 2 * toutRouge_;
}

}  // namespace feux
