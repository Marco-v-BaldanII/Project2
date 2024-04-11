#include <iostream>
#include <random>
#include <chrono>

int getRandomNumber(int min, int max) {
    static std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}