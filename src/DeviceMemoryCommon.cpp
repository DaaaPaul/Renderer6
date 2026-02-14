#include <iostream>
#include "DeviceMemoryCommon.h"
#include "Common.h"

namespace DeviceMemory {
	namespace Common {
		ktxTexture2* fKtxLoadImage(const char* const& FILE_PATH) {
			ktxTexture2* pKtxTexture{};

			if(ktxTexture_CreateFromNamedFile(FILE_PATH, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, reinterpret_cast<ktxTexture**>(&pKtxTexture)) != KTX_SUCCESS) {
				throw std::runtime_error("Failed to load ktx texture from " + std::string(FILE_PATH));
			}

			if(ktxTexture2_NeedsTranscoding(pKtxTexture)) {
				const ktx_transcode_fmt_e TARGET_FORMAT{ KTX_TTF_BC7_RGBA };

				if(ktxTexture2_TranscodeBasis(pKtxTexture, TARGET_FORMAT, 0) != KTX_SUCCESS) {
					throw std::runtime_error("Failed to transcode ktx texture to ktx_transcode_fmt " + std::to_string(TARGET_FORMAT));
				}
			}

			return pKtxTexture;
		}

		[[nodiscard]] VkBuffer fCreateBuffer(VkDevice pLogicalDevice, BufferInfo const& INFO) {
			VkBufferCreateInfo bufferInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = INFO.mBufferSize,
				.usage = INFO.mBufferUsage,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 1,
				.pQueueFamilyIndices = &INFO.mGraphicsQueueFamilyIndex,
			};

			VkBuffer returnBuffer{};
			vkCreateBuffer(pLogicalDevice, &bufferInfo, nullptr, &returnBuffer);

			return returnBuffer;
		}

		[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> fGetMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS) {
			std::pair<VkDeviceSize, std::vector<VkDeviceSize>> allocationSizeAndBufferOffsets{};
			uint32_t buffersCount = static_cast<uint32_t>(BUFFER_MEMORY_REQUIREMENTS.size());
			allocationSizeAndBufferOffsets.second.resize(buffersCount, UINT64_MAX);

			for (int i = 0; i < buffersCount; i++) {
				while (allocationSizeAndBufferOffsets.first % BUFFER_MEMORY_REQUIREMENTS[i].alignment != 0) {
					allocationSizeAndBufferOffsets.first++;
				}

				allocationSizeAndBufferOffsets.second[i] = allocationSizeAndBufferOffsets.first;

				allocationSizeAndBufferOffsets.first += BUFFER_MEMORY_REQUIREMENTS[i].size;
			}

			return allocationSizeAndBufferOffsets;
		}

