#pragma once

#include <glm/glm.hpp>

namespace MathHead {
	glm::mat4 lookAt(glm::vec3 const& CAMERA_POS, glm::vec3 const& TARGET_POS, glm::vec3 const& WORLD_UP) noexcept;
}