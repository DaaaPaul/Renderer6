#include <climits>
#include <utility>
#include <cstring>
#include <iostream>
#include "HostVisible.hpp"

namespace DeviceMemory {
	HostVisible::HostVisible() :
		mpDevices{},
		mpHostVisibleMemory{},
		mHostVisiblepBuffers{},
		mHostVisibleBufferInfos{},
		mBufferOffsets{},
		mBufferSizes{},
		mpDescriptorPool{}, 
		mDeviceLocalDescriptorSetInfos{},
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {}

	HostVisible::HostVisible(Backend::Devices* pGivenDevices, std::vector<Common::BufferInfo> const& GIVEN_BUFFER_INFO, std::vector<Common::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS) :
		mpDevices{ pGivenDevices },
		mpHostVisibleMemory{},
		mHostVisiblepBuffers{},
		mHostVisibleBufferInfos{ GIVEN_BUFFER_INFO },
		mBufferOffsets{},
		mBufferSizes{},
		mpDescriptorPool{}, 
		mDeviceLocalDescriptorSetInfos{ GIVEN_DESCRIPTOR_SET_INFOS },
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {

		std::cout << "HOST VISIBLE MEMORY PARAMETERS:\n";
		for(Common::BufferInfo const& INFO : mHostVisibleBufferInfos) {
			std::cout << "\tsize: " << INFO.mBUFFER_SIZE << "\n";
			std::cout << "\tusage: " << INFO.mBUFFER_USAGE << "\n";
		}

		std::cout << "Creating HostVisible...\n";

		// buffer stuff
		{
			const size_t BUFFERS_COUNT{ mHostVisibleBufferInfos.size() };

			// create the buffers themselves with their memory requirements info
			mHostVisiblepBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
			mBufferSizes.resize(BUFFERS_COUNT, 0);
			std::vector<VkMemoryRequirements> hostVisibleBuffersMemRequirements(BUFFERS_COUNT, {});
			for (int i = 0; i < BUFFERS_COUNT; i++) {
				mHostVisiblepBuffers[i] = Common::fCreateBuffer(mpDevices->mpLogicalDevice, mHostVisibleBufferInfos[i]);
				vkGetBufferMemoryRequirements(mpDevices->mpLogicalDevice, mHostVisiblepBuffers[i], &hostVisibleBuffersMemRequirements[i]);
				mBufferSizes[i] = hostVisibleBuffersMemRequirements[i].size;
			}

			// create the memory
			VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = Common::fGetMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).first,
				.memoryTypeIndex = Common::fGetMemoryTypeIndex(mpDevices->mpPhysicalDevice, hostVisibleBuffersMemRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			};
			vkAllocateMemory(mpDevices->mpLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mpHostVisibleMemory);
	
			// bind buffers
			mBufferOffsets = Common::fGetMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).second;
			for(int i = 0; i < BUFFERS_COUNT; i++) {
				vkBindBufferMemory(mpDevices->mpLogicalDevice, mHostVisiblepBuffers[i], mpHostVisibleMemory, mBufferOffsets[i]);
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
				std::cout << "Created descriptor set layout and set for host visible memory\n";
			}
		}

		std::cout << "Created HostVisible\n";
	}

	HostVisible::~HostVisible() {
		std::cout << "Destroying HostVisible...\n";
	
		vkFreeMemory(mpDevices->mpLogicalDevice, mpHostVisibleMemory, nullptr);
		for(VkBuffer& buffer : mHostVisiblepBuffers) {
			vkDestroyBuffer(mpDevices->mpLogicalDevice, buffer, nullptr);
		}
	
		for(size_t i = 0; i < mDescriptorpSets.size(); i++) {
			vkFreeDescriptorSets(mpDevices->mpLogicalDevice, mpDescriptorPool, 1, &mDescriptorpSets[i]);
			vkDestroyDescriptorSetLayout(mpDevices->mpLogicalDevice, mDescriptorpSetLayouts[i], nullptr);
		}
		if(mpDescriptorPool) {
			vkDestroyDescriptorPool(mpDevices->mpLogicalDevice, mpDescriptorPool, nullptr);
		}

		std::cout << "Destroyed HostVisible\n";
	}

	void HostVisible::writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES) {
		void* mappedMemory{};
		CHECK_VK_SUCCESS(
		vkMapMemory(mpDevices->mpLogicalDevice, mpHostVisibleMemory, mBufferOffsets[INDEX], mBufferSizes[INDEX], 0, &mappedMemory),
		"Failed to map memory"
		)
		std::memcpy(mappedMemory, pDATA, NUM_BYTES);
		vkUnmapMemory(mpDevices->mpLogicalDevice, mpHostVisibleMemory);

		// std::cout << "Wrote " << NUM_BYTES << " bytes of data from " << pDATA << " to host visible memory offset " << mBufferOffsets[INDEX] << " (address " << mappedMemory << ")" << "\n";
	}

	void HostVisible::createDescriptorSet(Common::DescriptorSetInfo const& INFO, size_t const& INDEX) {
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

	void HostVisible::updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != mDeviceLocalDescriptorSetInfos[SET_INDEX].mLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}
	
		std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
		for(size_t const& BUFFER_INDEX : BUFFER_INDICES) {
			toWriteBuffers.emplace_back(mHostVisiblepBuffers[BUFFER_INDEX], 0, VK_WHOLE_SIZE);
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