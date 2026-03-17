#include <utility>
#include <cstring>
#include <iostream>
#include "HostVisible.hpp"

namespace DeviceMemory {
	void HostVisible::createBuffers() {
		if(!CREATE_INFO.bufferInfos.empty()) {
			const uint16_t BUFFER_COUNT = CREATE_INFO.bufferInfos.size();

			buffers.resize(BUFFER_COUNT, VK_NULL_HANDLE);
			bufferSizes.resize(BUFFER_COUNT, 0);
			bufferOffsets.resize(BUFFER_COUNT, 0);
			bufferPointers.resize(BUFFER_COUNT, 0);
			buffersMapped.resize(BUFFER_COUNT, nullptr);

			for (int i = 0; i < BUFFER_COUNT; i++) {
				buffers[i] = createBuffer(pDevices->getLogicalDevice(), CREATE_INFO.bufferInfos[i]);
			}
		}
	}

	void HostVisible::setupMemory() {
		const uint16_t BUFFER_COUNT = CREATE_INFO.bufferInfos.size();

		std::vector<VkMemoryRequirements> bufferRequirements(BUFFER_COUNT, {});
		for (int i = 0; i < BUFFER_COUNT; i++) {
			vkGetBufferMemoryRequirements(pDevices->getLogicalDevice(), buffers[i], &bufferRequirements[i]);
			bufferSizes[i] = bufferRequirements[i].size;
		}

		VkDeviceSize memorySize = getMemoryAllocationSizeAndOffsets(bufferRequirements).first;
		std::vector<VkDeviceSize> memoryOffsets(getMemoryAllocationSizeAndOffsets(bufferRequirements).second);
		uint32_t memoryType = getMemoryTypeIndex(pDevices->getPhysicalDevice(), bufferRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkMemoryAllocateFlagsInfo addressFlag{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
			.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
		};
		VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = &addressFlag,
			.allocationSize = memorySize,
			.memoryTypeIndex = memoryType
		};
		vkAllocateMemory(pDevices->getLogicalDevice(), &hostVisibleMemoryAllocateInfo, nullptr, &pHostVisibleMemory);

		bufferOffsets.assign(memoryOffsets.begin(), memoryOffsets.begin() + BUFFER_COUNT);
		for(int i = 0; i < BUFFER_COUNT; i++) {
			vkBindBufferMemory(pDevices->getLogicalDevice(), buffers[i], pHostVisibleMemory, bufferOffsets[i]);
		}
		
		VkBufferDeviceAddressInfo rollingBuffer{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
		for(int i = 0; i < BUFFER_COUNT; i++) {
			if(CREATE_INFO.bufferInfos[i].usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
				rollingBuffer.buffer = buffers[i];
				bufferPointers[i] = vkGetBufferDeviceAddress(pDevices->getLogicalDevice(), &rollingBuffer);
				std::cout << "HOST VISIBLE MEMORY: ";
				std::cout << "Buffer " << i << " address: " << bufferPointers[i] << "\n";
			}
		}
	}

	void HostVisible::createDescriptorSets() {
		if(!CREATE_INFO.descriptorSetInfos.empty()) {
			const uint16_t DESCRIPTOR_SET_COUNT = CREATE_INFO.descriptorSetInfos.size();

			pDescriptorPool = createDescriptorPool(pDevices->getLogicalDevice(), CREATE_INFO.descriptorSetInfos);

			descriptorSetLayouts.resize(DESCRIPTOR_SET_COUNT, VK_NULL_HANDLE);
			descriptorSets.resize(DESCRIPTOR_SET_COUNT, VK_NULL_HANDLE);

			for(int i = 0; i < DESCRIPTOR_SET_COUNT; i++) {
				descriptorSetLayouts[i] = createDescriptorSetLayout(pDevices->getLogicalDevice(), CREATE_INFO.descriptorSetInfos[i]);
				descriptorSets[i] = createDescriptorSet(pDevices->getLogicalDevice(), pDescriptorPool, descriptorSetLayouts[i], CREATE_INFO.descriptorSetInfos[i]);
			}
		}
	}


	HostVisible::HostVisible(Backend::Devices* pGivenDevices, CreateInfo&& givenCreateInfo, std::function<void(HostVisible&)> const& POPULATE_FUNCTION) :
		pDevices{ pGivenDevices },
		pHostVisibleMemory{},
		CREATE_INFO(std::move(givenCreateInfo)),
		buffers{},
		bufferOffsets{},
		bufferSizes{},
		bufferPointers{},
		buffersMapped{},
		pDescriptorPool{}, 
		descriptorSetLayouts{},
		descriptorSets{} {

		createBuffers();
		setupMemory();
		createDescriptorSets();

		POPULATE_FUNCTION(*this);
	}

	HostVisible::~HostVisible() {
		vkUnmapMemory(pDevices->getLogicalDevice(), pHostVisibleMemory);
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
		void* pBuffer{};
		CHECK_VK_SUCCESS(
		vkMapMemory(pDevices->getLogicalDevice(), pHostVisibleMemory, bufferOffsets[INDEX], bufferSizes[INDEX], 0, &pBuffer),
		"Failed to map memory"
		)
		std::memcpy(pBuffer, pDATA, NUM_BYTES);
		vkUnmapMemory(pDevices->getLogicalDevice(), pHostVisibleMemory);
	}

	void HostVisible::descriptorSetBindingToBuffers(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_DESCRIPTOR_INDICES) {
		if(BUFFER_DESCRIPTOR_INDICES.size() != CREATE_INFO.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}
	
		std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
		for(size_t const& BUFFER_INDEX : BUFFER_DESCRIPTOR_INDICES) {
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