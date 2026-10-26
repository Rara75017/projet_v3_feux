#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace tests {

using Test = std::pair<std::string, std::function<void()>>;
inline void verifier(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

template<class Exception, class Fonction>
void verifierException(Fonction action) {
    bool recue = false;
    try { action(); } catch (const Exception&) { recue = true; }
    verifier(recue, "Exception attendue non recue");
}

void ajouterTestsDomaine(std::vector<Test>& tests);
void ajouterTestsSimulation(std::vector<Test>& tests);
void ajouterTestsConsole(std::vector<Test>& tests);

}  // namespace tests
