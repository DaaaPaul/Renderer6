#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include "Common.h"

class GlfwWindowWrapper {
    friend class VulkanBackendWrapper;
    friend class VulkanSwapchainWrapper;

    private:
    GLFWwindow* mGlfwWindow{};

    uint16_t mWindowWidth{};
    uint16_t mWindowHeight{};
    const std::string mWINDOW_NAME{};

    void arise();

    public:
    GlfwWindowWrapper(uint16_t const& GIVEN_WIDTH, uint16_t const& GIVEN_HEIGHT, std::string const& GIVEN_NAME);
    ~GlfwWindowWrapper();

    [[nodiscard]] inline GLFWwindow* getGlfwWindow() noexcept { return mGlfwWindow; }

    DELETE_COPY_CONSTRUCTORS(GlfwWindowWrapper)
    DELETE_MOVE_CONSTRUCTORS(GlfwWindowWrapper)
};