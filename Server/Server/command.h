#pragma once

// Con tro ham


using FuncPtr = bool(*)(const std::string& input, std::string& filename);

FuncPtr getFuncPtr(const std::string& cmd);
			// Get command pointer



// ~~~~~~~~~~Command functions~~~~~~~~~~~~~~~~~~~~

// List App
bool ListApp(const std::string& input, std::string& filename);

// Start App
bool startApp(const std::string& input, std::string& filename);

// Stop App
bool stopApp(const std::string& input, std::string& filename);

// List Service
bool ListService(const std::string& input, std::string& filename);

// Start Service
bool startService(const std::string& input, std::string& filename);

// Stop Service
bool stopService(const std::string& input, std::string& filename);

// Shutdown
bool ShutdownServer(const std::string& input, std::string& filename);

// Reset
bool RestartServer(const std::string& input, std::string& filename);

// Copy file
bool copyFile(const std::string& input, std::string& filename);

// Delete File
bool deleteFile(const std::string& input, std::string& filename);

// Capture Screen
bool captureScreen(const std::string& input, std::string& filename);

// Start/Stop Webcam
bool webCam(const std::string& input, std::string& filename);

// Keylogger
bool Keylogger(const std::string& input, std::string& filename);

// Lock keyboard
bool lockKeyboard(const std::string& input, std::string& filename);