		[[nodiscard]] uint32_t fGetMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES) {
			uint32_t finalMemoryRequirementsMask = UINT32_MAX;
			for (VkMemoryRequirements const& BUFFER_MEMORY_REQUIREMENT : BUFFER_MEMORY_REQUIREMENTS) {
				finalMemoryRequirementsMask &= BUFFER_MEMORY_REQUIREMENT.memoryTypeBits;
			}

			VkPhysicalDeviceMemoryProperties memoryProperties{};
			vkGetPhysicalDeviceMemoryProperties(pPhysicalDevice, &memoryProperties);

			uint32_t memoryTypeIndexReturn{ UINT32_MAX };
			for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
				if ((finalMemoryRequirementsMask & (1 << i)) &&
					((memoryProperties.memoryTypes[i].propertyFlags & MEMORY_PROPERTIES) == MEMORY_PROPERTIES)) {
					memoryTypeIndexReturn = i;
				}
			};

			return memoryTypeIndexReturn;
		}

		[[nodiscard]] VkDescriptorPool fCreateDescriptorPool(VkDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO) {
			VkDescriptorPool pReturnDescriptorPool{};
		
			// create pool sizes (ASSUMING UNIQUE DESCRIPTOR TYPE PER ITS OWN UNIQUE BINDING)
			std::vector<VkDescriptorPoolSize> poolSizes{};
			for(Common::DescriptorSetInfo const& CUSTOM_SET_INFO : INFO) {
				const std::vector<VkDescriptorSetLayoutBinding> BINDINGS{ CUSTOM_SET_INFO.mLayoutBindings };

				for(VkDescriptorSetLayoutBinding const& BINDING : BINDINGS) {
					poolSizes.emplace_back(BINDING.descriptorType, BINDING.descriptorCount);
				}
			}

			// create descriptor pool
			const VkDescriptorPoolCreateInfo DESCRIPTOR_POOL_INFO{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
				.maxSets = static_cast<uint32_t>(INFO.size()),
				.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
				.pPoolSizes = poolSizes.data(),
			};
			CHECK_VK_SUCCESS(
				vkCreateDescriptorPool(pLogicalDevice, &DESCRIPTOR_POOL_INFO, nullptr, &pReturnDescriptorPool),
				"Failed to create descriptor pool"
			)

			return pReturnDescriptorPool;
		}

		void fAllocateBeginOneTimeCommandBuffer(VkDevice& rpDevice, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf, uint32_t const& GRAPHICS_QF_INDEX) {
			// create transient command pool
			{
				const VkCommandPoolCreateInfo COMMAND_POOL_INFO{
					.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
					.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
					.queueFamilyIndex = GRAPHICS_QF_INDEX
				};
				CHECK_VK_SUCCESS(
					vkCreateCommandPool(rpDevice, &COMMAND_POOL_INFO, nullptr, &rpCmdPool),
					"Failed to create temporary command pool"
				)
			}

			// create command buffer
			{
				const VkCommandBufferAllocateInfo COMMAND_BUFFER_INFO{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
					.commandPool = rpCmdPool,
					.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
					.commandBufferCount = 1,
				};
				CHECK_VK_SUCCESS(
					vkAllocateCommandBuffers(rpDevice, &COMMAND_BUFFER_INFO, &rpCmdBuf),
					"Failed to create temporary command buffer"
				)
			}

			// begin recording
			{
				const VkCommandBufferBeginInfo ONE_TIME_SUBMIT_BEGIN(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
				CHECK_VK_SUCCESS(
					vkBeginCommandBuffer(rpCmdBuf, &ONE_TIME_SUBMIT_BEGIN),
					"Failed to begin temporary command buffer recording"
				)
			}
		}

		void fEndSubmitDeallocateOneTimeCommandBuffer(VkDevice& rpDevice, VkQueue& rpQueue, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf) {
			// end command buffer
			{
				CHECK_VK_SUCCESS(
					vkEndCommandBuffer(rpCmdBuf),
					"Failed to end temporary command buffer recording"
				)
			}

			// create fence to wait on
			VkFence copyCommandDone{};
			{
				const VkFenceCreateInfo FENCE_INFO(VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, 0);
				CHECK_VK_SUCCESS(
					vkCreateFence(rpDevice, &FENCE_INFO, nullptr, &copyCommandDone),
					"Failed to create copy command done fence"
				)
			}

			// submit it
			{
				const VkSubmitInfo ONE_TIME_SUBMIT_INFO{
					.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.commandBufferCount = 1,
					.pCommandBuffers = &rpCmdBuf,
				};

				CHECK_VK_SUCCESS(
					vkQueueSubmit(rpQueue, 1, &ONE_TIME_SUBMIT_INFO, copyCommandDone),
					"Failed to submit temporary command buffer"
				)
			}

			CHECK_VK_SUCCESS(
				vkWaitForFences(rpDevice, 1, &copyCommandDone, VK_TRUE, UINT64_MAX),
				"Failed to wait for copy command done fence"
			)

			vkDestroyFence(rpDevice, copyCommandDone, nullptr);
			vkFreeCommandBuffers(rpDevice, rpCmdPool, 1, &rpCmdBuf);
			vkDestroyCommandPool(rpDevice, rpCmdPool, nullptr);
		}

		void fTransitionImageLayout(VkCommandBuffer pCmdBuf, VkImage const& pIMAGE, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
		VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
		VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX) {
			const VkImageMemoryBarrier2 IMAGE_MEMORY_BARRIER2{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = SRC_STAGE,
				.srcAccessMask = SRC_ACCESS,
				.dstStageMask = DST_STAGE,
				.dstAccessMask = DST_ACCESS,
				.oldLayout = OLD_LAYOUT,
				.newLayout = NEW_LAYOUT,
				.srcQueueFamilyIndex = GRAPHICS_QF_INDEX,
				.dstQueueFamilyIndex = GRAPHICS_QF_INDEX,
				.image = pIMAGE,
				.subresourceRange = SUBRESOURCE_RANGE,
			};

			const VkDependencyInfo PARENT_MEMORY_BARRIER2{
				.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
				.imageMemoryBarrierCount = 1,
				.pImageMemoryBarriers = &IMAGE_MEMORY_BARRIER2,
			};

			vkCmdPipelineBarrier2(pCmdBuf, &PARENT_MEMORY_BARRIER2);
		}
	}
}
