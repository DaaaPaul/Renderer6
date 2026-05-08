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
		set_callbacks();
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
		g_glfw_window = glfwCreateWindow(gMonitorWidth, gMonitorHeight, gTITLE, gGlfwMonitor, nullptr);

		if(!g_glfw_window) {
			throw std::runtime_error("glfwCreateWindow failed");
		}

		glfwSetInputMode(g_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void set_callbacks() {
		glfwSetWindowUserPointer(g_glfw_window, gFrameBufferResizedPointer);
		glfwSetFramebufferSizeCallback(g_glfw_window, framebufferResizeCallback);
		glfwSetScrollCallback(g_glfw_window, Camera::scroll_callback);
		glfwSetCursorPosCallback(g_glfw_window, Camera::mouse_moved_callback);
	}

	void destroyGlfwWindow() {
		glfwDestroyWindow(g_glfw_window);
		glfwTerminate();
	}

	void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height) {
		bool* resized = reinterpret_cast<bool*>(glfwGetWindowUserPointer(pGlfwWindow));
		*resized = true;
	}
}
