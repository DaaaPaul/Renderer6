#pragma once

#include <glm/glm.hpp>
#include "Engine/CameraComponent.hpp"
#include "Memory/UniformBuffer.hpp"

struct UniformBufferBlock {
	glm::mat4 model{};
	glm::mat4 view{};
	glm::mat4 projection{};
	glm::vec4 light_positions[4]{};
	glm::vec4 light_colors[4]{};
	glm::vec4 camera_pos{};
    float exposure{};
    float gamma{};
    float prefilteredCubeMipLevels{};
    float scaleIBLAmbient{};

	explicit UniformBufferBlock(const CameraComponent& camera);
};