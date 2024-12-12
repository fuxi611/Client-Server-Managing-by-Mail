#pragma once

// Internal Libraries
#include <iostream>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <regex>
#include <stack>
#include <queue>
#include <ctime>
#include <cstdio>
#include <filesystem>

//#include <string>
//#include <vector>
//#include <array>
//#include <map>

//#include <algorithm>
//#include <iterator>
//#include <iomanip>
//#include <chrono>

// External Libraries
#include <nlohmann/json.hpp>


using json = nlohmann::json; // Using json type


// File stream
bool saveStringToFile(const std::string& text,const std::string& filename);
					// Save string to file

bool loadStringFromFile(std::string& content, const std::string& filename);
					// Load string from file

bool loadFileJSON(const std::string& filename, json& config);
					// Load json file

std::string getMimeType(const std::string& filename);
					// Determine MIME type based on file extension												 
													 
													 
													

// Links
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
					// Use in CURL

bool openLink(const std::string& url);				
					// Open given link

bool checkInternet();
					// Check internet connection



// String 
void replaceDelimitersWithSpace(std::string& str);  
					// Use in stringstream

bool containsString(const std::vector<std::string>& vec, const std::string& str);
					// Check Vector contain given string

std::string extractEmail(const std::string& input);
					// Function to extract email address from a string

bool checkIPAddress(std::string& ip);				
					// Check ip address

bool deleteFile(const std::string& filePath);
					// Function to delete a file by its address

std::string getFilename(const std::string& filePath);
					// Get filename from address


// Encode & Decode base64
std::string base64UrlDecode(const std::string& input);
					// 
std::string base64UrlEncode(const std::string& input);
					// 
std::string base64_decode(const std::string& encoded);
					//
std::string base64_encode(const std::string& input);
					//


