#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


bool checkServerOnline();
					// Check server online

bool checkValidMails(const json& content);
					// Check mail content

bool sendData(SOCKET clientSocket, const json& data);
					// Sent data to socket

bool getData(SOCKET clientSocket, json& reply);
					// Get data from socket

bool createSocket(SOCKET& clientSocket);
bool releaseSocket(SOCKET& clientSocket);

