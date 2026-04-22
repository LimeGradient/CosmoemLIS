# Life is Strange: Multiplayer
Make choices in Life is Strange with your friends.

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
1. Clone the Repository: `git clone https://github.com/LimeGradient/LISMultiplayer && cd LISMultiplayer`
2. Build the project with CMake:
    * Client and Server (Windows Only): `cmake -S . -B build`
    * Client only (Crossplatform): `cmake -S . -B build -DLIS_BUILD_SERVER=OFF`
    * Server only (Windows Only): `cmake -S . -B build -DLIS_BUILD_CLIENT=OFF`
3. Run the game and inject the DLL