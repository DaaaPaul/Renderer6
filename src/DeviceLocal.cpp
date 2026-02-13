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
		mpImages{},
		mpDescriptorPool{}, 
		mDeviceLocalDescriptorSetInfos{},
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {}

	DeviceLocal::DeviceLocal(Backend::Devices* pGivenDevices, const DeviceMemory::Common::DeviceLocalConstructArguements (*const pCONSTRUCT_FUNCTION)(), void (*const pPOPULATE_FUNCTION)(DeviceMemory::DeviceLocal& toBePopulated)) : 
		mpDevices{ pGivenDevices },
		mpDeviceLocalMemory{},
		mDeviceLocalpBuffers{},
		mDeviceLocalBufferInfos{ GIVEN_BUFFER_INFO },
		mBufferOffsets{},
		mBufferSizes{}, 
		mpImages{},
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

		(*pBootFunction)();

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

	void DeviceLocal::copyBufferToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		Common::fAllocateBeginOneTimeCommandBuffer(mpDevices->mpLogicalDevice, tempCommandPool, tempCommandBuffer, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);
		vkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, mDeviceLocalpBuffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		Common::fEndSubmitDeallocateOneTimeCommandBuffer(mpDevices->mpLogicalDevice, mpDevices->mGraphicsFamilypQueues[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		Common::fAllocateBeginOneTimeCommandBuffer(mpDevices->mpLogicalDevice, tempCommandPool, tempCommandBuffer, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);
		
		// recorded commands
		Common::fTransitionImageLayout(tempCommandBuffer, mpImages[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE, 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_NONE, // none because if srcAccess is none, then there is no point of dstAccess
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);

		vkCmdCopyBufferToImage(tempCommandBuffer, SRC_BUFFER, mpImages[INDEX], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());

		Common::fTransitionImageLayout(tempCommandBuffer, mpImages[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);
		// end of recorded commands

		Common::fEndSubmitDeallocateOneTimeCommandBuffer(mpDevices->mpLogicalDevice, mpDevices->mGraphicsFamilypQueues[0], tempCommandPool, tempCommandBuffer);
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
