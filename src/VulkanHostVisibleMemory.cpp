#include <climits>
#include <utility>
#include "VulkanHostVisibleMemory.hpp"

VulkanHostVisibleMemory::VulkanHostVisibleMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO) :
	mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
	mHostVisibleMemory{},
	mHostVisibleBuffers{},
	mHostVisibleBufferInfos{ GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO } {

	// create buffers and memory
	{
		// create the buffers themselves with their memory requirements info
		mHostVisibleBuffers.resize(mHostVisibleBufferInfos.size(), VK_NULL_HANDLE);
		std::vector<VkMemoryRequirements> hostVisibleBuffersMemRequirements(mHostVisibleBuffers.size(), {});
		for (int i = 0; i < mHostVisibleBufferInfos.size(); i++) {
			mHostVisibleBuffers[i] = VulkanHostVisibleMemory::createBuffer(*this, mHostVisibleBufferInfos[i]);
			VulkanPFNs::gpVkGetBufferMemoryRequirements(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleBuffers[i], &hostVisibleBuffersMemRequirements[i]);
		}

		// create the memory
		VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = VulkanHostVisibleMemory::getMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).first,
			.memoryTypeIndex = VulkanHostVisibleMemory::getMemoryTypeIndex(mVulkanDevicesWrapper->mPhysicalDevice, hostVisibleBuffersMemRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		};
		VulkanPFNs::gpVkAllocateMemory(mVulkanDevicesWrapper->mLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mHostVisibleMemory);
	}
}

VulkanHostVisibleMemory::~VulkanHostVisibleMemory() {

}

[[nodiscard]] VkBuffer VulkanHostVisibleMemory::createBuffer(VulkanHostVisibleMemory const& HOST_VISIBLE_MEMORY, VulkanBufferInfo const& INFO) {
	VkBufferCreateInfo bufferInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = INFO.mBUFFER_SIZE,
		.usage = INFO.mBUFFER_USAGE,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &INFO.mGRAPHICS_QUEUE_FAMILY_INDEX,
	};

	VkBuffer returnBuffer{};
	VulkanPFNs::gpVkCreateBuffer(HOST_VISIBLE_MEMORY.mVulkanDevicesWrapper->mLogicalDevice, &bufferInfo, nullptr, &returnBuffer);

	return returnBuffer;
}

[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> VulkanHostVisibleMemory::getMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS) {
	std::pair<VkDeviceSize, std::vector<VkDeviceSize>> allocationSizeAndBufferOffsets{};
	uint32_t buffersCount = static_cast<uint32_t>(BUFFER_MEMORY_REQUIREMENTS.size());
	allocationSizeAndBufferOffsets.second.resize(buffersCount, UINT64_MAX);

	for (int i = 0; i < buffersCount; i++) {
		while (allocationSizeAndBufferOffsets.first % BUFFER_MEMORY_REQUIREMENTS[i].alignment != 0) {
			allocationSizeAndBufferOffsets.first++;
		}

		allocationSizeAndBufferOffsets.second[i] = allocationSizeAndBufferOffsets.first;

		allocationSizeAndBufferOffsets.first += BUFFER_MEMORY_REQUIREMENTS[i].size;
	}

	return allocationSizeAndBufferOffsets;
}

[[nodiscard]] uint32_t VulkanHostVisibleMemory::getMemoryTypeIndex(VkPhysicalDevice physicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES) {
	uint32_t finalMemoryRequirementsMask = UINT32_MAX;
	for (VkMemoryRequirements const& BUFFER_MEMORY_REQUIREMENT : BUFFER_MEMORY_REQUIREMENTS) {
		finalMemoryRequirementsMask &= BUFFER_MEMORY_REQUIREMENT.memoryTypeBits;
	}

	VkPhysicalDeviceMemoryProperties memoryProperties{};
	VulkanPFNs::gpVkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	uint32_t memoryTypeIndexReturn{ UINT32_MAX };
	for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((finalMemoryRequirementsMask & (1 << i)) &&
			((memoryProperties.memoryTypes[i].propertyFlags & MEMORY_PROPERTIES) == MEMORY_PROPERTIES)) {
			memoryTypeIndexReturn = i;
		}
	};

	return memoryTypeIndexReturn;
}