#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Memory/Wrappers/Buffer.hpp"

class UniformBuffer : public Buffer {
	public:
	explicit UniformBuffer(VkDeviceSize size, 
						   VkSharingMode sharing_mode, 
						   const std::vector<uint32_t>& queue_family_indices);
};