#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

struct PBRPushConstantBlock {
	glm::vec4 base_color_factor{};
    float metallicFactor{};
    float roughnessFactor{};
};