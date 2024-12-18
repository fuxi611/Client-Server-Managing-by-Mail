# HELP for Window dev

## Download resources
1. Project
- Pull from GitHub using GitHub Desktop
- Choose valid folder on local devices
- Open using Visual Studio 2022
  
1. External Libraries
- Pull vcpgk for C++ package management using github
- Open Powercell on Window as admin
- Change directory to vcpgk folder: "cd [directory]"
- Using "./vcpgk" to enter command
- Ex: "./vcpgk install [library]"
- List of lib: curl, nlohmann, json, opencv,
- Type "./vcpgk integrate install" into command line to setup vcpgk for visual studio
  
1. Access Google API for Client
a) Go to the Google Cloud Console: https://console.cloud.google.com/
- Ask admin to create and give "credential.json" to you (Manually)
- Put "credential.json" into client folder, same directory as .cpp files

b) Open the Server
- Press run
- Look for IP of server and copy it

c) Open the client
- Press run
- Terminal will as for IP of server, pass to it
- Terminal will tempt to check for Token for the first time
  + The first time will make you to obtain token by giving you a link, copy it and go to that website to get token
  + If you already have token then the code will continue
- 
