#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <cstdint>
#include "Util.h"

namespace Backend {
	namespace Window {
		inline GLFWwindow* gpGlfwWindow{};

		inline constexpr int gWINDOW_WIDTH = 800;
		inline constexpr int gWINDOW_HEIGHT = 600;
		inline constexpr const char* gWINDOW_TITLE = "Renderer6";
		inline bool gFramebufferResized = false;
		inline bool* gpFrameBufferResized = &gFramebufferResized;

		void init();
		void deInit();

		void createGlfwWindow();
		void destroyGlfwWindow() noexcept;

		void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height);

		namespace Util {
			std::vector<const char*> getRequiredWindowExtensionsForInstance();
		}
	}
}
