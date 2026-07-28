#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <vector>
#include "utility/Utility.h"
#include "engine/CameraComponent.hpp"

namespace Window {
	struct WindowUser {
		bool window_resized{};
		CameraComponent* camera_component{};
	};

	inline constexpr bool g_MONITOR_MODE = false;

	inline GLFWwindow* g_glfw_window{};
	inline constexpr const char* g_WINDOW_TITLE = "Renderer6";
	inline constexpr int g_WIDTH = 800;
	inline constexpr int g_HEIGHT = 600;

	inline WindowUser g_window_user{};
	inline WindowUser* g_window_user_pointer = &g_window_user;

	std::vector<const char*> get_instance_window_extensions();

	void init();
	void destroy();

	void window_resize_callback(GLFWwindow* glfw_window, int width, int height);
}
