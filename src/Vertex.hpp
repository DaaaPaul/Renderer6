#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

struct Vertex {
	const glm::vec4 mPOSITION{};
	const glm::vec4 mCOLOR{};

	[[nodiscard]] static const VkVertexInputBindingDescription getInputBindingDescription();
	[[nodiscard]] static const std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions();
};