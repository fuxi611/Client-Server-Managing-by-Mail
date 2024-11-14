#pragma once

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <stdexcept>
#include <thread>


// Load data from saved data ~~~~~~~~~~~~~~~~~~~~~~~~~
bool loadClient();					// Load Client data from file

bool loadTokens();					// Load both Tokens from file

bool testAccessToken();				// Test 


// Get data from Google API ~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string getAuthorizationUrl();	// Open this link to get authorizationCode

std::string getResponse(const std::string& authorizationCode);
									// Get response from Google API

bool checkTokenResponse(const std::string& response);
									// Check error of the response

bool getTokenFromURLs();			// Get and Save Refresh TOKEN

bool getAccessToken();				// Get new Access Token from Refress Token





// Show data to console ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printClientToken();			// Print Client Data



// Get email content 
// bool markEmailAsRead(const std::string& messageId);
									// Mark read mail

// bool getMailContent(const std::string& messageId, json& mailInfo);
									// get info of a mail
									
// bool getMailList(const std::string URLS, std::vector<nlohmann::json>& jsonResponseVector);
									// Get mails vector from URLS

// bool extractData(const json raw, json& extracted);
									// Extract importance data

bool getLabeledMail(std::vector<nlohmann::json>& emailsContent);
									// Print Mails




// Sent email content
bool checkMailContent(const nlohmann::json& content);


bool sendClientReply(const std::string& sender, const std::string& subject,
	const std::string& bodyText, const std::string& filename);
									// Sent client reply to user
