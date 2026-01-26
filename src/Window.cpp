#include <iostream>
#include "Window.hpp"
#include "Common.h"

Window::Window(uint16_t const &GIVEN_WIDTH, uint16_t const &GIVEN_HEIGHT, std::string const &GIVEN_NAME) : 
    WINDOW_WIDTH{GIVEN_WIDTH},
    WINDOW_HEIGHT{GIVEN_HEIGHT},
    WINDOW_NAME{GIVEN_NAME} {
    std::cout << "SET WINDOW CREATE PARAMETERS:\n"
        "\tWINDOW_WIDTH " << WINDOW_WIDTH << "\n"
        "\tWINDOW_HEIGHT " << WINDOW_HEIGHT << "\n"
        "\tWINDOW_NAME " << WINDOW_NAME << "\n\n";

    arise();
}

Window::~Window() {
    std::cout << "Destroying window...\n";

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Destroyed window\n";
}

void Window::arise() {
    std::cout << "Creating window...\n";

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME.c_str(), nullptr, nullptr);

    CHECK_NULLPTR(window);

    std::cout << "Created window\n";
}