#include "Utils.h"

using json = nlohmann::json;




// File stream
bool saveStringToFile(const std::string& text,const std::string& filename) {
    std::ofstream outFile(filename);  // Open the file for writing

    if (outFile.is_open()) {  // Check if the file was opened successfully
        outFile << text;  // Write the string to the file
        outFile.close();  // Close the file after writing
    }
    else {
        std::cerr << "Failed to open file: " << filename << std::endl;  // Error handling if the file can't be opened
        return false;
    }
    return true;
}

bool loadStringFromFile(std::string& content, const std::string& filename) {
    std::ifstream inFile(filename);  // Open the file for reading

    if (inFile.is_open()) {  // Check if the file was opened successfully
        std::ostringstream ss;
        ss << inFile.rdbuf();  // Read the entire file content into the stringstream
        content = ss.str();    // Convert stringstream to string
        inFile.close();        // Close the file after reading
        return true;           // Return true if successful
    }
    else {
        std::cerr << "Failed to open file: " << filename << std::endl;  // Error handling if the file can't be opened
        return false;          // Return false if the file can't be opened
    }
}

bool loadFileJSON(const std::string& filename, json& config) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    try {
        inputFile >> config;
    }
    catch (const json::parse_error& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }

    return true;
}

std::string getMimeType(const std::string& filename) {
    if (filename.find(".jpg") != std::string::npos || filename.find(".jpeg") != std::string::npos) {
        return "image/jpeg";
    }
    else if (filename.find(".png") != std::string::npos) {
        return "image/png";
    }
    else if (filename.find(".gif") != std::string::npos) {
        return "image/gif";
    }
    else if (filename.find(".bmp") != std::string::npos) {
        return "image/bmp";
    }
    else if (filename.find(".webp") != std::string::npos) {
        return "image/webp";
    }
    else if (filename.find(".svg") != std::string::npos) {
        return "image/svg+xml";

        // Video files
    }
    else if (filename.find(".mp4") != std::string::npos) {
        return "video/mp4";
    }
    else if (filename.find(".avi") != std::string::npos) {
        return "video/x-msvideo";
    }
    else if (filename.find(".mov") != std::string::npos) {
        return "video/quicktime";
    }
    else if (filename.find(".wmv") != std::string::npos) {
        return "video/x-ms-wmv";
    }
    else if (filename.find(".flv") != std::string::npos) {
        return "video/x-flv";
    }
    else if (filename.find(".mkv") != std::string::npos) {
        return "video/x-matroska";
    }
    else if (filename.find(".webm") != std::string::npos) {
        return "video/webm";

        // Audio files
    }
    else if (filename.find(".mp3") != std::string::npos) {
        return "audio/mpeg";
    }
    else if (filename.find(".wav") != std::string::npos) {
        return "audio/wav";
    }
    else if (filename.find(".ogg") != std::string::npos) {
        return "audio/ogg";
    }
    else if (filename.find(".flac") != std::string::npos) {
        return "audio/flac";
    }
    else if (filename.find(".aac") != std::string::npos) {
        return "audio/aac";
    }
    else if (filename.find(".m4a") != std::string::npos) {
        return "audio/mp4";

        // Document files
    }
    else if (filename.find(".pdf") != std::string::npos) {
        return "application/pdf";
    }
    else if (filename.find(".doc") != std::string::npos || filename.find(".docx") != std::string::npos) {
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    }
    else if (filename.find(".xls") != std::string::npos || filename.find(".xlsx") != std::string::npos) {
        return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    }
    else if (filename.find(".ppt") != std::string::npos || filename.find(".pptx") != std::string::npos) {
        return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    }
    else if (filename.find(".txt") != std::string::npos) {
        return "text/plain";
    }
    else if (filename.find(".csv") != std::string::npos) {
        return "text/csv";
    }
    else if (filename.find(".json") != std::string::npos) {
        return "application/json";
    }
    else if (filename.find(".xml") != std::string::npos) {
        return "application/xml";

        // Compressed files
    }
    else if (filename.find(".zip") != std::string::npos) {
        return "application/zip";
    }
    else if (filename.find(".tar") != std::string::npos) {
        return "application/x-tar";
    }
    else if (filename.find(".gz") != std::string::npos) {
        return "application/gzip";
    }
    else if (filename.find(".rar") != std::string::npos) {
        return "application/vnd.rar";
    }
    else if (filename.find(".7z") != std::string::npos) {
        return "application/x-7z-compressed";

        // Code files
    }
    else if (filename.find(".html") != std::string::npos || filename.find(".htm") != std::string::npos) {
        return "text/html";
    }
    else if (filename.find(".css") != std::string::npos) {
        return "text/css";
    }
    else if (filename.find(".js") != std::string::npos) {
        return "application/javascript";
    }
    else if (filename.find(".cpp") != std::string::npos || filename.find(".hpp") != std::string::npos) {
        return "text/x-c";
    }
    else if (filename.find(".py") != std::string::npos) {
        return "text/x-python";
    }
    else if (filename.find(".java") != std::string::npos) {
        return "text/x-java-source";
    }
    else if (filename.find(".json") != std::string::npos) {
        return "application/json";

        // Default
    }
    else {
        return "application/octet-stream";  // generic binary stream
    }
}



