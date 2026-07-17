#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <vector>
#include "Utility/Utility.h"
#include "Engine/CameraComponent.hpp"

namespace Window {
	struct WindowUser {
		bool window_resized{};
		CameraComponent* camera_component{};
	};

	inline constexpr bool g_MONITOR_MODE = true;

	inline GLFWwindow* g_glfw_window{};
	inline constexpr const char* g_WINDOW_TITLE = "Renderer6";
	inline constexpr int g_WIDTH = 800;
	inline constexpr int g_HEIGHT = 600;

	inline WindowUser g_window_user{};
	inline WindowUser* g_window_user_pointer = &g_window_user;

	std::vector<const char*> get_instance_window_extensions();

	void init();
	void destroy();

	inline void window_resize_callback(GLFWwindow* glfw_window, int width, int height) {
		g_window_user_pointer->window_resized = true;
	}
}
