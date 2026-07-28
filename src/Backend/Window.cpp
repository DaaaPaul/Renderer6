#include <stdexcept>
#include "backend/Window.h"
#include "engine/CameraComponent.hpp"

namespace Window {
	std::vector<const char*> get_instance_window_extensions() {
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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		if(g_MONITOR_MODE) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);

			g_glfw_window = glfwCreateWindow(video_mode->width, video_mode->height, g_WINDOW_TITLE, monitor, nullptr);
			glfwSetInputMode(g_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else {
			g_glfw_window = glfwCreateWindow(g_WIDTH, g_HEIGHT, g_WINDOW_TITLE, nullptr, nullptr);
		}

		glfwSetWindowUserPointer(g_glfw_window, g_window_user_pointer);
		glfwSetFramebufferSizeCallback(g_glfw_window, window_resize_callback);
		glfwSetScrollCallback(g_glfw_window, CameraComponent::scroll_callback);
		glfwSetCursorPosCallback(g_glfw_window, CameraComponent::mouse_moved_callback);
	}

	void destroy() {
		glfwDestroyWindow(g_glfw_window);
		glfwTerminate();
	}

	void window_resize_callback(GLFWwindow* glfw_window, int width, int height) {
		g_window_user_pointer->window_resized = true;
	}
}
