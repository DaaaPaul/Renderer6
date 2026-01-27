#include <iostream>
#include "VulkanPFNs.h"
#include "GlfwWindowWrapper.hpp"
#include "Common.h"

GlfwWindowWrapper::GlfwWindowWrapper(uint16_t const& GIVEN_WIDTH, uint16_t const& GIVEN_HEIGHT, std::string const& GIVEN_NAME) : 
    mGlfwWindow{},
    mWindowWidth{GIVEN_WIDTH},
    mWindowHeight{GIVEN_HEIGHT},
    mWINDOW_NAME{GIVEN_NAME} {
    std::cout << "SET WINDOW CREATE PARAMETERS:\n"
        "\t-WINDOW_WIDTH " << mWindowWidth << "\n"
        "\t-WINDOW_HEIGHT " << mWindowHeight << "\n"
        "\t-WINDOW_NAME " << mWINDOW_NAME << "\n";

    arise();
}

GlfwWindowWrapper::~GlfwWindowWrapper() {
    std::cout << "Destroying GlfwWindowWrapper...\n";

    glfwDestroyWindow(mGlfwWindow);
    glfwTerminate();

    std::cout << "Destroyed GlfwWindowWrapper\n";
}

void GlfwWindowWrapper::arise() {
    std::cout << "Creating GlfwWindowWrapper...\n";

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mGlfwWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, mWINDOW_NAME.c_str(), nullptr, nullptr);

    CHECK_NULLPTR(mGlfwWindow);

    std::cout << "Created GlfwWindowWrapper\n";
}