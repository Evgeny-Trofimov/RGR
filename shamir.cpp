#include "shamir.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>

//шифровка по шифру Шамира
bool EncryptFileShamir(const string& inputFile, const string& outputFile) {
    try {
        uint64_t ca_private, cb_private;
        
        cout << "Введите приватный ключ CA: ";
        if (!(cin >> ca_private)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw runtime_error("Ошибка ввода приватного ключа CA");
        }  
        
        cout << "Введите приватный ключ CB: ";
        if (!(cin >> cb_private)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw runtime_error("Ошибка ввода приватного ключа CB");
        } 

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }
        
        //читаем файл
        vector<unsigned char> buffer(
            (istreambuf_iterator<char>(input)),
            istreambuf_iterator<char>()
        );
        input.close();
        
        if (buffer.empty()) {
            throw runtime_error("Входной файл пуст");
        }
        
        //1-ая стадия
        vector<uint64_t> intermedia(buffer.size());
        for (size_t i = 0; i < buffer.size(); ++i) {
            uint64_t byte_value = static_cast<uint64_t>(buffer[i]);
            intermedia[i] = ModExp(byte_value, ca_private, SHAMIR_PRIME);
        }
        
        //2-ая стадия
        vector<uint64_t> encrypted_data(buffer.size());
        for (size_t i = 0; i < buffer.size(); ++i) {
            encrypted_data[i] = ModExp(intermedia[i], cb_private, SHAMIR_PRIME);
        }
        
        //записываем данные
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл: " + outputFile);
        }
        
        //записываем размер
        uint64_t original_size = buffer.size();
        output.write(reinterpret_cast<const char*>(&original_size), sizeof(original_size));
        
        for (uint64_t value : encrypted_data) {
            output.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
        
        output.close();
        
        cout << "Файл успешно зашифрован: " << outputFile << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в EncryptFile: " << e.what() << endl;
        return false;
    }
}

//дешифрока по шифру Шамира
bool DecryptFileShamir(const string& inputFile, const string& outputFile) {
    try {
        uint64_t ca_private, cb_private;
        
        cout << "Введите приватный ключ CA: ";
        if (!(cin >> ca_private)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw runtime_error("Ошибка ввода приватного ключа CA");
        }  
        
        cout << "Введите приватный ключ CB: ";
        if (!(cin >> cb_private)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw runtime_error("Ошибка ввода приватного ключа CB");
        } 
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        uint64_t da_private = ModInverse(ca_private, SHAMIR_PRIME - 1);
        uint64_t db_private = ModInverse(cb_private, SHAMIR_PRIME - 1);
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }
        
        //читаем размер
        uint64_t original_size;
        if (!input.read(reinterpret_cast<char*>(&original_size), sizeof(original_size))) {
            throw runtime_error("Не удалось прочитать размер исходных данных");
        }
        
        //читаем данные
        vector<uint64_t> encrypted_data(original_size);
        for (size_t i = 0; i < original_size; ++i) {
            if (!input.read(reinterpret_cast<char*>(&encrypted_data[i]), sizeof(uint64_t))) {
                throw runtime_error("Не удалось прочитать зашифрованные данные");
            }
        }
        input.close();
        
        cout << "Дешифрование по протоколу Шамира" << endl;
        cout << "Простое число p: " << SHAMIR_PRIME << endl;
        
        //3-я стадия
        vector<uint64_t> intermedia(original_size);
        for (size_t i = 0; i < original_size; ++i) {
            intermedia[i] = ModExp(encrypted_data[i], da_private, SHAMIR_PRIME);
        }
        
        //4-ая стадия
        vector<unsigned char> decrypted_data(original_size);
        for (size_t i = 0; i < original_size; ++i) {
            uint64_t decrypted_value = ModExp(intermedia[i], db_private, SHAMIR_PRIME);
            //проверяем, что дешифрованный байт входит в диапазон от 0 до 255
            if (decrypted_value > 255) {
                throw runtime_error("Ошибка дешифрования: значение превышает 255");
            }
            decrypted_data[i] = static_cast<unsigned char>(decrypted_value);
        }
        
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл: " + outputFile);
        }
        
        output.write(reinterpret_cast<const char*>(decrypted_data.data()), decrypted_data.size());
        output.close();
        
        cout << "Файл успешно дешифрован: " << outputFile << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в DecryptFile: " << e.what() << endl;
        return false;
    }
}
