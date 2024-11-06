#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>

// Функция для подключения клиента к серверу
int connectToServer(const std::string &serverIp, int port) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        exit(1);
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to server: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }

    return clientSocket;
}

// Функция для отправки активности клиента на сервер
void sendClientActivity(int clientSocket) {
    std::string activityMessage = "Client is active!";
    while (true) {
        // Отправка сообщения на сервер
        ssize_t sendResult = send(clientSocket, activityMessage.c_str(), activityMessage.length(), 0);
        if (sendResult == -1) {
            std::cerr << "Send failed: " << strerror(errno) << std::endl;
            break;
        }

        // Пауза на 5 секунд перед отправкой следующего сообщения
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

// Функция для получения данных с сервера (например, запрос состояния)
void receiveServerData(int clientSocket) {
    char buffer[512];
    while (true) {
        ssize_t recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recvResult > 0) {
            buffer[recvResult] = '\0'; // Завершаем строку
            std::cout << "Server response: " << buffer << std::endl;
        } else if (recvResult == 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        } else {
            std::cerr << "Recv failed: " << strerror(errno) << std::endl;
            break;
        }
    }
}

int main() {
    // IP-адрес и порт сервера
    std::string serverIp = "127.0.0.1";
    int serverPort = 5000;

    // Подключение к серверу
    int clientSocket = connectToServer(serverIp, serverPort);
    std::cout << "Connected to server at " << serverIp << ":" << serverPort << std::endl;

    // Запуск потока для отправки активности на сервер
    std::thread activityThread(sendClientActivity, clientSocket);

    // Запуск потока для получения данных от сервера
    std::thread receiveThread(receiveServerData, clientSocket);

    // Ожидаем завершения потоков
    activityThread.join();
    receiveThread.join();

    // Закрытие сокета
    close(clientSocket);

    return 0;
}
