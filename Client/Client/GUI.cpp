#include "Utils.h"
#include "GUI.h"
#include "API.h"
#include "Client.h"

// Main function to continuously check for unread emails and spawn threads to reply
bool sentMailToServer(SOCKET& clientSocket, std::vector<json> mails) {
    bool check = true;
    while (!mails.empty()) {
        json mail = mails.back();
        mails.pop_back();

        // 1. Check mail valid
        std::cout << "Send mail:" << mail.dump() << std::endl;
        if (!checkValidMails(mail)) {
            check = false;
            std::cout << "Shuting off!\n";
            continue;
        }

        // 2. Sent to server
        if (!sendData(clientSocket, mail)) {
            std::cout << "Lost connection!\n";
            return false;
        }

        // 3. Wait for responds

        // 4. Get responds
        json respond;
        if (!getData(clientSocket, respond)) {
            std::cout << "Lost connection!\n";
            return false;
        }

        // 5. Sent email
        std::cout << "Get respond:" << respond.dump() << std::endl;

        // Send reply to mail
        sendMail(respond);

        deleteFile(respond["filename"].get<std::string>());

    }
    // Send signal to disconnect
    //json signal = json();
    //sendData(clientSocket, signal);
    return check;
}

bool connectToServer(std::vector<json> mails) {
    // 0. Check 
    SOCKET clientSocket;

    // 1. Set up socket
    createSocket(clientSocket);

    // 2. sent data + get data
    bool check = sentMailToServer(clientSocket, mails);

    // 3. Close socket
    releaseSocket(clientSocket);

    return check;
}

void checkAndReply() {
    bool running = true;
    while (running) {

        // Get mail content
        std::vector<json> emails;
        getLabeledMail(emails);
        if (emails.size() > 0) running = connectToServer(emails);

        if (!running) {
            std::cout << "Turn off connection!\n";
        }
        else {
            std::cout << "Checked for new emails. Waiting for 10 seconds before next check..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10)); // Check every 60 seconds
        }
    }
}