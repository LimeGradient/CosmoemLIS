#include "UI/Window.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

int main() {
    Window* win = Window::get();
    win->init();
}