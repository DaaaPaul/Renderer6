#include <stdexcept>
#include "Window.h"
#include "Camera.hpp"

namespace Window {
	std::vector<const char*> getInstanceWindowExtensions() {
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
		for (int i = 0; i < requiredCount; ++i) {
			requiredVector.push_back(required[i]);
		}

		return requiredVector;
	}

	void init() {
		glfwInit();
		getGlfwMonitor();
		createGlfwWindow();
		setWindowCallbacks();
	}
	void deInit() {
		destroyGlfwWindow();
	}

	void getGlfwMonitor() {
		gGlfwMonitor = glfwGetPrimaryMonitor();

		if(!gGlfwMonitor) {
			throw std::runtime_error("glfwGetPrimaryMonitor failed");
		}

		GLFWvidmode const* VIDEO_MODE = glfwGetVideoMode(gGlfwMonitor);
		gMonitorWidth = VIDEO_MODE->width;
		gMonitorHeight = VIDEO_MODE->height;
		gAspectRatio = static_cast<float>(gMonitorWidth) / static_cast<float>(gMonitorHeight);
	}

	void createGlfwWindow() {
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		gGlfwWindow = glfwCreateWindow(gMonitorWidth, gMonitorHeight, gTITLE, gGlfwMonitor, nullptr);

		if(!gGlfwWindow) {
			throw std::runtime_error("glfwCreateWindow failed");
		}

		glfwSetInputMode(gGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void setWindowCallbacks() {
		glfwSetWindowUserPointer(gGlfwWindow, gFrameBufferResizedPointer);
		glfwSetFramebufferSizeCallback(gGlfwWindow, framebufferResizeCallback);
		glfwSetScrollCallback(gGlfwWindow, Camera::scrolled);
		glfwSetCursorPosCallback(gGlfwWindow, Camera::mouseMoved);
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
