#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdint>
#include "Common.h"

class GlfwWindowWrapper {
    friend class VulkanBackendWrapper;
    friend class VulkanSwapchainWrapper;

    struct GlfwWindowWrapperParameters {
        const uint16_t mWIDTH{};
        const uint16_t mHEIGHT{};
        const std::string mNAME{};
    };

    private:
    GLFWwindow* mGlfwWindow{};
    const GlfwWindowWrapperParameters mPARAMETERS{};

    public:
    explicit GlfwWindowWrapper(GlfwWindowWrapperParameters const& GIVEN_PARAMETERS);
    ~GlfwWindowWrapper();

    constexpr static GlfwWindowWrapperParameters getConstructParameters() noexcept { return GlfwWindowWrapperParameters(800, 600, "Renderer6"); }
    [[nodiscard]] static std::vector<const char*> getGlfwWindowExtensions();
    [[nodiscard]] inline GLFWwindow* getGlfwWindow() noexcept { return mGlfwWindow; }

    DELETE_COPY_CONSTRUCTORS(GlfwWindowWrapper)
    DELETE_MOVE_CONSTRUCTORS(GlfwWindowWrapper)
};