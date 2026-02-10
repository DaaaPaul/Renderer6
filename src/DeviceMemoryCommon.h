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

		struct ConstructArguements {
			Backend::Devices* mpDevices{};
			std::vector<Common::BufferInfo> mBufferInfos{};
			std::vector<Common::DescriptorSetInfo> mDescriptorSetInfos{};
		};

		ktxTexture2* fKtxLoadImage(const char* const& FILE_PATH, uint32_t& texWidth, uint32_t& texHeight, size_t& texSize, unsigned char*& pTexData);
		[[nodiscard]] VkBuffer fCreateBuffer(VkDevice pLogicalDevice, BufferInfo const& INFO);
		[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> fGetMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
		[[nodiscard]] uint32_t fGetMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES);
		[[nodiscard]] VkDescriptorPool fCreateDescriptorPool(VkDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO);
	}
}
