#ifndef __RANDOM_H__
#define __RANDOM_H__

#pragma once

#include <iostream>
#include <random>
#include <chrono>
#include <unordered_set>

inline int getRandomNumber(int min, int max) {
    static std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

class UniqueRandomNumberGenerator {
private:
    std::unordered_set<int> generatedNumbers;
    std::random_device rd;
    std::mt19937 gen;

public:
    UniqueRandomNumberGenerator() : gen(rd()) {}

    int generateUniqueNumber(int min, int max) {
        std::uniform_int_distribution<int> distribution(min, max);
        int number;

        do {
            number = distribution(gen);
        } while (generatedNumbers.find(number) != generatedNumbers.end());

        generatedNumbers.insert(number);
        return number;
    }

    void reset() {
        generatedNumbers.clear();
    }
};

#endif __RANDOM_H__