#pragma once

#include <glm/glm.hpp>

struct PushConstantBlock {
	glm::vec4 base_color_factor{};
    float metallicFactor{};
    float roughnessFactor{};
    int baseColorTextureSet{};
    int physicalDescriptorTextureSet{};
    int normalTextureSet{};
    int occlusionTextureSet{};
    int emissiveTextureSet{};
    float alphaMask{};
    float alphaMaskCutoff{};
};