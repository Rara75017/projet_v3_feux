#include "console/OptionsLigneCommande.hpp"

#include <charconv>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace feux {
namespace {

int lireEntier(std::string_view texte) {
    int valeur = 0;
    const auto resultat = std::from_chars(texte.data(), texte.data() + texte.size(), valeur);
    if (texte.empty() || resultat.ec != std::errc{} || resultat.ptr != texte.data() + texte.size()) {
        throw std::invalid_argument("Entier decimal attendu: " + std::string(texte));
    }
    return valeur;
}

}  // namespace

Options OptionsLigneCommande::lire(int argc, const char* const argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        return {PlanFeux{}, Scenario{}, true};
    }
    int duree = 20, vertNS = 4, vertEO = 4, orange = 2, toutRouge = 1;
    std::optional<int> arret, reprise;
    std::set<std::string> vues;
    for (int i = 1; i < argc; ++i) {
        const std::string option = argv[i];
        if (option == "--help") {
            throw std::invalid_argument("--help doit etre utilise seul");
        }
        if (option != "--duree" && option != "--vert-ns" && option != "--vert-eo"
                && option != "--orange" && option != "--tout-rouge"
                && option != "--arret" && option != "--reprise") {
            throw std::invalid_argument("Option inconnue: " + option);
        }
        if (!vues.insert(option).second) {
            throw std::invalid_argument("Option repetee: " + option);
        }
        if (++i >= argc) {
            throw std::invalid_argument("Valeur manquante: " + option);
        }
        const int valeur = lireEntier(argv[i]);
        if (option == "--duree") duree = valeur;
        else if (option == "--vert-ns") vertNS = valeur;
        else if (option == "--vert-eo") vertEO = valeur;
        else if (option == "--orange") orange = valeur;
        else if (option == "--tout-rouge") toutRouge = valeur;
        else if (option == "--arret") arret = valeur;
        else reprise = valeur;
    }
    return {PlanFeux{vertNS, vertEO, orange, toutRouge},
            Scenario{duree, arret, reprise}, false};
}

const char* OptionsLigneCommande::aide() noexcept {
    return
        "Usage: feux_tricolores [options]\n"
        "--duree N       Simulation de 0 a 3600 secondes (defaut 20)\n"
        "--vert-ns N     Vert Nord-Sud, de 1 a 3600 (defaut 4)\n"
        "--vert-eo N     Vert Est-Ouest, de 1 a 3600 (defaut 4)\n"
        "--orange N      Orange, de 1 a 3600 (defaut 2)\n"
        "--tout-rouge N  Intervalle tout rouge, de 1 a 3600 (defaut 1)\n"
        "--arret N       Instant d'arret d'urgence, optionnel\n"
        "--reprise N     Instant de reprise apres l'arret, optionnel\n"
        "--help          Afficher cette aide\n";
}

}  // namespace feux
