#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Common.h"
#include "Window.hpp"

namespace Backend {
	struct Backend {
		struct BackendConstructParameters {
			const std::vector<const char*> mENABLED_LOADER_LAYERS{};
			const std::vector<const char*> mENABLED_EXTENSIONS{};
			const VkApplicationInfo mAPP_INFO{};
			VkInstanceCreateInfo mCreateInfo{};
		};

		VkInstance mpInstance{};
		Window* mpWindow{};
		BackendConstructParameters mParameters{};

		static void sCheckHaveInstanceExtensions(std::vector<const char*> const& CHECK_HAVE_ME);
		static void sCheckHaveLoaderLayers(std::vector<const char*> const& CHECK_HAVE_ME);
		[[nodiscard]] static BackendConstructParameters sGetConstructParameters();

		Backend();
		explicit Backend(Window* givenWindow, BackendConstructParameters const& GIVEN_VULKAN_BACKEND_CREATE_PARAMETERS);
		~Backend();

		DELETE_COPY_CONSTRUCTORS(Backend)
		DELETE_MOVE_CONSTRUCTORS(Backend)
	};
}