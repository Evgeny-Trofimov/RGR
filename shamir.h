#ifndef SHAMIR_H
#define SHAMIR_H

#include <cstdint>
#include <string>

using namespace std;

extern "C" {
    bool EncryptFileShamir(const string& inputFile, const string& outputFile);
    bool DecryptFileShamir(const string& inputFile, const string& outputFile);
}

#endif
