#include "VulkanHostVisibleMemory.hpp"

VulkanHostVisibleMemory::VulkanHostVisibleMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanHostVisibleMemoryBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO) :
	mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
	mHostVisibleMemory{},
	mHostVisibleBuffers{},
	mHostVisibleBufferInfos{ GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO } {

}

VulkanHostVisibleMemory::~VulkanHostVisibleMemory() {

}

[[nodiscard]] VkBuffer VulkanHostVisibleMemory::createBuffer(VulkanHostVisibleMemoryBufferInfo const& info) {
	VkBufferCreateInfo bufferInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = info.BUFFER_SIZE,
		.usage = info.BUFFER_USAGE,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &info.GRAPHICS_QUEUE_FAMILY_INDEX,
	};


}