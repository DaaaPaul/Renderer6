#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdint>
#include "Common.h"

struct GlfwWindowWrapper {
    struct GlfwWindowWrapperParameters {
        const uint16_t mWIDTH{};
        const uint16_t mHEIGHT{};
        const char* mNAME{};
    };

    GLFWwindow* mpGlfwWindow{};
	bool mFrameBufferResizedAlert{};
    const GlfwWindowWrapperParameters mPARAMETERS{};

	static void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height);

    constexpr static GlfwWindowWrapperParameters getConstructParameters() noexcept { return GlfwWindowWrapperParameters(800, 600, "Renderer6"); }
    [[nodiscard]] static std::vector<const char*> getGlfwWindowExtensions();

    explicit GlfwWindowWrapper(GlfwWindowWrapperParameters const& GIVEN_PARAMETERS);
    ~GlfwWindowWrapper();

    DELETE_COPY_CONSTRUCTORS(GlfwWindowWrapper)
    DELETE_MOVE_CONSTRUCTORS(GlfwWindowWrapper)
};