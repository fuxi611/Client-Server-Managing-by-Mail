#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")




// Data management
void printServerIP();	
						// Find server IP

bool checkMailContent(const nlohmann::json& content);
						// Check mail content
	
bool loadData();
						// Load data




// Server handling
bool prepareServer();	
						// Setup server for connection
						
bool connectClient();	
						// Listen for connection

bool receiveClientData(SOCKET clientSocket, nlohmann::json& data);
						// receive data

bool sendDataToClient(SOCKET clientSocket, const nlohmann::json& data);
						// send data

bool shutdownServer();	
						// Shutting down the server






// Message
void runServer();		
						// Running server

bool ReadEmailContent(const nlohmann::json& get, nlohmann::json& send);
						// Read and reply


