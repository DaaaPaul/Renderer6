#pragma once

#include <GLFW/glfw3.h>
#include <vector>

namespace Window {
	inline GLFWwindow* g_glfw_window{};
	inline GLFWmonitor* gGlfwMonitor{};

	inline int gMonitorWidth{};
	inline int gMonitorHeight{};
	inline float gAspectRatio{};
	inline constexpr const char* gTITLE = "Renderer6";
	inline bool g_window_resized = false;
	inline bool* gFrameBufferResizedPointer = &g_window_resized;

	std::vector<const char*> getInstanceWindowExtensions();

	void init();
	void destroy();

	void getGlfwMonitor();
	void createGlfwWindow();
	void set_callbacks();
	void destroyGlfwWindow();

	void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
}
