#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Util.h"

namespace Particle {
	struct Particle {
		glm::vec4 color{};
		glm::vec2 position{};
		glm::vec2 velocity{};

		static constexpr VkVertexInputBindingDescription getInputBinding(uint32_t const& N)  {
			return VkVertexInputBindingDescription{
				.binding = N,
				.stride = sizeof(Particle),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};
		}
		static constexpr std::vector<VkVertexInputAttributeDescription> getInputAttributes(uint32_t const& N)  {
			return { 
				VkVertexInputAttributeDescription{
					.location = 0,
					.binding = N,
					.format = VK_FORMAT_R32G32B32A32_SFLOAT,
					.offset = offsetof(Particle, color)
				}, 
				VkVertexInputAttributeDescription{
					.location = 1,
					.binding = N,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(Particle, position)
				}, 
			};
		}
		static constexpr VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const& N)  {
			return VkDescriptorSetLayoutBinding{
				.binding = N,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
			};
		}
	};

	namespace Delta {
		constexpr VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const& N)  {
			return VkDescriptorSetLayoutBinding{
				.binding = N,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
			};
		}
	}
}