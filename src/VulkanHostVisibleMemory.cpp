#include <climits>
#include <utility>
#include <cstring>
#include <iostream>
#include "VulkanHostVisibleMemory.hpp"

VulkanHostVisibleMemory::VulkanHostVisibleMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO) :
	mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
	mHostVisibleMemory{},
	mHostVisibleBuffers{},
	mHostVisibleBufferInfos{ GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO },
	mBufferOffsets{},
	mBufferSizes{} {

	std::cout << "HOST VISIBLE MEMORY PARAMETERS:\n";
	for(VulkanBufferInfo const& INFO : mHostVisibleBufferInfos) {
		std::cout << "\tsize: " << INFO.mBUFFER_SIZE << "\n";
		std::cout << "\tusage: " << INFO.mBUFFER_USAGE << "\n";
	}

	std::cout << "Creating VulkanHostVisibleMemory...\n";

	// create buffers and memory and bind them
	const size_t BUFFERS_COUNT{ mHostVisibleBufferInfos.size() };

	// create the buffers themselves with their memory requirements info
	mHostVisibleBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
	mBufferSizes.resize(BUFFERS_COUNT, 0);
	std::vector<VkMemoryRequirements> hostVisibleBuffersMemRequirements(BUFFERS_COUNT, {});
	for (int i = 0; i < BUFFERS_COUNT; i++) {
		mHostVisibleBuffers[i] = VulkanHostVisibleMemory::createBuffer(*this, mHostVisibleBufferInfos[i]);
		VulkanPFNs::gpVkGetBufferMemoryRequirements(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleBuffers[i], &hostVisibleBuffersMemRequirements[i]);
		mBufferSizes[i] = hostVisibleBuffersMemRequirements[i].size;
	}

	// create the memory
	VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = VulkanHostVisibleMemory::getMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).first,
		.memoryTypeIndex = VulkanHostVisibleMemory::getMemoryTypeIndex(mVulkanDevicesWrapper->mPhysicalDevice, hostVisibleBuffersMemRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};
	VulkanPFNs::gpVkAllocateMemory(mVulkanDevicesWrapper->mLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mHostVisibleMemory);
	
	mBufferOffsets = VulkanHostVisibleMemory::getMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).second;
	for(int i = 0; i < BUFFERS_COUNT; i++) {
		VulkanPFNs::gpVkBindBufferMemory(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleBuffers[i], mHostVisibleMemory, mBufferOffsets[i]);
	}

	std::cout << "Created VulkanHostVisibleMemory\n";
}

VulkanHostVisibleMemory::~VulkanHostVisibleMemory() {
	std::cout << "Destroying VulkanHostVisibleMemory...\n";
	
	VulkanPFNs::gpVkFreeMemory(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleMemory, nullptr);
	for(VkBuffer& buffer : mHostVisibleBuffers) {
		VulkanPFNs::gpVkDestroyBuffer(mVulkanDevicesWrapper->mLogicalDevice, buffer, nullptr);
	}
	
	std::cout << "Destroyed VulkanHostVisibleMemory\n";
}

void VulkanHostVisibleMemory::writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES) {
	void* mappedMemory{};
	CHECK_VK_SUCCESS(
	VulkanPFNs::gpVkMapMemory(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleMemory, mBufferOffsets[INDEX], mBufferSizes[INDEX], 0, &mappedMemory),
	"Failed to map memory"
	)
	std::memcpy(mappedMemory, pDATA, NUM_BYTES);
	VulkanPFNs::gpVkUnmapMemory(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleMemory);

	std::cout << "Wrote " << NUM_BYTES << " bytes of data from " << pDATA << " to host visible memory offset " << mBufferOffsets[INDEX] << "\n";
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