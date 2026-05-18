#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include "Utility/RuntimeError.hpp"

namespace Instance {
	inline VkInstance g_instance{};

	inline std::vector<const char*> g_layers{
		"VK_LAYER_KHRONOS_validation"
	};
	std::vector<const char*> init_extensions();
	inline std::vector<const char*> g_extensions(init_extensions());

	void init();
	void destroy();

	RuntimeError check_have_layers(const std::vector<const char*>& needed_layers);
	RuntimeError check_have_extensions(const std::vector<const char*>& needed_extensions);

	VkInstance create_instance();
}
