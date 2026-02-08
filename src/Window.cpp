#include <iostream>
#include "Window.hpp"
#include "Common.h"

namespace Backend {
	Window::Window() :
		mpGlfwWindow{},
		mFrameBufferResizedAlert{},
		mPARAMETERS{} {}

	Window::Window(WindowParameters const& GIVEN_PARAMETERS) :
		mpGlfwWindow{},
		mFrameBufferResizedAlert{ false },
		mPARAMETERS{ GIVEN_PARAMETERS } {
		// print parameters
		std::cout << "SET WINDOW CREATE PARAMETERS:\n"
			"\t-width: " << mPARAMETERS.mWIDTH << "\n"
			"\t-height: " << mPARAMETERS.mHEIGHT << "\n"
			"\t-name: " << mPARAMETERS.mNAME << "\n";

		// construct the GLFWwindow*
		std::cout << "Creating Window...\n";

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		mpGlfwWindow = glfwCreateWindow(mPARAMETERS.mWIDTH, mPARAMETERS.mHEIGHT, mPARAMETERS.mNAME, nullptr, nullptr);
		glfwSetWindowUserPointer(mpGlfwWindow, this);
		glfwSetFramebufferSizeCallback(mpGlfwWindow, sFramebufferResizeCallback);

		CHECK_NULLPTR(mpGlfwWindow, "glfwCreateWindow failed");

		std::cout << "Created Window\n";
	}

	Window::~Window() {
		std::cout << "Destroying Window...\n";

		glfwDestroyWindow(mpGlfwWindow);
		glfwTerminate();

		std::cout << "Destroyed Window\n";
	}

	void Window::sFramebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height) {
		Window* pUser = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pGlfwWindow));
		pUser->mFrameBufferResizedAlert = true;
	}

	[[nodiscard]] std::vector<const char*> Window::sGetGlfwWindowExtensions() {
		glfwInit();

		uint32_t requiredGlfwExtensionsCount{};
		const char** requiredGlfwExtensionsNames = glfwGetRequiredInstanceExtensions(&requiredGlfwExtensionsCount);
		std::vector<const char*> requiredGlfwExtensionsNamesVector{};

		if (!requiredGlfwExtensionsNames) {
			#ifdef __APPLE__
			requiredGlfwExtensionsNamesVector.push_back("VK_KHR_surface");
			requiredGlfwExtensionsNamesVector.push_back("VK_EXT_metal_surface");
			#endif

			#ifdef _WIN32
			CHECK_NULLPTR(requiredGlfwExtensionsNames, "glfwGetRequiredInstanceExtensions failed")
			#endif
		}

		for (int i = 0; i < requiredGlfwExtensionsCount; i++) {
			requiredGlfwExtensionsNamesVector.push_back(requiredGlfwExtensionsNames[i]);
		}

		return requiredGlfwExtensionsNamesVector;
	}
}
