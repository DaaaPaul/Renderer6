#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <vector>
#include "Utility.h"

#undef WINDOW_MONITOR_MODE

namespace Window {
	struct Extent {
		int width{};
		int height{};
	};

	struct WindowCreated {
		GLFWwindow* window{};
		std::function<void()> setup{};
	};

	inline GLFWwindow* g_glfw_window{};

	inline constexpr Extent g_DEBUG_EXTENT{800, 600};
	inline constexpr const char* g_WINDOW_TITLE = "Renderer6";

	inline bool g_window_resized = false;
	inline bool* g_p_window_resized = &g_window_resized;

	std::vector<const char*> get_instance_window_extensions();

	void init();
	void destroy();

	Extent get_window_extent_monitor();
	Extent get_window_extent_independant();
	WindowCreated create_glfw_window_monitor(const Extent& extent);
	WindowCreated create_glfw_window_independant(const Extent& extent);

	inline float get_aspect_ratio() {
		Extent extent = 

		#ifdef WINDOW_MONITOR_MODE
		get_window_extent_monitor();
		#else
		get_window_extent_independant();
		#endif

		return static_cast<float>(extent.width) / extent.height;
	}

	void window_resize_callback(GLFWwindow* glfw_window, int width, int height);
}
