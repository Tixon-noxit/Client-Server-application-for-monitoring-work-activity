#include <iostream>
#include <windows.h>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

// -lgdi32

std::string getCurrentDateTime() {
    // Текущее время
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

void captureScreen() {
    // Дескриптор окна
    HWND hwnd = GetDesktopWindow();
    HDC hdcScreen = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
    
    // Размер экрана
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right;
    int height = rect.bottom;

    // Объект для хранения скриншота
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HGDIOBJ oldObj = SelectObject(hdcMemDC, hBitmap);

    BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    // Имя файла с добавлением текущей даты и времени
    std::string filename = "screenshot_" + getCurrentDateTime() + ".bmp";
    
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    DWORD dwBmpSize;

    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = width;
    biHeader.biHeight = height;
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biSizeImage = ((width * 24 + 31) / 32) * 4 * height;
    biHeader.biXPelsPerMeter = 0;
    biHeader.biYPelsPerMeter = 0;
    biHeader.biClrUsed = 0;
    biHeader.biClrImportant = 0;

    // Открываю файл для записи
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD dwBytesWritten = 0;

    // Записываю заголовок файла
    bfHeader.bfType = 0x4D42; // 'BM'
    bfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + biHeader.biSizeImage;
    bfHeader.bfReserved1 = 0;
    bfHeader.bfReserved2 = 0;
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);

    // Записываю пиксели
    BYTE* lpPixels = new BYTE[biHeader.biSizeImage];
    GetDIBits(hdcMemDC, hBitmap, 0, height, lpPixels, (BITMAPINFO*)&biHeader, DIB_RGB_COLORS);
    WriteFile(hFile, lpPixels, biHeader.biSizeImage, &dwBytesWritten, NULL);

    CloseHandle(hFile);

    delete[] lpPixels;
    SelectObject(hdcMemDC, oldObj);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcScreen);

    std::cout << "Screenshot saved as: " << filename << std::endl;
}

int main() {
    captureScreen();
    Sleep(5000);
    return 0;
}
