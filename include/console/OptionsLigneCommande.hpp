#pragma once

#include "domaine/PlanFeux.hpp"
#include "simulation/Scenario.hpp"

namespace feux {

struct Options {
    PlanFeux plan;
    Scenario scenario;
    bool aide{false};
};

class OptionsLigneCommande {
public:
    static Options lire(int argc, const char* const argv[]);
    static const char* aide() noexcept;
};

}  // namespace feux
