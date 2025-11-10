#include "utils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <random>
#include <ctime>

extern uint64_t DH_PRIME;
extern uint64_t SHAMIR_PRIME;
extern const uint64_t EL_GAMAL_PRIME;

uint64_t ModInverse(uint64_t a, uint64_t m) {
    try {
        int64_t m0 = m, t, q;
        int64_t x0 = 0, x1 = 1;
        
        if (m == 1) return 0;
        
        while (a > 1) {
            q = a / m;
            t = m;
            m = a % m;
            a = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        
        if (x1 < 0) x1 += m0;
        return static_cast<uint64_t>(x1);
    } catch (const exception& e) {
        throw runtime_error("Ошибка в computeModInverse");
    }
}

uint64_t ModExp(uint64_t base, uint64_t exponent, uint64_t mod) {
    try {
        if (mod == 1) return 0;
        
        uint64_t result = 1;
        base = base % mod;
        
        while (exponent > 0) {
            if (exponent & 1) {
                result = (result * base) % mod;
            }
            exponent = exponent >> 1;
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

bool GenerateDiffieHellmanKeys() {
    try {
        ifstream checkFile("diff_key.txt");
        if (checkFile.good()) {
            cout << "Файл diff_key.txt уже существует." << endl;
            
            uint64_t existing_alice = 0, existing_bob = 0;
            checkFile >> existing_alice >> existing_bob;
            checkFile.close();
            
            cout << "Существующие ключи:" << endl;
            cout << "ALICE_PRIVATE = " << existing_alice << endl;
            cout << "BOB_PRIVATE = " << existing_bob << endl;
            
            cout << "Хотите перезаписать ключи? (y/n): ";
            char choice;
            cin >> choice;
            if (choice != 'y' && choice != 'Y') {
                cout << "Генерация ключей отменена." << endl;
                return true;
            }
        }
        
        uint64_t alice_private = GenerateRandom(2, DH_PRIME - 2);
        uint64_t bob_private = GenerateRandom(2, DH_PRIME - 2);
        
        ofstream keyFile("diff_key.txt");
        if (!keyFile) {
            throw runtime_error("Не удалось создать файл diff_key.txt");
        }
        
        keyFile << alice_private << endl;
        keyFile << bob_private << endl;
        keyFile.close();
        
        cout << "Ключи Диффи-Хеллмана сгенерированы и сохранены в diff_key.txt:" << endl;
        cout << "ALICE_PRIVATE = " << alice_private << endl;
        cout << "BOB_PRIVATE = " << bob_private << endl;
        
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка при генерации ключей Диффи-Хеллмана: " << e.what() << endl;
        return false;
    }
}

bool GenerateShamirKeys() {
    try {
        ifstream checkFile("shamir_key.txt");
        if (checkFile.good()) {
            cout << "Файл shamir_key.txt уже существует." << endl;
            
            uint64_t existing_ca = 0, existing_cb = 0;
            checkFile >> existing_ca >> existing_cb;
            checkFile.close();
            
            cout << "Существующие ключи:" << endl;
            cout << "CA_PRIVATE = " << existing_ca << endl;
            cout << "CB_PRIVATE = " << existing_cb << endl;
            
            cout << "Хотите перезаписать ключи? (y/n): ";
            char choice;
            cin >> choice;
            if (choice != 'y' && choice != 'Y') {
                cout << "Генерация ключей отменена." << endl;
                return true;
            }
        }
        
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

bool GenerateElGamalKeys() {
    try {
        ifstream checkFile("el_key.txt");
        if (checkFile.good()) {
            cout << "Файл el_key.txt уже существует." << endl;
            
            uint64_t existing_alice = 0, existing_session = 0;
            checkFile >> existing_alice >> existing_session;
            checkFile.close();
            
            cout << "Существующие ключи:" << endl;
            cout << "ALICE_PRIVATE (Ca) = " << existing_alice << endl;
            cout << "SESSION_KEY (k) = " << existing_session << endl;
            
            cout << "Хотите перезаписать ключи? (y/n): ";
            char choice;
            cin >> choice;
            if (choice != 'y' && choice != 'Y') {
                cout << "Генерация ключей отменена." << endl;
                return true;
            }
        }
        
        uint64_t alice_private = GenerateRandom(2, EL_GAMAL_PRIME - 2);
        uint64_t session_key = GenerateRandom(2, EL_GAMAL_PRIME - 2);
        
        ofstream keyFile("el_key.txt");
        if (!keyFile) {
            throw runtime_error("Не удалось создать файл el_key.txt");
        }
        
        keyFile << alice_private << endl;
        keyFile << session_key << endl;
        keyFile.close();
        
        cout << "Ключи Эль-Гамаля сгенерированы и сохранены в el_key.txt:" << endl;
        cout << "ALICE_PRIVATE (Ca) = " << alice_private << endl;
        cout << "SESSION_KEY (k) = " << session_key << endl;
        
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка при генерации ключей Эль-Гамаля: " << e.what() << endl;
        return false;
    }
}
