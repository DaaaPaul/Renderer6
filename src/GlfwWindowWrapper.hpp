#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include "Common.h"

class VulkanBackendWrapper;

class GlfwWindowWrapper {
private:
    friend class VulkanBackendWrapper;

    GLFWwindow* glfwWindow;

    uint16_t windowWidth;
    uint16_t windowHeight;
    const std::string WINDOW_NAME;

    void arise();

public:
    GlfwWindowWrapper(uint16_t const& GIVEN_WIDTH, uint16_t const& GIVEN_HEIGHT, std::string const& GIVEN_NAME);
    ~GlfwWindowWrapper();

    DELETE_COPY_CONSTRUCTORS(GlfwWindowWrapper)
    DELETE_MOVE_CONSTRUCTORS(GlfwWindowWrapper)
};