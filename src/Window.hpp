#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include "Common.h"
#include "VulkanBackend.hpp"

class Window {
private:
    friend class VulkanBackend;

    GLFWwindow* glfwWindow;

    uint16_t windowWidth;
    uint16_t windowHeight;
    const std::string WINDOW_NAME;

    void arise();

public:
    Window(uint16_t const& GIVEN_WIDTH, uint16_t const& GIVEN_HEIGHT, std::string const& GIVEN_NAME);
    ~Window();

    DELETE_COPY_CONSTRUCTORS(Window)
    DELETE_MOVE_CONSTRUCTORS(Window)
};