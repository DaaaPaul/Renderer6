#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Buffer.hpp"
#include "UniformBuffer.hpp"
#include "Utility/Vulkan.h"

UniformBuffer::UniformBuffer(VkDeviceSize size, VkSharingMode sharing_mode, const std::vector<uint32_t>& queue_family_indices) :
	Buffer(VK_NO_FLAGS, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT  | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, sharing_mode, queue_family_indices) {
						   
}