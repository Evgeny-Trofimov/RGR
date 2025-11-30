#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <string>

using namespace std;

// Константы
extern uint64_t SHAMIR_PRIME;

// Функции
extern "C" {
    uint64_t ModExp(uint64_t base, uint64_t exponent, uint64_t mod);
    uint64_t ModInverse(uint64_t a, uint64_t m);
    uint64_t GenerateRandom(uint64_t min, uint64_t max);
    bool GenerateShamirKeys();
    bool GenerateScytaleKey();
    bool GeneratePolybiusKey();
}

#endif
