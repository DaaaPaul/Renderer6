#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "Geometry/Vertex.hpp"
#include "Backend/Window.h"
#include "Utility/Vulkan.h"

#define PRINTLN(x) std::cout << x << '\n'

#define UINT32(vector_size) static_cast<uint32_t>(vector_size)

#define POINTER_SIZE(num) (8 * num)

#define DA_PI 3.14159274f

#define GLFW_PRESSED(glfw_key) glfwGetKey(Window::g_glfw_window, glfw_key) == GLFW_PRESS

#define VK_CHECK(create_command, error_message) \
	if(create_command != VK_SUCCESS) { \
        throw std::runtime_error(error_message); \
	}

#define VK_NO_FLAGS 0U

using VkLogicalDevice = VkDevice;

namespace Utility {
	std::vector<std::string> to_string(const std::vector<const char*>& c_strings);

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these);

	float random();

	inline bool equal(float float1, float float2, float epsilon = 0.001f) {
		return fabs(float1 - float2) <= (((float1 > float2) ? float1 : float2) * epsilon + epsilon);
	}

	inline bool equal(glm::vec3 vec3_1, glm::vec3 vec3_2, float epsilon = 0.001f) {
		return equal(vec3_1.x, vec3_2.x, epsilon) && equal(vec3_1.y, vec3_2.y, epsilon) && equal(vec3_1.z, vec3_2.z, epsilon);
	}

	void load_gltf_model(const char* file_path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

	ktxTexture2* get_ktx_texture(const char* ktx_path);

	std::vector<char> get_file_bytes(const std::string& file_path);

	namespace Memory { // TODO: must go soon
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