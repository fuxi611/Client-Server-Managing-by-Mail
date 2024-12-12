#include "Utils.h"
#include "Server.h"
#include "Command.h"

#define PORT 12345
#define BUFFER_SIZE 4096 

SOCKET serverSocket, clientSocket;




// Server data
std::string SERVER_IP = "";
std::string DATAFILE = "data.json"; 
std::vector<std::string> IP_ADDRESSES; // Available devices
std::vector<std::string> COMMAND_LIST; // Available commands
std::vector<std::string> USER_EMAILS;  // Registered users



void printServerIP() {
    WSADATA wsaData;
    char hostname[256];
    struct addrinfo hints, * result;
    struct sockaddr_in sa;
    char ipStr[INET_ADDRSTRLEN];  // Buffer to store IP address as string

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed. Error: " << WSAGetLastError() << std::endl;
        return;
    }

    // Get the local machine's hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        std::cerr << "Error getting hostname. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Set up hints for getaddrinfo
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4 addresses only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Get address info for the local hostname
    int res = getaddrinfo(hostname, NULL, &hints, &result);
    if (res != 0) {
        std::cerr << "Error getting address info for hostname. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Convert the first IP address from the result into a string
    sa.sin_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr;
    if (inet_ntop(AF_INET, &sa.sin_addr, ipStr, sizeof(ipStr)) == nullptr) {
        std::cerr << "Error converting IP address to string. Error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);  // Clean up
        WSACleanup();
        return;
    }

    std::cout << "Server IP Address: " << ipStr << std::endl;
    SERVER_IP = ipStr;

    // Clean up
    freeaddrinfo(result);  // Free the address info
    WSACleanup();
}

bool checkMailContent(const json& content) {
    std::cout << content.dump() << std::endl;
    std::string temp = "";

    // Check valid command
    temp = content["command"];
    if (!containsString(COMMAND_LIST, temp)) {
        return false;
    }

    // Check IP destination
    temp = content["ip_address"];
    if (!containsString(IP_ADDRESSES,temp)) {
        return false;
    }

    return true;
}

bool loadData() {
    json data = json();
    loadFileJSON(DATAFILE, data);
    
    // Load IP_ADDRESS
    if (data.contains("ip_addresses") && data["ip_addresses"].is_array()) {
        std::cout << "IP ADDRESSES:\n";
        for (const auto& ip : data["ip_addresses"]) {
            std::cout << "- " << ip << '\n';
            IP_ADDRESSES.push_back(ip.get<std::string>());
        }
    }
    else {
        std::cout << "The 'ip_addresses' field is missing or not an array.\n";
    }

    // Load COMMAND_LIST
    if (data.contains("commands") && data["commands"].is_array()) {
        std::cout << "COMMANDS:\n";
        for (const auto& ip : data["commands"]) {
            std::cout << "- " << ip << '\n';
            COMMAND_LIST.push_back(ip.get<std::string>());
        }
    }
    else {
        std::cout << "The 'commands' field is missing or not an array.\n";
    }

    // Load USER_EMAILS
    if (data.contains("user_emails") && data["user_emails"].is_array()) {
        std::cout << "USER EMAILS:\n";
        for (const auto& ip : data["user_emails"]) {
            std::cout << "- " << ip << '\n';
            USER_EMAILS.push_back(ip.get<std::string>());
        }
    }
    else {
        std::cout << "The 'user_emails' field is missing or not an array.\n";
    }
    return true;
}






// Send file throught socket
bool sendFile(SOCKET clientSocket, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return false;
    }

    char buffer[4096]; // Chunk size
    while (file) {
        // Read a chunk from the file
        file.read(buffer, sizeof(buffer));
        std::streamsize bytesRead = file.gcount();

        // Send the chunk
        if (send(clientSocket, buffer, static_cast<int>(bytesRead), 0) == SOCKET_ERROR) {
            std::cerr << "File send error: " << WSAGetLastError() << "\n";
            file.close();
            return false;
        }
    }

    // Send EOF marker to indicate end of file
    const char eofMarker[] = "<EOF>";
    if (send(clientSocket, eofMarker, sizeof(eofMarker), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send EOF marker. Error: " << WSAGetLastError() << "\n";
        file.close();
        return false;
    }

    file.close();
    std::cout << "File sent successfully: " << filename << "\n";
    return true;
}

// Initialize the server (prepareServer)
bool prepareServer() {
    WSADATA wsa;
    sockaddr_in serverAddr;

    // Initialize Winsock
    std::cout << "Initializing Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
        return false;
    }

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    // Define server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Binding failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    // Start listening for connections
    if (listen(serverSocket, 1) == SOCKET_ERROR) { // Max backlog set to 1
        std::cerr << "Listening failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Server is ready and listening on port " << PORT << std::endl;
    return true;
}

