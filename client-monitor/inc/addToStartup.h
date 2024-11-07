#include <iostream>
#include <windows.h>
#include <shlobj.h>

#pragma once

void addToStartup(const char* exePath, const char* programName);
bool isProgramInStartup(const char* programName);