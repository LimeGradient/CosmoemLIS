#include <ixwebsocket/IXNetSystem.h>

#include "Logging.hpp"
#include "ui/GameCaptureWindow.hpp"

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    
    ix::initNetSystem();

    Cosmoem::GameCaptureWindow* gcw = new Cosmoem::GameCaptureWindow();
    gcw->init();

    return 0;
}