#include "IndexBuffer.hpp"
#include "Utility/Utility.h"

IndexBuffer::IndexBuffer(uint32_t id, VkDeviceSize size, VkSharingMode sharing_mode, const std::vector<uint32_t>& queue_family_indices) :
	Buffer(id, VK_NO_FLAGS, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sharing_mode, queue_family_indices) {
						   
}