#include "Window.hpp"
#include "Common.h"

Window::Window(uint16_t const &GIVEN_WIDTH, uint16_t const &GIVEN_HEIGHT, std::string const &GIVEN_NAME) : 
    WINDOW_WIDTH{GIVEN_WIDTH},
    WINDOW_HEIGHT{GIVEN_HEIGHT},
    WINDOW_NAME{GIVEN_NAME} {
    arise();
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::arise() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME.c_str(), nullptr, nullptr);

    CHECK_NULLPTR(window);
}