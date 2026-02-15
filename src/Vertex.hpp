#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

namespace Vertex {
	struct Vertex {
		const glm::vec4 mPOSITION{};
		const glm::vec4 mCOLOR{};
		const glm::vec2 mTEXTURE_COORDINATE{};

		[[nodiscard]] static const VkVertexInputBindingDescription sGetInputBindingDescription();
		[[nodiscard]] static const std::vector<VkVertexInputAttributeDescription> sGetInputAttributeDescriptions();
	};
}
