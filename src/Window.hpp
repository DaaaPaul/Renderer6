#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdint>
#include "Common.h"

namespace Backend {
	struct Window {
		struct WindowParameters {
			const uint16_t mWIDTH{};
			const uint16_t mHEIGHT{};
			const char* mNAME{};
		};

		GLFWwindow* mpGlfwWindow{};
		bool mFrameBufferResizedAlert{};
		const WindowParameters mPARAMETERS{};

		static void sFramebufferResizeCallback(GLFWwindow* pGlfwWindow, int width, int height);

		[[nodiscard]] constexpr static WindowParameters sGetConstructParameters() noexcept { return WindowParameters(800, 600, "Renderer6"); }
		[[nodiscard]] static std::vector<const char*> sGetGlfwWindowExtensions();

		Window();
		explicit Window(WindowParameters const& GIVEN_PARAMETERS);
		~Window();

		DELETE_COPY_CONSTRUCTORS(Window)
		DELETE_MOVE_CONSTRUCTORS(Window)
	};
}
