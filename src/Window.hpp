#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include "Common.h"

class Window {
private:
    GLFWwindow *window;

    const uint16_t WINDOW_WIDTH;
    const uint16_t WINDOW_HEIGHT;
    const std::string WINDOW_NAME;

    void arise();

public:
    Window(uint16_t const &GIVEN_WIDTH, uint16_t const &GIVEN_HEIGHT, std::string const &GIVEN_NAME);
    ~Window();

    DELETE_COPY_CONSTRUCTORS(Window)
    DELETE_MOVE_CONSTRUCTORS(Window)
};