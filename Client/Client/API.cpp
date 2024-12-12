#include "API.h" // It's header file
#include "Utils.h" // Utils function




// Set Up OAuth 2.0 Credentials
std::string CLIENT_ID = "";
std::string CLIENT_SECRET = "";
const std::string CILENT_FILE = "credentials.json";
const std::string REDIRECT_URI = "urn:ietf:wg:oauth:2.0:oob";
const std::string TOKEN_URL = "https://oauth2.googleapis.com/token";
const std::string SCOPE_URL = "https://www.googleapis.com/auth/gmail.modify";

// Get TOKEN
std::string ACCESS_TOKEN = "";
std::string REFRESH_TOKEN = "";
const std::string REFRESH_TOKEN_FILE = "refresh_token.txt";
const std::string URLS_PREFIX = "https://www.googleapis.com/gmail/v1/users/me";

// Mail data
const std::string LABEL_NAME = "COMMAND";
const std::string USER_FILE = "data.json";
std::string TEMP_FOLDER = "temp/";




// Get API here
// Load data
// Get Mail content
// Sent Mail content
// Extract Data from mail
// Sent data to Socket




// Load data from saved data ~~~~~~~~~~~~~~~~~~~~~~~~~
bool loadClient() {
    json config = json();
    if (!loadFileJSON(CILENT_FILE, config)) {
        return false;
    }

    try {
        // Accessing the nested "installed" object to get client_id and client_secret
        CLIENT_ID = config.at("installed").at("client_id").get<std::string>();
        CLIENT_SECRET = config.at("installed").at("client_secret").get<std::string>();
    }
    catch (const json::exception& e) {
        std::cerr << "Error accessing JSON keys: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool loadTokens() {
    if(!loadClient())
        return false;
    loadStringFromFile(REFRESH_TOKEN, REFRESH_TOKEN_FILE);
    if (!getAccessToken()) {
        getTokenFromURLs();
    }
    return true;
}

bool testAccessToken() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return false;
    }

    std::string readBuffer;
    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/profile";
    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(headers, ("Authorization: Bearer " + ACCESS_TOKEN).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    if (httpCode == 200) {
        try {
            auto jsonResponse = nlohmann::json::parse(readBuffer);
            std::cout << "Access token is valid. User ID: " << jsonResponse["emailAddress"] << std::endl;
            return true;
        }
        catch (const nlohmann::json::parse_error& e) {
            std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Access token is invalid or expired. HTTP Code: " << httpCode << std::endl;
        return false;
    }
}



// Get data from Google API ~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string getAuthorizationUrl() {
    std::string auth_url = "https://accounts.google.com/o/oauth2/v2/auth?"
        "client_id=" + CLIENT_ID +
        "&redirect_uri=" + REDIRECT_URI +
        "&response_type=code" +  // response_type is required
        "&scope=https://www.googleapis.com/auth/gmail.modify";
    std::cout << "Copy and paste this url to web\n";
    return auth_url;
}

std::string getResponse(const std::string& authorizationCode) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, TOKEN_URL.c_str());

        std::string postFields = "code=" + authorizationCode +
            "&client_id=" + CLIENT_ID +
            "&client_secret=" + CLIENT_SECRET +
            "&redirect_uri=" + REDIRECT_URI +
            "&grant_type=authorization_code";

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }
    }
    return response;
}

bool checkTokenResponse(const std::string& response) {
    if (response.empty()) {
        std::cerr << "Error: No response from server or failed to exchange authorization code for tokens!" << std::endl;
        return false; // Exit or handle error appropriately
    }
    try {
        // Parse the JSON response
        auto jsonResponse = nlohmann::json::parse(response);

        // Check if there's an error field in the response
        if (jsonResponse.contains("error")) {
            std::cerr << "Error: " << jsonResponse["error"] << std::endl;
            if (jsonResponse.contains("error_description")) {
                std::cerr << "Description: " << jsonResponse["error_description"] << std::endl;
            }
            return false; // Indicate that there was an error
        }

        // You can add additional checks here if needed

        return true; // Indicate success
    }
    catch (nlohmann::json::parse_error& e) {
        std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
        return false; // Indicate parsing error
    }
}

