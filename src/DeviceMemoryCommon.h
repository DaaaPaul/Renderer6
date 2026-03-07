#pragma once

#include <ktx.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include "Devices.hpp"
#include "Vertex.hpp"

namespace DeviceMemory {
	struct BufferInfo {
		VkDeviceSize size{};
		VkBufferUsageFlags usage{};
		uint32_t graphicsQfIndex{};
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

	void loadGltfModel(const char* const& PATH, std::vector<Vertex::Vertex>& vertices, std::vector<uint32_t>& indices);
	ktxTexture2* loadKtxImage(const char* const& FILE_PATH);
	[[nodiscard]] uint32_t calculateMipLevels(VkExtent2D const& EXTENT) noexcept;

	[[nodiscard]] VkBuffer createBuffer(VkLogicalDevice pLogicalDevice, BufferInfo const& BUFFER_INFO);
	[[nodiscard]] VkImage createImage(VkLogicalDevice pLogicalDevice, ImageInfo const& IMAGE_INFO);
	[[nodiscard]] VkImageView createImageView(VkLogicalDevice pLogicalDevice, VkImage image, ImageViewInfo const& IMAGE_VIEW_INFO);
	[[nodiscard]] VkSampler createSampler(VkLogicalDevice pLogicalDevice, SamplerInfo const& SAMPLER_INFO);

	[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> getMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
	[[nodiscard]] uint32_t getMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES);
	[[nodiscard]] VkDescriptorPool createDescriptorPool(VkLogicalDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO);

	void createBeginOneTimeCommandBuffer(VkLogicalDevice& rpDevice, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf, uint32_t const& GRAPHICS_QF_INDEX);
	void endSubmitDestroyOneTimeCommandBuffer(VkLogicalDevice& rpDevice, VkQueue& rpQueue, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf);
	void transitionImageLayout(VkCommandBuffer pCmdBuf, VkImage const& pIMAGE, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
	VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
	VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
}
