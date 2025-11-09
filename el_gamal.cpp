#include "el_gamal.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

// Определения параметров Эль-Гамаля
const uint64_t EL_GAMAL_PRIME = 4294967311;
const uint64_t EL_GAMAL_GENERATOR = 3;

// Шифрование файла по Эль-Гамалю
bool encryptFileElGamal(const string& inputFile, const string& outputFile) {
    try {
        // Запрос ключей
        uint64_t alice_private, session_key;
        
        cout << "Введите приватный ключ Алисы (Ca): ";
        if (!(cin >> alice_private)) {  // ← ТОЛЬКО ОДИН ввод
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Алисы");
        }  
        
        cout << "Введите сессионный ключ k: ";
        if (!(cin >> session_key)) {    // ← ТОЛЬКО ОДИН ввод
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода сессионного ключа");
        }  
        
        // Очищаем буфер после ввода чисел
        cin.ignore(1000, '\n');
        
        // Проверка валидности ключей
        if (alice_private == 0 || alice_private >= EL_GAMAL_PRIME - 1) {
            throw runtime_error("Приватный ключ Алисы должен быть в диапазоне [1, p-2]");
        }
        if (session_key == 0 || session_key >= EL_GAMAL_PRIME - 1) {
            throw runtime_error("Сессионный ключ должен быть в диапазоне [1, p-2]");
        }
        
        // Вычисляем открытый ключ Алисы Da = g^Ca mod p
        uint64_t alice_public = ModExp(EL_GAMAL_GENERATOR, alice_private, EL_GAMAL_PRIME);
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл");
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
        
        cout << "Шифрование Эль-Гамаль" << endl;
        cout << "Приватный ключ Алисы (Ca): " << alice_private << endl;
        cout << "Публичный ключ Алисы (Da): " << alice_public << endl;
        cout << "Сессионный ключ k: " << session_key << endl;
        cout << "Размер файла: " << buffer.size() << " байт" << endl;
        
        // Вычисляем r = g^k mod p
        uint64_t r = ModExp(EL_GAMAL_GENERATOR, session_key, EL_GAMAL_PRIME);
        
        // Вычисляем Da^k mod p
        uint64_t da_power_k = ModExp(alice_public, session_key, EL_GAMAL_PRIME);
        
        cout << "Компонент r: " << r << endl;
        cout << "Da^k mod p: " << da_power_k << endl;
        
        // Шифруем каждый байт: e = m * Da^k mod p
        vector<uint64_t> encrypted_data;
        for (size_t i = 0; i < buffer.size(); ++i) {
            uint64_t byte_value = static_cast<uint64_t>(buffer[i]);
            // Проверяем, что байт не превышает простое число
            if (byte_value >= EL_GAMAL_PRIME) {
                throw runtime_error("Значение байта превышает простое число p");
            }
            uint64_t encrypted_value = (byte_value * da_power_k) % EL_GAMAL_PRIME;
            encrypted_data.push_back(encrypted_value);
        }
        
        // Записываем зашифрованный файл
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл");
        }
        
        // Записываем компонент r побайтово
        for (size_t i = 0; i < sizeof(r); ++i) {
            output.put(static_cast<char>((r >> (i * 8)) & 0xFF));
        }
        
        // Записываем размер данных побайтово
        uint64_t data_size = encrypted_data.size();
        for (size_t i = 0; i < sizeof(data_size); ++i) {
            output.put(static_cast<char>((data_size >> (i * 8)) & 0xFF));
        }
        
        // Записываем все компоненты e побайтово
        for (uint64_t e : encrypted_data) {
            for (size_t i = 0; i < sizeof(e); ++i) {
                output.put(static_cast<char>((e >> (i * 8)) & 0xFF));
            }
        }
        
        if (output.fail()) {
            throw runtime_error("Ошибка записи в выходной файл");
        }
        
        output.close();
        
        cout << "Файл успешно зашифрован: " << outputFile << endl;
        cout << "Размер зашифрованного файла: " 
             << (sizeof(r) + sizeof(data_size) + encrypted_data.size() * sizeof(uint64_t)) 
             << " байт" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в encryptFileElGamal: " << e.what() << endl;
        return false;
    }
}