bool getTokenFromURLs() {
    std::string authURL = getAuthorizationUrl();
    std::cout << "Open the following URL in your browser to authorize:\n" << authURL << std::endl;

    std::string authorizationCode;
    openLink(authURL);
    std::cout << "Enter the authorization code: ";
    std::cin >> authorizationCode;

    std::string response = getResponse(authorizationCode);
    if (!checkTokenResponse(response)) {
        std::cerr << "Failed to handle token response!" << std::endl;
        return false; // Exit or handle error appropriately
    }

    json jsonResponse = json::parse(response);
    REFRESH_TOKEN = jsonResponse["refresh_token"].get<std::string>();
    ACCESS_TOKEN = jsonResponse["access_token"].get<std::string>();

    saveStringToFile(REFRESH_TOKEN, REFRESH_TOKEN_FILE);
    return true;
}

bool getAccessToken() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // OAuth 2.0 token endpoint
    std::string url = "https://oauth2.googleapis.com/token";

    // Prepare the POST data (Form URL Encoded)
    std::string postData = "client_id=" + CLIENT_ID +
        "&client_secret=" + CLIENT_SECRET +
        "&refresh_token=" + REFRESH_TOKEN +
        "&grant_type=refresh_token";

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Curl initialization failed!" << std::endl;
        return false;
    }

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return false;
    }

    // Parse the JSON response
    try {
        json response = json::parse(readBuffer);
        if (response.contains("access_token")) {
            ACCESS_TOKEN = response["access_token"];
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return true;  // Successfully retrieved the access token
        }
        else {
            std::cerr << "Error: " << response.dump() << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;  // Error in response (no access_token)
        }
    }
    catch (const json::exception& e) {
        std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return false;  // JSON parsing error
    }
}



// Show data to console ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printClientToken(){
    std::cout << "CLIENT_ID: " << CLIENT_ID << "\n";
    std::cout << "CLIENT_SECRET: " << CLIENT_SECRET << "\n";
    std::cout << "ACCESS_TOKEN: " << ACCESS_TOKEN << "\n";
    std::cout << "REFRESH_TOKEN: " << REFRESH_TOKEN << "\n";
}



// Get email content
bool markEmailAsRead(const std::string& messageId) {
    // Set up cURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return false;
    }

    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId + "/modify";
    std::string readBuffer;

    // Create JSON payload to remove the UNREAD label
    json requestBody = {
        {"removeLabelIds", {"UNREAD"}}
    };

    // Convert JSON payload to string
    std::string payload = requestBody.dump();

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + ACCESS_TOKEN).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    bool success = true;
    if (res != CURLE_OK) {
        std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        success = false;
    }
    else {
        std::cout << "Email marked as read successfully" << std::endl;
    }

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return success;
}

bool extractData(const json rawData, json& extractData) {
    // Extract headers if available
    if (rawData["payload"].contains("headers")) {
        const auto& headers = rawData["payload"]["headers"];

        for (const auto& header : headers) {
            const std::string& name = header["name"];
            const std::string& value = header["value"];

            if (name == "From") {
                extractData["sender"] = extractEmail(value); // Extract only email
            }
            else if (name == "Subject") {
                extractData["subject"] = value;
            }
        }
    }

    // Extract the body content, considering possible different structures
    std::string content;
    if (rawData["payload"].contains("parts")) {
        // Iterate over parts to find plain text content
        for (const auto& part : rawData["payload"]["parts"]) {
            if (part["mimeType"] == "text/plain" && part["body"].contains("data")) {
                content = part["body"]["data"];
                content = base64UrlDecode(content); // Decode if base64url-encoded
                break;
            }
        }
    }
    else if (rawData["payload"]["body"].contains("data")) {
        // Single body data, decode if present
        content = rawData["payload"]["body"]["data"];
        content = base64UrlDecode(content);
    }
    extractData["content"] = content;

    // Include headers in the result for reference, if needed
    if (rawData["payload"].contains("headers")) {
        extractData["headers"] = rawData["payload"]["headers"];
    }

    // Extract data from Subject
    // Define a set of delimiters
    std::string sub = extractData["subject"];
    replaceDelimitersWithSpace(sub);

    std::istringstream ss(sub);
    // Attempt to extract the first and second words
    std::string w1, w2;
    if (ss >> w1 >> w2) {
        extractData["command"] = w1;
        extractData["ip_address"] = w2;
        std::cout << "Get command and IP" << std::endl;
        extractData.erase("subject");
        extractData.erase("headers");
    }
    else {
        std::cout << "Not enough values in the stringstream." << std::endl;
    }


    // Clear the stringstream
    ss.str("");    // Clear the contents
    ss.clear();    // Reset any error flags

    return true;
}

