//#include "Utils.h"
//#include "Server.h"
//using json = nlohmann::json; // Using json type


#include <winsock2.h> 
#include <iostream> 
#include <windows.h> 
#include<string> 
#include <ws2tcpip.h> 
#include<filesystem> 
#include<fstream> 
#pragma comment(lib, "ws2_32.lib")  // Link thư viện Winsock 

namespace fs = std::filesystem;
#define PORT 12345
#define BUFFER_SIZE 4096 

void captureScreen(const char* filename) {
    int x1 = 0, y1 = 0;
    int x2 = GetSystemMetrics(SM_CXSCREEN);
    int y2 = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, x2, y2);

    SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, x2, y2, hScreen, x1, y1, SRCCOPY);
    ReleaseDC(NULL, hScreen);

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fileHeader.bfType = 0x4D42;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    fileHeader.bfSize = fileHeader.bfOffBits + (x2 * y2 * 4);
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;

    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = x2;
    infoHeader.biHeight = -y2;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file.write((char*)&fileHeader, sizeof(fileHeader));
    file.write((char*)&infoHeader, sizeof(infoHeader));

    int imageSize = x2 * y2 * 4;
    char* bmpData = new char[imageSize];
    GetDIBits(hDC, hBitmap, 0, y2, bmpData, (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);
    file.write(bmpData, imageSize);
    delete[] bmpData;

    file.close();
    DeleteObject(hBitmap);
    DeleteDC(hDC);
}

void sendScreenshot(SOCKET clientSocket) {
    captureScreen("screenshot.bmp");
    std::ifstream file("screenshot.bmp", std::ios::binary | std::ios::ate);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Gửi kích thước ảnh trước 
    send(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
    char buffer[BUFFER_SIZE];
    int bytesSent = 0;

    while (bytesSent < fileSize) {
        file.read(buffer, BUFFER_SIZE);
        int bytesRead = file.gcount();
        int result = send(clientSocket, buffer, bytesRead, 0);

        if (result == SOCKET_ERROR) {
            std::cerr << "Error sending data.\n";
            break;
        }
        bytesSent += result;
    }
    file.close();
}

void reset(const std::string& buffer) {
    if (std::string(buffer) == "RESET") {
        std::cout << "Resetting server..." << std::endl;
        system("shutdown /r");  // Lệnh reset server trên Windows 
    }
}

bool stopApplicationByName(const std::string& appName) {
    std::string command = "taskkill /IM " + appName + " /F";
    int result = system(command.c_str());
    return result == 0;
}

bool startApp(const std::string& appName) {
    //STARTUPINFOA si; 
    //PROCESS_INFORMATION pi; 
    //ZeroMemory(&si, sizeof(si)); 
    //si.cb = sizeof(si); 
    //ZeroMemory(&pi, sizeof(pi)); 

    //// Sử dụng `CreateProcessA` với chuỗi kiểu `char*` 
    //if (!CreateProcessA( 
    //    NULL, (LPSTR)appPath.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi 
    //)) { 
    //    std::cerr << "Can't run app: " << GetLastError() << std::endl; 
    //    return false; 
    //} 

    //// Đóng các handle của quá trình và luồng 
    //CloseHandle(pi.hProcess); 
    //CloseHandle(pi.hThread); 
    //return true; 

    std::string command = "start " + appName;
    int result = system(command.c_str());
    return result == 0;
}

void runCommand(SOCKET clientSocket) {
    // Thực thi lệnh tasklist để lấy danh sách tiến trình 
    FILE* pipe = _popen("tasklist /FI \"SESSIONNAME eq Console\"", "r");
    if (!pipe) {
        std::cerr << "tasklist fail" << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        // Gửi từng dòng kết quả về cho client 
        if (send(clientSocket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            std::cerr << "ERROR : " << WSAGetLastError() << std::endl;
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }
    _pclose(pipe);
}

void findAndOpenExecutable(const fs::path& directory, const std::string& exeName) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".exe") {
                if (entry.path().filename() == exeName) {
                    std::cout << "Đã tìm thấy ứng dụng: " << entry.path().string() << std::endl;
                    // Mở ứng dụng 
                    if (startApp(entry.path().string())) {
                        std::cout << "Ứng dụng đã được khởi chạy thành công." << std::endl;
                    }
                    else {
                        std::cout << "Lỗi khi khởi chạy ứng dụng." << std::endl;
                    }
                    return; // Kết thúc sau khi tìm và mở ứng dụng 
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Loi: " << e.what() << std::endl;
    }
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    std::cout << "Initializing Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Waiting for client connections..." << std::endl;

    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Client connected." << std::endl;

    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            std::cout << "Command from Client: " << buffer << std::endl;
            // Xử lý lệnh từ client 
            if (strcmp(buffer, "List app") == 0) {
                runCommand(clientSocket);
                // Gửi chuỗi thông báo kết thúc 
                std::string endMessage = "END_OF_RESPONSE";
                send(clientSocket, endMessage.c_str(), endMessage.length(), 0);
            }
            else if (strcmp(buffer, "Stop app") == 0) {
                recv(clientSocket, buffer, sizeof(buffer), 0);
                if (stopApplicationByName(buffer)) {
                    std::string successMessage = "success nho";
                    send(clientSocket, successMessage.c_str(), successMessage.length(), 0);
                }
                else {
                    std::string errorMessage = "cant stop";
                    send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
                }
            }
            else if (strcmp(buffer, "Start app") == 0) {
                recv(clientSocket, buffer, sizeof(buffer), 0);
                if (startApp(buffer)) {
                    std::string successMessage = "success nho";
                    send(clientSocket, successMessage.c_str(), successMessage.length(), 0);
                }
                else {
                    std::string errorMessage = "cant stop";
                    send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
                }
            }
            else if (strcmp(buffer, "Screen shot") == 0) {
                sendScreenshot(clientSocket);
            }
            else if (strcmp(buffer, "Exit") == 0) {
                std::cout << "Client disconnected." << std::endl;
                break;
            }
            else {
                std::cout << buffer << " is invalid command" << std::endl;
                std::string response = "Invalid command";
                send(clientSocket, response.c_str(), response.size(), 0);
            }
        }
        else if (bytes_received == SOCKET_ERROR) {
            std::cerr << "cant transport data: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    // Đóng socket và dọn dẹp Winsock 
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}