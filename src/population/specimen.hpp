#pragma once

#include <iostream>
#include <unordered_set>
#include "uuid_v4.h"
#include "logger.h"
#include "population_utils.h"

class Specimen {
public:
    std::string id;
    uint32_t age = 0;
    const uint32_t ageToReproduce = 15;
    const uint32_t maxAge = 100;
    const uint32_t maxChildren = 10;
    double probabilityToDie = 0.0;
    double probabilityToReproduce = 0.0;
    bool isAlive = true;
    Specimen* partner = nullptr;
    std::pair<Specimen*, Specimen*> parents;
    std::vector<Specimen*> children;
    uint32_t generation;
    bool isAlreadyReproducedWithPartner = false;
    bool hasChildrenPreviousYear = false;
    bool hadPartner = false;
    bool isMale = false;

    Specimen(uint32_t generation) : generation(generation) {
        UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
        id = uuidGenerator.getUUID().bytes();
        isMale = PopulationUtils::checkProbability(0.5f);
    }

    ~Specimen() {
        if (partner) {
            if (partner->partner == this) {
                partner->hadPartner = true;
                partner->partner = nullptr;
            }
            else {
                Logger::getInstance().error("Specimen has wrong partner");
            }
        }
        for (auto it = children.begin(); it != children.end(); ++it) {
            if ((*it)->parents.first == this) {
                (*it)->parents.first = nullptr;
            }
            else if ((*it)->parents.second == this) {
                (*it)->parents.second = nullptr;
            }
        }
        if (parents.first) {
            parents.first->removeChild(this);
        }
        if (parents.second) {
            parents.second->removeChild(this);
        }
    }

    Specimen* tryToReproduce(uint32_t generation) {
        // std::cout << "tryToReproduce probabilityToReproduce: " << probabilityToReproduce << " checkProbability: " << PopulationUtils::checkProbability(probabilityToReproduce) << std::endl;
        // std::cout << "tryToReproduce isReadyToReproduceWithPartner: " << isReadyToReproduceWithPartner() << std::endl;
        if (isReadyToReproduceWithPartner()) {
            Specimen* child = new Specimen(generation);
            child->parents = std::make_pair(this, partner);
            children.push_back(child);
            partner->children.push_back(child);
            hasChildrenPreviousYear = true;
            partner->hasChildrenPreviousYear = true;
            isAlreadyReproducedWithPartner = true;
            partner->isAlreadyReproducedWithPartner = true;
            return child;
        }
        if (!isAlreadyReproducedWithPartner && !partner->isAlreadyReproducedWithPartner) {
            hasChildrenPreviousYear = false;
            partner->hasChildrenPreviousYear = false;
        }
        return nullptr;
    }

    void removeChild(Specimen* child) {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    bool isReadyToReproduceWithPartner() {
        return age >= ageToReproduce
            && children.size() <= maxChildren
            && hasPartner()
            && !isAlreadyReproducedWithPartner
            && PopulationUtils::checkProbability(probabilityToReproduce);
    }

    bool isReadyToMeetPartner() {
        return age >= ageToReproduce && !hadPartner;
    }

    bool hasPartner() {
        return partner != nullptr;
    }

    bool hasChildren() {
        return children.size() > 0;
    }

    void live() {
        age++;
        probabilityToDie = std::pow((double)age / maxAge, PopulationUtils::e);

        double childrenCoefficient = children.size() > 0
            ? 1.0f / std::pow(static_cast<double>(children.size()), 0.5f)
            : 1.0f;
        probabilityToReproduce = (1 - std::pow((double)age / maxAge, PopulationUtils::e)) * childrenCoefficient;
        isAlreadyReproducedWithPartner = false;
        if (hasChildrenPreviousYear) {
            probabilityToReproduce *= 0.5f;
        }
        // if (age >= ageToReproduce) {
        //     std::cout << "childrenCoefficient: " << childrenCoefficient << std::endl;
        //     std::cout << "probabilityToReproduce: " << probabilityToReproduce << std::endl;
        //     std::cout << "children.size(): " << children.size() << std::endl;
        //     std::cout << "hasChildrenPreviousYear: " << hasChildrenPreviousYear << std::endl;
        // }

        if (PopulationUtils::checkProbability(probabilityToDie)) {
            isAlive = false;
        }
    }

    bool operator==(const Specimen* other) const {
        return id == other->id;
    }
};

struct SpecimenHash {
    size_t operator()(const Specimen* specimen) const {
        return std::hash<std::string>()(specimen->id);
    }
};