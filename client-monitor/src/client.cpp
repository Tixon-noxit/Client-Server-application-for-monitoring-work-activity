#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib") // Подключение с Winsock

// Инициализация Winsock
void initWinsock() {
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        std::cerr << "WSAStartup failed with error: " << wsResult << std::endl;
        exit(1);
    }
}

// Подключение клиента к серверу
SOCKET connectToServer(const std::string &serverIp, int port) {
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }

    return clientSocket;
}

// Отправка активности клиента на сервер
void sendClientActivity(SOCKET clientSocket) {
    std::string activityMessage = "Client is active!";
    while (true) {
        // Отправка сообщения на сервер
        int sendResult = send(clientSocket, activityMessage.c_str(), activityMessage.length(), 0);
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }

        // Пауза на 5 секунд перед отправкой следующего сообщения
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

// Получение данных с сервера (например, запрос состояния)
void receiveServerData(SOCKET clientSocket) {
    char buffer[512];
    while (true) {
        int recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recvResult > 0) {
            buffer[recvResult] = '\0'; // Завершаем строку
            std::cout << "Server response: " << buffer << std::endl;
        } else if (recvResult == 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        } else {
            std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

int main() {
    initWinsock();

    // IP-адрес и порт сервера
    std::string serverIp = "127.0.0.1";
    int serverPort = 5000;

    // Подключение к серверу
    SOCKET clientSocket = connectToServer(serverIp, serverPort);
    std::cout << "Connected to server at " << serverIp << ":" << serverPort << std::endl;

    // Запуск потока для отправки активности на сервер
    std::thread activityThread(sendClientActivity, clientSocket);

    // Запуск потока для получения данных от сервера
    std::thread receiveThread(receiveServerData, clientSocket);

    // Ожидаем завершения потоков
    activityThread.join();
    receiveThread.join();

    // Закрытие сокета и очистка ресурсов
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
