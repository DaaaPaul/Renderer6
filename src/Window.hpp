#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdint>
#include "Common.h"

namespace Backend {
	class Window {
		public:
		struct CreateInfo {
			uint16_t width{};
			uint16_t height{};
			const char* NAME{};
		};

		private:
		GLFWwindow* pGlfwWindow{};
		const CreateInfo CREATE_INFO{};
		static void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height);

		public:
		explicit Window(CreateInfo const& GIVEN_CREATE_INFO);
		~Window();
		bool framebufferResized{};
		[[nodiscard]] static std::vector<const char*> getInstanceRequiredWindowExtensions();
		[[nodiscard]] GLFWwindow*& getGlfwWindow() { return pGlfwWindow; }
		[[nodiscard]] CreateInfo const& getCreateInfo() const { return CREATE_INFO; }

		DELETE_COPY_CONSTRUCTORS(Window)
		DELETE_MOVE_CONSTRUCTORS(Window)
	};
}
