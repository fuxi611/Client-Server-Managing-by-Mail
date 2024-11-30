#include "Utils.h"
#include "Source.h"
#include "API.h"
#include "Client.h"
#include "GUI.h"


int main() {
    // 0. Check Internet connection
    if(!checkInternet()) return -1;

    // 1. Check and connect to local server
    if (!checkServerOnline()) return -1;

    // 2. Check and connect to online Gmail server
    if (!loadTokens() || !testAccessToken()) return -1;
    // printClientToken();

    // 3. Get mail from Gmail
    // 4. Extract data from mail
    // 5. Sent data to server and wait for respond one-by-one
    // 6. Sent respond email to gmail
    // 7. Repeat step 3
    checkAndReply();
    
    std::cout << "Client closed!" << std::endl;
	return 0;
}



