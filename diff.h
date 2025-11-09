#ifndef DIFFIE_HELLMAN_H
#define DIFFIE_HELLMAN_H

#include <cstdint>
#include <string>

using namespace std;

extern "C" {
    bool encryptFileDiffieHellman(const string& inputFile, const string& outputFile);
    bool decryptFileDiffieHellman(const string& inputFile, const string& outputFile);
}

extern uint64_t DH_PRIME;
extern uint64_t DH_GENERATOR;

#endif
