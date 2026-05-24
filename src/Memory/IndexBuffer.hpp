#pragma once

#include "Memory/Buffer.hpp"

class IndexBuffer : public Buffer {
	public:
	explicit IndexBuffer(uint32_t name_index, 
						 VkDeviceSize size, 
						 VkSharingMode sharing_mode, 
						 const std::vector<uint32_t>& queue_family_indices);
};