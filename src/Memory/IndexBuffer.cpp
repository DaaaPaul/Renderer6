#include "IndexBuffer.hpp"
#include "Utility/Utility.h"

IndexBuffer::IndexBuffer(VkDeviceSize size, VkSharingMode sharing_mode, const std::vector<uint32_t>& queue_family_indices) :
	Buffer(VK_NO_FLAGS, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, sharing_mode, queue_family_indices) {
						   
}