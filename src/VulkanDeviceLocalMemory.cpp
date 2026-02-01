#include "VulkanDeviceLocalMemory.hpp"
#include <iostream>

VulkanDeviceLocalMemory::VulkanDeviceLocalMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO) : 
	mpVulkanDevicesWrapper{ pGivenVulkanDevicesWrapper },
	mpDeviceLocalMemory{},
	mDeviceLocalpBuffers{},
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
	mDeviceLocalpBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
	mBufferSizes.resize(BUFFERS_COUNT, 0);
	std::vector<VkMemoryRequirements> deviceLocalBuffersMemRequirements(BUFFERS_COUNT, {});
	for (int i = 0; i < BUFFERS_COUNT; i++) {
		mDeviceLocalpBuffers[i] = VulkanMemoryCommon::fCreateBuffer(mpVulkanDevicesWrapper->mpLogicalDevice, mDeviceLocalBufferInfos[i]);
		VulkanPFNs::gpVkGetBufferMemoryRequirements(mpVulkanDevicesWrapper->mpLogicalDevice, mDeviceLocalpBuffers[i], &deviceLocalBuffersMemRequirements[i]);
		mBufferSizes[i] = deviceLocalBuffersMemRequirements[i].size;
	}

	// create the memory
	VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(deviceLocalBuffersMemRequirements).first,
		.memoryTypeIndex = VulkanMemoryCommon::fGetMemoryTypeIndex(mpVulkanDevicesWrapper->mpPhysicalDevice, deviceLocalBuffersMemRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	};
	VulkanPFNs::gpVkAllocateMemory(mpVulkanDevicesWrapper->mpLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mpDeviceLocalMemory);

	// bind buffers
	mBufferOffsets = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(deviceLocalBuffersMemRequirements).second;
	for(int i = 0; i < BUFFERS_COUNT; i++) {
		VulkanPFNs::gpVkBindBufferMemory(mpVulkanDevicesWrapper->mpLogicalDevice, mDeviceLocalpBuffers[i], mpDeviceLocalMemory, mBufferOffsets[i]);
	}

	std::cout << "Created VulkanDeviceLocalMemory\n";
}

VulkanDeviceLocalMemory::~VulkanDeviceLocalMemory() {
	std::cout << "Destroying VulkanDeviceLocalMemory...\n";
	
	VulkanPFNs::gpVkFreeMemory(mpVulkanDevicesWrapper->mpLogicalDevice, mpDeviceLocalMemory, nullptr);
	for(VkBuffer& buffer : mDeviceLocalpBuffers) {
		VulkanPFNs::gpVkDestroyBuffer(mpVulkanDevicesWrapper->mpLogicalDevice, buffer, nullptr);
	}
	
	std::cout << "Destroyed VulkanDeviceLocalMemory\n";
}

void VulkanDeviceLocalMemory::copyToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS) {
	VkCommandPool tempCommandPool{};
	VkCommandBuffer tempCommandBuffer{};

	// create transient command pool
	{
		const VkCommandPoolCreateInfo COMMAND_POOL_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(mpVulkanDevicesWrapper->mpPhysicalDevice)
		};
		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkCreateCommandPool(mpVulkanDevicesWrapper->mpLogicalDevice, &COMMAND_POOL_INFO, nullptr, &tempCommandPool),
			"Failed to create temporary command pool"
		)
	}

	// create command buffer
	{
		const VkCommandBufferAllocateInfo COMMAND_BUFFER_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = tempCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkAllocateCommandBuffers(mpVulkanDevicesWrapper->mpLogicalDevice, &COMMAND_BUFFER_INFO, &tempCommandBuffer),
			"Failed to create temporary command buffer"
		)
	}

	// record the copy
	{
		const VkCommandBufferBeginInfo ONE_TIME_SUBMIT_BEGIN(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkBeginCommandBuffer(tempCommandBuffer, &ONE_TIME_SUBMIT_BEGIN),
			"Failed to begin temporary command buffer recording"
		)
		VulkanPFNs::gpVkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, mDeviceLocalpBuffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkEndCommandBuffer(tempCommandBuffer),
			"Failed to end temporary command buffer recording"
		)
	}

	// create fence to wait on
	VkFence copyCommandDone{};
	{
		const VkFenceCreateInfo FENCE_INFO(VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, 0);
		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkCreateFence(mpVulkanDevicesWrapper->mpLogicalDevice, &FENCE_INFO, nullptr, &copyCommandDone),
			"Failed to create copy command done fence"
		)
	}

	// submit it right away
	{
		const VkSubmitInfo ONE_TIME_SUBMIT_INFO{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &tempCommandBuffer,
		};

		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkQueueSubmit(mpVulkanDevicesWrapper->mGraphicsFamilypQueues[0], 1, &ONE_TIME_SUBMIT_INFO, copyCommandDone),
			"Failed to submit temporary command buffer"
		)
	}

	for(VkBufferCopy const& REGION : COPY_REGIONS) {
		std::cout << "Copied buffer:\n";
		std::cout << "\tSource offset: " << REGION.srcOffset << "\n";
		std::cout << "\tDestination offset: " << REGION.dstOffset << "\n";
		std::cout << "\tBytes: " << REGION.size << "\n";
	}

	CHECK_VK_SUCCESS(
		VulkanPFNs::gpVkWaitForFences(mpVulkanDevicesWrapper->mpLogicalDevice, 1, &copyCommandDone, VK_TRUE, UINT64_MAX),
		"Failed to wait for copy command done fence"
	)

	VulkanPFNs::gpVkDestroyFence(mpVulkanDevicesWrapper->mpLogicalDevice, copyCommandDone, nullptr);
	VulkanPFNs::gpVkFreeCommandBuffers(mpVulkanDevicesWrapper->mpLogicalDevice, tempCommandPool, 1, &tempCommandBuffer);
	VulkanPFNs::gpVkDestroyCommandPool(mpVulkanDevicesWrapper->mpLogicalDevice, tempCommandPool, nullptr);
}