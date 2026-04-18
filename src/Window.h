#pragma once

#include <GLFW/glfw3.h>

namespace Backend {
	namespace Window {
		inline GLFWwindow* gGlfwWindow{};

		inline constexpr int gWINDOW_WIDTH = 800;
		inline constexpr int gWINDOW_HEIGHT = 600;
		inline constexpr const char* gWINDOW_TITLE = "Renderer6";
		inline bool gFramebufferResized = false;
		inline bool* gFrameBufferResizedPointer = &gFramebufferResized;

		void init();
		void deInit();

		void createGlfwWindow();
		void destroyGlfwWindow() noexcept;

		void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
	}
}
