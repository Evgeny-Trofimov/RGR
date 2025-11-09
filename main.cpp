#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include "diff.h"
#include "shamir.h"
#include "el_gamal.h"
#include "utils.h"

using namespace std;

//проверка пароля
bool checkPassword() {
    string password;
    cout << "Введите пароль для доступа к программе: ";
    cin >> password;
    
    //очистка буфера после ввода
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    return password == "123";
}

void showMainMenu() {
    cout << "\n=== Криптографическая система ===" << endl;
    cout << "1. Шифрование файла" << endl;
    cout << "2. Дешифрование файла" << endl;
    cout << "3. Генерация ключей" << endl;
    cout << "4. Выход" << endl;
    cout << "Выберите действие: ";
}

void showMethodMenu() {
    cout << "\n=== Выбор метода шифрования ===" << endl;
    cout << "1. Диффи-Хеллман" << endl;
    cout << "2. Протокол Шамира" << endl;
    cout << "3. Эль-Гамаль" << endl;
    cout << "Выберите метод: ";
}

void showKeyGenMenu() {
    cout << "\n=== Генерация ключей ===" << endl;
    cout << "1. Диффи-Хеллман" << endl;
    cout << "2. Протокол Шамира" << endl;
    cout << "3. Эль-Гамаль" << endl;
    cout << "Выберите метод для генерации ключей: ";
}

//выбор пользователем способа ввода данных
string getInputDataForEncryption() {
    int choice;
    cout << "Хотите ввести с клавиатуры или загрузить из файла? (1- с клавиатуры, 2- из файла): ";
    cin >> choice;
    
    //очистка буфера после ввода
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (choice == 1) {
        string data;
        cout << "Введите данные для обработки: ";
        getline(cin, data);
        
        //создание временного файла для шифрования строки
        string tempFile = "temp.txt";
        ofstream out(tempFile);
        if (!out) {
            throw runtime_error("Не удалось создать временный файл");
        }
        out << data;
        out.close();
        
        return tempFile;
    } else if (choice == 2) {
        string filePath;
        cout << "Введите путь до файла: ";
        getline(cin, filePath);
        
        //проверка существования файла
        ifstream check(filePath);
        if (!check) {
            throw runtime_error("Файл не существует: " + filePath);
        }
        check.close();
        
        return filePath;
    } else {
        throw runtime_error("Неверный выбор способа ввода");
    }
}

//вывод результата на экран
void displayResult(const string& filePath) {
    int choice;
    cout << "Вывести результат на экран? (1 - да, 2 - нет): ";
    cin >> choice;
    
    //очистка буфера после ввода
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (choice == 1) {
        ifstream file(filePath, ios::binary);
        if (!file) {
            cout << "Не удалось открыть файл" << endl;
            return;
        }
        
        vector<unsigned char> buffer(
            (istreambuf_iterator<char>(file)),
            istreambuf_iterator<char>()
        );
        file.close();
        
        cout << "=== Содержимое файла ===" << endl;
        for (size_t i = 0; i < buffer.size(); ++i) {
            cout << buffer[i];
        }
        cout << endl << "=== Конец содержимого ===" << endl;
    }
}

int main() {
    try {
        //проверка корректности пароля
        if (!checkPassword()) {
            cout << "Неверный пароль! Доступ запрещен." << endl;
            return 0;
        }
        
        cout << "Пароль принят. Добро пожаловать!" << endl;
        
        int mainChoice, methodChoice;
        string inputFile, outputFile;
        
        while (true) {
            showMainMenu();
            cin >> mainChoice;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Неверный ввод! Попробуйте снова." << endl;
                continue;
            }
            
            // Очищаем буфер после ввода mainChoice
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (mainChoice == 4) {
                cout << "Выход из программы." << endl;
                break;
            }
            
            if (mainChoice == 3) {
                // Генерация ключей
                showKeyGenMenu();
                cin >> methodChoice;
                
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Неверный ввод! Попробуйте снова." << endl;
                    continue;
                }
                
                // Очищаем буфер после ввода methodChoice
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                bool success = false;
                switch (methodChoice) {
                    case 1: // Диффи-Хеллман
                        success = GenerateDiffieHellmanKeys();
                        break;
                    case 2: // Протокол Шамира
                        success = GenerateShamirKeys();
                        break;
                    case 3: // Эль-Гамаль
                        success = GenerateElGamalKeys();
                        break;
                    default:
                        cout << "Неверный выбор метода" << endl;
                        continue;
                }
                
                if (success) {
                    cout << "Ключи успешно сгенерированы!" << endl;
                } else {
                    cout << "Ошибка при генерации ключей" << endl;
                }
                continue;
            }
            
            if (mainChoice != 1 && mainChoice != 2) {
                cout << "Неверный выбор! Попробуйте снова." << endl;
                continue;
            }
            
            showMethodMenu();
            cin >> methodChoice;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Неверный ввод! Попробуйте снова." << endl;
                continue;
            }
            
            // Очищаем буфер после ввода methodChoice
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            // Получаем входные данные в зависимости от операции
            if (mainChoice == 1) {
                // Шифрование: можно ввести с клавиатуры или из файла
                inputFile = getInputDataForEncryption();
            } else {
                // Дешифрование: только из файла
                cout << "Введите путь к зашифрованному файлу: ";
                getline(cin, inputFile);
            }

            cout << "Введите путь к выходному файлу: ";
            getline(cin, outputFile);
            
            bool success = false;
            
            switch (methodChoice) {
                case 1: // Диффи-Хеллман
                    if (mainChoice == 1) {
                        success = encryptFileDiffieHellman(inputFile, outputFile);
                    } else {
                        success = decryptFileDiffieHellman(inputFile, outputFile);
                    }
                    break;
                
                case 2: // Протокол Шамира
                    if (mainChoice == 1) {
                        success = EncryptFile(inputFile, outputFile);
                    } else {
                        success = DecryptFile(inputFile, outputFile);
                    }
                    break;
                    
                case 3: // Эль-Гамаль
                    if (mainChoice == 1) {
                        success = encryptFileElGamal(inputFile, outputFile);
                    } else {
                        success = decryptFileElGamal(inputFile, outputFile);
                    }
                    break;
                    
                default:
                    cout << "Неверный выбор метода" << endl;
                    continue;
            }
            
            if (success) {
                cout << "Операция завершена успешно" << endl;
                // Предлагаем вывести результат на экран
                displayResult(outputFile);
            } else {
                cout << "Ошибка при выполнении операции" << endl;
            }
            
            // Удаляем временный файл, если он был создан (только для шифрования)
            if (mainChoice == 1 && inputFile == "temp_input.txt") {
                remove("temp_input.txt");
            }
        }
        
        return 0;
    } catch (const exception& e) {
        cerr << "Критическая ошибка в main: " << e.what() << endl;
        return 1;
    }
}
