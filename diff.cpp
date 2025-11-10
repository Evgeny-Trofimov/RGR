#include "diff.h"
#include "utils.h"
#include "constants.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

//шифровка файла с использованием общего секрета Диффи-Хеллмана
bool encryptFileDiffieHellman(const string& inputFile, const string& outputFile) {
    try {
        //запрос ключей
        uint64_t alice_private, bob_private;
        
        cout << "Введите приватный ключ Алисы: ";
        if (!(cin >> alice_private)) {
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Алисы");
        }      
        
        cout << "Введите приватный ключ Боба: ";
        if (!(cin >> bob_private)) {
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Боба");
        }  
        
        //очищаем буфер
        cin.ignore(1000, '\n');

        //вычисление открытых ключей
        uint64_t alice_public = ModExp(DH_GENERATOR, alice_private, DH_PRIME);
        uint64_t bob_public = ModExp(DH_GENERATOR, bob_private, DH_PRIME);
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл");
        }
        
        //считаем весь файл
        vector<unsigned char> buffer(
            (istreambuf_iterator<char>(input)),
            istreambuf_iterator<char>()
        );
        input.close();
        
        //вычисление общего секрета
        uint64_t shared_secret = ModExp(bob_public, alice_private, DH_PRIME);
        
        cout << "Шифрование Диффи-Хеллман" << endl;
        cout << "Приватный ключ Алисы: " << alice_private << endl;
        cout << "Приватный ключ Боба: " << bob_private << endl;
        cout << "Публичный ключ Алисы: " << alice_public << endl;
        cout << "Публичный ключ Боба: " << bob_public << endl;
        cout << "Вычисленный общий секрет: " << shared_secret << endl;
        cout << "Размер файла: " << buffer.size() << " байт" << endl;
        
        //шифровка
        for (size_t i = 0; i < buffer.size(); ++i) {
            uint64_t byte_value = static_cast<uint64_t>(buffer[i]);
            uint64_t encrypted_value = (byte_value + shared_secret) % 256;
            buffer[i] = static_cast<unsigned char>(encrypted_value);
        }
        
        //записываем зашифрованный текст в файл
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл");
        }
        
        output.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        
        output.close();
        
        cout << "Файл успешно зашифрован: " << outputFile << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в encryptFileDiffieHellman: " << e.what() << endl;
        return false;
    }
}

//дешифровка файла с использованием общего секрета Диффи-Хеллмана
bool decryptFileDiffieHellman(const string& inputFile, const string& outputFile) {
    try {
        //запрос ключей
        uint64_t alice_private, bob_private;
        
        cout << "Введите приватный ключ Алисы: ";
        if (!(cin >> alice_private)) {
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Алисы");
        }      
        
        cout << "Введите приватный ключ Боба: ";
        if (!(cin >> bob_private)) {
            cin.clear();
            cin.ignore(1000, '\n');
            throw runtime_error("Ошибка ввода приватного ключа Боба");
        }  
        
        //очистка буфера
        cin.ignore(1000, '\n');
        
        //вычисление публичных ключей
        uint64_t alice_public = ModExp(DH_GENERATOR, alice_private, DH_PRIME);
        uint64_t bob_public = ModExp(DH_GENERATOR, bob_private, DH_PRIME);
        
        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл");
        }
        
        //считываем зашифрованный файл
        vector<unsigned char> buffer(
            (istreambuf_iterator<char>(input)),
            istreambuf_iterator<char>()
        );
        input.close();
        
        //вычисление общего секрета
        uint64_t shared_secret = ModExp(bob_public, alice_private, DH_PRIME);
        
        cout << "Дешифрование Диффи-Хеллман" << endl;
        cout << "Приватный ключ Алисы: " << alice_private << endl;
        cout << "Приватный ключ Боба: " << bob_private << endl;
        cout << "Публичный ключ Алисы: " << alice_public << endl;
        cout << "Публичный ключ Боба: " << bob_public << endl;
        cout << "Вычисленный общий секрет: " << shared_secret << endl;
        cout << "Размер файла: " << buffer.size() << " байт" << endl;
        
        //дешифровка
        for (size_t i = 0; i < buffer.size(); ++i) {
            uint64_t encrypted_value = static_cast<uint64_t>(buffer[i]);
            uint64_t decrypted_value = (encrypted_value - shared_secret) % 256;
            buffer[i] = static_cast<unsigned char>(decrypted_value);
        }
        
        //записываем расшифрованный файл
        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл");
        }
        
        output.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        
        output.close();
        
        cout << "Файл успешно дешифрован: " << outputFile << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в decryptFileDiffieHellman: " << e.what() << endl;
        return false;
    }
}
