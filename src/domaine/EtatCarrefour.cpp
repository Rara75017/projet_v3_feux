#include "domaine/EtatCarrefour.hpp"

namespace feux {

const char* nomCouleur(Couleur couleur) noexcept {
    switch (couleur) {
        case Couleur::Rouge: return "ROUGE";
        case Couleur::Orange: return "ORANGE";
        case Couleur::Vert: return "VERT";
    }
    return "INCONNU";
}

const char* nomPhase(Phase phase) noexcept {
    switch (phase) {
        case Phase::ToutRougeAvantNordSud: return "TOUT_ROUGE_AVANT_NS";
        case Phase::VertNordSud: return "VERT_NS";
        case Phase::OrangeNordSud: return "ORANGE_NS";
        case Phase::ToutRougeAvantEstOuest: return "TOUT_ROUGE_AVANT_EO";
        case Phase::VertEstOuest: return "VERT_EO";
        case Phase::OrangeEstOuest: return "ORANGE_EO";
    }
    return "INCONNU";
}

}  // namespace feux
