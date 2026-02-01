#include "VulkanDeviceLocalMemory.hpp"
#include <iostream>

VulkanDeviceLocalMemory::VulkanDeviceLocalMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO) : 
	mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
	mDeviceLocalMemory{},
	mDeviceLocalBuffers{},
	mDeviceLocalBufferInfos{ GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO },
	mBufferOffsets{},
	mBufferSizes{} {

	std::cout << "DEVICE LOCAL MEMORY PARAMETERS:\n";
	for(VulkanMemoryCommon::VulkanBufferInfo const& INFO : mDeviceLocalBufferInfos) {
		std::cout << "\tsize: " << INFO.mBUFFER_SIZE << "\n";
		std::cout << "\tusage: " << INFO.mBUFFER_USAGE << "\n";
	}

	std::cout << "Creating VulkanDeviceLocalMemory...\n";

	const size_t BUFFERS_COUNT{ mDeviceLocalBufferInfos.size() };

	// create the buffers themselves with their memory requirements info
	mDeviceLocalBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
	mBufferSizes.resize(BUFFERS_COUNT, 0);
	std::vector<VkMemoryRequirements> deviceLocalBuffersMemRequirements(BUFFERS_COUNT, {});
	for (int i = 0; i < BUFFERS_COUNT; i++) {
		mDeviceLocalBuffers[i] = VulkanMemoryCommon::fCreateBuffer(mVulkanDevicesWrapper->mLogicalDevice, mDeviceLocalBufferInfos[i]);
		VulkanPFNs::gpVkGetBufferMemoryRequirements(mVulkanDevicesWrapper->mLogicalDevice, mDeviceLocalBuffers[i], &deviceLocalBuffersMemRequirements[i]);
		mBufferSizes[i] = deviceLocalBuffersMemRequirements[i].size;
	}

	// create the memory
	VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(deviceLocalBuffersMemRequirements).first,
		.memoryTypeIndex = VulkanMemoryCommon::fGetMemoryTypeIndex(mVulkanDevicesWrapper->mPhysicalDevice, deviceLocalBuffersMemRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	};
	VulkanPFNs::gpVkAllocateMemory(mVulkanDevicesWrapper->mLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mDeviceLocalMemory);

	// bind buffers
	mBufferOffsets = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(deviceLocalBuffersMemRequirements).second;
	for(int i = 0; i < BUFFERS_COUNT; i++) {
		VulkanPFNs::gpVkBindBufferMemory(mVulkanDevicesWrapper->mLogicalDevice, mDeviceLocalBuffers[i], mDeviceLocalMemory, mBufferOffsets[i]);
	}

	std::cout << "Created VulkanDeviceLocalMemory\n";
}

VulkanDeviceLocalMemory::~VulkanDeviceLocalMemory() {
	std::cout << "Destroying VulkanDeviceLocalMemory...\n";
	
	VulkanPFNs::gpVkFreeMemory(mVulkanDevicesWrapper->mLogicalDevice, mDeviceLocalMemory, nullptr);
	for(VkBuffer& buffer : mDeviceLocalBuffers) {
		VulkanPFNs::gpVkDestroyBuffer(mVulkanDevicesWrapper->mLogicalDevice, buffer, nullptr);
	}
	
	std::cout << "Destroyed VulkanDeviceLocalMemory\n";
}

void VulkanDeviceLocalMemory::copyToBuffer(size_t const& INDEX) {
	
}