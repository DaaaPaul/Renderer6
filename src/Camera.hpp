#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
	struct EulerAngles {
		float x{0.0f};
		float y{0.0f};
		float z{0.0f};
	};

	struct OrthonormalBasis {
		glm::vec3 x{1.0f, 0.0f, 0.0f};
		glm::vec3 y{0.0f, 1.0f, 0.0f};
		glm::vec3 z{0.0f, 0.0f, 1.0f};

		OrthonormalBasis normalize() const noexcept;
		bool isNormalized() const noexcept;
	};

	class Camera {
		glm::vec3 pos{0.0f, 0.0f, 0.0f};
		glm::vec3 worldY{0.0f, 1.0f, 0.0f};
		OrthonormalBasis basis{};

		public:
		explicit Camera(glm::vec3 const& POS, glm::vec3 const& WORLD_Y, EulerAngles const& ROTATION);
		glm::mat4 convertViewMatrix() const noexcept;
	};

	glm::mat3 rotate(glm::vec3 const&, float const&) noexcept;
	OrthonormalBasis applyRotation(OrthonormalBasis const&, EulerAngles const&) noexcept;
}