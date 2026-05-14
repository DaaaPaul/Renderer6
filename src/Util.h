#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "Vertex.hpp"
#include "Window.h"
#include "Vulkan.h"

//#define DEBUGGING

#define println(x) std::cout << x << '\n'

#define UINT32(vecSize) static_cast<uint32_t>(vecSize)

#define POINTER_SIZE(num) (8 * num)

#define DA_PI 3.14159274f

#define PRESSED(glfwKey) glfwGetKey(Window::g_glfw_window, glfwKey) == GLFW_PRESS

#define VK_CHECK(createCmd, errMsg) \
	if(createCmd != VK_SUCCESS) { \
        throw std::runtime_error(errMsg); \
	}

#define VK_NO_FLAGS 0U

using VkLogicalDevice = VkDevice;

namespace Util {
	std::vector<std::string> toString(std::vector<const char*> const& C_STRS);

	bool containsAll(std::vector<std::string> const& HAVE, std::vector<std::string> const& CHECK);

	float random();

	inline bool equal(float const& F1, float const& F2, float const& EPSILON = 0.001f) {
		return fabs(F1 - F2) <= (((F1 > F2) ? F1 : F2) * EPSILON + 0.001f);
	}

	inline bool equal(glm::vec3 const& V1, glm::vec3 const& V2, float const& EPSILON = 0.001f) {
		return equal(V1.x, V2.x, EPSILON) && equal(V1.y, V2.y, EPSILON) && equal(V1.z, V2.z, EPSILON);
	}

	void loadGltfModel(const char* const& PATH, std::vector<Vertex::Vertex>& vertices, std::vector<uint32_t>& indices);

	ktxTexture2* loadKtxImage(const char* const& PATH);

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

		VkImageView create_image_view(VkImageViewCreateInfo const&);
		VkDeviceSize alignNextHighest(VkDeviceSize const&, VkDeviceSize const&);
		std::pair<VkDeviceSize, std::vector<VkDeviceSize>> doMemoryCalculations(std::vector<VkMemoryRequirements> const&, std::vector<ItemType> const&, VkDeviceSize const&);
		uint32_t getMemoryTypeIndex(std::vector<VkMemoryRequirements> const&, VkMemoryPropertyFlags const&);
	}
}