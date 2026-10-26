#pragma once

#include "domaine/FeuTricolore.hpp"
#include "domaine/PlanFeux.hpp"

namespace feux {

// Possede les deux feux. Les appelants ne peuvent pas les modifier directement.
class ControleurCarrefour {
public:
    explicit ControleurCarrefour(PlanFeux plan = PlanFeux{});

    void avancerUneSeconde();
    void arreterUrgence() noexcept;
    void reprendre();
    EtatCarrefour etat() const noexcept;

private:
    void appliquerPhase(Phase phase);
    void passerALaPhaseSuivante();

    PlanFeux plan_;
    FeuTricolore nordSud_{"Nord-Sud"};
    FeuTricolore estOuest_{"Est-Ouest"};
    Phase phase_{Phase::ToutRougeAvantNordSud};
    int secondesRestantes_;
    std::uint64_t temps_{0};
    bool urgence_{false};
};

}  // namespace feux
