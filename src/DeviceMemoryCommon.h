#pragma once

#include <ktx.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include "Devices.hpp"
#include "Vertex.hpp"

namespace DeviceMemory {
	namespace Common {
		struct BufferInfo {
			VkDeviceSize mBufferSize{};
			VkBufferUsageFlags mBufferUsage{};
			uint32_t graphicsQfIndex{};
		};

		struct DescriptorSetInfo {
			std::vector<VkDescriptorSetLayoutBinding> mLayoutBindings{};
		};

		struct ImageViewInfo {
			VkImageViewType mImageViewType{};
			VkFormat mFormat{};
			VkImageSubresourceRange mImageSubresourceRange{};
		};

		struct ImageInfo {
			VkImageType mImageType{};
			VkFormat mFormat{};
			VkExtent3D mExtent3D{};
			uint32_t mMipLevels{};
			VkSampleCountFlagBits mSampleCount{};
			VkImageUsageFlags mUsage{};
			uint32_t graphicsQfIndex{};
			VkImageLayout mInitialLayout{};
			ImageViewInfo mImageViewInfo{};
		};

		struct SamplerInfo {
			VkFilter mMagFilter{};
			VkFilter mMinFilter{};
			VkSamplerMipmapMode mMipmapMode{};
			VkSamplerAddressMode mAddressModeU{};
			VkSamplerAddressMode mAddressModeV{};
			float mMipLodBias{};
			VkBool32 mAnisotropyEnable{};
			float mMaxAnisotropy{};
			float mMinLod{};
			float mMaxLod{};
			VkBorderColor mBorderColor{};
		};

		void loadGltfModel(const char* const& PATH, std::vector<Vertex::Vertex>& vertices, std::vector<uint32_t>& indices);
		ktxTexture2* loadKtxImage(const char* const& FILE_PATH);
		[[nodiscard]] VkBuffer createBuffer(VkLogicalDevice pLogicalDevice, BufferInfo const& INFO);
		[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> getMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
		[[nodiscard]] uint32_t getMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES);
		[[nodiscard]] VkDescriptorPool createDescriptorPool(VkLogicalDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO);

		void createBeginOneTimeCommandBuffer(VkLogicalDevice& rpDevice, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf, uint32_t const& GRAPHICS_QF_INDEX);
		void endSubmitDestroyOneTimeCommandBuffer(VkLogicalDevice& rpDevice, VkQueue& rpQueue, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf);
		void transitionImageLayout(VkCommandBuffer pCmdBuf, VkImage const& pIMAGE, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
		VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
		VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
	}
}
