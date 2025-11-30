#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include <dlfcn.h>
#include <map>
#include "utils.h"

using namespace std;

enum CryptoMethod {
    METHOD_NONE = 0,
    METHOD_SCYTALE = 1,
    METHOD_SHAMIR = 2,
    METHOD_POLYBIUS = 3
};

enum MenuAction {
    ACTION_ENCRYPT = 1,
    ACTION_DECRYPT = 2,
    ACTION_GENERATE_KEYS = 3,
    ACTION_EXIT = 4
};

//объявления типов функций для динамической загрузки
typedef bool (*EncryptFunc)(const string&, const string&);
typedef bool (*DecryptFunc)(const string&, const string&);

//структура для хранения указателей на функции библиотеки
struct LibraryFunctions {
    void* handle;
    EncryptFunc encrypt;
    DecryptFunc decrypt;
    string name;
    
    LibraryFunctions() : handle(nullptr), encrypt(nullptr), decrypt(nullptr), name("") {}
};

//функция для работы с библиотеками
bool LoadLibrary(const string& libName, LibraryFunctions& lib, int mode = RTLD_LAZY) {
    lib.handle = dlopen(libName.c_str(), mode);
    if (!lib.handle) {
        return false;
    }
    
    //загружаем функции шифрования/дешифрования
    lib.encrypt = (EncryptFunc)dlsym(lib.handle, "EncryptFileScytale");
    if (!lib.encrypt) {
        lib.encrypt = (EncryptFunc)dlsym(lib.handle, "EncryptFileShamir");
    }
    if (!lib.encrypt) {
        lib.encrypt = (EncryptFunc)dlsym(lib.handle, "EncryptFilePolybius");
    }
    
    lib.decrypt = (DecryptFunc)dlsym(lib.handle, "DecryptFileScytale");
    if (!lib.decrypt) {
        lib.decrypt = (DecryptFunc)dlsym(lib.handle, "DecryptFileShamir");
    }
    if (!lib.decrypt) {
        lib.decrypt = (DecryptFunc)dlsym(lib.handle, "DecryptFilePolybius");
    }
    
    return (lib.encrypt != nullptr && lib.decrypt != nullptr);
}

void UnloadLibrary(LibraryFunctions& lib) {
    if (lib.handle) {
        dlclose(lib.handle);
        lib.handle = nullptr;
        lib.encrypt = nullptr;
        lib.decrypt = nullptr;
    }
}

//проверка пароля
bool CheckPassword() {
    string password;
    cout << "Введите пароль для доступа к программе: ";
    cin >> password;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    return password == "123";
}

void ShowMainMenu() {
    cout << "\n=== Криптографическая система ===" << endl;
    cout << ACTION_ENCRYPT << ". Шифрование" << endl;
    cout << ACTION_DECRYPT << ". Дешифрование" << endl;
    cout << ACTION_GENERATE_KEYS << ". Генерация ключей" << endl;
    cout << ACTION_EXIT << ". Выход" << endl;
    cout << "Выберите действие: ";
}

void ShowMethodMenu(const map<CryptoMethod, LibraryFunctions>& availableMethods) {
    cout << "\n=== Выбор метода ===" << endl;
    for (const auto& method : availableMethods) {
        cout << method.first << ". " << method.second.name << endl;
    }
    cout << "Выберите метод: ";
}

void ShowKeyGenMenu() {
    cout << "\n=== Генерация ключей ===" << endl;
    cout << METHOD_SCYTALE << ". Скитала" << endl;
    cout << METHOD_SHAMIR << ". Шамир" << endl;
    cout << METHOD_POLYBIUS << ". Полибий" << endl;
    cout << "Выберите метод: ";
}

//выбор пользователем способа ввода данных
string GetInputData() {
    int choice;
    cout << "Хотите ввести с клавиатуры или загрузить из файла? (1 - с клавиатуры, 2 - из файла): ";
    cin >> choice;
    
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

//вывод результата
void DisplayResult(const string& filePath) {
    int choice;
    cout << "Вывести результат на экран? (1 - да, 2 - нет): ";
    cin >> choice;
    
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
        cout.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        cout << endl << "=== Конец содержимого ===" << endl;
    }
}

