#include "Utils.h"
#include "API.h"
#include "Client.h"
using json = nlohmann::json; // Using json type

void checkAndReply();

int main() {
	// Load data
    if (!loadTokens() || !testAccessToken())
        return -1;
	printClientToken();

    checkAndReply();
	return 0;
}


// Main function to continuously check for unread emails and spawn threads to reply
void checkAndReply() {
    bool running = true;
    while (running) {

        // Get mail content
        std::vector<json> emails;
        getLabeledMail(emails);

        for (const json& content : emails) {
            if (!checkMailContent(content)) {
                running = false;
                continue;
            }
            //else {
            //    // Send to socket
            //    sendClientData(content);

            //    // Get reply from socket
            //    json reply = json();
            //    getClientData(reply);

            //    // Send reply to mail
            //    sendClientReply(reply["sender"],reply["command"],
            //                    reply["content"],reply["filename"]);
            //}
        }

        if (!running) {
            std::cout << "Turn off connection!\n";
        }
        else {
            std::cout << "Checked for new emails. Waiting for 10 seconds before next check..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10)); // Check every 60 seconds
        }
    }
}