#include "scytale.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>

using namespace std;

//шифрование методом Скитала
bool EncryptFileScytale(const string& inputFile, const string& outputFile) {
    try {
        int diameter;
        
        cout << "Введите диаметр цилиндра (ключ): ";
        if (!(cin >> diameter)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw runtime_error("Ошибка ввода диаметра цилиндра");
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (diameter <= 0) {
            throw runtime_error("Диаметр должен быть положительным числом");
        }

        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }

        input.seekg(0, ios::end);
        size_t file_size = input.tellg();
        input.seekg(0, ios::beg);

        vector<unsigned char> file_data(file_size);
        input.read(reinterpret_cast<char*>(file_data.data()), file_size);
        input.close();

        if (file_data.empty()) {
            throw runtime_error("Входной файл пуст");
        }

        size_t data_size = file_data.size();
        
        //вычисляем количество строк
        int rows = data_size / diameter;
        if (data_size % diameter != 0) {
            rows++;
        }
        
        //создаем матрицу
        vector<vector<unsigned char>> matrix(rows, vector<unsigned char>(diameter, 0));
        
        //заполняем матрицу данными по строкам
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < diameter; j++) {
                size_t index = i * diameter + j;
                if (index < data_size) {
                    matrix[i][j] = file_data[index];
                }
            }
        }

        //шифруем
        vector<unsigned char> encrypted_data;
        for (int col = 0; col < diameter; col++) {
            for (int row = 0; row < rows; row++) {
                encrypted_data.push_back(matrix[row][col]);
            }
        }

        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл: " + outputFile);
        }

        output.write(reinterpret_cast<const char*>(encrypted_data.data()), encrypted_data.size());
        output.close();

        cout << "Файл успешно зашифрован методом Скитала: " << outputFile << endl;

        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в EncryptFileScytale: " << e.what() << endl;
        return false;
    }
}

//дещифрование методом Скитала
bool DecryptFileScytale(const string& inputFile, const string& outputFile) {
    try {
        int diameter;
        
        cout << "Введите диаметр цилиндра (ключ): ";
        if (!(cin >> diameter)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw runtime_error("Ошибка ввода диаметра цилиндра");
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (diameter <= 0) {
            throw runtime_error("Диаметр должен быть положительным числом");
        }

        ifstream input(inputFile, ios::binary);
        if (!input) {
            throw runtime_error("Не удалось открыть входной файл: " + inputFile);
        }

        input.seekg(0, ios::end);
        size_t file_size = input.tellg();
        input.seekg(0, ios::beg);

        vector<unsigned char> encrypted_data(file_size);
        input.read(reinterpret_cast<char*>(encrypted_data.data()), file_size);
        input.close();

        if (encrypted_data.empty()) {
            throw runtime_error("Входной файл пуст");
        }

        size_t data_size = encrypted_data.size();
        
        //вычисляем количество строк
        int rows = data_size / diameter;
        if (data_size % diameter != 0) {
            rows++;
        }
        
        //создаем матрицу
        vector<vector<unsigned char>> matrix(rows, vector<unsigned char>(diameter, 0));
        
        //заполняем матрицу по столбцам из зашифрованных данных
        size_t encrypted_index = 0;
        for (int col = 0; col < diameter; col++) {
            for (int row = 0; row < rows; row++) {
                if (encrypted_index < data_size) {
                    matrix[row][col] = encrypted_data[encrypted_index++];
                }
            }
        }

        //читаем данные по строкам для восстановления исходного текста
        vector<unsigned char> decrypted_data;
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < diameter; col++) {
                decrypted_data.push_back(matrix[row][col]);
            }
        }

        //убираем дополнение нулями
        size_t original_size = data_size;
        if (decrypted_data.size() > original_size) {
            decrypted_data.resize(original_size);
        }

        ofstream output(outputFile, ios::binary);
        if (!output) {
            throw runtime_error("Не удалось создать выходной файл: " + outputFile);
        }

        output.write(reinterpret_cast<const char*>(decrypted_data.data()), decrypted_data.size());
        output.close();

        cout << "Файл успешно дешифрован методом Скитала: " << outputFile << endl;
        
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка в DecryptFileScytale: " << e.what() << endl;
        return false;
    }
}
