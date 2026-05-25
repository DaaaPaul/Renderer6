#include "StagingBuffer.hpp"
#include "Utility/Utility.h"

StagingBuffer::StagingBuffer(uint32_t name_index, VkDeviceSize size, VkSharingMode sharing_mode, const std::vector<uint32_t>& queue_family_indices) :
	Buffer(name_index, VK_NO_FLAGS, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sharing_mode, queue_family_indices) {
						   
}