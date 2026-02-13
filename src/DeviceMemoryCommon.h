#pragma once

#include <ktx.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include "Devices.hpp"

namespace DeviceMemory {
	namespace Common {
		struct BufferInfo {
			const VkDeviceSize mBUFFER_SIZE{};
			const VkBufferUsageFlags mBUFFER_USAGE{};
			const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
		};

		struct DescriptorSetInfo {
			const std::vector<VkDescriptorSetLayoutBinding> mLAYOUT_BINDINGS{};
		};

		struct ImageInfo {
			const VkImageType mIMAGE_TYPE{};
			const VkFormat mFORMAT{};
			const VkExtent3D mEXTENT3D{};
			const uint32_t mMIP_LEVELS{};
			const VkSampleCountFlagBits mSAMPLE_COUNT{};
			const VkImageUsageFlags mUSAGE{};
			const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
			const VkImageLayout mINITIAL_LAYOUT{};
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

		void fAllocateBeginOneTimeCommandBuffer(VkDevice pDevice, VkCommandPool pCmdPool, VkCommandBuffer pCmdBuf, uint32_t const& GRAPHICS_QF_INDEX);
		void fEndSubmitDeallocateOneTimeCommandBuffer(VkDevice pDevice, VkQueue pQueue, VkCommandPool pCmdPool, VkCommandBuffer pCmdBuf);
		void fTransitionImageLayout(VkCommandBuffer pCmdBuf, VkImage const& pIMAGE, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
		VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
		VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
	}
}
