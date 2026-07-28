#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include "engine/CameraComponent.hpp"

struct UniformBufferBlock {
	glm::mat4 model{};
	glm::mat4 view{};
	glm::mat4 projection{};
	glm::vec4 light_positions[4]{};
	glm::vec4 light_colors[4]{};
	glm::vec4 camera_pos{};

	void update(const CameraComponent& camera);

	UniformBufferBlock() = default;
	explicit UniformBufferBlock(const CameraComponent& camera);
};