#include <iostream>
#include "Window.hpp"
#include "Common.h"

namespace Backend {
	void Window::framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height) {
		Window* pSelf = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pGlfwWindow));
		pSelf->framebufferResized = true;
	}

	Window::Window(CreateInfo const& CREATE_INFO) :
		pGlfwWindow{},
		CREATE_INFO{ CREATE_INFO },
		framebufferResized{ false } {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		pGlfwWindow = glfwCreateWindow(CREATE_INFO.width, CREATE_INFO.height, CREATE_INFO.NAME, nullptr, nullptr);
		glfwSetWindowUserPointer(pGlfwWindow, this);
		glfwSetFramebufferSizeCallback(pGlfwWindow, framebufferResizeCallback);

		CHECK_NULLPTR(pGlfwWindow, "glfwCreateWindow failed");
	}

	Window::~Window() {
		glfwDestroyWindow(pGlfwWindow);
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
