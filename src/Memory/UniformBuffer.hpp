#pragma once

#include "Memory/Buffer.hpp"

class UniformBuffer : Buffer {
	public:
	explicit UniformBuffer(VkDeviceSize size, VkSharingMode sharing_mode, const std::vector<uint32_t>& queue_family_indices);
};