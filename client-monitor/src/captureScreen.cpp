#include "..\inc\captureScreen.h"

// -lgdi32


/**
 * @brief Снимает скриншот с экрана и сохраняет его в память в формате BMP.
 *
 * Функция создает изображение экрана, захватывает его и сохраняет в память в формате BMP.
 * Структура результата включает заголовки BITMAPFILEHEADER и BITMAPINFOHEADER,
 * за которыми следует пиксельные данные изображения.
 *
 * @return std::vector<BYTE> Массив байтов, содержащий данные изображения в формате BMP.
 * Включает заголовки BMP и пиксельные данные.
 */
std::vector <BYTE> captureScreenToMemory() {
    HWND hwnd = GetDesktopWindow();
    HDC hdcScreen = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen);

    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right;
    int height = rect.bottom;

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HGDIOBJ oldObj = SelectObject(hdcMemDC, hBitmap);
    BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    BITMAPINFOHEADER biHeader = {};
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = width;
    biHeader.biHeight = height;
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biSizeImage = ((width * 24 + 31) / 32) * 4 * height;

    BITMAPFILEHEADER bfHeader = {};
    bfHeader.bfType = 0x4D42; // 'BM'
    bfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + biHeader.biSizeImage;
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::vector <BYTE> imageData(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + biHeader.biSizeImage);
    memcpy(imageData.data(), &bfHeader, sizeof(BITMAPFILEHEADER));
    memcpy(imageData.data() + sizeof(BITMAPFILEHEADER), &biHeader, sizeof(BITMAPINFOHEADER));

    BYTE *lpPixels = new BYTE[biHeader.biSizeImage];
    GetDIBits(hdcMemDC, hBitmap, 0, height, lpPixels, (BITMAPINFO * ) & biHeader, DIB_RGB_COLORS);
    memcpy(imageData.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), lpPixels, biHeader.biSizeImage);

    delete[] lpPixels;
    SelectObject(hdcMemDC, oldObj);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcScreen);

    return imageData;
}

/**
 * @brief Отправляет изображение на сервер по протоколу HTTP через сокет.
 *
 * Функция устанавливает TCP-соединение с сервером по указанному IP-адресу и порту,
 * а затем отправляет изображение в виде байтового массива через HTTP POST-запрос.
 * Изображение передается в теле запроса с заголовками, указывающими на тип контента и длину данных.
 *
 * @param imageData Вектор байтов, содержащий данные изображения в формате BMP или другом.
 * @param serverIP IP-адрес сервера, на который отправляется запрос.
 * @param port Порт, на который устанавливается соединение для отправки данных.
 */
void sendImageToServer(const std::vector <BYTE> &imageData, const std::string &serverIP, int port) {

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_addr.s_addr = inet_addr(serverIP.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        std::cerr << "Соединение не удалось" << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::ostringstream oss;
    oss << "POST /upload HTTP/1.1\r\n"
        << "Host: " << serverIP << "\r\n"
        << "Content-Type: application/octet-stream\r\n"
        << "Content-Length: " << imageData.size() << "\r\n"
        << "Connection: close\r\n\r\n";

    std::string header = oss.str();
    send(sock, header.c_str(), header.size(), 0);
    send(sock, reinterpret_cast<const char *>(imageData.data()), imageData.size(), 0);

    closesocket(sock);
    WSACleanup();
}
