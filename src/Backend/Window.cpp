#include <stdexcept>
#include "Backend/Window.h"
#include "Engine/CameraComponent.hpp"

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

		Extent window_extent = 
		#ifdef WINDOW_MONITOR_MODE
		get_window_extent_monitor();
		#else
		get_window_extent_independant();
		#endif
		
		WindowInfo window_create = 
		#ifdef WINDOW_MONITOR_MODE
		create_glfw_window_monitor(window_extent);
		#else
		create_glfw_window_independant(window_extent);
		#endif

		g_glfw_window = window_create.window;
		window_create.setup();
	}
	void destroy() {
		glfwDestroyWindow(g_glfw_window);
		glfwTerminate();
	}

	Extent get_window_extent_monitor() {
		const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		return {
			video_mode->width,
			video_mode->height
		};
	}

	Extent get_window_extent_independant() {
		return g_DEBUG_EXTENT;
	}

	WindowInfo create_glfw_window_monitor(const Extent& extent) {
		return {
			glfwCreateWindow(extent.width, extent.height, g_WINDOW_TITLE, glfwGetPrimaryMonitor(), nullptr),
			[]() -> void {
				glfwSetInputMode(g_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				glfwSetWindowUserPointer(g_glfw_window, g_window_user_pointer);
				glfwSetFramebufferSizeCallback(g_glfw_window, window_resize_callback);
				glfwSetScrollCallback(g_glfw_window, CameraComponent::scroll_callback);
				glfwSetCursorPosCallback(g_glfw_window, CameraComponent::mouse_moved_callback);
			}
		};
	}

	WindowInfo create_glfw_window_independant(const Extent& extent) {
		return {
			glfwCreateWindow(extent.width, extent.height, g_WINDOW_TITLE, nullptr, nullptr),
			[]() -> void {
				glfwSetWindowUserPointer(g_glfw_window, g_window_user_pointer);
				glfwSetFramebufferSizeCallback(g_glfw_window, window_resize_callback);
				glfwSetScrollCallback(g_glfw_window, CameraComponent::scroll_callback);
				glfwSetCursorPosCallback(g_glfw_window, CameraComponent::mouse_moved_callback);
			}
		};
	}
}
