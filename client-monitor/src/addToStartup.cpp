#include <iostream>
#include <windows.h>
#include <shlobj.h>

// -lws2_32

void addToStartup() {

    SetConsoleCP(CP_UTF8);    
    SetConsoleOutputCP(CP_UTF8);

    //Путь к текущей директории
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
        std::cerr << "Ошибка при получении пути к исполнимому файлу." << std::endl;
        return;
    }

    // Директория из полного пути
    char exeDir[MAX_PATH];
    strcpy(exeDir, exePath);
    char* lastSlash = strrchr(exeDir, '\\');
    if (lastSlash != NULL) {
        *lastSlash = '\0';
    }

    // Путь к client_chop_centaur.exe
    strcat(exeDir, "\\client_chop_centaur.exe");
    std::cout << "путь к client_chop_centaur.exe" << exeDir<< std::endl;
    Sleep(500);
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
    if (result == ERROR_SUCCESS) {
        result = RegSetValueExA(hKey, "MyClientApp", 0, REG_SZ, (const BYTE*)exeDir, strlen(exeDir) + 1);
        if (result == ERROR_SUCCESS) {
            std::cout << "Программа добавлена в автозапуск!" << std::endl;
        } else {
            std::cerr << "Ошибка при добавлении в реестр: " << result << std::endl;
        }
        RegCloseKey(hKey);
    } else {
        std::cerr << "Ошибка при открытии реестра: " << result << std::endl;
    }
}

int main() {
    addToStartup();
    Sleep(5000);
    return 0;
}
