#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "imgui.h"
#include "Memory/Texture.hpp"
#include "Memory/Sampler.hpp"

namespace Gui {
	struct PushConstantBlock {
		glm::vec2 scale{};
		glm::vec2 translate{};
	};

	inline ImGuiStyle g_style{};

	void init(float width, float height);
}