#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace Backend {
	namespace Instance {
		inline VkInstance gInstance{};

		inline std::vector<const char*> gLayers{
			"VK_LAYER_KHRONOS_validation"
		};
		inline std::vector<const char*> gExtensions{};

		void init();
		void deInit();

		void createInstance();
		void destroyInstance() noexcept;

		void checkHaveExtensions(std::vector<const char*> const& EXTENSIONS);
		void checkHaveLayers(std::vector<const char*> const& LAYERS);

		void initExtensions() noexcept;
	}
}