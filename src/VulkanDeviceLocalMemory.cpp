#include "VulkanDeviceLocalMemory.hpp"
#include <iostream>

VulkanDeviceLocalMemory::VulkanDeviceLocalMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::BufferInfo> const& GIVEN_BUFFER_INFO, std::vector<VulkanMemoryCommon::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS) : 
	mpVulkanDevicesWrapper{ pGivenVulkanDevicesWrapper },
	mpDeviceLocalMemory{},
	mDeviceLocalBufferInfos{ GIVEN_BUFFER_INFO },
	mDeviceLocalpBuffers{},
	mBufferOffsets{},
	mBufferSizes{}, 
	mpDescriptorPool{}, 
	mDeviceLocalDescriptorSetInfos{ GIVEN_DESCRIPTOR_SET_INFOS },
	mDescriptorpSetLayouts{},
	mDescriptorpSets{} {

	std::cout << "DEVICE LOCAL MEMORY PARAMETERS:\n";
	for(VulkanMemoryCommon::BufferInfo const& INFO : mDeviceLocalBufferInfos) {
		std::cout << "\tsize: " << INFO.mBUFFER_SIZE << "\n";
		std::cout << "\tusage: " << INFO.mBUFFER_USAGE << "\n";
	}

	std::cout << "Creating VulkanDeviceLocalMemory...\n";

	// memory and buffer stuff
	{
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
	}

	// descriptor set stuff
	if(!GIVEN_DESCRIPTOR_SET_INFOS.empty()) {
		mpDescriptorPool = VulkanMemoryCommon::fCreateDescriptorPool(mpVulkanDevicesWrapper->mpLogicalDevice, mDeviceLocalDescriptorSetInfos);

		// create the descriptor sets
		mDescriptorpSetLayouts.resize(mDeviceLocalDescriptorSetInfos.size(), VK_NULL_HANDLE);
		mDescriptorpSets.resize(mDeviceLocalDescriptorSetInfos.size(), VK_NULL_HANDLE);

		for(size_t i = 0; i < mDeviceLocalDescriptorSetInfos.size(); i++) {
			createDescriptorSet(mDeviceLocalDescriptorSetInfos[i], i);
			std::cout << "Created descriptor set layout and set for device local memory\n";
		}
	}


	std::cout << "Created VulkanDeviceLocalMemory\n";
}

VulkanDeviceLocalMemory::~VulkanDeviceLocalMemory() {
	std::cout << "Destroying VulkanDeviceLocalMemory...\n";
	
	VulkanPFNs::gpVkFreeMemory(mpVulkanDevicesWrapper->mpLogicalDevice, mpDeviceLocalMemory, nullptr);
	for(VkBuffer& buffer : mDeviceLocalpBuffers) {
		VulkanPFNs::gpVkDestroyBuffer(mpVulkanDevicesWrapper->mpLogicalDevice, buffer, nullptr);
	}

	for(size_t i = 0; i < mDescriptorpSets.size(); i++) {
		VulkanPFNs::gpVkFreeDescriptorSets(mpVulkanDevicesWrapper->mpLogicalDevice, mpDescriptorPool, 1, &mDescriptorpSets[i]);
		VulkanPFNs::gpVkDestroyDescriptorSetLayout(mpVulkanDevicesWrapper->mpLogicalDevice, mDescriptorpSetLayouts[i], nullptr);
	}
	if(mpDescriptorPool) {
		VulkanPFNs::gpVkDestroyDescriptorPool(mpVulkanDevicesWrapper->mpLogicalDevice, mpDescriptorPool, nullptr);
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

void VulkanDeviceLocalMemory::createDescriptorSet(VulkanMemoryCommon::DescriptorSetInfo const& INFO, size_t const& INDEX) {
	const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.flags = 0,
		.bindingCount = INFO.mBINDINGS_COUNT,
		.pBindings = INFO.mpLAYOUT_BINDINGS,
	};

	CHECK_VK_SUCCESS(
		VulkanPFNs::gpVkCreateDescriptorSetLayout(mpVulkanDevicesWrapper->mpLogicalDevice, &DESCRIPTOR_SET_LAYOUT_INFO, nullptr, &mDescriptorpSetLayouts[INDEX]),
		"Failed to create descriptor set layout"
	)

	const VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = mpDescriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &mDescriptorpSetLayouts[INDEX],
	};

	CHECK_VK_SUCCESS(
		VulkanPFNs::gpVkAllocateDescriptorSets(mpVulkanDevicesWrapper->mpLogicalDevice, &DESCRIPTOR_SET_ALLOCATE_INFO, &mDescriptorpSets[INDEX]),
		"Failed to create descriptor set"
	)
}

void VulkanDeviceLocalMemory::updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
	if(BUFFER_INDICES.size() != mDeviceLocalDescriptorSetInfos[SET_INDEX].mpLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorCount) {
		throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
	}
	
	std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
	for(size_t const& BUFFER_INDEX : BUFFER_INDICES) {
		toWriteBuffers.emplace_back(mDeviceLocalpBuffers[BUFFER_INDEX], 0, VK_WHOLE_SIZE);
	}
	
	const VkWriteDescriptorSet WRITE_INFO{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = mDescriptorpSets[SET_INDEX],
		.dstBinding = SET_BINDING_NUM,
		.dstArrayElement = 0,
		.descriptorCount = mDeviceLocalDescriptorSetInfos[SET_INDEX].mpLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorCount,
		.descriptorType = mDeviceLocalDescriptorSetInfos[SET_INDEX].mpLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorType,
		.pBufferInfo = toWriteBuffers.data()
	};

	VulkanPFNs::gpVkUpdateDescriptorSets(mpVulkanDevicesWrapper->mpLogicalDevice, 1, &WRITE_INFO, 0, nullptr);

	std::cout << "Set " << SET_INDEX << " binding " << SET_BINDING_NUM << " now describes buffer(s) ";
	for(size_t const& INDEX : BUFFER_INDICES) {
		std::cout << INDEX << " ";
	}
	std::cout << "\n";
}