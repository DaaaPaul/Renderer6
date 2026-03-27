#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {
	namespace PipelineLayout {
		std::vector<VkPipelineLayout> layouts{};

		VkPipelineLayout newLayout(VkPipelineLayoutCreateInfo const&);
		void clear() noexcept;
	}
}