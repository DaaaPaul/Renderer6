#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "Vertex.hpp"
#include "Window.h"

#define UINT32(vecSize) \
	static_cast<uint32_t>(vecSize)

#define CHECK_VK_SUCCESS(vkCreateCmd, errMsg) \
	if(vkCreateCmd != VK_SUCCESS) { \
		std::string errCode = #vkCreateCmd " did not return VK_SUCCESS, instead returning " + std::to_string(static_cast<int>(vkCreateCmd)) + ". "; \
        throw std::runtime_error(errCode + errMsg); \
	}

#define PTR_TO_DECIMAL(p) \
	reinterpret_cast<uintptr_t>(p)

#define POINTER_SIZE(num) \
	(8 * num)

#define DA_PI 3.14159265358979323846f

#define IMAGE_VIEW_TYPE(imageType) \
((imageType == VK_IMAGE_TYPE_1D) ? VK_IMAGE_VIEW_TYPE_1D : \
((imageType == VK_IMAGE_TYPE_2D) ? VK_IMAGE_VIEW_TYPE_2D : \
((imageType == VK_IMAGE_TYPE_3D) ? VK_IMAGE_VIEW_TYPE_3D : static_cast<VkImageViewType>(9999))))

#define VK_NO_FLAGS 0U

#define PRESSED(glfwKey) \
	glfwGetKey(Backend::Window::gGlfwWindow, glfwKey) == GLFW_PRESS

using VkLogicalDevice = VkDevice;

namespace Util {
	std::vector<std::string> constCharToString(std::vector<const char*> const&);
	bool containsAll(std::vector<std::string> const& HAVE, std::vector<std::string> const& CHECK);
	std::vector<char> getFileBytes(std::string const& PATH);
	float random();
	bool equal(float const&, float const&);
	bool equal(glm::vec3 const&, glm::vec3 const&);

	namespace Vulkan {
		void beginOneTimeCommandBuffer(VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer, uint32_t const& GRAPHICS_QF_INDEX);
		void endOneTimeCommandBuffer(VkQueue queue, VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer);
		void transitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageSubresourceRange const& SUBRESOURCE_RANGE, VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
	}

	namespace Resources {
		void loadGltfModel(const char* const& PATH, std::vector<Vertex::Vertex>& vertices, std::vector<uint32_t>& indices);
		ktxTexture2* loadKtxImage(const char* const& PATH);
	}

	namespace Window {
		std::vector<const char*> getRequiredWindowExtensionsForInstance();
	}

	namespace Memory {
		struct BufferBundle {
			VkBuffer buffer{};
			VkDeviceSize offset{};
			VkDeviceAddress address{};
		};

		struct ImageBundle {
			VkImage image{};
			VkDeviceSize offset{};
		};

		struct DescriptorSetBundle {
			VkDescriptorSet set{};
			VkDescriptorSetLayout layout{};
		};

		enum class ItemType : uint32_t {
			LINEAR = 0,
			NON_LINEAR = 1
		};

		VkImageView createImageView(VkImageViewCreateInfo const&);
		VkDeviceSize alignNextHighest(VkDeviceSize const&, VkDeviceSize const&);
		VkDeviceSize alignNextLowest(VkDeviceSize const&, VkDeviceSize const&);
		std::pair<VkDeviceSize, std::vector<VkDeviceSize>> doMemoryCalculations(std::vector<VkMemoryRequirements> const&, std::vector<ItemType> const&, VkDeviceSize const&);
		uint32_t getMemoryTypeIndex(std::vector<VkMemoryRequirements> const&, VkMemoryPropertyFlags const&);
	}

	namespace FrameData {
		VkCommandPool createCmdPool(VkCommandPoolCreateFlags const&, uint32_t const&);
		VkCommandBuffer createCmdBuffer(VkCommandPool, VkCommandBufferLevel const&);
		VkFence createFence(VkFenceCreateFlags const&);
		VkSemaphore createSemaphore(VkSemaphoreTypeCreateInfo const&);
	}

	namespace FeatureChain {
		template<class F>
		bool checkFeatureHasAll(F const& HAVE, F const& CHECK) {
			bool hasAll = true;

			uint32_t firstVkBool32Offset = offsetof(F, pNext) + sizeof(void*);
			uint32_t howManyVkBool32 = (sizeof(F) - firstVkBool32Offset) / sizeof(VkBool32) - 1;

			char const* SNIPER_HAVE = reinterpret_cast<char const*>(&HAVE);
			SNIPER_HAVE += firstVkBool32Offset;
			VkBool32 const* BOOL_SNIPER_HAVE = reinterpret_cast<VkBool32 const*>(SNIPER_HAVE);

			const char* SNIPER_CHECK = reinterpret_cast<char const*>(&CHECK);
			SNIPER_CHECK += firstVkBool32Offset;
			VkBool32 const* BOOL_SNIPER_CHECK = reinterpret_cast<VkBool32 const*>(SNIPER_CHECK);
		
			for(int i = 0; i < howManyVkBool32 && hasAll; i++) {
				if(*BOOL_SNIPER_HAVE == VK_FALSE && *BOOL_SNIPER_CHECK == VK_TRUE) {
					hasAll = false;
				} else {
					BOOL_SNIPER_HAVE++;
					BOOL_SNIPER_CHECK++;
				}
			}

			return hasAll;
		}
	}
}