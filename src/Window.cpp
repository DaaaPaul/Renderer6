#include <stdexcept>
#include "Window.h"
#include "Camera.hpp"

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
			gGlfwWindow = glfwCreateWindow(gWINDOW_WIDTH, gWINDOW_HEIGHT, gWINDOW_TITLE, nullptr, nullptr);

			if(!gGlfwWindow) {
				throw std::runtime_error("glfwCreateWindow failed");
			}

			glfwSetWindowUserPointer(gGlfwWindow, gFrameBufferResizedPointer);
			glfwSetFramebufferSizeCallback(gGlfwWindow, framebufferResizeCallback);
			glfwSetScrollCallback(gGlfwWindow, Engine::glfwScrollCallback);
			glfwSetCursorPosCallback(gGlfwWindow, Engine::glfwMouseMovedCallback);
		}

		void destroyGlfwWindow() {
			glfwDestroyWindow(gGlfwWindow);
			glfwTerminate();
		}

		void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height) {
			bool* resized = reinterpret_cast<bool*>(glfwGetWindowUserPointer(pGlfwWindow));
			*resized = true;
		}
	}
}
