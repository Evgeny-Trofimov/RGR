#include "utils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <random>

using namespace std;

uint64_t SHAMIR_PRIME = 4294967311;

//теорема Евклида
uint64_t ModInverse(uint64_t a, uint64_t m) {
    try{
        int64_t m0 = m;
        int64_t y = 0, x = 1;
        
        if (m == 1) return 0;
        
        while (a > 1) {
            int64_t q = a / m;
            int64_t t = m;
            
            m = a % m;
            a = t;
            t = y;
            
            y = x - q * y;
            x = t;
        }
        
        if (x < 0) x += m0;
            return static_cast<uint64_t>(x);
        
    }catch (const exception& e) {
        throw runtime_error("Ошибка в ModInverse");
    }
}

//быстрое возведение в степень по модулю
uint64_t ModExp(uint64_t base, uint64_t exponent, uint64_t mod) {
    try {
        if (mod == 1) return 0;
        
        uint64_t result = 1;
        base = base % mod;
        
        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result = (result * base) % mod;
            }
            exponent = exponent / 2;
            base = (base * base) % mod;
        }
        return result;
    } catch (const exception& e) {
        throw runtime_error("Ошибка в ModExp: " + string(e.what()));
    }
}

uint64_t GenerateRandom(uint64_t min, uint64_t max) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis(min, max);
    return dis(gen);
}

uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

bool GenerateScytaleKey() {
    try {
        int diameter = static_cast<int>(GenerateRandom(2, 100));
        
        ofstream keyFile("scytale_key.txt");
        if (!keyFile) {
            throw runtime_error("Не удалось создать файл scytale_key.txt");
        }

        keyFile << diameter << endl;
        keyFile.close();
        
        cout << "Сгенерирован ключ для Скитала и сохранен в scytale_key.txt." << endl;
        cout << "Диаметр цилиндра: " << diameter << endl;

        return true;
    } catch (const exception& e) {
        cerr << "Ошибка при генерации ключа Скитала: " << e.what() << endl;
        return false;
    }
}

bool GenerateShamirKeys() {
    try {
        uint64_t ca_private, cb_private;
        
        do {
            ca_private = GenerateRandom(2, SHAMIR_PRIME - 2);
        } while (gcd(ca_private, SHAMIR_PRIME - 1) != 1);
        
        do {
            cb_private = GenerateRandom(2, SHAMIR_PRIME - 2);
        } while (gcd(cb_private, SHAMIR_PRIME - 1) != 1);
        
        ofstream keyFile("shamir_key.txt");
        if (!keyFile) {
            throw runtime_error("Не удалось создать файл shamir_key.txt");
        }
        
        keyFile << ca_private << endl;
        keyFile << cb_private << endl;
        keyFile.close();
        
        cout << "Ключи протокола Шамира сгенерированы и сохранены в shamir_key.txt:" << endl;
        cout << "CA_PRIVATE = " << ca_private << endl;
        cout << "CB_PRIVATE = " << cb_private << endl;
        
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка при генерации ключей Шамира: " << e.what() << endl;
        return false;
    }
}

bool GeneratePolybiusKey() {
    try {
        string key;
        for (int i = 0; i < 6; ++i) {
            key += to_string(GenerateRandom(0, 9)); 
        }

        ofstream keyFile("polybius_key.txt");
        if (!keyFile) {
            throw runtime_error("Не удалось создать файл polybius_key.txt");
        }

        keyFile << key;
        keyFile.close();

        cout << "Ключ для Полибия сгенерирован и сохранен в polybius_key.txt:" << endl;
        cout << "Ключ: " << key << endl;

        return true;
    } catch (const exception& e) {
        cerr << "Ошибка при генерации ключа Полибия: " << e.what() << endl;
        return false;
    }
}
