#include <stdexcept>
#include "Window.h"

namespace Backend {
	namespace Window {
		void init() {
			createGlfwWindow();
		}
		void deInit() {
			destroyGlfwWindow();
		}

		void createGlfwWindow() {
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			gpGlfwWindow = glfwCreateWindow(gWINDOW_WIDTH, gWINDOW_HEIGHT, gWINDOW_TITLE, nullptr, nullptr);

			if(!gpGlfwWindow) {
				throw std::runtime_error("glfwCreateWindow failed");
			}

			glfwSetWindowUserPointer(gpGlfwWindow, gpFrameBufferResized);
			glfwSetFramebufferSizeCallback(gpGlfwWindow, framebufferResizeCallback);
		}

		void destroyGlfwWindow() noexcept {
			glfwDestroyWindow(gpGlfwWindow);
			glfwTerminate();
		}

		void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height) {
			bool* pResized = reinterpret_cast<bool*>(glfwGetWindowUserPointer(pGlfwWindow));
			*pResized = true;
		}
	}
}
