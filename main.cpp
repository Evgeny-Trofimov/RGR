#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include <dlfcn.h>

using namespace std;

//объявления типов функций для динамической загрузки
typedef bool (*EncryptFunc)(const string&, const string&);
typedef bool (*DecryptFunc)(const string&, const string&);
typedef bool (*KeyGenFunc)();

//структура для хранения указателей на функции библиотеки
struct LibraryFunctions {
    void* handle;
    EncryptFunc encrypt;
    DecryptFunc decrypt;
    KeyGenFunc generateDiffieHellmanKeys;
    KeyGenFunc generateShamirKeys;
    KeyGenFunc generateElGamalKeys;
    
    LibraryFunctions() : handle(nullptr), encrypt(nullptr), decrypt(nullptr), 
                        generateDiffieHellmanKeys(nullptr), generateShamirKeys(nullptr), 
                        generateElGamalKeys(nullptr) {}
};

//функции для работы с библиотеками
bool loadLibrary(const string& libName, LibraryFunctions& lib, int mode = RTLD_LAZY) {
    lib.handle = dlopen(libName.c_str(), mode);
    if (!lib.handle) {
        cerr << "Ошибка загрузки библиотеки " << libName << ": " << dlerror() << endl;
        return false;
    }
    
    //загружаем функции шифрования/дешифрования
    lib.encrypt = (EncryptFunc)dlsym(lib.handle, "encryptFileDiffieHellman");
    if (!lib.encrypt) {
        lib.encrypt = (EncryptFunc)dlsym(lib.handle, "EncryptFile");
    }
    if (!lib.encrypt) {
        lib.encrypt = (EncryptFunc)dlsym(lib.handle, "encryptFileElGamal");
    }
    
    lib.decrypt = (DecryptFunc)dlsym(lib.handle, "decryptFileDiffieHellman");
    if (!lib.decrypt) {
        lib.decrypt = (DecryptFunc)dlsym(lib.handle, "DecryptFile");
    }
    if (!lib.decrypt) {
        lib.decrypt = (DecryptFunc)dlsym(lib.handle, "decryptFileElGamal");
    }
    
    //загружаем функции генерации ключей
    lib.generateDiffieHellmanKeys = (KeyGenFunc)dlsym(lib.handle, "GenerateDiffieHellmanKeys");
    lib.generateShamirKeys = (KeyGenFunc)dlsym(lib.handle, "GenerateShamirKeys");
    lib.generateElGamalKeys = (KeyGenFunc)dlsym(lib.handle, "GenerateElGamalKeys");
    
    return true;
}

//отгрузка библиотек
void unloadLibrary(LibraryFunctions& lib) {
    if (lib.handle) {
        dlclose(lib.handle);
        lib.handle = nullptr;
        lib.encrypt = nullptr;
        lib.decrypt = nullptr;
        lib.generateDiffieHellmanKeys = nullptr;
        lib.generateShamirKeys = nullptr;
        lib.generateElGamalKeys = nullptr;
    }
}

//проверка существования so-файлов
bool checkLibrariesExist() {
    const vector<string> libraries = {
        "libconstants.so", "libutils.so", "libdiff.so", "libshamir.so", "libel_gamal.so"
    };
    
    bool allExist = true;
    cout << "Проверка наличия библиотек..." << endl;
    for (const auto& lib : libraries) {
        ifstream file(lib);
        if (!file) {
            cerr << "  ОШИБКА: Библиотека " << lib << " не найдена!" << endl;
            allExist = false;
        } else {
            cout << "  OK: " << lib << " найдена" << endl;
            file.close();
        }
    }
    
    if (allExist) {
        cout << "Все необходимые библиотеки найдены." << endl;
    } else {
        cerr << "Не все необходимые библиотеки найдены!" << endl;
    }
    
    return allExist;
}

//проверка пароля
bool checkPassword() {
    string password;
    cout << "Введите пароль для доступа к программе: ";
    cin >> password;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    return password == "123";
}

void showMainMenu() {
    cout << "\n=== Криптографическая система ===" << endl;
    cout << "1. Шифрование" << endl;
    cout << "2. Дешифрование" << endl;
    cout << "3. Генерация ключей" << endl;
    cout << "4. Выход" << endl;
    cout << "Выберите действие: ";
}

