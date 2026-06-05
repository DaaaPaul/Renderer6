#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <vector>
#include "Utility/Utility.h"
#include "Engine/CameraComponent.hpp"

#undef WINDOW_MONITOR_MODE

namespace Window {
	struct Extent {
		int width{};
		int height{};
	};

	struct WindowInfo {
		GLFWwindow* window{};
		std::function<void()> setup{};
	};

	struct WindowUser {
		bool window_resized{};
		CameraComponent* camera_component{};
	};

	inline GLFWwindow* g_glfw_window{};

	inline constexpr Extent g_DEBUG_EXTENT{800, 600};
	inline constexpr const char* g_WINDOW_TITLE = "Renderer6";

	inline WindowUser g_window_user{};
	inline WindowUser* g_window_user_pointer = &g_window_user;

	std::vector<const char*> get_instance_window_extensions();

	void init();
	void destroy();

	Extent get_window_extent_monitor();
	Extent get_window_extent_independant();
	WindowInfo create_glfw_window_monitor(const Extent& extent);
	WindowInfo create_glfw_window_independant(const Extent& extent);

	inline float get_aspect_ratio() {
		Extent extent = 

		#ifdef WINDOW_MONITOR_MODE
		get_window_extent_monitor();
		#else
		get_window_extent_independant();
		#endif

		return static_cast<float>(extent.width) / extent.height;
	}

	inline void window_resize_callback(GLFWwindow* glfw_window, int width, int height) {
		g_window_user_pointer->window_resized = true;
	}
}
