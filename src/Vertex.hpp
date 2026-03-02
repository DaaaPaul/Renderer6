#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

namespace Vertex {
	struct Vertex {
		glm::vec4 position{};
		glm::vec4 color{};
		glm::vec2 texCoord{};

		[[nodiscard]] static const VkVertexInputBindingDescription sGetInputBindingDescription();
		[[nodiscard]] static const std::vector<VkVertexInputAttributeDescription> sGetInputAttributeDescriptions();
	};

	inline bool operator==(Vertex const& L, Vertex const& R) {
		return L.position == R.position && L.color == R.color && L.texCoord == R.texCoord;
	}
}

template<> struct std::hash<Vertex::Vertex> {
    std::size_t operator()(Vertex::Vertex const& VERTEX) const noexcept {
        std::size_t h1 = std::hash<float>{}(VERTEX.position[2]);
        std::size_t h2 = std::hash<float>{}(VERTEX.texCoord[1]);
        return h1 ^ (h2 << 1);
    }
};