// Дешифрование файла по Эль-Гамалю
bool decryptFileElGamal(const string& inputFile, const string& outputFile) {
    try {
        // Запрос ключа
        uint64_t alice_private;

        cout << "Введите приватный ключ Алисы (Ca): ";
        if (!(cin >> alice_private)) {  // ← ТОЛЬКО ОДИН ввод
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Алисы");
        }  
        
        // Очищаем буфер после ввода чисел
        cin.ignore(1000, '\n');
        
        // Проверка валидности ключа
        if (alice_private == 0 || alice_private >= EL_GAMAL_PRIME - 1) {
            throw runtime_error("Приватный ключ Алисы должен быть в диапазоне [1, p-2]");
        }
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл");
        }
        
        // Проверяем размер файла
        input.seekg(0, ios::end);
        size_t file_size = input.tellg();
        input.seekg(0, ios::beg);
        
        if (file_size < sizeof(uint64_t) * 2) {
            throw runtime_error("Файл слишком мал для содержания зашифрованных данных");
        }
        
        // Читаем компонент r побайтово
        uint64_t r = 0;
        for (size_t i = 0; i < sizeof(r); ++i) {
            if (input.eof()) {
                throw runtime_error("Неожиданный конец файла при чтении r");
            }
            unsigned char byte = static_cast<unsigned char>(input.get());
            r |= static_cast<uint64_t>(byte) << (i * 8);
        }
        
        // Читаем размер данных побайтово
        uint64_t data_size = 0;
        for (size_t i = 0; i < sizeof(data_size); ++i) {
            if (input.eof()) {
                throw runtime_error("Неожиданный конец файла при чтении data_size");
            }
            unsigned char byte = static_cast<unsigned char>(input.get());
            data_size |= static_cast<uint64_t>(byte) << (i * 8);
        }
        
        // Проверяем, что размер данных разумен
        if (data_size > (file_size - sizeof(r) - sizeof(data_size)) / sizeof(uint64_t)) {
            throw runtime_error("Некорректный размер данных в файле");
        }
        
        // Читаем все компоненты e побайтово
        vector<uint64_t> encrypted_data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            uint64_t e = 0;
            for (size_t i = 0; i < sizeof(e); ++i) {
                if (input.eof()) {
                    throw runtime_error("Неожиданный конец файла при чтении e");
                }
                unsigned char byte = static_cast<unsigned char>(input.get());
                e |= static_cast<uint64_t>(byte) << (i * 8);
            }
            encrypted_data[j] = e;
        }
        input.close();
        
        cout << "Дешифрование Эль-Гамаль" << endl;
        cout << "Приватный ключ Алисы (Ca): " << alice_private << endl;
        cout << "Полученный компонент r: " << r << endl;
        cout << "Количество зашифрованных блоков: " << encrypted_data.size() << endl;
        
        // Вычисляем r^(p-1-Ca) mod p
        uint64_t exponent = EL_GAMAL_PRIME - 1 - alice_private;
        uint64_t r_power = ModExp(r, exponent, EL_GAMAL_PRIME);
        
        cout << "Вычисленный показатель (p-1-Ca): " << exponent << endl;
        cout << "r^(p-1-Ca) mod p: " << r_power << endl;
        
        // Дешифруем каждый блок: m = e * r^(p-1-Ca) mod p
        vector<unsigned char> decrypted_data;
        for (size_t i = 0; i < encrypted_data.size(); ++i) {
            uint64_t encrypted_value = encrypted_data[i];
            
            // Проверяем, что зашифрованное значение меньше p
            if (encrypted_value >= EL_GAMAL_PRIME) {
                throw runtime_error("Зашифрованное значение превышает простое число p");
            }
            
            uint64_t decrypted_value = (encrypted_value * r_power) % EL_GAMAL_PRIME;
            
            // Проверяем, что результат в пределах байта
            if (decrypted_value > 255) {
                throw runtime_error("Ошибка дешифрования: значение превышает 255");
            }
            decrypted_data.push_back(static_cast<unsigned char>(decrypted_value));
        }
        
        // Записываем дешифрованный файл
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл");
        }
        
        for (size_t i = 0; i < decrypted_data.size(); ++i) {
            output.put(static_cast<char>(decrypted_data[i]));
        }
        
        if (output.fail()) {
            throw runtime_error("Ошибка записи в выходной файл");
        }
        
        output.close();
        
        cout << "Файл успешно дешифрован: " << outputFile << endl;
        cout << "Размер дешифрованного файла: " << decrypted_data.size() << " байт" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в decryptFileElGamal: " << e.what() << endl;
        return false;
    }
}
