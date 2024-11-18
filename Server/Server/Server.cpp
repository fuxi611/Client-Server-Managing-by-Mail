#include "Utils.h"
#include "Server.h"
using json = nlohmann::json; // Using json type

PCSTR IP_INDEX = "10.20.0.242";
#define PORT 8080
#define BUFFER_SIZE 4096


// Manage Socket Client-Server
// Create Connection between Client-Server
// Sent data to Server
// Get data from Server

// Server data
bool checkMailContent(const json& content) {
    std::cout << content.dump() << std::endl;
    return false;
}


bool getData(SOCKET clientSocket, json& reply) {
    char buffer[BUFFER_SIZE];
    int bytesRead;
    std::string receivedData;

    // Receive data from client in chunks
    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';  // Null-terminate to safely use as a C-string
        receivedData += buffer;    // Append the data chunk to the receivedData string
        std::cout << buffer << " ";

        // Check for end-of-transmission marker
        if (receivedData.find("<END>") != std::string::npos) {
            // Remove "<END>" marker from data if necessary
            receivedData.erase(receivedData.find("<END>"));
            break;
        }
    }

    // Check if the connection was closed or an error occurred
    if (bytesRead == 0) {
        std::cout << "Connection closed by client." << std::endl;
    }
    else if (bytesRead == SOCKET_ERROR) {
        std::cerr << "recv failed. Error Code: " << WSAGetLastError() << std::endl;
        return false;
    }

    // Parse the accumulated data into JSON
    try {
        reply = json::parse(receivedData);
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    return true;
}

bool sendData(SOCKET clientSocket, const json& data) {
    std::string jsonString = data.dump() + "<END>";
    size_t length = jsonString.length();
    int bytesSent = send(clientSocket, jsonString.c_str(), length, 0);

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Send error: " << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}

bool sendClientData(const json& data) {
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    std::cout << "Initializing Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    serverAddr.sin_family = AF_INET;
    if (InetPtonA(AF_INET, IP_INDEX, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    std::cout << "Connected to server." << std::endl;

    // Send data to server
    // Directly send jsonString as a C-string without dynamic allocation
    sendData(clientSocket, data);

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return true;
}

bool getClientData(json& data) {
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    std::cout << "Initializing Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    serverAddr.sin_family = AF_INET;
    if (InetPtonA(AF_INET, IP_INDEX, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    std::cout << "Connected to server." << std::endl;

    // Get data from server
    getData(clientSocket, data);

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return true;
}
