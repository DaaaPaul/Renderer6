#include <stdexcept>
#include "Window.h"
#include "Camera.hpp"

namespace Backend {
	namespace Window {
		void init() {
			glfwInit();
			getGlfwMonitor();
			createGlfwWindow();
			setWindowCallbacks();
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
			gGlfwWindow = glfwCreateWindow(gMonitorWidth, gMonitorHeight, gTITLE, gGlfwMonitor, nullptr);

			if(!gGlfwWindow) {
				throw std::runtime_error("glfwCreateWindow failed");
			}

			glfwSetInputMode(gGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		void setWindowCallbacks() {
			glfwSetWindowUserPointer(gGlfwWindow, gFrameBufferResizedPointer);
			glfwSetFramebufferSizeCallback(gGlfwWindow, framebufferResizeCallback);
			glfwSetScrollCallback(gGlfwWindow, Engine::Camera::scrolled);
			glfwSetCursorPosCallback(gGlfwWindow, Engine::Camera::mouseMoved);
		}

		void destroyGlfwWindow() {
			glfwDestroyWindow(gGlfwWindow);
			glfwTerminate();
		}

		void framebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height) {
			bool* resized = reinterpret_cast<bool*>(glfwGetWindowUserPointer(pGlfwWindow));
			*resized = true;
		}
	}
}
