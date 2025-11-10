#ifndef EL_GAMAL_H
#define EL_GAMAL_H

#include <cstdint>
#include <string>

using namespace std;

extern "C" {
    bool encryptFileElGamal(const string& inputFile, const string& outputFile);
    bool decryptFileElGamal(const string& inputFile, const string& outputFile);
}

#endif
