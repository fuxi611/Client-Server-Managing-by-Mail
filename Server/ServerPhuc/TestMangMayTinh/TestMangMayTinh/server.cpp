#include <winsock2.h>
#include <iostream>
#include <windows.h>
#include<string>
#include <ws2tcpip.h>
#include<filesystem>
#include<fstream>
#include<thread>
#pragma comment(lib, "ws2_32.lib")  // Link thư viện Winsock
namespace fs = std::filesystem;
#define PORT 8080
#define BUFFER_SIZE 4096
bool ListApp(std::string& filename) {
    std::string command = "tasklist /FI \"SESSIONNAME eq Console\" /FO LIST > " + filename;
    int result = system(command.c_str());

    if (result == 0) {
        return true;
    }
    else {
        return false;
    }
}
bool ListService(std::string& filename) {
    // Thực thi lệnh tasklist để lấy danh sách tiến trình
   /* std::ifstream ip("tasklist /FI \"SESSIONNAME eq Service\"");
    if (!ip.is_open()) {
        std::cerr << "tasklist fail" << std::endl;
        return false;
    }
    std::ofstream op(filename);
    std::string line;
    while (getline(ip, line)) {
        op << line << std::endl;
    }
    ip.close();
    op.close();*/
    // Lệnh tasklist lọc chỉ ứng dụng đang chạy
    std::string command = "sc query > " + filename;
    int result = system(command.c_str());

    if (result == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool startApp(const std::string& input, std::string& filename) {
    std::string command = "start " + input;
    int result = system(command.c_str());
    return result == 0;
}
bool startService(const std::string& input, std::string& filename) {
    std::string command = "net start " + input;
    int result = system(command.c_str());
    return result == 0;
}

void captureScreen(std::string& filename) {
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
//void sendScreenshot(SOCKET clientSocket) {
//    captureScreen("screenshot.bmp");
//
//    std::ifstream file("screenshot.bmp", std::ios::binary | std::ios::ate);
//    int fileSize = file.tellg();
//    file.seekg(0, std::ios::beg);
//    // Gửi kích thước ảnh trước
//    send(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
//
//    char buffer[BUFFER_SIZE];
//    int bytesSent = 0;
//
//    while (bytesSent < fileSize) {
//        file.read(buffer, BUFFER_SIZE);
//        int bytesRead = file.gcount();
//        int result = send(clientSocket, buffer, bytesRead, 0);
//
//        if (result == SOCKET_ERROR) {
//            std::cerr << "Error sending data.\n";
//            break;
//        }
//        bytesSent += result;
//    }
//    file.close();
//}

bool RestartServer(const std::string& input, std::string& filename) {
    std::string command = "shutdown /r /t " + input;
    int result = system(command.c_str());
    return result == 0;
}
bool ShutdownServer(const std::string& input, std::string& filename) {
    std::string command = "shutdown /r /t " + input;
    int result = system(command.c_str());
    return result == 0;
}

bool stopApp(const std::string& input, std::string& filename) {
    std::string command = "taskkill /IM " + input + " /F";
    int result = system(command.c_str());
    return result == 0;
}
bool stopService(const std::string& input, std::string& filename) {
    std::string command = "net stop " + input;
    int result = system(command.c_str());
    return result == 0;
}

int main() {
    // WSADATA wsa;
    // SOCKET serverSocket, clientSocket;
    // sockaddr_in serverAddr, clientAddr;
    // int addrLen = sizeof(clientAddr);

    // // Khởi tạo Winsock
    // std::cout << "Initializing Winsock..." << std::endl;
    // if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    //     std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
    //     return 1;
    // }

    // // Tạo socket
    // serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // if (serverSocket == INVALID_SOCKET) {
    //     std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
    //     WSACleanup();
    //     return 1;
    // }

    // // Cấu hình địa chỉ server
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_addr.s_addr = INADDR_ANY;
    // serverAddr.sin_port = htons(PORT);

    // // Gán socket với địa chỉ và cổng
    // if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
    //     std::cerr << "Bind failed. Error Code: " << WSAGetLastError() << std::endl;
    //     closesocket(serverSocket);
    //     WSACleanup();
    //     return 1;
    // }

    // // Lắng nghe kết nối từ client
    // listen(serverSocket, 3);
    // std::cout << "Waiting for client connections..." << std::endl;

    // // Chấp nhận kết nối từ client
    // clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
    // if (clientSocket == INVALID_SOCKET) {
    //     std::cerr << "Accept failed. Error Code: " << WSAGetLastError() << std::endl;
    //     closesocket(serverSocket);
    //     WSACleanup();
    //     return 1;
    // }
    // std::cout << "Client connected." << std::endl;
    //char buffer[BUFFER_SIZE];
    //while (true) {
    //    memset(buffer, 0, sizeof(buffer));
    //    int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
    //    if (bytes_received > 0) {
    //        std::cout << "Command from Client: " << buffer << std::endl;
    //        // Xử lý lệnh từ client
    //        if (strcmp(buffer, "List app") == 0) {
    //            //runCommand(clientSocket);
    //            // Gửi chuỗi thông báo kết thúc
    //            std::string endMessage = "END_OF_RESPONSE";
    //            send(clientSocket, endMessage.c_str(), endMessage.length(), 0);
    //        }
    //        else if (strcmp(buffer, "Stop app") == 0) {
    //            recv(clientSocket, buffer, sizeof(buffer), 0);
    //            if (stopApplicationByName(buffer)) {
    //                std::string successMessage = "success nho";
    //                send(clientSocket, successMessage.c_str(), successMessage.length(), 0);
    //            }
    //            else {
    //                std::string errorMessage = "cant stop";
    //                send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
    //            }
    //        }
    //        else if (strcmp(buffer, "Start app") == 0) {
    //            recv(clientSocket, buffer, sizeof(buffer), 0);
    //            if (startApp(buffer)) {
    //                std::string successMessage = "success nho";
    //                send(clientSocket, successMessage.c_str(), successMessage.length(), 0);
    //            }
    //            else {
    //                std::string errorMessage = "cant stop";
    //                send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
    //            }
    //        }
    //        else if (strcmp(buffer, "Screen shot")==0) {
    //            sendScreenshot(clientSocket);
    //        }
    //        else if (strcmp(buffer, "Exit") == 0) {
    //            std::cout << "Client disconnected." << std::endl;
    //            break;
    //        }
    //        else {
    //            std::cout <<buffer <<" is invalid command"<< std::endl;
    //            std::string response = "Invalid command";
    //            send(clientSocket, response.c_str(), response.size(), 0);
    //        }
    //    }
    //    else if (bytes_received == SOCKET_ERROR) {
    //        std::cerr << "cant transport data: " << WSAGetLastError() << std::endl;
    //        break;
    //    }
    //}

     // Đóng socket và dọn dẹp Winsock
    /* closesocket(clientSocket);
     closesocket(serverSocket);
     WSACleanup();*/
    std::string filename = "data.txt";
    ListService(filename);
    return 0;
}
