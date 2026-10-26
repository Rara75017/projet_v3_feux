#include "TestSupport.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: feux_unit_tests <cas|--list>\n";
        return 2;
    }
    std::vector<tests::Test> tous;
    tests::ajouterTestsDomaine(tous);
    tests::ajouterTestsSimulation(tous);
    tests::ajouterTestsConsole(tous);
    if (std::string(argv[1]) == "--list") {
        for (const auto& test : tous) std::cout << test.first << '\n';
        return 0;
    }
    for (const auto& test : tous) {
        if (test.first == argv[1]) {
            try {
                test.second();
                std::cout << "PASS " << test.first << '\n';
                return 0;
            } catch (const std::exception& erreur) {
                std::cerr << "FAIL " << test.first << ": " << erreur.what() << '\n';
                return 1;
            }
        }
    }
    std::cerr << "Cas inconnu\n";
    return 2;
}
