#include "utils.h"
#include "Command.h"
#include <windows.h>
using namespace std;

FuncPtr getFuncPtr(const string& cmd) {
    FuncPtr command = NULL;
    if (cmd == "CAPTURE") {
        command = captureScreen;
	}
	else if(cmd == "TASKLIST"){
        command = taskList;
	}
	return command;
}

bool taskList(const string& input, string& filename) {
    filename = "tasklist.txt";
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