#include "console/AffichageConsole.hpp"

#include <ostream>
#include <stdexcept>

namespace feux {

AffichageConsole::AffichageConsole(std::ostream& sortie) : sortie_(sortie) {}

void AffichageConsole::verifierSortie() const {
    if (!sortie_) throw std::runtime_error("Ecriture de la trace impossible");
}

void AffichageConsole::debut() {
    sortie_ << "temps;phase;nord_sud;est_ouest;restant;urgence\n";
    verifierSortie();
}

void AffichageConsole::afficher(const EtatCarrefour& etat) {
    sortie_ << etat.temps << ';'
            << (etat.urgence ? "URGENCE" : nomPhase(etat.phase)) << ';'
            << nomCouleur(etat.nordSud) << ';' << nomCouleur(etat.estOuest) << ';'
            << etat.secondesRestantes << ';' << (etat.urgence ? "oui" : "non") << '\n';
    verifierSortie();
}

}  // namespace feux
