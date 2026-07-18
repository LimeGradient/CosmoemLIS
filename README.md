# CosmoemLIS
Make choices in Life is Strange with your friends.  
This project is **heavily WIP**.

### Supported Games:
- [ ] Life is Strange
- [ ] Life is Strange: Before the Storm
- [ ] Life is Strange 2
- [ ] Life is Strange: True Colors
- [ ] Life is Strange Remastered
- [X] Life is Strange: Before the Storm Remastered
- [ ] Life is Strange: Double Exposure
- [ ] Life is Strange: Reunion

## How to Use
Guide on how to use this!
### Hosting:
1. Download CosmoemLISHost.dll from the latest release.
2. Open your (supported) Life is Strange game
3. Using a program to inject DLL's into your program (Game launcher is WIP) inject the Host DLL into your game

## How to Build
1. Clone the Repository: `git clone https://github.com/LimeGradient/CosmoemLIS && cd CosmoemLIS`
2. Build the project with CMake:
    * Client and Server (Windows Only): `cmake -S . -B build`
    * Client only (Crossplatform): `cmake -S . -B build -DLIS_BUILD_SERVER=OFF`
    * Server only (Windows Only): `cmake -S . -B build -DLIS_BUILD_CLIENT=OFF`
3. Run the game and inject the DLL