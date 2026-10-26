#include "simulation/Scenario.hpp"
#include <stdexcept>
#include <initializer_list>

namespace feux {

Scenario::Scenario(int duree, std::optional<int> arret, std::optional<int> reprise)
    : duree_(duree), arret_(arret), reprise_(reprise) {
    if (duree_ < 0 || duree_ > 3600) {
        throw std::domain_error("La duree de simulation doit etre entre 0 et 3600");
    }
    for (const auto instant : {arret_, reprise_}) {
        if (instant && (*instant < 0 || *instant > duree_)) {
            throw std::domain_error("Un evenement doit etre compris dans la simulation");
        }
    }
    if (reprise_ && !arret_) {
        throw std::domain_error("La reprise exige un arret");
    }
    if (reprise_ && *reprise_ <= *arret_) {
        throw std::domain_error("La reprise doit suivre strictement l'arret");
    }
}

}  // namespace feux
