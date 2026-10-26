#pragma once

#include "simulation/IAffichage.hpp"
#include <iosfwd>  // IWYU pragma: keep (std::ostream est utilise par reference ci-dessous)

namespace feux {

class AffichageConsole final : public IAffichage {
public:
    explicit AffichageConsole(std::ostream& sortie);
    void debut() override;
    void afficher(const EtatCarrefour& etat) override;

private:
    void verifierSortie() const;
    std::ostream& sortie_;
};

}  // namespace feux