// Links
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool openLink(const std::string& url) {
#if defined(_WIN32) || defined(_WIN64)
    // Windows
    std::string command = "start " + url;
#elif defined(__linux__)
    // Linux
    std::string command = "xdg-open " + url;
#elif defined(__APPLE__)
    // macOS
    std::string command = "open " + url;
#else
    // Unsupported platform
    std::cerr << "Unsupported platform!" << std::endl;
    return false;
#endif

    // Execute the system command to open the URL
    system(command.c_str());
    return true;
}




// String 
void replaceDelimitersWithSpace(std::string& str) {
    std::unordered_set<char> delimiters = { ',', ';', ':', ' ', '\t' };
    for (size_t i = 0; i < str.length(); ++i) {
        if (delimiters.find(str[i]) != delimiters.end()) {
            str[i] = ' '; // Replace the delimiter with a space
        }
    }

    // Optional: Trim extra spaces
    str.erase(std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
        }), str.end()); // Remove duplicate spaces

    if (!str.empty() && str.back() == ' ') {
        str.pop_back(); // Remove trailing space
    }
}

bool containsString(const std::vector<std::string>& vec, const std::string& str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

std::string extractEmail(const std::string& input) {
    std::regex emailRegex(R"(([\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,}))"); // Simple email regex
    std::smatch match;

    if (std::regex_search(input, match, emailRegex)) {
        return match[0]; // Return the first match (email address)
    }
    return ""; // Return empty string if no email is found
}



// Encode & Decode base64
std::string base64UrlDecode(const std::string& input) {
    // Copy input and replace Base64URL characters with Base64 equivalents
    std::string base64Url = input;
    std::replace(base64Url.begin(), base64Url.end(), '-', '+');
    std::replace(base64Url.begin(), base64Url.end(), '_', '/');

    // Add padding if necessary
    while (base64Url.size() % 4 != 0) {
        base64Url += '=';
    }

    // Base64 decoding table
    const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<int> base64Index(256, -1);
    for (int i = 0; i < 64; ++i) {
        base64Index[base64Chars[i]] = i;
    }

    // Decode Base64
    std::string decoded;
    int val = 0, bits = -8;
    for (const unsigned char& c : base64Url) {
        if (base64Index[c] == -1) break; // Ignore padding
        val = (val << 6) + base64Index[c];
        bits += 6;

        if (bits >= 0) {
            decoded.push_back(static_cast<char>((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return decoded;
}
std::string base64UrlEncode(const std::string& input) {
    static const char* base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string encoded;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (encoded.size() % 4) encoded.push_back('=');

    // Convert Base64 to Base64 URL-safe
    for (char& c : encoded) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }

    // Remove padding
    while (!encoded.empty() && encoded.back() == '=') {
        encoded.pop_back();
    }

    return encoded;
}
std::string base64_decode(const std::string& encoded) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    // Build a decoding lookup table for quick reference
    std::vector<int> decodingTable(256, -1);
    for (int i = 0; i < 64; ++i) {
        decodingTable[base64_chars[i]] = i;
    }

    std::string decoded;
    int val = 0;
    int bits = -8;

    for (const unsigned char& c : encoded) {
        // Ignore padding and invalid characters
        if (decodingTable[c] == -1) break;

        val = (val << 6) + decodingTable[c];
        bits += 6;

        if (bits >= 0) {
            decoded.push_back(static_cast<char>((val >> bits) & 0xFF));
            bits -= 8;
        }
    }

    return decoded;
}
std::string base64_encode(const std::string& input) {
    static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string encoded;
    int val = 0;
    int bits = -6;

    for (unsigned char c : input) {
        val = (val << 8) + c;  // Shift left 8 bits and add the character
        bits += 8;

        while (bits >= 0) {
            encoded.push_back(base64_chars[(val >> bits) & 0x3F]);
            bits -= 6;
        }
    }

    if (bits > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (bits + 8)) & 0x3F]);
    }

    while (encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

