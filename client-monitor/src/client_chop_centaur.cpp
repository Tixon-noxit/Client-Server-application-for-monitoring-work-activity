#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>

void initWinsock() {
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        std::cerr << "WSAStartup failed with error: " << wsResult << std::endl;
        exit(1);
    }
}

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

// Функция для отправки запроса на сервер
bool sendActivity(SOCKET clientSocket, const std::string& ip, const std::string& username) {
    std::string jsonData = "{\"ip\": \"" + ip + "\", \"username\": \"" + username + "\"}";
    std::string httpRequest = "POST /api/add_client HTTP/1.1\r\n"
                              "Host: 127.0.0.1\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: " + std::to_string(jsonData.length()) + "\r\n\r\n" +
                              jsonData;

    int sendResult = send(clientSocket, httpRequest.c_str(), httpRequest.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

// Функция потока для периодической отправки активности клиента
DWORD WINAPI sendClientActivity(LPVOID param) {
    std::string serverIp = "127.0.0.1";
    int serverPort = 5000;
    std::string ip = "192.168.0.1";
    std::string username = "Client1";

    while (true) {
        SOCKET clientSocket = connectToServer(serverIp, serverPort);

        if (sendActivity(clientSocket, ip, username)) {
            std::cout << "Activity sent successfully.\n";
        } else {
            std::cerr << "Failed to send activity.\n";
        }
        closesocket(clientSocket);

        Sleep(5000);
    }
    return 0;
}

int main() {
    initWinsock();

    HANDLE activityThread = CreateThread(
        NULL,
        0,
        sendClientActivity,
        NULL,
        0,
        NULL
    );

    if (activityThread == NULL) {
        std::cerr << "CreateThread failed with error: " << GetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Основной поток ждет, удерживая приложение
    WaitForSingleObject(activityThread, INFINITE);
    CloseHandle(activityThread);
    WSACleanup();
    return 0;
}
