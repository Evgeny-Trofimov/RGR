#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <string>

using namespace std;

extern "C" {
    uint64_t ModInverse(uint64_t a, uint64_t m);
    uint64_t ModExp(uint64_t base, uint64_t exponent, uint64_t mod);
    uint64_t gcd(uint64_t a, uint64_t b);
    uint64_t GenerateRandom(uint64_t min, uint64_t max);
    bool GenerateDiffieHellmanKeys();
    bool GenerateShamirKeys();
    bool GenerateElGamalKeys();
}

#endif
