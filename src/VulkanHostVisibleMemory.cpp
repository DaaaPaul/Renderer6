#include <climits>
#include <utility>
#include <cstring>
#include <iostream>
#include "VulkanHostVisibleMemory.hpp"

VulkanHostVisibleMemory::VulkanHostVisibleMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO) :
	mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
	mHostVisibleMemory{},
	mHostVisibleBuffers{},
	mHostVisibleBufferInfos{ GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO },
	mBufferOffsets{},
	mBufferSizes{} {

	std::cout << "HOST VISIBLE MEMORY PARAMETERS:\n";
	for(VulkanMemoryCommon::VulkanBufferInfo const& INFO : mHostVisibleBufferInfos) {
		std::cout << "\tsize: " << INFO.mBUFFER_SIZE << "\n";
		std::cout << "\tusage: " << INFO.mBUFFER_USAGE << "\n";
	}

	std::cout << "Creating VulkanHostVisibleMemory...\n";

	const size_t BUFFERS_COUNT{ mHostVisibleBufferInfos.size() };

	// create the buffers themselves with their memory requirements info
	mHostVisibleBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
	mBufferSizes.resize(BUFFERS_COUNT, 0);
	std::vector<VkMemoryRequirements> hostVisibleBuffersMemRequirements(BUFFERS_COUNT, {});
	for (int i = 0; i < BUFFERS_COUNT; i++) {
		mHostVisibleBuffers[i] = VulkanMemoryCommon::fCreateBuffer(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleBufferInfos[i]);
		VulkanPFNs::gpVkGetBufferMemoryRequirements(mVulkanDevicesWrapper->mLogicalDevice, mHostVisibleBuffers[i], &hostVisibleBuffersMemRequirements[i]);
		mBufferSizes[i] = hostVisibleBuffersMemRequirements[i].size;
	}

	// create the memory
	VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).first,
		.memoryTypeIndex = VulkanMemoryCommon::fGetMemoryTypeIndex(mVulkanDevicesWrapper->mPhysicalDevice, hostVisibleBuffersMemRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};
	VulkanPFNs::gpVkAllocateMemory(mVulkanDevicesWrapper->mLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mHostVisibleMemory);
	
	// bind buffers
	mBufferOffsets = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).second;
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