#include <stdexcept>
#include "backend/Window.h"
#include "engine/CameraComponent.hpp"

namespace Window {
	std::vector<const char*> get_instance_window_extensions() {
		glfwInit();

		uint32_t requiredCount{};
		const char** required = glfwGetRequiredInstanceExtensions(&requiredCount);
		std::vector<const char*> required_vector(requiredCount);

		for (int i = 0; i < requiredCount; ++i) {
			required_vector[i] = required[i];
		}
		#ifdef __APPLE__
		if (!required) {
			required_vector.push_back("VK_KHR_surface");
			required_vector.push_back("VK_EXT_metal_surface");
		}
		#endif

		return required_vector;
	}

	void init() {
		glfwSetErrorCallback(glfw_error_callback);
		
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

	void glfw_error_callback(int error_code, const char* description) {
		Utility::println("glfw_error_callback: (" + std::to_string(error_code) + ")" + description);
	}
}
