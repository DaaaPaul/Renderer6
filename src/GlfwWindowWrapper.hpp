#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include "Common.h"

class VulkanBackendWrapper;

class GlfwWindowWrapper {
    friend class VulkanBackendWrapper;

    private:
    GLFWwindow* mGlfwWindow{};

    uint16_t mWindowWidth{};
    uint16_t mWindowHeight{};
    const std::string mWINDOW_NAME{};

    void arise();

    public:
    GlfwWindowWrapper(uint16_t const& GIVEN_WIDTH, uint16_t const& GIVEN_HEIGHT, std::string const& GIVEN_NAME);
    ~GlfwWindowWrapper();

    DELETE_COPY_CONSTRUCTORS(GlfwWindowWrapper)
    DELETE_MOVE_CONSTRUCTORS(GlfwWindowWrapper)
};