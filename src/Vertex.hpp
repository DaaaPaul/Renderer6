#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <type_traits>
#include <cstdint>

namespace Vertex {
	struct Vertex {
		glm::vec4 position{};
		glm::vec2 texCoord{};

		static constexpr VkVertexInputBindingDescription getInputBinding(uint32_t const& N)  {
			return VkVertexInputBindingDescription{
				.binding = N,
				.stride = sizeof(Vertex),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};
		}
		static constexpr std::vector<VkVertexInputAttributeDescription> getInputAttributes(uint32_t const& N)  {
			return { 
				VkVertexInputAttributeDescription{
					.location = 0,
					.binding = N,
					.format = VK_FORMAT_R32G32B32A32_SFLOAT,
					.offset = offsetof(Vertex, position)
				}, 
				VkVertexInputAttributeDescription{
					.location = 1,
					.binding = N,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(Vertex, texCoord)
				}
			};
		}
	};

	inline bool operator==(Vertex const& L, Vertex const& R) {
		return L.position == R.position && L.texCoord == R.texCoord;
	}
}

template<> 
struct std::hash<Vertex::Vertex> {
    std::size_t operator()(Vertex::Vertex const& VERTEX) const {
        std::size_t h1 = std::hash<float>{}(VERTEX.position[2]);
        std::size_t h2 = std::hash<float>{}(VERTEX.texCoord[1]);
        return h1 ^ (h2 << 1);
    }
};