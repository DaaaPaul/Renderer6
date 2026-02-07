#include <climits>
#include <utility>
#include <cstring>
#include <iostream>
#include "VulkanHostVisibleMemory.hpp"

VulkanHostVisibleMemory::VulkanHostVisibleMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::BufferInfo> const& GIVEN_BUFFER_INFO, std::vector<VulkanMemoryCommon::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS) :
	mpVulkanDevicesWrapper{ pGivenVulkanDevicesWrapper },
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
	for(VulkanMemoryCommon::BufferInfo const& INFO : mHostVisibleBufferInfos) {
		std::cout << "\tsize: " << INFO.mBUFFER_SIZE << "\n";
		std::cout << "\tusage: " << INFO.mBUFFER_USAGE << "\n";
	}

	std::cout << "Creating VulkanHostVisibleMemory...\n";

	// buffer stuff
	{
		const size_t BUFFERS_COUNT{ mHostVisibleBufferInfos.size() };

		// create the buffers themselves with their memory requirements info
		mHostVisiblepBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
		mBufferSizes.resize(BUFFERS_COUNT, 0);
		std::vector<VkMemoryRequirements> hostVisibleBuffersMemRequirements(BUFFERS_COUNT, {});
		for (int i = 0; i < BUFFERS_COUNT; i++) {
			mHostVisiblepBuffers[i] = VulkanMemoryCommon::fCreateBuffer(mpVulkanDevicesWrapper->mpLogicalDevice, mHostVisibleBufferInfos[i]);
			VulkanPFNs::gpVkGetBufferMemoryRequirements(mpVulkanDevicesWrapper->mpLogicalDevice, mHostVisiblepBuffers[i], &hostVisibleBuffersMemRequirements[i]);
			mBufferSizes[i] = hostVisibleBuffersMemRequirements[i].size;
		}

		// create the memory
		VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).first,
			.memoryTypeIndex = VulkanMemoryCommon::fGetMemoryTypeIndex(mpVulkanDevicesWrapper->mpPhysicalDevice, hostVisibleBuffersMemRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		};
		VulkanPFNs::gpVkAllocateMemory(mpVulkanDevicesWrapper->mpLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mpHostVisibleMemory);
	
		// bind buffers
		mBufferOffsets = VulkanMemoryCommon::fGetMemoryAllocationSizeAndOffsets(hostVisibleBuffersMemRequirements).second;
		for(int i = 0; i < BUFFERS_COUNT; i++) {
			VulkanPFNs::gpVkBindBufferMemory(mpVulkanDevicesWrapper->mpLogicalDevice, mHostVisiblepBuffers[i], mpHostVisibleMemory, mBufferOffsets[i]);
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
			std::cout << "Created descriptor set layout and set for host visible memory\n";
		}
	}

	std::cout << "Created VulkanHostVisibleMemory\n";
}

VulkanHostVisibleMemory::~VulkanHostVisibleMemory() {
	std::cout << "Destroying VulkanHostVisibleMemory...\n";
	
	VulkanPFNs::gpVkFreeMemory(mpVulkanDevicesWrapper->mpLogicalDevice, mpHostVisibleMemory, nullptr);
	for(VkBuffer& buffer : mHostVisiblepBuffers) {
		VulkanPFNs::gpVkDestroyBuffer(mpVulkanDevicesWrapper->mpLogicalDevice, buffer, nullptr);
	}
	
	for(size_t i = 0; i < mDescriptorpSets.size(); i++) {
		VulkanPFNs::gpVkFreeDescriptorSets(mpVulkanDevicesWrapper->mpLogicalDevice, mpDescriptorPool, 1, &mDescriptorpSets[i]);
		VulkanPFNs::gpVkDestroyDescriptorSetLayout(mpVulkanDevicesWrapper->mpLogicalDevice, mDescriptorpSetLayouts[i], nullptr);
	}
	if(mpDescriptorPool) {
		VulkanPFNs::gpVkDestroyDescriptorPool(mpVulkanDevicesWrapper->mpLogicalDevice, mpDescriptorPool, nullptr);
	}

	std::cout << "Destroyed VulkanHostVisibleMemory\n";
}

void VulkanHostVisibleMemory::writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES) {
	void* mappedMemory{};
	CHECK_VK_SUCCESS(
	VulkanPFNs::gpVkMapMemory(mpVulkanDevicesWrapper->mpLogicalDevice, mpHostVisibleMemory, mBufferOffsets[INDEX], mBufferSizes[INDEX], 0, &mappedMemory),
	"Failed to map memory"
	)
	std::memcpy(mappedMemory, pDATA, NUM_BYTES);
	VulkanPFNs::gpVkUnmapMemory(mpVulkanDevicesWrapper->mpLogicalDevice, mpHostVisibleMemory);

	std::cout << "Wrote " << NUM_BYTES << " bytes of data from " << pDATA << " to host visible memory offset " << mBufferOffsets[INDEX] << " (address " << mappedMemory << ")" << "\n";
}

void VulkanHostVisibleMemory::createDescriptorSet(VulkanMemoryCommon::DescriptorSetInfo const& INFO, size_t const& INDEX) {
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

void VulkanHostVisibleMemory::updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
	if(BUFFER_INDICES.size() != mDeviceLocalDescriptorSetInfos[SET_INDEX].mpLAYOUT_BINDINGS[SET_BINDING_NUM].descriptorCount) {
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