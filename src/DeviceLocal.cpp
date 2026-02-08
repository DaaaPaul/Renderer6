#include "DeviceLocal.hpp"
#include <iostream>

namespace DeviceMemory {
	DeviceLocal::DeviceLocal() :
		mpDevices{},
		mpDeviceLocalMemory{},
		mDeviceLocalBufferInfos{},
		mDeviceLocalpBuffers{},
		mBufferOffsets{},
		mBufferSizes{}, 
		mpDescriptorPool{}, 
		mDeviceLocalDescriptorSetInfos{},
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {}

	DeviceLocal::DeviceLocal(Backend::Devices* pGivenDevices, std::vector<Common::BufferInfo> const& GIVEN_BUFFER_INFO, std::vector<Common::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS) : 
		mpDevices{ pGivenDevices },
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
		for(Common::BufferInfo const& INFO : mDeviceLocalBufferInfos) {
			std::cout << "\tsize: " << INFO.mBUFFER_SIZE << "\n";
			std::cout << "\tusage: " << INFO.mBUFFER_USAGE << "\n";
		}

		std::cout << "Creating DeviceLocal...\n";

		// memory and buffer stuff
		{
			const size_t BUFFERS_COUNT{ mDeviceLocalBufferInfos.size() };

			// create the buffers themselves with their memory requirements info
			mDeviceLocalpBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
			mBufferSizes.resize(BUFFERS_COUNT, 0);
			std::vector<VkMemoryRequirements> deviceLocalBuffersMemRequirements(BUFFERS_COUNT, {});
			for (int i = 0; i < BUFFERS_COUNT; i++) {
				mDeviceLocalpBuffers[i] = Common::fCreateBuffer(mpDevices->mpLogicalDevice, mDeviceLocalBufferInfos[i]);
				vkGetBufferMemoryRequirements(mpDevices->mpLogicalDevice, mDeviceLocalpBuffers[i], &deviceLocalBuffersMemRequirements[i]);
				mBufferSizes[i] = deviceLocalBuffersMemRequirements[i].size;
			}

			// create the memory
			VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = Common::fGetMemoryAllocationSizeAndOffsets(deviceLocalBuffersMemRequirements).first,
				.memoryTypeIndex = Common::fGetMemoryTypeIndex(mpDevices->mpPhysicalDevice, deviceLocalBuffersMemRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			};
			vkAllocateMemory(mpDevices->mpLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mpDeviceLocalMemory);

			// bind buffers
			mBufferOffsets = Common::fGetMemoryAllocationSizeAndOffsets(deviceLocalBuffersMemRequirements).second;
			for(int i = 0; i < BUFFERS_COUNT; i++) {
				vkBindBufferMemory(mpDevices->mpLogicalDevice, mDeviceLocalpBuffers[i], mpDeviceLocalMemory, mBufferOffsets[i]);
			}
		}

		// descriptor set stuff
		if(!GIVEN_DESCRIPTOR_SET_INFOS.empty()) {
			mpDescriptorPool = Common::fCreateDescriptorPool(mpDevices->mpLogicalDevice, mDeviceLocalDescriptorSetInfos);

			// create the descriptor sets
			mDescriptorpSetLayouts.resize(mDeviceLocalDescriptorSetInfos.size(), VK_NULL_HANDLE);
			mDescriptorpSets.resize(mDeviceLocalDescriptorSetInfos.size(), VK_NULL_HANDLE);

			for(size_t i = 0; i < mDeviceLocalDescriptorSetInfos.size(); i++) {
				createDescriptorSet(mDeviceLocalDescriptorSetInfos[i], i);
				std::cout << "Created descriptor set layout and set for device local memory\n";
			}
		}


