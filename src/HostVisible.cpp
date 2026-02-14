#include <climits>
#include <utility>
#include <cstring>
#include <iostream>
#include "HostVisible.hpp"

namespace DeviceMemory {
	HostVisible::HostVisible() :
		mpDevices{},
		mpHostVisibleMemory{},
		mpBuffers{},
		mBufferInfos{},
		mBufferOffsets{},
		mBufferSizes{},
		mpDescriptorPool{}, 
		mDescriptorSetInfos{},
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {}

	HostVisible::HostVisible(Backend::Devices* pGivenDevices, const DeviceMemory::Common::HostVisibleConstructArguements (*const pCONSTRUCT_FUNCTION)(), void (*const pPOPULATE_FUNCTION)(DeviceMemory::HostVisible& toBePopulated)) :
		mpDevices{ pGivenDevices },
		mpHostVisibleMemory{},
		mpBuffers{},
		mBufferInfos{ pCONSTRUCT_FUNCTION().mBufferInfos },
		mBufferOffsets{},
		mBufferSizes{},
		mpDescriptorPool{}, 
		mDescriptorSetInfos{ pCONSTRUCT_FUNCTION().mDescriptorSetInfos },
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {

		// memory and buffer stuff
		{
			const size_t BUFFERS_COUNT{ mBufferInfos.size() };

			// create the buffers themselves with their memory requirements info
			mpBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
			mBufferSizes.resize(BUFFERS_COUNT, 0);
			std::vector<VkMemoryRequirements> buffersMemoryRequirements(BUFFERS_COUNT, {});
			for (int i = 0; i < BUFFERS_COUNT; i++) {
				mpBuffers[i] = Common::fCreateBuffer(mpDevices->mpLogicalDevice, mBufferInfos[i]);
				vkGetBufferMemoryRequirements(mpDevices->mpLogicalDevice, mpBuffers[i], &buffersMemoryRequirements[i]);
				mBufferSizes[i] = buffersMemoryRequirements[i].size;
			}

			// create the memory
			VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = Common::fGetMemoryAllocationSizeAndOffsets(buffersMemoryRequirements).first,
				.memoryTypeIndex = Common::fGetMemoryTypeIndex(mpDevices->mpPhysicalDevice, buffersMemoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			};
			vkAllocateMemory(mpDevices->mpLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mpHostVisibleMemory);
	
			// bind buffers
			mBufferOffsets = Common::fGetMemoryAllocationSizeAndOffsets(buffersMemoryRequirements).second;
			for(int i = 0; i < BUFFERS_COUNT; i++) {
				vkBindBufferMemory(mpDevices->mpLogicalDevice, mpBuffers[i], mpHostVisibleMemory, mBufferOffsets[i]);
			}
		}

		// descriptor set stuff
		if(!mDescriptorSetInfos.empty()) {
			mpDescriptorPool = Common::fCreateDescriptorPool(mpDevices->mpLogicalDevice, mDescriptorSetInfos);

			// create the descriptor sets
			mDescriptorpSetLayouts.resize(mDescriptorSetInfos.size(), VK_NULL_HANDLE);
			mDescriptorpSets.resize(mDescriptorSetInfos.size(), VK_NULL_HANDLE);

			for(size_t i = 0; i < mDescriptorSetInfos.size(); i++) {
				createDescriptorSet(mDescriptorSetInfos[i], i);
			}
		}

		pPOPULATE_FUNCTION(*this);
	}

	HostVisible::~HostVisible() {
		vkFreeMemory(mpDevices->mpLogicalDevice, mpHostVisibleMemory, nullptr);
		for(VkBuffer& buffer : mpBuffers) {
			vkDestroyBuffer(mpDevices->mpLogicalDevice, buffer, nullptr);
		}
	
		for(size_t i = 0; i < mDescriptorpSets.size(); i++) {
			vkFreeDescriptorSets(mpDevices->mpLogicalDevice, mpDescriptorPool, 1, &mDescriptorpSets[i]);
			vkDestroyDescriptorSetLayout(mpDevices->mpLogicalDevice, mDescriptorpSetLayouts[i], nullptr);
		}
		if(mpDescriptorPool) {
			vkDestroyDescriptorPool(mpDevices->mpLogicalDevice, mpDescriptorPool, nullptr);
		}
	}

	void HostVisible::writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES) {
		void* mappedMemory{};

		CHECK_VK_SUCCESS(
		vkMapMemory(mpDevices->mpLogicalDevice, mpHostVisibleMemory, mBufferOffsets[INDEX], mBufferSizes[INDEX], 0, &mappedMemory),
		"Failed to map memory"
		)

		std::memcpy(mappedMemory, pDATA, NUM_BYTES);
		vkUnmapMemory(mpDevices->mpLogicalDevice, mpHostVisibleMemory);
	}

	void HostVisible::createDescriptorSet(Common::DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.mLayoutBindings.size()),
			.pBindings = INFO.mLayoutBindings.data(),
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

	void HostVisible::updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != mDescriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}
	
		std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
		for(size_t const& BUFFER_INDEX : BUFFER_INDICES) {
			toWriteBuffers.emplace_back(mpBuffers[BUFFER_INDEX], 0, VK_WHOLE_SIZE);
		}
	
		const VkWriteDescriptorSet WRITE_INFO{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = mDescriptorpSets[SET_INDEX],
			.dstBinding = SET_BINDING_NUM,
			.dstArrayElement = 0,
			.descriptorCount = mDescriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = mDescriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorType,
			.pBufferInfo = toWriteBuffers.data()
		};

		vkUpdateDescriptorSets(mpDevices->mpLogicalDevice, 1, &WRITE_INFO, 0, nullptr);
	}
}