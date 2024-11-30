#include "Utils.h"
#include "Client.h"

std::string IP_ADDRESS = "127.0.0.1";
#define PORT 12345
#define BUFFER_SIZE 4096


// Filter mails
std::vector<std::string> WHITELIST; // Approved emails

// Manage Socket Client-Server
// Create Connection between Client-Server
// Sent data to Server
// Get data from Server



// Check data
bool checkServerOnline() {
    // Input server IP:
    std::cout << "Enter server IP: ";
    std::string ip_address; std::cin >> ip_address;

    // Check ip address
    if (checkIPAddress(ip_address)) {
        IP_ADDRESS = ip_address.c_str();
        std::cout << ip_address << std::endl;
    }
    else {
        std::cout << "Invalid IP address!" << std::endl;
        IP_ADDRESS = "127.0.0.1";
        return false;
    }

    // Check server is online
    // Construct the ping command
#ifdef _WIN32
    // For Windows: use "-n 1" to send 1 ping packet
    std::string command = "ping -n 1 " + ip_address + " > nul 2>&1";
#else
    // For Linux/Mac: use "-c 1" to send 1 ping packet
    std::string command = "ping -c 1 " + ip_address + " > /dev/null 2>&1";
#endif

    // Execute the command and check the return status
    int status = std::system(command.c_str());
    return (status == 0); // status 0 means the ping was successful

    // Create whitelist for appoviate email
    // WHITELIST = ???
}

bool checkValidMails(const json& content) {
    // Check incorrect data
    // if (content["sender"])
    if (content["command"] == "TERMINATE") return false;
    if (!WHITELIST.empty() && containsString(WHITELIST,content["command"].get<std::string>()))
        return false;
    // Error may happen:
    // 1. Empty fields value,
    // 2. Unknown value
    // 3. Big data

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

bool getData(SOCKET clientSocket, json& reply) {
    char buffer[BUFFER_SIZE];
    int bytesRead;
    std::string receivedData;

    // Receive JSON metadata first
    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';  // Null-terminate the received chunk
        receivedData += buffer;   // Append the chunk to the complete message

        // Check for end-of-transmission marker
        if (receivedData.find("<END>") != std::string::npos) {
            // Remove the "<END>" marker from the received data
            receivedData.erase(receivedData.find("<END>"));
            break;
        }
    }

    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cerr << "Connection closed by client." << std::endl;
        }
        else {
            std::cerr << "recv failed. Error Code: " << WSAGetLastError() << std::endl;
        }
        return false;
    }

    // Parse JSON metadata
    try {
        reply = json::parse(receivedData);
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }

    // Check if the server is sending a file
    if (reply.contains("filename")) {
        std::string filename = reply["filename"];
        if (filename.empty()) return true;

        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return false;
        }

        // Receive the file content
        std::cout << "Receiving file: " << filename << std::endl;
        std::string fileData;
        while (true) {
            bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesRead <= 0) {
                if (bytesRead == 0) {
                    std::cerr << "Connection closed by client during file transfer." << std::endl;
                }
                else {
                    std::cerr << "recv failed during file transfer. Error Code: " << WSAGetLastError() << std::endl;
                }
                file.close();
                return false;
            }

            // Check for EOF marker
            std::string chunk(buffer, bytesRead);
            if (chunk.find("<EOF>") != std::string::npos) {
                // Remove the EOF marker and write the remaining data
                chunk.erase(chunk.find("<EOF>"));
                file.write(chunk.c_str(), chunk.size());
                break;
            }

            // Write received data to the file
            file.write(chunk.c_str(), bytesRead);
        }

        file.close();
        std::cout << "File received and saved as: " << filename << std::endl;
    }

    return true;
}

bool createSocket(SOCKET& clientSocket) {
    WSADATA wsa;
    sockaddr_in serverAddr;
    PCSTR SERVER_ADDRESS = IP_ADDRESS.c_str();

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
    if (InetPtonA(AF_INET, SERVER_ADDRESS, &serverAddr.sin_addr) <= 0) {
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

    return true;
}

bool releaseSocket(SOCKET& clientSocket) {
    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return true;
}