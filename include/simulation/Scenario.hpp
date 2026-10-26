#pragma once

#include <optional>

namespace feux {

class Scenario {
public:
    explicit Scenario(int duree = 20, std::optional<int> arret = std::nullopt,
                      std::optional<int> reprise = std::nullopt);

    int duree() const noexcept { return duree_; }
    const std::optional<int>& arret() const noexcept { return arret_; }
    const std::optional<int>& reprise() const noexcept { return reprise_; }

private:
    int duree_;
    std::optional<int> arret_;
    std::optional<int> reprise_;
};

}  // namespace feux
