#ifndef __RANDOM_H__
#define __RANDOM_H__

#pragma once

#include <iostream>
#include <random>
#include <chrono>

inline int getRandomNumber(int min, int max) {
    static std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

#endif __RANDOM_H__