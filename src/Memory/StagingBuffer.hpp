#pragma once

#include "Buffer.hpp"

class StagingBuffer : public Buffer {
	public:
	explicit StagingBuffer(VkDeviceSize size, 
						   VkSharingMode sharing_mode, 
						   const std::vector<uint32_t>& queue_family_indices);
};