int main() {
    try {
        if (!CheckPassword()) {
            cout << "Неверный пароль! Доступ запрещен." << endl;
            return 0;
        }
        
        cout << "Пароль принят. Добро пожаловать!" << endl;
        
        //инициализация доступных библиотек
        map<CryptoMethod, LibraryFunctions> availableMethods;
        
        //попытка загрузить каждую библиотеку
        LibraryFunctions scytaleLib;
        if (LoadLibrary("./libscytale.so", scytaleLib)) {
            scytaleLib.name = "Скитала";
            availableMethods[METHOD_SCYTALE] = scytaleLib;
            cout << "Библиотека Скитала загружена" << endl;
        }
        
        LibraryFunctions shamirLib;
        if (LoadLibrary("./libshamir.so", shamirLib)) {
            shamirLib.name = "Шамир";
            availableMethods[METHOD_SHAMIR] = shamirLib;
            cout << "Библиотека Шамира загружена" << endl;
        }
        
        LibraryFunctions polybiusLib;
        if (LoadLibrary("./libpolybius.so", polybiusLib)) {
            polybiusLib.name = "Полибий";
            availableMethods[METHOD_POLYBIUS] = polybiusLib;
            cout << "Библиотека Полибия загружена" << endl;
        }
        
        if (availableMethods.empty()) {
            cerr << "Не загружено ни одной библиотеки шифрования!" << endl;
            return 1;
        }
        
        int mainChoice, methodChoice;
        string inputFile, outputFile;
        
        while (true) {
            ShowMainMenu();
            cin >> mainChoice;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Неверный ввод! Попробуйте снова." << endl;
                continue;
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            switch (static_cast<MenuAction>(mainChoice)) {
                case ACTION_ENCRYPT:
                case ACTION_DECRYPT: {
                    if (availableMethods.empty()) {
                        cout << "Нет доступных методов шифрования" << endl;
                        break;
                    }
                    
                    ShowMethodMenu(availableMethods);
                    cin >> methodChoice;
                    
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Неверный ввод! Попробуйте снова." << endl;
                        continue;
                    }
                    
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    if (static_cast<MenuAction>(mainChoice) == ACTION_ENCRYPT) {
                        inputFile = GetInputData();
                    } else {
                        cout << "Введите путь к зашифрованному файлу: ";
                        getline(cin, inputFile);
                    }

                    cout << "Введите путь к выходному файлу: ";
                    getline(cin, outputFile);
                    
                    bool success = false;
                    
                    switch (static_cast<CryptoMethod>(methodChoice)) {
                        case METHOD_SCYTALE:
                            if (availableMethods.count(METHOD_SCYTALE)) {
                                if (static_cast<MenuAction>(mainChoice) == ACTION_ENCRYPT) {
                                    success = availableMethods[METHOD_SCYTALE].encrypt(inputFile, outputFile);
                                } else {
                                    success = availableMethods[METHOD_SCYTALE].decrypt(inputFile, outputFile);
                                }
                            } else {
                                cout << "Метод Скитала недоступен" << endl;
                            }
                            break;
                            
                        case METHOD_SHAMIR:
                            if (availableMethods.count(METHOD_SHAMIR)) {
                                if (static_cast<MenuAction>(mainChoice) == ACTION_ENCRYPT) {
                                    success = availableMethods[METHOD_SHAMIR].encrypt(inputFile, outputFile);
                                } else {
                                    success = availableMethods[METHOD_SHAMIR].decrypt(inputFile, outputFile);
                                }
                            } else {
                                cout << "Метод Шамира недоступен" << endl;
                            }
                            break;
                            
                        case METHOD_POLYBIUS:
                            if (availableMethods.count(METHOD_POLYBIUS)) {
                                if (static_cast<MenuAction>(mainChoice) == ACTION_ENCRYPT) {
                                    success = availableMethods[METHOD_POLYBIUS].encrypt(inputFile, outputFile);
                                } else {
                                    success = availableMethods[METHOD_POLYBIUS].decrypt(inputFile, outputFile);
                                }
                            } else {
                                cout << "Метод Полибия недоступен" << endl;
                            }
                            break;
                            
                        default:
                            cout << "Неверный выбор метода" << endl;
                            continue;
                    }
                    
                    if (success) {
                        cout << "Операция завершена успешно" << endl;
                        DisplayResult(outputFile);
                    } else {
                        cout << "Ошибка при выполнении операции" << endl;
                    }
                    
                    //удаляем временный файл
                    if (static_cast<MenuAction>(mainChoice) == ACTION_ENCRYPT && inputFile == "temp.txt") {
                        remove("temp.txt");
                    }
                    break;
                }
                    
                case ACTION_GENERATE_KEYS: {
                    ShowKeyGenMenu();
                    cin >> methodChoice;
                    
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Неверный ввод! Попробуйте снова." << endl;
                        continue;
                    }
                    
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    bool success = false;
                    
                    switch (static_cast<CryptoMethod>(methodChoice)) {
                        case METHOD_SCYTALE:
                            success = GenerateScytaleKey();
                            break;
                            
                        case METHOD_SHAMIR:
                            success = GenerateShamirKeys();
                            break;
                            
                        case METHOD_POLYBIUS:
                            success = GeneratePolybiusKey();
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
                    break;
                }
                    
                case ACTION_EXIT:
                    cout << "Выход из программы." << endl;
                    //выгрузка библиотек
                    for (auto& method : availableMethods) {
                        UnloadLibrary(method.second);
                    }
                    return 0;
                    
                default:
                    cout << "Неверный выбор! Попробуйте снова." << endl;
                    continue;
            }
        }
        
        return 0;
    } catch (const exception& e) {
        cerr << "Критическая ошибка в main: " << e.what() << endl;
        return 1;
    }
}