		std::cout << "Created DeviceLocal\n";
	}

	DeviceLocal::~DeviceLocal() {
		std::cout << "Destroying DeviceLocal...\n";
	
		vkFreeMemory(mpDevices->mpLogicalDevice, mpDeviceLocalMemory, nullptr);
		for(VkBuffer& buffer : mDeviceLocalpBuffers) {
			vkDestroyBuffer(mpDevices->mpLogicalDevice, buffer, nullptr);
		}

		for(size_t i = 0; i < mDescriptorpSets.size(); i++) {
			vkFreeDescriptorSets(mpDevices->mpLogicalDevice, mpDescriptorPool, 1, &mDescriptorpSets[i]);
			vkDestroyDescriptorSetLayout(mpDevices->mpLogicalDevice, mDescriptorpSetLayouts[i], nullptr);
		}
		if(mpDescriptorPool) {
			vkDestroyDescriptorPool(mpDevices->mpLogicalDevice, mpDescriptorPool, nullptr);
		}
	
		std::cout << "Destroyed DeviceLocal\n";
	}

	void DeviceLocal::copyToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		// create transient command pool
		{
			const VkCommandPoolCreateInfo COMMAND_POOL_INFO{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
				.queueFamilyIndex = mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX
			};
			CHECK_VK_SUCCESS(
				vkCreateCommandPool(mpDevices->mpLogicalDevice, &COMMAND_POOL_INFO, nullptr, &tempCommandPool),
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
				vkAllocateCommandBuffers(mpDevices->mpLogicalDevice, &COMMAND_BUFFER_INFO, &tempCommandBuffer),
				"Failed to create temporary command buffer"
			)
		}

		// record the copy
		{
			const VkCommandBufferBeginInfo ONE_TIME_SUBMIT_BEGIN(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
			CHECK_VK_SUCCESS(
				vkBeginCommandBuffer(tempCommandBuffer, &ONE_TIME_SUBMIT_BEGIN),
				"Failed to begin temporary command buffer recording"
			)
			vkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, mDeviceLocalpBuffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
			CHECK_VK_SUCCESS(
				vkEndCommandBuffer(tempCommandBuffer),
				"Failed to end temporary command buffer recording"
			)
		}

		// create fence to wait on
		VkFence copyCommandDone{};
		{
			const VkFenceCreateInfo FENCE_INFO(VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, 0);
			CHECK_VK_SUCCESS(
				vkCreateFence(mpDevices->mpLogicalDevice, &FENCE_INFO, nullptr, &copyCommandDone),
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
				vkQueueSubmit(mpDevices->mGraphicsFamilypQueues[0], 1, &ONE_TIME_SUBMIT_INFO, copyCommandDone),
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
			vkWaitForFences(mpDevices->mpLogicalDevice, 1, &copyCommandDone, VK_TRUE, UINT64_MAX),
			"Failed to wait for copy command done fence"
		)

		vkDestroyFence(mpDevices->mpLogicalDevice, copyCommandDone, nullptr);
		vkFreeCommandBuffers(mpDevices->mpLogicalDevice, tempCommandPool, 1, &tempCommandBuffer);
		vkDestroyCommandPool(mpDevices->mpLogicalDevice, tempCommandPool, nullptr);
	}

	void DeviceLocal::createDescriptorSet(Common::DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.mLAYOUT_BINDINGS.size()),
			.pBindings = INFO.mLAYOUT_BINDINGS.data(),
		};

		CHECK_VK_SUCCESS(
			vkCreateDescriptorSetLayout(mpDevices->mpLogicalDevice, &DESCRIPTOR_SET_LAYOUT_INFO, nullptr, &mDescriptorpSetLayouts[INDEX]),
			"Failed to create descriptor set layout"
		)

		const VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = mpDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &mDescriptorpSetLayouts[INDEX],
		};

		CHECK_VK_SUCCESS(
			vkAllocateDescriptorSets(mpDevices->mpLogicalDevice, &DESCRIPTOR_SET_ALLOCATE_INFO, &mDescriptorpSets[INDEX]),
			"Failed to create descriptor set"
		)
	}

	void DeviceLocal::updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != mDeviceLocalDescriptorSetInfos[SET_INDEX].mLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorCount) {
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
			.descriptorCount = mDeviceLocalDescriptorSetInfos[SET_INDEX].mLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorCount,
			.descriptorType = mDeviceLocalDescriptorSetInfos[SET_INDEX].mLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorType,
			.pBufferInfo = toWriteBuffers.data()
		};

		vkUpdateDescriptorSets(mpDevices->mpLogicalDevice, 1, &WRITE_INFO, 0, nullptr);

		std::cout << "Set " << SET_INDEX << " binding " << SET_BINDING_NUM << " now describes buffer(s) ";
		for(size_t const& INDEX : BUFFER_INDICES) {
			std::cout << INDEX << " ";
		}
		std::cout << "\n";
	}
}
