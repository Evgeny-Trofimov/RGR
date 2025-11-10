#include "el_gamal.h"
#include "utils.h"
#include "constants.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

//шифровка файла по Эль-Гамаля
bool encryptFileElGamal(const string& inputFile, const string& outputFile) {
    try {
        uint64_t alice_private, session_key;
        
        cout << "Введите приватный ключ Алисы (Ca): ";
        if (!(cin >> alice_private)) {
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Алисы");
        }  
        
        cout << "Введите сессионный ключ k: ";
        if (!(cin >> session_key)) {
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода сессионного ключа");
        }  
        
        cin.ignore(1000, '\n');
        
        //проверка ключей
        if (alice_private == 0 || alice_private >= EL_GAMAL_PRIME - 1) {
            throw runtime_error("Приватный ключ Алисы должен быть в диапазоне [1, p-2]");
        }
        if (session_key == 0 || session_key >= EL_GAMAL_PRIME - 1) {
            throw runtime_error("Сессионный ключ должен быть в диапазоне [1, p-2]");
        }
        
        //вычисляем открытый ключ Da = g^Ca mod p
        uint64_t alice_public = ModExp(EL_GAMAL_GENERATOR, alice_private, EL_GAMAL_PRIME);
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл");
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
        
        cout << "Шифрование Эль-Гамаль" << endl;
        cout << "Приватный ключ Алисы (Ca): " << alice_private << endl;
        cout << "Публичный ключ Алисы (Da): " << alice_public << endl;
        cout << "Сессионный ключ k: " << session_key << endl;
        cout << "Размер файла: " << buffer.size() << " байт" << endl;
        
        //вычисляем r = g^k mod p
        uint64_t r = ModExp(EL_GAMAL_GENERATOR, session_key, EL_GAMAL_PRIME);
        
        //вычисляем Da^k mod p
        uint64_t da_power_k = ModExp(alice_public, session_key, EL_GAMAL_PRIME);
        
        cout << "Компонент r: " << r << endl;
        cout << "Da^k mod p: " << da_power_k << endl;
        
        //шифровка каждого байта e = m * Da^k mod p
        vector<uint64_t> encrypted_data;
        for (size_t i = 0; i < buffer.size(); ++i) {
            uint64_t byte_value = static_cast<uint64_t>(buffer[i]);
            //проверяем, что байт не превышает p
            if (byte_value >= EL_GAMAL_PRIME) {
                throw runtime_error("Значение байта превышает простое число p");
            }
            uint64_t encrypted_value = (byte_value * da_power_k) % EL_GAMAL_PRIME;
            encrypted_data.push_back(encrypted_value);
        }
        
        //записываем зашифрованный файл
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл");
        }
        
        //записываем r
        output.write(reinterpret_cast<const char*>(&r), sizeof(r));
        
        //записываем размер данных
        uint64_t data_size = encrypted_data.size();
        output.write(reinterpret_cast<const char*>(&data_size), sizeof(data_size));
        
        //записываем все компоненты e
        output.write(reinterpret_cast<const char*>(encrypted_data.data()), 
                    encrypted_data.size() * sizeof(uint64_t));
        
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

//дешифровка файла по Эль-Гамаля
bool decryptFileElGamal(const string& inputFile, const string& outputFile) {
    try {
        uint64_t alice_private;

        cout << "Введите приватный ключ Алисы (Ca): ";
        if (!(cin >> alice_private)) {
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Алисы");
        }  
        
        cin.ignore(1000, '\n');
        
        //проверка ключа
        if (alice_private == 0 || alice_private >= EL_GAMAL_PRIME - 1) {
            throw runtime_error("Приватный ключ Алисы должен быть в диапазоне [1, p-2]");
        }
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл");
        }
        
        //проверяем размер файла
        input.seekg(0, ios::end);
        size_t file_size = input.tellg();
        input.seekg(0, ios::beg);
        
        if (file_size < sizeof(uint64_t) * 2) {
            throw runtime_error("Файл слишком мал для содержания зашифрованных данных");
        }
        
        //читаем r
        uint64_t r = 0;
        input.read(reinterpret_cast<char*>(&r), sizeof(r));
        
        //читаем размер данных
        uint64_t data_size = 0;
        input.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));
        
        //проверяем, что размер данных разумен
        if (data_size > (file_size - sizeof(r) - sizeof(data_size)) / sizeof(uint64_t)) {
            throw runtime_error("Некорректный размер данных в файле");
        }
        
        //читаем все компоненты e
        vector<uint64_t> encrypted_data(data_size);
        input.read(reinterpret_cast<char*>(encrypted_data.data()), data_size * sizeof(uint64_t));
        
        input.close();
        
        cout << "Дешифрование Эль-Гамаль" << endl;
        cout << "Приватный ключ Алисы (Ca): " << alice_private << endl;
        cout << "Полученный компонент r: " << r << endl;
        cout << "Количество зашифрованных блоков: " << encrypted_data.size() << endl;
        
        //вычисляем r^(p-1-Ca) mod p
        uint64_t exponent = EL_GAMAL_PRIME - 1 - alice_private;
        uint64_t r_power = ModExp(r, exponent, EL_GAMAL_PRIME);
        
        cout << "Вычисленный показатель (p-1-Ca): " << exponent << endl;
        cout << "r^(p-1-Ca) mod p: " << r_power << endl;
        
        //дешифруем каждый блок: m = e * r^(p-1-Ca) mod p
        vector<unsigned char> decrypted_data;
        for (size_t i = 0; i < encrypted_data.size(); ++i) {
            uint64_t encrypted_value = encrypted_data[i];
            
            //проверяем, что зашифрованное значение меньше p
            if (encrypted_value >= EL_GAMAL_PRIME) {
                throw runtime_error("Зашифрованное значение превышает простое число p");
            }
            
            uint64_t decrypted_value = (encrypted_value * r_power) % EL_GAMAL_PRIME;
            
            //поверяем, что результат в пределах байта
            if (decrypted_value > 255) {
                throw runtime_error("Ошибка дешифрования: значение превышает 255");
            }
            decrypted_data.push_back(static_cast<unsigned char>(decrypted_value));
        }
        
        //записываем в файл
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл");
        }
        
        output.write(reinterpret_cast<const char*>(decrypted_data.data()), decrypted_data.size());
        
        output.close();
        
        cout << "Файл успешно дешифрован: " << outputFile << endl;
        cout << "Размер дешифрованного файла: " << decrypted_data.size() << " байт" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в decryptFileElGamal: " << e.what() << endl;
        return false;
    }
}
