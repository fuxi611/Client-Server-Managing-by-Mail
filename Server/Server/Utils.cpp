#include "Utils.h"


// File stream
bool saveStringToFile(const std::string& text, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);  // Open the file for writing in binary mode

    if (outFile.is_open()) {  // Check if the file was opened successfully
        outFile.write(text.c_str(), text.size());  // Write the raw binary data from string
        outFile.close();  // Close the file after writing
    }
    else {
        std::cerr << "Failed to open file: " << filename << std::endl;  // Error handling if the file can't be opened
        return false;
    }
    return true;
}

bool loadStringFromFile(std::string& content, const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);  // Open the file for reading in binary mode

    if (inFile.is_open()) {  // Check if the file was opened successfully
        // Get the file size and reserve space in the string
        inFile.seekg(0, std::ios::end);
        size_t size = inFile.tellg();
        inFile.seekg(0, std::ios::beg);
        content.resize(size);

        // Read the binary content into the string
        inFile.read(&content[0], size);
        inFile.close();  // Close the file after reading
        return true;
    }
    else {
        std::cerr << "Failed to open file: " << filename << std::endl;  // Error handling if the file can't be opened
        return false;
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



// Links
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

bool checkInternet() {
    int ret = std::system("ping -n 1 8.8.8.8 > nul"); // For Windows
    bool connect = (ret == 0);

    if (!connect) {
        std::cout << "Internet Unavailable...Can not connect to server";
        return false;
    }
    else {
        std::cout << "Internet Available...";
        return true;
    }
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

bool checkIPAddress(std::string& ip) {
    // Regular expression for a valid IPv4 address
    const std::regex ipRegex(
        R"(^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$)"
    );
    return std::regex_match(ip, ipRegex);
}

bool getWordsFromString(std::vector<std::string>& vec, std::string& str)
{
    replaceDelimitersWithSpace(str);
    std::stringstream ss(str);
    std::string token;
    while (ss >> token) {
        vec.push_back(token);
    }

    return true;
}






