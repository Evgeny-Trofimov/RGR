#include "polybius.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <set>

using namespace std;

//шифрование методом Полибия
bool EncryptFilePolybius(const string& inputFile, const string& outputFile) {
    try {
        cout << "Шифрование методом Полибия с ключом" << endl;

        string key;
        cout << "Введите ключ для шифрования Полибия: ";
        getline(cin, key);
        
        if (key.empty()) {
            throw runtime_error("Ключ не может быть пустым");
        }

        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }

        vector<unsigned char> file_data(
            (istreambuf_iterator<char>(input)),
            istreambuf_iterator<char>()
        );
        input.close();

        if (file_data.empty()) {
            throw runtime_error("Входной файл пуст");
        }

        cout << "Размер исходных данных: " << file_data.size() << " байт" << endl;

        //создание таблицы 16x16 на основе ключа
        vector<vector<unsigned char>> table(16, vector<unsigned char>(16));
        set<unsigned char> used;
        int row = 0, col = 0;

        //заполнение ключом
        for (char c : key) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (used.find(uc) == used.end()) {
                if (row < 16) {
                    table[row][col] = uc;
                    used.insert(uc);
                    col++;
                    if (col == 16) {
                        col = 0;
                        row++;
                    }
                }
            }
        }

        //заполнение оставшимися байтами (0–255)
        for (unsigned int i = 0; i < 256; ++i) {
            unsigned char c = static_cast<unsigned char>(i);
            if (used.find(c) == used.end()) {
                if (row < 16) {
                    table[row][col] = c;
                    used.insert(c);
                    col++;
                    if (col == 16) {
                        col = 0;
                        row++;
                    }
                }
            }
        }

        //проверка, что таблица заполнена
        if (row >= 16 && col > 0) {
            cerr << "Ошибка: таблица Полибия переполнена\n";
            throw runtime_error("Не удалось создать таблицу Полибия");
        }

        //шифруем данные
        string encrypted_text;
        for (unsigned char byte : file_data) {
            //поиск позиции байта в таблице
            bool found = false;
            for (int i = 0; i < 16 && !found; ++i) {
                for (int j = 0; j < 16 && !found; ++j) {
                    if (table[i][j] == byte) {
                        encrypted_text += static_cast<char>(i);
                        encrypted_text += static_cast<char>(j);
                        found = true;
                    }
                }
            }
            if (!found) {
                throw runtime_error("Байт не найден в таблице Полибия");
            }
        }

        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл: " + outputFile);
        }

        output.write(encrypted_text.data(), encrypted_text.size());
        output.close();

        cout << "Файл успешно зашифрован методом Полибия: " << outputFile << endl;
        cout << "Использованный ключ: " << key << endl;

        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в EncryptFilePolybius: " << e.what() << endl;
        return false;
    }
}

//дешифрование методом Полибия
bool DecryptFilePolybius(const string& inputFile, const string& outputFile) {
    try {
        cout << "Дешифрование методом Полибия с ключом" << endl;

        string key;
        cout << "Введите ключ для дешифрования Полибия: ";
        getline(cin, key);
        
        if (key.empty()) {
            throw runtime_error("Ключ не может быть пустым");
        }

        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }

        vector<unsigned char> encrypted_data(
            (istreambuf_iterator<char>(input)),
            istreambuf_iterator<char>()
        );
        input.close();

        if (encrypted_data.empty()) {
            throw runtime_error("Входной файл пуст");
        }

        cout << "Размер зашифрованных данных: " << encrypted_data.size() << " байт" << endl;

        //проверяем четность размера данных
        if (encrypted_data.size() % 2 != 0) {
            throw runtime_error("Некорректная длина шифротекста");
        }

        //создание таблицы 16x16 на основе ключа
        vector<vector<unsigned char>> table(16, vector<unsigned char>(16));
        set<unsigned char> used;
        int row = 0, col = 0;

        //заполнение ключом
        for (char c : key) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (used.find(uc) == used.end()) {
                if (row < 16) {
                    table[row][col] = uc;
                    used.insert(uc);
                    col++;
                    if (col == 16) {
                        col = 0;
                        row++;
                    }
                }
            }
        }

        //заполнение оставшимися байтами (0–255)
        for (unsigned int i = 0; i < 256; ++i) {
            unsigned char c = static_cast<unsigned char>(i);
            if (used.find(c) == used.end()) {
                if (row < 16) {
                    table[row][col] = c;
                    used.insert(c);
                    col++;
                    if (col == 16) {
                        col = 0;
                        row++;
                    }
                }
            }
        }

        //проверка, что таблица заполнена
        if (row >= 16 && col > 0) {
            cerr << "Ошибка: таблица Полибия переполнена\n";
            throw runtime_error("Не удалось создать таблицу Полибия");
        }

        //дешифруем данные
        vector<unsigned char> decrypted_data;
        for (size_t i = 0; i < encrypted_data.size(); i += 2) {
            int row_index = static_cast<unsigned char>(encrypted_data[i]);
            int col_index = static_cast<unsigned char>(encrypted_data[i + 1]);
            
            if (row_index >= 16 || col_index >= 16) {
                throw runtime_error("Некорректные координаты в шифротексте");
            }
            
            decrypted_data.push_back(table[row_index][col_index]);
        }

        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл: " + outputFile);
        }

        output.write(reinterpret_cast<const char*>(decrypted_data.data()), decrypted_data.size());
        output.close();

        cout << "Файл успешно дешифрован методом Полибия: " << outputFile << endl;
        cout << "Использованный ключ: " << key << endl;

        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в DecryptFilePolybius: " << e.what() << endl;
        return false;
    }
}
