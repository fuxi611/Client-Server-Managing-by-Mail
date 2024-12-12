#pragma once

// Con tro ham

using FuncPtr = bool(*)(const std::string& input, std::string& filename);

FuncPtr getFuncPtr(const std::string& cmd);
			// Get command pointer



// ~~~~~~~~~~Command functions

// List App

// Start App

// Stop App

// List Service
bool taskList(const std::string& input, std::string& filename);

// Start Service

// Stop Service

// Shutdown

// Reset

// Copy file

// Delete File

// Capture Screen
bool captureScreen(const std::string& input, std::string& filename);

// Start/Stop Webcam

// Keylogger

// Lock keyboard


