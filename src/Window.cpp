#include <iostream>
#include "VulkanPFNs.h"
#include "Window.hpp"
#include "Common.h"

Window::Window(uint16_t const &GIVEN_WIDTH, uint16_t const &GIVEN_HEIGHT, std::string const &GIVEN_NAME) : 
    window{},
    WINDOW_WIDTH{GIVEN_WIDTH},
    WINDOW_HEIGHT{GIVEN_HEIGHT},
    WINDOW_NAME{GIVEN_NAME} {
    std::cout << "SET WINDOW CREATE PARAMETERS:\n"
        "\t-WINDOW_WIDTH " << WINDOW_WIDTH << "\n"
        "\t-WINDOW_HEIGHT " << WINDOW_HEIGHT << "\n"
        "\t-WINDOW_NAME " << WINDOW_NAME << "\n";

    glfwInitVulkanLoader(VulkanPFNs::gpVkGetInstanceProcAddr);

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

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME.c_str(), nullptr, nullptr);

    CHECK_NULLPTR(window);

    std::cout << "Created window\n";
}