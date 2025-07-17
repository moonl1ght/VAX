#pragma once

#include <iostream>
#include <cstdlib>

namespace PopulationUtils {
    double e = 2.71828;

    bool checkProbability(double probability) {
        double random = (double)std::rand() / RAND_MAX;
        return random < probability;
    };
}