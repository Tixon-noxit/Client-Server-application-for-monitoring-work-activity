#include <iostream>
#include <windows.h>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

#pragma once

std::vector <BYTE> captureScreenToMemory();

void sendImageToServer(const std::vector <BYTE> &imageData, const std::string &serverIP, int port);
