#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include "Vertex.hpp"
#include "Window.h"

#define println(x) std::cout << x << '\n';

#define UINT32(vecSize) static_cast<uint32_t>(vecSize)

#define VK_CHECK(createCmd, errMsg) \
	if(createCmd != VK_SUCCESS) { \
        throw std::runtime_error(errMsg); \
	}

#define POINTER_SIZE(num) (8 * num)

#define DA_PI 3.14159274f

#define PRESSED(glfwKey) glfwGetKey(Backend::Window::gGlfwWindow, glfwKey) == GLFW_PRESS

using VkLogicalDevice = VkDevice;

namespace Util {
	std::vector<std::string> toStringVector(std::vector<const char*> const&);
	bool containsAll(std::vector<std::string> const& HAVE, std::vector<std::string> const& CHECK);
	std::vector<char> getFileBytes(std::string const& PATH);
	float random();
	inline bool equal(float const& F1, float const& F2, float const& EPSILON = 0.001f) {
		return fabs(F1 - F2) <= (((F1 > F2) ? F1 : F2) * EPSILON + 0.001f);
	}
	inline bool equal(glm::vec3 const& V1, glm::vec3 const& V2, float const& EPSILON = 0.001f) {
		return equal(V1.x, V2.x, EPSILON) && equal(V1.y, V2.y, EPSILON) && equal(V1.z, V2.z, EPSILON);
	}

	namespace Vulkan {
		void begin(VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer, uint32_t qfIndex);
		void end(VkQueue queue, VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer);
		void transitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageSubresourceRange const& SUBRESOURCE_RANGE, VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
	}

	namespace Resources {
		void loadGltfModel(const char* const& PATH, std::vector<Vertex::Vertex>& vertices, std::vector<uint32_t>& indices);
		ktxTexture2* loadKtxImage(const char* const& PATH);
	}

	namespace Window {
		std::vector<const char*> getVkWindowExtensions();
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
		
			for(int i = 0; i < howManyVkBool32 && hasAll; ++i) {
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