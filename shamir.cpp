#include "shamir.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

// Определение параметра для протокола Шамира
uint64_t SHAMIR_PRIME = 4294967311;

// Шифрование по протоколу Шамира
bool EncryptFile(const string& inputFile, const string& outputFile) {
    try {      
        // Запрос ключей
        uint64_t ca_private, cb_private;
        
        cout << "Введите приватный ключ CA: ";
        if (!(cin >> ca_private)) {    // ← ТОЛЬКО ОДИН ввод
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа CA");
        }  
        
        cout << "Введите приватный ключ CB: ";
        if (!(cin >> cb_private)) {    // ← ТОЛЬКО ОДИН ввод
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа CB");
        } 
        
        // Очищаем буфер после ввода чисел
        cin.ignore(1000, '\n');
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }
        
        // Читаем весь файл
        vector<unsigned char> buffer(
            (istreambuf_iterator<char>(input)),
            istreambuf_iterator<char>()
        );
        input.close();
        
        if (buffer.empty()) {
            throw runtime_error("Входной файл пуст");
        }
        
        uint64_t da_private = ModInverse(ca_private, SHAMIR_PRIME - 1);
        uint64_t db_private = ModInverse(cb_private, SHAMIR_PRIME - 1);

        cout << "Инициализация ключей Шамира:" << endl;
        cout << "CA: " << ca_private << " -> DA: " << da_private << endl;
        cout << "CB: " << cb_private << " -> DB: " << db_private << endl;
        cout << "Проверка: CA * DA mod (p-1) = " << (ca_private * da_private) % (SHAMIR_PRIME - 1) << " (должно быть 1)" << endl;
        cout << "Проверка: CB * DB mod (p-1) = " << (cb_private * db_private) % (SHAMIR_PRIME - 1) << " (должно быть 1)" << endl;
        
        // Первое шифрование: X1 = M^CA mod p
        vector<uint64_t> intermediate1(buffer.size());
        for (size_t i = 0; i < buffer.size(); ++i) {
            uint64_t byte_value = static_cast<uint64_t>(buffer[i]);
            intermediate1[i] = ModExp(byte_value, ca_private, SHAMIR_PRIME);
        }
        
        // Второе шифрование: X2 = X1^CB mod p
        vector<uint64_t> encrypted_data(buffer.size());
        for (size_t i = 0; i < buffer.size(); ++i) {
            encrypted_data[i] = ModExp(intermediate1[i], cb_private, SHAMIR_PRIME);
        }
        
        // Записываем зашифрованные данные (как uint64_t)
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл: " + outputFile);
        }
        
        // Записываем размер исходных данных
        uint64_t original_size = buffer.size();
        output.write(reinterpret_cast<const char*>(&original_size), sizeof(original_size));
        
        // Записываем зашифрованные данные
        for (uint64_t value : encrypted_data) {
            output.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
        
        output.close();
        
        cout << "Файл успешно зашифрован: " << outputFile << endl;
        cout << "Размер зашифрованного файла: " << (sizeof(original_size) + encrypted_data.size() * sizeof(uint64_t)) << " байт" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в EncryptFile: " << e.what() << endl;
        return false;
    }
}

// Дешифрование по протоколу Шамира
bool DecryptFile(const string& inputFile, const string& outputFile) {
    try {
        // Запрос ключей
        uint64_t ca_private, cb_private;
        
        cout << "Введите приватный ключ CA: ";
        if (!(cin >> ca_private)) {    // ← ТОЛЬКО ОДИН ввод
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа CA");
        }  
        
        cout << "Введите приватный ключ CB: ";
        if (!(cin >> cb_private)) {    // ← ТОЛЬКО ОДИН ввод
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа CB");
        } 
        
        // Очищаем буфер после ввода чисел
        cin.ignore(1000, '\n');
        
        uint64_t da_private = ModInverse(ca_private, SHAMIR_PRIME - 1);
        uint64_t db_private = ModInverse(cb_private, SHAMIR_PRIME - 1);
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }
        
        // Читаем размер исходных данных
        uint64_t original_size;
        if (!input.read(reinterpret_cast<char*>(&original_size), sizeof(original_size))) {
            throw runtime_error("Не удалось прочитать размер исходных данных");
        }
        
        // Читаем зашифрованные данные
        vector<uint64_t> encrypted_data(original_size);
        for (size_t i = 0; i < original_size; ++i) {
            if (!input.read(reinterpret_cast<char*>(&encrypted_data[i]), sizeof(uint64_t))) {
                throw runtime_error("Не удалось прочитать зашифрованные данные");
            }
        }
        input.close();
        
        cout << "Дешифрование по протоколу Шамира" << endl;
        cout << "Ключ CA: " << ca_private << " -> DA: " << da_private << endl;
        cout << "Ключ CB: " << cb_private << " -> DB: " << db_private << endl;
        cout << "Простое число p: " << SHAMIR_PRIME << endl;
        cout << "Размер данных: " << original_size << " байт" << endl;
        
        // Первое дешифрование: X3 = X2^DA mod p
        vector<uint64_t> intermediate1(original_size);
        for (size_t i = 0; i < original_size; ++i) {
            intermediate1[i] = ModExp(encrypted_data[i], da_private, SHAMIR_PRIME);
        }
        
        // Второе дешифрование: M = X3^DB mod p
        vector<unsigned char> decrypted_data(original_size);
        for (size_t i = 0; i < original_size; ++i) {
            uint64_t decrypted_value = ModExp(intermediate1[i], db_private, SHAMIR_PRIME);
            // Проверяем, что результат в пределах байта
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
