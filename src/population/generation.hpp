#pragma once

#include <iostream>
#include <unordered_set>
#include "logger.h"
#include "specimen.hpp"

class Generation {
public:
    uint32_t value;
    std::unordered_set<Specimen*, SpecimenHash> specimens;

    Generation(uint32_t value) : value(value) {}
    ~Generation() {}

    void addSpecimen(Specimen* specimen) {
        specimens.insert(specimen);
    }

    void removeSpecimen(Specimen* specimen) {
        specimens.erase(specimen);
    }

    bool isEmpty() {
        return specimens.size() == 0;
    }

    bool operator==(const Generation& other) const {
        return value == other.value;
    }
};

struct GenerationHash {
    size_t operator()(const Generation& generation) const {
        return std::hash<uint32_t>()(generation.value);
    }
};