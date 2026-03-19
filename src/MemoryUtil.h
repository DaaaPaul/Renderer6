#pragma once

#include <ktx.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include "Vertex.hpp"

namespace Memory {
	struct BufferBundle {
		VkBuffer buffer{};
		VkDeviceSize offset{};
		VkDeviceAddress address{};
	};

	struct DescriptorSetBundle {
		VkDescriptorSet set{};
		VkDescriptorSetLayout layout{};
	};

	struct DescriptorSetInfo {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
	};

	struct ImageViewInfo {
		VkImageViewType type{};
		VkFormat format{};
		VkImageSubresourceRange subresourceRange{};

		bool operator!=(ImageViewInfo const& RIGHT) const noexcept {
			return 
				!(type == RIGHT.type &&
				format == RIGHT.format &&
				subresourceRange.aspectMask == RIGHT.subresourceRange.aspectMask &&
				subresourceRange.baseMipLevel == RIGHT.subresourceRange.baseMipLevel &&
				subresourceRange.levelCount == RIGHT.subresourceRange.levelCount &&
				subresourceRange.baseArrayLayer == RIGHT.subresourceRange.baseArrayLayer &&
				subresourceRange.layerCount == RIGHT.subresourceRange.layerCount);
		}
	};

	struct ImageInfo {
		VkImageType type{};
		VkFormat format{};
		VkExtent3D extent{};
		uint32_t mipLevelsCount{};
		VkSampleCountFlagBits sampleCount{};
		VkImageUsageFlags usage{};
		uint32_t graphicsQfIndex{};
		VkImageLayout initialLayout{};
		ImageViewInfo viewInfo{};
	};

	struct SamplerInfo {
		VkFilter magFilter{};
		VkFilter minFilter{};
		VkSamplerMipmapMode mipmapMode{};
		VkSamplerAddressMode addressModeU{};
		VkSamplerAddressMode addressModeV{};
		float mipLodBias{};
		VkBool32 anisotropyEnable{};
		float maxAnisotropy{};
		float minLod{};
		float maxLod{};
		VkBorderColor borderColor{};
	};



	VkDescriptorSetLayout createDescriptorSetLayout(VkLogicalDevice pLogicalDevice, DescriptorSetInfo const& INFO);
	VkDescriptorPool createDescriptorPool(VkLogicalDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO);
	VkDescriptorSet createDescriptorSet(VkLogicalDevice pLogicalDevice, VkDescriptorPool pPool, VkDescriptorSetLayout pLayout, DescriptorSetInfo const& INFO);
	VkBuffer createBuffer(VkLogicalDevice pLogicalDevice, BufferInfo const& BUFFER_INFO);
	VkImage createImage(VkLogicalDevice pLogicalDevice, ImageInfo const& IMAGE_INFO);
	VkImageView createImageView(VkLogicalDevice pLogicalDevice, VkImage image, ImageViewInfo const& IMAGE_VIEW_INFO);
	VkSampler createSampler(VkLogicalDevice pLogicalDevice, SamplerInfo const& SAMPLER_INFO);

	std::pair<VkDeviceSize, std::vector<VkDeviceSize>> getMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
	uint32_t getMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkDeviceMemoryPropertyFlags const& MEMORY_PROPERTIES);

	void createBeginOneTimeCommandBuffer(VkLogicalDevice& pDevice, VkCommandPool& pCmdPool, VkCommandBuffer& pCmdBuf, uint32_t const& GRAPHICS_QF_INDEX);
	void endSubmitDestroyOneTimeCommandBuffer(VkLogicalDevice& pDevice, VkQueue& pQueue, VkCommandPool& pCmdPool, VkCommandBuffer& pCmdBuf);
	void transitionImageLayout(VkCommandBuffer pCmdBuf, VkImage& pImage, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
	VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
	VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
}
