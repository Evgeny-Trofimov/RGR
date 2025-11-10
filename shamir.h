#ifndef SHAMIR_H
#define SHAMIR_H

#include <cstdint>
#include <string>

using namespace std;

extern "C" {
    bool EncryptFile(const string& inputFile, const string& outputFile);
    bool DecryptFile(const string& inputFile, const string& outputFile);
}

#endif
