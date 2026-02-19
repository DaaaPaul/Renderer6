#include <iostream>
#include "Window.hpp"
#include "Common.h"

namespace Backend {
	// private:
	void Window::framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height) {
		Window* self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pGlfwWindow));
		self->framebufferResized = true;
	}

	// public:
	Window::Window(CreateInfo const& CREATE_INFO) :
		glfwWindow{},
		CREATE_INFO{ CREATE_INFO },
		framebufferResized{ false } {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		glfwWindow = glfwCreateWindow(CREATE_INFO.width, CREATE_INFO.height, CREATE_INFO.NAME, nullptr, nullptr);
		glfwSetWindowUserPointer(glfwWindow, this);
		glfwSetFramebufferSizeCallback(glfwWindow, framebufferResizeCallback);

		CHECK_NULLPTR(glfwWindow, "glfwCreateWindow failed");
	}

	Window::~Window() {
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
	}

	[[nodiscard]] std::vector<const char*> Window::getInstanceRequiredWindowExtensions() {
		glfwInit();

		uint32_t requiredGlfwExtensionsCount{};
		const char** requiredGlfwExtensionsNames = glfwGetRequiredInstanceExtensions(&requiredGlfwExtensionsCount);
		std::vector<const char*> requiredGlfwExtensionsNamesVector{};

		if (!requiredGlfwExtensionsNames) {
			#ifdef __APPLE__
			requiredGlfwExtensionsNamesVector.push_back("VK_KHR_surface");
			requiredGlfwExtensionsNamesVector.push_back("VK_EXT_metal_surface");
			#endif

			#ifdef _WIN32
			CHECK_NULLPTR(requiredGlfwExtensionsNames, "glfwGetRequiredInstanceExtensions failed")
			#endif
		}

		for (int i = 0; i < requiredGlfwExtensionsCount; i++) {
			requiredGlfwExtensionsNamesVector.push_back(requiredGlfwExtensionsNames[i]);
		}

		return requiredGlfwExtensionsNamesVector;
	}
}
