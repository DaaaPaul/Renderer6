#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Buffer.hpp"

class StagingBuffer : public Buffer {
	public:
	explicit StagingBuffer(VkDeviceSize size, 
						   VkSharingMode sharing_mode, 
						   const std::vector<uint32_t>& queue_family_indices);
};

