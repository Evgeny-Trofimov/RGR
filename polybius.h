#ifndef POLYBIUS_H
#define POLYBIUS_H

#include <string>

using namespace std;

extern "C" {
    bool EncryptFilePolybius(const string& inputFile, const string& outputFile);
    bool DecryptFilePolybius(const string& inputFile, const string& outputFile);
}

#endif
