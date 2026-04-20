#pragma once

#include <glm/glm.hpp>

namespace Engine {
	struct EulerAngles {
		float x{0.0f};
		float y{0.0f};
		float z{0.0f};
	};

	struct OrthonormalBasis {
		glm::vec3 xAxis{1.0f, 0.0f, 0.0f};
		glm::vec3 yAxis{0.0f, 1.0f, 0.0f};
		glm::vec3 zAxis{0.0f, 0.0f, 1.0f};
	};

	struct Camera {
		glm::vec3 pos{0.0f, 0.0f, 0.0f};
		glm::vec3 worldY{0.0f, 1.0f, 0.0f};
		glm::vec3 X{1.0f, 0.0f, 0.0f};
		glm::vec3 Y{0.0f, 1.0f, 0.0f};
		glm::vec3 Z{0.0f, 0.0f, -1.0f};
	};

	glm::mat3 rotate(glm::vec3 const&, float const&) noexcept;
	OrthonormalBasis applyEuler(OrthonormalBasis const&, EulerAngles const&) noexcept;
	OrthonormalBasis normalize(OrthonormalBasis const&) noexcept;
}