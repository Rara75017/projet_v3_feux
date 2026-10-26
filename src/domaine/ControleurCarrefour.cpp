#include "domaine/ControleurCarrefour.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

namespace feux {

ControleurCarrefour::ControleurCarrefour(PlanFeux plan)
    : plan_(std::move(plan)), secondesRestantes_(plan_.duree(phase_)) {}

void ControleurCarrefour::appliquerPhase(Phase phase) {
    switch (phase) {
        case Phase::ToutRougeAvantNordSud:
        case Phase::ToutRougeAvantEstOuest:
            nordSud_.passerAuRouge();
            estOuest_.passerAuRouge();
            break;
        case Phase::VertNordSud:
            estOuest_.passerAuRouge();
            nordSud_.passerAuVert();
            break;
        case Phase::OrangeNordSud:
            estOuest_.passerAuRouge();
            nordSud_.passerALOrange();
            break;
        case Phase::VertEstOuest:
            nordSud_.passerAuRouge();
            estOuest_.passerAuVert();
            break;
        case Phase::OrangeEstOuest:
            nordSud_.passerAuRouge();
            estOuest_.passerALOrange();
            break;
        default:
            throw std::logic_error("Phase inconnue");
    }
    phase_ = phase;
    secondesRestantes_ = plan_.duree(phase);
}

void ControleurCarrefour::passerALaPhaseSuivante() {
    switch (phase_) {
        case Phase::ToutRougeAvantNordSud:
            appliquerPhase(Phase::VertNordSud); break;
        case Phase::VertNordSud:
            appliquerPhase(Phase::OrangeNordSud); break;
        case Phase::OrangeNordSud:
            appliquerPhase(Phase::ToutRougeAvantEstOuest); break;
        case Phase::ToutRougeAvantEstOuest:
            appliquerPhase(Phase::VertEstOuest); break;
        case Phase::VertEstOuest:
            appliquerPhase(Phase::OrangeEstOuest); break;
        case Phase::OrangeEstOuest:
            appliquerPhase(Phase::ToutRougeAvantNordSud); break;
    }
}

void ControleurCarrefour::avancerUneSeconde() {
    if (temps_ == std::numeric_limits<std::uint64_t>::max()) {
        throw std::overflow_error("Compteur de temps sature");
    }
    ++temps_;
    if (urgence_) return;
    --secondesRestantes_;
    if (secondesRestantes_ == 0) passerALaPhaseSuivante();
}

void ControleurCarrefour::arreterUrgence() noexcept {
    nordSud_.passerAuRouge();
    estOuest_.passerAuRouge();
    urgence_ = true;
    secondesRestantes_ = 0;
}

void ControleurCarrefour::reprendre() {
    if (!urgence_) return;
    // Repartir par un intervalle tout rouge complet, meme apres une interruption.
    appliquerPhase(Phase::ToutRougeAvantNordSud);
    urgence_ = false;
}

EtatCarrefour ControleurCarrefour::etat() const noexcept {
    return {temps_, phase_, nordSud_.couleur(), estOuest_.couleur(),
            secondesRestantes_, urgence_};
}

}  // namespace feux
