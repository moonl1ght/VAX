#ifndef population_hpp
#define population_hpp

#include <iostream>
#include <unordered_set>
#include <map>
#include <uuid_v4.h>
#include <ranges>
#include "Logger.hpp"
#include "generation.hpp"
#include "specimen.hpp"

class Game {
public:
    uint32_t year = 0;
    const uint32_t maxYears = 1000;
    std::unordered_set<Specimen*, SpecimenHash> specimens;
    std::map<uint32_t, Generation*> generations;
    uint32_t totalSpecimensBorn = 0;
    uint32_t totalSpecimensDead = 0;
    uint32_t malesAlive = 0;
    uint32_t femalesAlive = 0;
    uint32_t pairsAlive = 0;

    Game() {
        year = 0;
        generations[year] = new Generation(year);
        uint32_t initialPopulation = 2;
        for (uint32_t i = 0; i < initialPopulation; i++) {
            Specimen* specimen = new Specimen(generations[year]->value);
            specimen->isMale = i % 2 == 0;
            incrementGenderAlive(specimen);
            specimens.insert(specimen);
            generations[year]->addSpecimen(specimen);
        }
        pairsAlive = 0;
    }

    ~Game() {}

    void incrementGenderAlive(Specimen* specimen) {
        if (specimen->isMale) {
            malesAlive++;
        }
        else {
            femalesAlive++;
        }
    }

    void decrementGenderAlive(Specimen* specimen) {
        if (specimen->isMale) {
            malesAlive--;
        }
        else {
            femalesAlive--;
        }
    }

    void run() {
        while (year < maxYears) {
            year++;
            Generation* generation = new Generation(year);
            std::unordered_set<Specimen*, SpecimenHash> newBornSpecimens;
            for (auto it = specimens.begin(); it != specimens.end(); ++it) {
                Specimen& specimen = const_cast<Specimen&>(**it);
                specimen.live();
            }
            for (auto it = specimens.begin(); it != specimens.end(); ++it) {
                Specimen& specimen = const_cast<Specimen&>(**it);
                if (specimen.isReadyToMeetPartner() && !specimen.hasPartner()) {
                    findPartner(&specimen);
                }
                if (specimen.hasPartner()) {
                    Specimen* child = specimen.tryToReproduce(generation->value);
                    if (child) {
                        newBornSpecimens.insert(child);
                        generation->addSpecimen(child);
                        totalSpecimensBorn++;
                        incrementGenderAlive(child);
                    }
                }
            }
            if (newBornSpecimens.size() > 0) {
                generations[year] = generation;
            }
            else {
                delete generation;
            }
            specimens.insert(newBornSpecimens.begin(), newBornSpecimens.end());
            cleanUp();
            Logger::getInstance().log("Year: ", year);
            Logger::getInstance().log(
                "Specimens: ", specimens.size(),
                "; Males: ", malesAlive,
                "; Females: ", femalesAlive,
                "; Pairs: ", pairsAlive
            );
            Logger::getInstance().log(
                "New born specimens: ", newBornSpecimens.size()
            );
            Logger::getInstance().log(
                "Total specimens born: ",
                totalSpecimensBorn,
                "; Total specimens dead: ",
                totalSpecimensDead
            );
            Logger::getInstance().log(
                "Generations: ", generations.size(),
                "; max generation: ", generations.rbegin()->first,
                "; min generation: ", generations.begin()->first
            );
            std::cout << std::endl << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void findPartner(Specimen* specimen) {
        // std::cout << "specimens.size(): " << specimens.size() << std::endl;
        // std::cout << "specimen->isMale: " << specimen->isMale << std::endl;
        // std::cout << "specimen->age: " << specimen->age << std::endl;
        // // std::cout << "specimen->ageToReproduce: " << specimen->ageToReproduce << std::endl;
        // // std::cout << "specimen->hadPartner: " << specimen->hadPartner << std::endl;
        // // std::cout << "specimen->isAlive: " << specimen->isAlive << std::endl;
        // std::cout << "specimen->isReadyToMeetPartner(): " << specimen->isReadyToMeetPartner() << std::endl;
        // std::cout << "specimen->hasPartner(): " << specimen->hasPartner() << std::endl;
        // // std::cout << "specimen->probabilityToReproduce: " << specimen->probabilityToReproduce << std::endl;
        for (auto it = specimens.begin(); it != specimens.end(); ++it) {
            Specimen& partner = const_cast<Specimen&>(**it);
            if (specimen->id == partner.id || specimen->isMale == partner.isMale) {
                continue;
            }
            if (partner.isReadyToMeetPartner() && !partner.hasPartner()) {
                specimen->partner = &partner;
                partner.partner = specimen;
                // std::cout << "Found partner: " << std::endl;
                pairsAlive++;
                break;
            }
        }
    }

    void cleanUp() {
        for (auto it = specimens.begin(); it != specimens.end();) {
            Specimen* specimen = *it;
            if (!specimen->isAlive) {
                totalSpecimensDead++;
                decrementGenderAlive(specimen);
                if (specimen->hasPartner()) {
                    pairsAlive--;
                }
                it = specimens.erase(it);
                auto generation = generations[specimen->generation];
                generation->removeSpecimen(specimen);
                if (generation->isEmpty()) {
                    generations.erase(specimen->generation);
                    delete generation;
                }
                delete specimen;
            }
            else {
                ++it;
            }
        }
    }
};

#endif