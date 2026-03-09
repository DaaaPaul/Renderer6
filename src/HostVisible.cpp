#include <utility>
#include <cstring>
#include <iostream>
#include "HostVisible.hpp"

namespace DeviceMemory {
	HostVisible::HostVisible(Backend::Devices* pGivenDevices, CreateInfo&& givenCreateInfo, std::function<void(HostVisible&)> const& POPULATE_FUNCTION) :
		pDevices{ pGivenDevices },
		pHostVisibleMemory{},
		CREATE_INFO(std::move(givenCreateInfo)),
		buffers{},
		bufferOffsets{},
		bufferSizes{},
		pDescriptorPool{}, 
		descriptorSetLayouts{},
		descriptorSets{} {

		// memory and buffer stuff
		{
			const size_t BUFFERS_COUNT{ CREATE_INFO.bufferInfos.size() };

			// create the buffers themselves with their memory requirements info
			buffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
			bufferSizes.resize(BUFFERS_COUNT, 0);
			std::vector<VkMemoryRequirements> buffersMemoryRequirements(BUFFERS_COUNT, {});
			for (int i = 0; i < BUFFERS_COUNT; i++) {
				buffers[i] = createBuffer(pDevices->getLogicalDevice(), CREATE_INFO.bufferInfos[i]);
				vkGetBufferMemoryRequirements(pDevices->getLogicalDevice(), buffers[i], &buffersMemoryRequirements[i]);
				bufferSizes[i] = buffersMemoryRequirements[i].size;
			}

			// create the memory
			VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = getMemoryAllocationSizeAndOffsets(buffersMemoryRequirements).first,
				.memoryTypeIndex = getMemoryTypeIndex(pDevices->getPhysicalDevice(), buffersMemoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			};
			vkAllocateMemory(pDevices->getLogicalDevice(), &hostVisibleMemoryAllocateInfo, nullptr, &pHostVisibleMemory);
	
			// bind buffers
			bufferOffsets = getMemoryAllocationSizeAndOffsets(buffersMemoryRequirements).second;
			for(int i = 0; i < BUFFERS_COUNT; i++) {
				vkBindBufferMemory(pDevices->getLogicalDevice(), buffers[i], pHostVisibleMemory, bufferOffsets[i]);
			}
		}

		// descriptor set stuff
		if(!CREATE_INFO.descriptorSetInfos.empty()) {
			const size_t DESCRIPTOR_SET_COUNT = CREATE_INFO.descriptorSetInfos.size();
			pDescriptorPool = createDescriptorPool(pDevices->getLogicalDevice(), CREATE_INFO.descriptorSetInfos);

			// create the descriptor sets
			descriptorSetLayouts.resize(DESCRIPTOR_SET_COUNT, VK_NULL_HANDLE);
			descriptorSets.resize(DESCRIPTOR_SET_COUNT, VK_NULL_HANDLE);

			for(size_t i = 0; i < DESCRIPTOR_SET_COUNT; i++) {
				createDescriptorSetAndLayout(CREATE_INFO.descriptorSetInfos[i], i);
			}
		}

		POPULATE_FUNCTION(*this);
	}

	HostVisible::~HostVisible() {
		vkFreeMemory(pDevices->getLogicalDevice(), pHostVisibleMemory, nullptr);
		for(VkBuffer& buffer : buffers) {
			vkDestroyBuffer(pDevices->getLogicalDevice(), buffer, nullptr);
		}
	
		for(size_t i = 0; i < descriptorSets.size(); i++) {
			vkFreeDescriptorSets(pDevices->getLogicalDevice(), pDescriptorPool, 1, &descriptorSets[i]);
			vkDestroyDescriptorSetLayout(pDevices->getLogicalDevice(), descriptorSetLayouts[i], nullptr);
		}
		if(pDescriptorPool) {
			vkDestroyDescriptorPool(pDevices->getLogicalDevice(), pDescriptorPool, nullptr);
		}
	}

	void HostVisible::writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES) {
		void* pMappedMemory{};

		CHECK_VK_SUCCESS(
		vkMapMemory(pDevices->getLogicalDevice(), pHostVisibleMemory, bufferOffsets[INDEX], bufferSizes[INDEX], 0, &pMappedMemory),
		"Failed to map memory"
		)

		std::memcpy(pMappedMemory, pDATA, NUM_BYTES);
		vkUnmapMemory(pDevices->getLogicalDevice(), pHostVisibleMemory);
	}

	void HostVisible::createDescriptorSetAndLayout(DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.layoutBindings.size()),
			.pBindings = INFO.layoutBindings.data(),
		};

		CHECK_VK_SUCCESS(
			vkCreateDescriptorSetLayout(pDevices->getLogicalDevice(), &DESCRIPTOR_SET_LAYOUT_INFO, nullptr, &descriptorSetLayouts[INDEX]),
			"Failed to create descriptor set layout"
		)

		const VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = pDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptorSetLayouts[INDEX],
		};

		CHECK_VK_SUCCESS(
			vkAllocateDescriptorSets(pDevices->getLogicalDevice(), &DESCRIPTOR_SET_ALLOCATE_INFO, &descriptorSets[INDEX]),
			"Failed to create descriptor set"
		)
	}

	void HostVisible::updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != CREATE_INFO.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount) {
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
			.descriptorCount = CREATE_INFO.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = CREATE_INFO.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorType,
			.pBufferInfo = toWriteBuffers.data()
		};

		vkUpdateDescriptorSets(pDevices->getLogicalDevice(), 1, &WRITE_INFO, 0, nullptr);
	}
}