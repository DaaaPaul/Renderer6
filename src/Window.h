#pragma once

#include <GLFW/glfw3.h>
#include <vector>

namespace Window {
	inline GLFWwindow* gGlfwWindow{};
	inline GLFWmonitor* gGlfwMonitor{};

	inline int gMonitorWidth{};
	inline int gMonitorHeight{};
	inline float gAspectRatio{};
	inline constexpr const char* gTITLE = "Renderer6";
	inline bool gFramebufferResized = false;
	inline bool* gFrameBufferResizedPointer = &gFramebufferResized;

	std::vector<const char*> getInstanceWindowExtensions();

	void init();
	void deInit();

	void getGlfwMonitor();
	void createGlfwWindow();
	void setWindowCallbacks();
	void destroyGlfwWindow();

	void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
}
