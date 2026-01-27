#include <iostream>
#include "VulkanPFNs.h"
#include "Window.hpp"
#include "Common.h"

Window::Window(uint16_t const& GIVEN_WIDTH, uint16_t const& GIVEN_HEIGHT, std::string const& GIVEN_NAME) : 
    glfwWindow{},
    windowWidth{GIVEN_WIDTH},
    windowHeight{GIVEN_HEIGHT},
    WINDOW_NAME{GIVEN_NAME} {
    std::cout << "SET WINDOW CREATE PARAMETERS:\n"
        "\t-WINDOW_WIDTH " << windowWidth << "\n"
        "\t-WINDOW_HEIGHT " << windowHeight << "\n"
        "\t-WINDOW_NAME " << WINDOW_NAME << "\n";

    arise();
}

Window::~Window() {
    std::cout << "Destroying window...\n";

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    std::cout << "Destroyed window\n";
}

void Window::arise() {
    std::cout << "Creating window...\n";

    glfwInitVulkanLoader(VulkanPFNs::gpVkGetInstanceProcAddr);
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, WINDOW_NAME.c_str(), nullptr, nullptr);

    CHECK_NULLPTR(glfwWindow);

    std::cout << "Created window\n";
}