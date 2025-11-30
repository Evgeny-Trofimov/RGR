#ifndef SCYTALE_H
#define SCYTALE_H

#include <string>

using namespace std;

extern "C" {
    bool EncryptFileScytale(const string& inputFile, const string& outputFile);
    bool DecryptFileScytale(const string& inputFile, const string& outputFile);
}

#endif
