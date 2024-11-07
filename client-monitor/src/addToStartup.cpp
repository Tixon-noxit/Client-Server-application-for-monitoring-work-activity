#include "..\inc\addToStartup.h"

// -lws2_32


/**
 * @brief Добавляет программу в автозапуск в реестре Windows.
 * 
 * Эта функция добавляет указанный исполнимый файл в раздел автозапуска в реестре Windows, чтобы программа запускалась 
 * автоматически при старте системы. Функция принимает путь к исполнимому файлу и имя программы, а затем сохраняет 
 * информацию в реестре.
 *
 * @param exePath Путь к исполнимому файлу, который нужно добавить в автозапуск. 
 *                Это полный путь к файлу, включая имя файла.
 * @param programName Имя программы, которое будет использовано в реестре для записи в разделе автозапуска.
 *
 * @return void
 *
 * @note
 * - Путь к исполнимому файлу должен быть указан с учетом того, что он может содержать имя файла в конце (например, `C:\\path\\to\\program.exe`).
 * - Если в процессе работы возникает ошибка, она будет выведена в консоль.
 * - Регистрируются только программы, которые находятся в текущей директории.
 *
 * @example
 * addToStartup("C:\\path\\to\\client_chop_centaur.exe", "MyClientApp");
 *
 * @see RegOpenKeyExA() Для открытия реестра.
 * @see RegSetValueExA() Для записи значения в реестр.
 */
void addToStartup(const char *exePath, const char *programName) {

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Директория из полного пути
    char exeDir[MAX_PATH];
    strcpy(exeDir, exePath);
    char *lastSlash = strrchr(exeDir, '\\');
    if (lastSlash != NULL) {
        *lastSlash = '\0';
    }

    // Путь к client_chop_centaur.exe
    strcat(exeDir, "\\client_chop_centaur.exe");
    std::cout << "Путь к " << programName << ": " << exeDir << std::endl;

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE,
                                &hKey);
    if (result == ERROR_SUCCESS) {
        result = RegSetValueExA(hKey, programName, 0, REG_SZ, (const BYTE *) exeDir, strlen(exeDir) + 1);
        if (result == ERROR_SUCCESS) {
            std::cout << "Программа " << programName << " добавлена в автозапуск!" << std::endl;
        } else {
            std::cerr << "Ошибка при добавлении в реестр: " << result << std::endl;
        }
        RegCloseKey(hKey);
    } else {
        std::cerr << "Ошибка при открытии реестра: " << result << std::endl;
    }
}


/**
 * @brief Проверяет, добавлена ли программа в автозапуск в реестре Windows.
 * 
 * Эта функция проверяет, существует ли запись о программе в разделе автозапуска реестра Windows. Если программа
 * присутствует, функция выводит путь к исполнимому файлу, связанному с программой, и возвращает `true`. В противном случае
 * возвращает `false`.
 *
 * @param programName Имя программы, которое будет использоваться для поиска в реестре.
 *
 * @return Возвращает `true`, если программа найдена в автозапуске, и `false` в противном случае.
 *
 * @note
 * - Проверяется только реестр для текущего пользователя.
 * - Если программа найдена, возвращается путь к исполнимому файлу, связанному с программой.
 * - В случае ошибки при открытии реестра или чтении значения, возвращается `false`.
 *
 * @example
 * bool isInStartup = isProgramInStartup("MyClientApp");
 * if (isInStartup) {
 *     std::cout << "Программа в автозапуске." << std::endl;
 * } else {
 *     std::cout << "Программа не в автозапуске." << std::endl;
 * }
 *
 * @see RegOpenKeyExA() Для открытия реестра.
 * @see RegQueryValueExA() Для чтения значения из реестра.
 */
bool isProgramInStartup(const char *programName) {

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ,
                                &hKey);
    if (result == ERROR_SUCCESS) {
        char exePath[MAX_PATH];
        DWORD bufferSize = MAX_PATH;
        result = RegQueryValueExA(hKey, programName, NULL, NULL, (LPBYTE) exePath, &bufferSize);
        RegCloseKey(hKey);

        if (result == ERROR_SUCCESS) {
            std::cout << "Программа найдена в автозапуске: " << exePath << std::endl;
            return true;
        } else {
            std::cout << "Программа не найдена в автозапуске." << std::endl;
            return false;
        }
    } else {
        std::cerr << "Ошибка при открытии реестра для чтения: " << result << std::endl;
        return false;
    }
}