void showMethodMenu() {
    cout << "\n=== Выбор метода ===" << endl;
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
void displayResult(const string& filePath) {
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
        if (!checkPassword()) {
            cout << "Неверный пароль! Доступ запрещен." << endl;
            return 0;
        }
        
        cout << "Пароль принят. Добро пожаловать!" << endl;
        
        //проверка наличия дин. библиотек
        if (!checkLibrariesExist()) {
            cerr << "Не все необходимые библиотеки найдены. Завершение работы." << endl;
            return 1;
        }
        
        //подгрузка библиотек
        LibraryFunctions constantsLib, utilsLib, diffLib, shamirLib, elgamalLib;
        
        cout << "Загрузка библиотек..." << endl;
        
        //загрузка библиотек
        cout << "  Загрузка libconstants.so..." << endl;
        if (!loadLibrary("./libconstants.so", constantsLib, RTLD_LAZY | RTLD_GLOBAL)) {
            cerr << "Ошибка загрузки libconstants.so. Завершение работы." << endl;
            return 1;
        }
        cout << "  OK: libconstants.so загружена" << endl;
        
        cout << "  Загрузка libutils.so..." << endl;
        if (!loadLibrary("./libutils.so", utilsLib, RTLD_LAZY | RTLD_GLOBAL)) {
            cerr << "Ошибка загрузки libutils.so. Завершение работы." << endl;
            unloadLibrary(constantsLib);
            return 1;
        }
        cout << "  OK: libutils.so загружена" << endl;
        
        cout << "  Загрузка libdiff.so..." << endl;
        if (!loadLibrary("./libdiff.so", diffLib)) {
            cerr << "Ошибка загрузки libdiff.so. Завершение работы." << endl;
            unloadLibrary(utilsLib);
            unloadLibrary(constantsLib);
            return 1;
        }
        cout << "  OK: libdiff.so загружена" << endl;
        
        cout << "  Загрузка libshamir.so..." << endl;
        if (!loadLibrary("./libshamir.so", shamirLib)) {
            cerr << "Ошибка загрузки libshamir.so. Завершение работы." << endl;
            unloadLibrary(diffLib);
            unloadLibrary(utilsLib);
            unloadLibrary(constantsLib);
            return 1;
        }
        cout << "  OK: libshamir.so загружена" << endl;
        
        cout << "  Загрузка libel_gamal.so..." << endl;
        if (!loadLibrary("./libel_gamal.so", elgamalLib)) {
            cerr << "Ошибка загрузки libel_gamal.so. Завершение работы." << endl;
            unloadLibrary(shamirLib);
            unloadLibrary(diffLib);
            unloadLibrary(utilsLib);
            unloadLibrary(constantsLib);
            return 1;
        }
        cout << "  OK: libel_gamal.so загружена" << endl;
        
        cout << "Все библиотеки успешно загружены!" << endl;
        
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
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (mainChoice == 4) {
                cout << "Выход из программы." << endl;
                break;
            }
            
            if (mainChoice == 3) {
                //генерация ключей
                showKeyGenMenu();
                cin >> methodChoice;
                
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Неверный ввод! Попробуйте снова." << endl;
                    continue;
                }
                
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                bool success = false;
                switch (methodChoice) {
                    case 1: //Диффи-Хеллман
                        success = utilsLib.generateDiffieHellmanKeys ? utilsLib.generateDiffieHellmanKeys() : false;
                        break;
                    case 2: //Шамир
                        success = utilsLib.generateShamirKeys ? utilsLib.generateShamirKeys() : false;
                        break;
                    case 3: //Эль-Гамаль
                        success = utilsLib.generateElGamalKeys ? utilsLib.generateElGamalKeys() : false;
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
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (mainChoice == 1) {
                //шифровка: можно ввести с клавиатуры или из файла
                inputFile = getInputDataForEncryption();
            } else {
                //дешифровка: только из файла
                cout << "Введите путь к зашифрованному файлу: ";
                getline(cin, inputFile);
            }

            cout << "Введите путь к выходному файлу: ";
            getline(cin, outputFile);
            
            bool success = false;
            
            switch (methodChoice) {
                case 1: //Диффи-Хеллман
                    if (mainChoice == 1) {
                        success = diffLib.encrypt ? diffLib.encrypt(inputFile, outputFile) : false;
                    } else {
                        success = diffLib.decrypt ? diffLib.decrypt(inputFile, outputFile) : false;
                    }
                    break;
                
                case 2: //Шамир
                    if (mainChoice == 1) {
                        success = shamirLib.encrypt ? shamirLib.encrypt(inputFile, outputFile) : false;
                    } else {
                        success = shamirLib.decrypt ? shamirLib.decrypt(inputFile, outputFile) : false;
                    }
                    break;
                    
                case 3: //Эль-Гамаль
                    if (mainChoice == 1) {
                        success = elgamalLib.encrypt ? elgamalLib.encrypt(inputFile, outputFile) : false;
                    } else {
                        success = elgamalLib.decrypt ? elgamalLib.decrypt(inputFile, outputFile) : false;
                    }
                    break;
                    
                default:
                    cout << "Неверный выбор метода" << endl;
                    continue;
            }
            
            if (success) {
                cout << "Операция завершена успешно" << endl;
                //предлагаем вывести результат
                displayResult(outputFile);
            } else {
                cout << "Ошибка при выполнении операции" << endl;
            }
            
            //удаляем временный файл
            if (mainChoice == 1 && inputFile == "temp.txt") {
                remove("temp.txt");
            }
        }
        
        //отгрузка библиотек (в обратном порядке)
        cout << "Выгрузка библиотек..." << endl;
        unloadLibrary(elgamalLib);
        unloadLibrary(shamirLib);
        unloadLibrary(diffLib);
        unloadLibrary(utilsLib);
        unloadLibrary(constantsLib);
        cout << "Все библиотеки выгружены." << endl;
        
        return 0;
    } catch (const exception& e) {
        cerr << "Критическая ошибка в main: " << e.what() << endl;
        return 1;
    }
}