bool getMailContent(const std::string& messageId, json& mailInfo) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return false;
    }

    std::string readBuffer;
    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId;

    // Set the Authorization header
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + ACCESS_TOKEN).c_str());

    // Set cURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // Check for success
    if (res != CURLE_OK) {
        std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    // Check if the HTTP response code indicates success
    if (httpCode == 200) {
        try {
            auto messageJson = json::parse(readBuffer);
            mailInfo = messageJson;
            markEmailAsRead(messageId);
            return true;
        }
        catch (const json::parse_error& e) {
            std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Failed to retrieve message. HTTP Code: " << httpCode << std::endl;
        return false;
    }


}

bool getMailList(const std::string URLS, std::vector<json>& mail_array) {
    CURL* curl; CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, URLS.c_str());

        // Set the Authorization header
        struct curl_slist* headers = NULL;
        std::string authHeader = "Authorization: Bearer " + ACCESS_TOKEN;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Performing the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return -1;
        }
        else {
            std::cout << "Get data successfully" << std::endl;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize CURL." << std::endl;
        return -1; // Return -1 if CURL initialization failed
    }

    // Parse readBuffer into JSON and store in vector
    try {
        json id_list = json::parse(readBuffer);
       
        // Check if the response contains an array under a specific key, e.g., "messages"
        if (id_list.contains("messages") && id_list["messages"].is_array()) {
            std::vector<json> id_arr = id_list["messages"].get<std::vector<json>>();
            std::cout << "Parsed JSON array into vector of JSON objects successfully." << std::endl;

            for (json id_mail : id_arr) {
                json content; getMailContent(id_mail["id"], content);
                markEmailAsRead(id_mail["id"]);
                json extract; extractData(content, extract);
                mail_array.push_back(extract);
            }
        }
        else {
            std::cerr << "Expected JSON array under key 'messages' but got a different structure." << std::endl;
            return false;
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool getLabeledMail(std::vector<json>& emailsContent) {
    std::string urls = URLS_PREFIX +
        "/messages?q=is:unread+label:" + LABEL_NAME + "";
    std::cout << urls << std::endl;
    if (!getMailList(urls, emailsContent)) {
        std::cout << "Can't read" << std::endl;
        return false;
    }
    return true;
}



// Sent email content
bool sendMail(const json& reply) {
    const std::string& receiver = reply["receiver"];
    const std::string& subject = reply["subject"];
    const std::string& bodypart = reply["bodypart"];
    const std::string& filename = reply["filename"];

    // Initialize the MIME content as multipart if there's an attachment
    std::string mimeContent = "Content-Type: multipart/mixed; boundary=\"boundary\"\r\n\r\n";
    mimeContent += "--boundary\r\n";
    mimeContent += "Content-Type: text/plain; charset=\"UTF-8\"\r\n\r\n";
    mimeContent += bodypart + "\r\n\r\n";

    // If filename is provided, read and encode the file for attachment
    if (!filename.empty()) {
        std::string fileContent;
        loadStringFromFile(fileContent,filename);
        std::string encodedFile = base64_encode(fileContent);
        std::string mimeType = getMimeType(filename);

        mimeContent += "--boundary\r\n";
        mimeContent += "Content-Type: " + mimeType + "; name=\"" + filename + "\"\r\n";
        mimeContent += "Content-Transfer-Encoding: base64\r\n";
        mimeContent += "Content-Disposition: attachment; filename=\"" + filename + "\"\r\n\r\n";
        mimeContent += encodedFile + "\r\n";
    }

    mimeContent += "--boundary--";

    // Construct the raw email message with headers
    std::string rawMessage = "To: " + receiver + "\r\nSubject: " + subject + "\r\n" + mimeContent;
    std::string encodedMessage = base64UrlEncode(rawMessage);

    // Prepare the JSON payload
    nlohmann::json jsonPayload;
    jsonPayload["raw"] = encodedMessage;

    // Set up CURL to send the email
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string url = "https://www.googleapis.com/gmail/v1/users/me/messages/send";
        std::string authHeader = "Authorization: Bearer " + ACCESS_TOKEN;
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, authHeader.c_str());

        std::string jsonString = jsonPayload.dump();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        bool success = (res == CURLE_OK); // Check if the email was sent successfully

        if (!success) {
            std::cerr << "Failed to send email: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return success;
    }
    return false; // Return false if CURL could not initialize
}
