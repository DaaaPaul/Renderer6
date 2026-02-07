#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

struct TransformationMatrices {
	const glm::mat4 mMODEL{};
	const glm::mat4 mVIEW{};
	const glm::mat4 mPROJECTION{};

	[[nodiscard]] const static VkDescriptorSetLayoutBinding getTransformationMatricesDescriptorSetLayoutBinding(uint32_t const& BINDING_NUMBER);
};