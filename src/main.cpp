#include "console/AffichageConsole.hpp"
#include "console/OptionsLigneCommande.hpp"
#include "domaine/ControleurCarrefour.hpp"
#include "simulation/Simulateur.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        const auto options = feux::OptionsLigneCommande::lire(argc, argv);
        if (options.aide) {
            std::cout << feux::OptionsLigneCommande::aide();
            return 0;
        }

        // Point d'assemblage : les objets sont construits et relies ici.
        feux::ControleurCarrefour controleur{options.plan};
        feux::AffichageConsole affichage{std::cout};
        feux::Simulateur simulateur{controleur, affichage};
        simulateur.executer(options.scenario);
        return 0;
    } catch (const std::invalid_argument& erreur) {
        std::cerr << "Erreur: " << erreur.what() << '\n';
        return 2;
    } catch (const std::domain_error& erreur) {
        std::cerr << "Erreur: " << erreur.what() << '\n';
        return 3;
    } catch (const std::exception& erreur) {
        std::cerr << "Erreur interne: " << erreur.what() << '\n';
        return 1;
    }
}
