#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdint>
#include "Common.h"

namespace Backend {
	struct Window {
		struct CreateInfo {
			uint16_t width{};
			uint16_t height{};
			const char* NAME{};
		};

		GLFWwindow* glfwWindow{};
		bool framebufferResized{};
		const CreateInfo CREATE_INFO{};

		static void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height);
		[[nodiscard]] static std::vector<const char*> getInstanceRequiredWindowExtensions();

		explicit Window(CreateInfo const& GIVEN_CREATE_INFO);
		~Window();

		DELETE_COPY_CONSTRUCTORS(Window)
		DELETE_MOVE_CONSTRUCTORS(Window)
	};
}
