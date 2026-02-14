#pragma once

#include <ktx.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include "Devices.hpp"

namespace DeviceMemory {
	namespace Common {
		struct BufferInfo {
			VkDeviceSize mBufferSize{};
			VkBufferUsageFlags mBufferUsage{};
			uint32_t mGraphicsQueueFamilyIndex{};
		};

		struct DescriptorSetInfo {
			std::vector<VkDescriptorSetLayoutBinding> mLayoutBindings{};
		};

		struct ImageInfo {
			VkImageType mImageType{};
			VkFormat mFormat{};
			VkExtent3D mExtent3D{};
			uint32_t mMipLevels{};
			VkSampleCountFlagBits mSampleCount{};
			VkImageUsageFlags mUsage{};
			uint32_t mGraphicsQueueFamilyIndex{};
			VkImageLayout mInitialLayout{};
		};

		struct HostVisibleConstructArguements {
			std::vector<Common::BufferInfo> mBufferInfos{};
			std::vector<Common::DescriptorSetInfo> mDescriptorSetInfos{};
		};

		struct DeviceLocalConstructArguements {
			std::vector<Common::BufferInfo> mBufferInfos{};
			std::vector<Common::ImageInfo> mImageInfos{};
			std::vector<Common::DescriptorSetInfo> mDescriptorSetInfos{};
		};

		ktxTexture2* fKtxLoadImage(const char* const& FILE_PATH);
		[[nodiscard]] VkBuffer fCreateBuffer(VkDevice pLogicalDevice, BufferInfo const& INFO);
		[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> fGetMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
		[[nodiscard]] uint32_t fGetMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES);
		[[nodiscard]] VkDescriptorPool fCreateDescriptorPool(VkDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO);

		void fAllocateBeginOneTimeCommandBuffer(VkDevice& rpDevice, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf, uint32_t const& GRAPHICS_QF_INDEX);
		void fEndSubmitDeallocateOneTimeCommandBuffer(VkDevice& rpDevice, VkQueue& rpQueue, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf);
		void fTransitionImageLayout(VkCommandBuffer pCmdBuf, VkImage const& pIMAGE, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
		VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
		VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
	}
}
