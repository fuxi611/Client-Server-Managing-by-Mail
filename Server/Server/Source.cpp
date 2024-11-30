#include "Utils.h"
#include "Server.h"
#include "GUI.h"


// Setup server to open for connection from server
// Listen for client request for connection
// While connected to one client, will not open for other client
//  
// Manage Socket Client-Server
// Create Connection between Client-Server
// Sent data to Server
// Get data from Server
//
// Close connection after client finish connecting
// Shutdown server


int main() {
    // 1. Load data
    printServerIP();
    loadData();

    // 2. Create Socket
    // 3. Pending 
    // 4. Connect to a client
    // 5. Command 
    // 6. Disconnect with that client
    // 7. Go to step 3 or stop
    runServer();
    

    return EXIT_SUCCESS;
}

//
//int main(int argc, char* argv[]) {
//    QApplication app(argc, argv);
//
//    QPushButton button("Hello, World!");
//    button.resize(200, 100);
//    button.show();
//
//    return app.exec();
//}