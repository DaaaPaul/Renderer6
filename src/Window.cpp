#include <iostream>
#include "Window.h"
#include "Util.h"

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

			CHECK_NULLPTR(gpGlfwWindow, "glfwCreateWindow failed")

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

		namespace Util {
			std::vector<const char*> getRequiredWindowExtensionsForInstance() {
				glfwInit();

				uint32_t requiredCount{};
				const char** required = glfwGetRequiredInstanceExtensions(&requiredCount);
				std::vector<const char*> requiredVector{};

				#ifdef __APPLE__
				if (!required) {
					requiredVector.push_back("VK_KHR_surface");
					requiredVector.push_back("VK_EXT_metal_surface");
				}
				#endif
				for (int i = 0; i < requiredCount; i++) {
					requiredVector.push_back(required[i]);
				}

				return requiredVector;
			}
		}
	}
}