// Accept and handle a single client connection (connectClient)
bool connectClient() {
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE] = { 0 };

    std::cout << "Waiting for client connection..." << std::endl;

    // Accept a client connection
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Client connection failed. Error Code: " << WSAGetLastError() << std::endl;
        return false;
    }

    std::cout << "Client connected." << std::endl;
    return true;
}

// Function to receive data from the client and populate a json object
bool receiveClientData(SOCKET clientSocket, json& data) {
    char buffer[BUFFER_SIZE];
    int bytesRead;
    std::string receivedData;

    // Receive data from client in chunks
    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';  // Null-terminate to safely use as a C-string
        receivedData += buffer;    // Append the data chunk to the receivedData string
        // std::cout << buffer << " ";

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

    // Check ending signal
    // std::cout << receivedData << std::endl;
    if (receivedData.size() < 1) return false;

    // Parse the accumulated data into JSON
    try {
        data = json::parse(receivedData);
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    return true;
}

bool sendDataToClient(SOCKET clientSocket, const json& data) {
    // Serialize JSON data and send it
    std::string jsonString = data.dump() + "<END>";
    size_t length = jsonString.length();
    if (send(clientSocket, jsonString.c_str(), length, 0) == SOCKET_ERROR) {
        std::cerr << "Send error: " << WSAGetLastError() << "\n";
        return false;
    }

    // Check if the JSON data specifies a file to send
    if (data.contains("filename")) {
        std::string filename = data["filename"];
        if (!sendFile(clientSocket, filename)) {
            std::cerr << "Failed to send file: " << filename << "\n";
            return false;
        }
    }

    return true;
}

// Shut down the server (shutdownServer)
bool shutdownServer() {
    // Close the client socket
    if (closesocket(clientSocket) == SOCKET_ERROR) {
        std::cerr << "Failed to close client socket. Error Code: " << WSAGetLastError() << std::endl;
    }

    // Close the server socket
    if (closesocket(serverSocket) == SOCKET_ERROR) {
        std::cerr << "Failed to close server socket. Error Code: " << WSAGetLastError() << std::endl;
    }

    // Cleanup Winsock
    WSACleanup();
    std::cout << "Server shut down successfully." << std::endl;
    return true;
}

bool createReply(json& reply, 
    const std::string& receiver, const std::string& filename,
    const std::string& subject,const std::string& bodypart) {

    reply["subject"] = subject;
    reply["bodypart"] = bodypart;
    reply["receiver"] = receiver;
    reply["filename"] = filename;
    return true;
}




void runServer() {
    // Prepare the server: Initialize, bind, and listen for incoming connections
    if (!prepareServer()) {
        std::cerr << "Server preparation failed." << std::endl;
        return;  // Exit if server preparation fails
    }

    std::cout << "Server is running and waiting for clients..." << std::endl;

    bool running = true; // Toggle server state to offline
    while (running) {

        // Connect to a client
        if (!connectClient()) {
            std::cerr << "Error in client connection." << std::endl;
            break;  // Exit if client connection fails
        }

        json get, send = json();
        while (receiveClientData(clientSocket, get)) {
            // Analyze data from here
            running = ReadEmailContent(get, send);

            // Create reply for user
            sendDataToClient(clientSocket, send);

        }
        // After processing the client, close the client socket
        closesocket(clientSocket);
        
        std::cout << "Client Disconnected!" << std::endl;
    }

    // Shutdown the server after exiting the loop
    if (!shutdownServer()) {
        std::cerr << "Failed to shut down the server properly." << std::endl;
    }
}

bool ReadEmailContent(const json& data, json& respond) {
    std::cout << data.dump() << std::endl;
    std::string subject = "Reply: " + data["command"].get<std::string>()
                           + " " + data["ip_address"].get<std::string>();
    std::string bodypart = "";
    std::string receiver = data["sender"].get<std::string>();
    std::string filename = "";

    // Check email
    if (!checkMailContent(data)) {
        std::cout << "Can't process the mail" << std::endl;
        bodypart = "Error!";
    }
    else {
        // Check command
        std::string command = data["command"].get<std::string>();
        FuncPtr commandPtr = getFuncPtr(command);
        if (!commandPtr || !commandPtr(bodypart,filename)) {
            std::cout << "Can't get server respond!\n";
            bodypart = "Error!";
        }
        else {
            std::cout << "Get server respond!\n";
            bodypart = "Successfully!";
        }
    }
    createReply(respond, receiver, filename, subject, bodypart);
    return true;
}
