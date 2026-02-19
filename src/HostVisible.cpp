#include <climits>
#include <utility>
#include <cstring>
#include <iostream>
#include "HostVisible.hpp"

namespace DeviceMemory {
	HostVisible::HostVisible(Backend::Devices* givenDevices, CreateInfo&& givenCreateInfo, void (*const populate)(DeviceMemory::HostVisible& self) :
		devices{ givenDevices },
		hostVisibleMemory{},
		CREATE_INFO(std::move(givenCreateInfo)),
		buffers{},
		bufferOffsets{},
		bufferSizes{},
		descriptorPool{}, 
		descriptorSetLayouts{},
		descriptorSets{} {

		// memory and buffer stuff
		{
			const size_t BUFFERS_COUNT{ bufferInfos.size() };

			// create the buffers themselves with their memory requirements info
			buffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
			bufferSizes.resize(BUFFERS_COUNT, 0);
			std::vector<VkMemoryRequirements> buffersMemoryRequirements(BUFFERS_COUNT, {});
			for (int i = 0; i < BUFFERS_COUNT; i++) {
				buffers[i] = Common::fCreateBuffer(devices->logicalDevice, bufferInfos[i]);
				vkGetBufferMemoryRequirements(devices->logicalDevice, buffers[i], &buffersMemoryRequirements[i]);
				bufferSizes[i] = buffersMemoryRequirements[i].size;
			}

			// create the memory
			VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = Common::fGetMemoryAllocationSizeAndOffsets(buffersMemoryRequirements).first,
				.memoryTypeIndex = Common::fGetMemoryTypeIndex(devices->physicalDevice, buffersMemoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			};
			vkAllocateMemory(devices->logicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &hostVisibleMemory);
	
			// bind buffers
			bufferOffsets = Common::fGetMemoryAllocationSizeAndOffsets(buffersMemoryRequirements).second;
			for(int i = 0; i < BUFFERS_COUNT; i++) {
				vkBindBufferMemory(devices->logicalDevice, buffers[i], hostVisibleMemory, bufferOffsets[i]);
			}
		}

		// descriptor set stuff
		if(!descriptorSetInfos.empty()) {
			descriptorPool = Common::fCreateDescriptorPool(devices->logicalDevice, descriptorSetInfos);

			// create the descriptor sets
			descriptorSetLayouts.resize(descriptorSetInfos.size(), VK_NULL_HANDLE);
			descriptorSets.resize(descriptorSetInfos.size(), VK_NULL_HANDLE);

			for(size_t i = 0; i < descriptorSetInfos.size(); i++) {
				createDescriptorSetAndLayout(descriptorSetInfos[i], i);
			}
		}

		pPOPULATE_FUNCTION(*this);
	}

	HostVisible::~HostVisible() {
		vkFreeMemory(devices->logicalDevice, hostVisibleMemory, nullptr);
		for(VkBuffer& buffer : buffers) {
			vkDestroyBuffer(devices->logicalDevice, buffer, nullptr);
		}
	
		for(size_t i = 0; i < descriptorSets.size(); i++) {
			vkFreeDescriptorSets(devices->logicalDevice, descriptorPool, 1, &descriptorSets[i]);
			vkDestroyDescriptorSetLayout(devices->logicalDevice, descriptorSetLayouts[i], nullptr);
		}
		if(descriptorPool) {
			vkDestroyDescriptorPool(devices->logicalDevice, descriptorPool, nullptr);
		}
	}

	void HostVisible::writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES) {
		void* mappedMemory{};

		CHECK_VK_SUCCESS(
		vkMapMemory(devices->logicalDevice, hostVisibleMemory, bufferOffsets[INDEX], bufferSizes[INDEX], 0, &mappedMemory),
		"Failed to map memory"
		)

		std::memcpy(mappedMemory, pDATA, NUM_BYTES);
		vkUnmapMemory(devices->logicalDevice, hostVisibleMemory);
	}

	void HostVisible::createDescriptorSetAndLayout(Common::DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.mLayoutBindings.size()),
			.pBindings = INFO.mLayoutBindings.data(),
		};

		CHECK_VK_SUCCESS(
			vkCreateDescriptorSetLayout(devices->logicalDevice, &DESCRIPTOR_SET_LAYOUT_INFO, nullptr, &descriptorSetLayouts[INDEX]),
			"Failed to create descriptor set layout"
		)

		const VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = descriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptorSetLayouts[INDEX],
		};

		CHECK_VK_SUCCESS(
			vkAllocateDescriptorSets(devices->logicalDevice, &DESCRIPTOR_SET_ALLOCATE_INFO, &descriptorSets[INDEX]),
			"Failed to create descriptor set"
		)
	}

	void HostVisible::updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}
	
		std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
		for(size_t const& BUFFER_INDEX : BUFFER_INDICES) {
			toWriteBuffers.emplace_back(buffers[BUFFER_INDEX], 0, VK_WHOLE_SIZE);
		}
	
		const VkWriteDescriptorSet WRITE_INFO{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[SET_INDEX],
			.dstBinding = SET_BINDING_NUM,
			.dstArrayElement = 0,
			.descriptorCount = descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorType,
			.pBufferInfo = toWriteBuffers.data()
		};

		vkUpdateDescriptorSets(devices->logicalDevice, 1, &WRITE_INFO, 0, nullptr);
	}
}