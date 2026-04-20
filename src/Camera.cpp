#include "Camera.hpp"

namespace Engine {
	OrthonormalBasis OrthonormalBasis::normalize() const noexcept {
		return OrthonormalBasis{ glm::normalize(x), glm::normalize(y), glm::normalize(z) };
	}

	bool OrthonormalBasis::isNormalized() const noexcept {
		OrthonormalBasis normalized = normalize();
		return normalized.x == x && normalized.y == y && normalized.z == z;
	}

	Camera::Camera(glm::vec3 const& POS, glm::vec3 const& WORLD_Y, EulerAngles const& ROTATION) :
		pos(POS), worldY(WORLD_Y), basis(applyRotation(OrthonormalBasis{}, ROTATION)) {
	
	}

	glm::mat4 Camera::convertViewMatrix() const noexcept {
		return glm::mat4(
			basis.x.x, basis.y.x, basis.z.x, 0.0f,
			basis.x.y, basis.y.y, basis.z.y, 0.0f,
			basis.x.z, basis.y.z, basis.z.z, 0.0f,
			-(basis.x.x * pos.x + basis.x.y * pos.y + basis.x.z * pos.z), -(basis.y.x * pos.x + basis.y.y * pos.y + basis.y.z * pos.z), -(basis.z.x * pos.x + basis.z.y * pos.y + basis.z.z * pos.z), 1.0f
		);
	}

	glm::mat3 rotate(glm::vec3 const& AXIS, float const& ANGLE_CC) noexcept {
		assert(glm::normalize(AXIS) == AXIS);

		const float COS = std::cos(ANGLE_CC);
		const float SIN = std::sin(ANGLE_CC);
		const float ONE_MINUS_COS = 1.0f - std::cos(ANGLE_CC);

		return glm::mat3(
			COS + AXIS.x * AXIS.x * ONE_MINUS_COS,
			AXIS.y * AXIS.x * ONE_MINUS_COS - AXIS.z * SIN,
			AXIS.z * AXIS.x * ONE_MINUS_COS + AXIS.y * SIN,

			AXIS.x * AXIS.y * ONE_MINUS_COS + AXIS.z * SIN,
			COS + AXIS.y * AXIS.y * ONE_MINUS_COS,
			AXIS.z * AXIS.y * ONE_MINUS_COS - AXIS.x * SIN,

			AXIS.x * AXIS.z * ONE_MINUS_COS - AXIS.y * SIN,
			AXIS.y * AXIS.z * ONE_MINUS_COS + AXIS.x * SIN,
			COS + AXIS.z * AXIS.z * ONE_MINUS_COS
		);
	}

	OrthonormalBasis applyRotation(OrthonormalBasis const& BASIS, EulerAngles const& EULER) noexcept {
		assert(BASIS.isNormalized());
		
		OrthonormalBasis moved(BASIS);
			
		const glm::mat3 ROTATION_ABOUT_XAXIS = rotate(moved.x, EULER.x);
		moved.y = ROTATION_ABOUT_XAXIS * moved.y;
		moved.z = ROTATION_ABOUT_XAXIS * moved.z;

		const glm::mat3 ROTATION_ABOUT_YAXIS = rotate(moved.y, EULER.y);
		moved.x = ROTATION_ABOUT_YAXIS * moved.x;
		moved.z = ROTATION_ABOUT_YAXIS * moved.z;

		const glm::mat3 ROTATION_ABOUT_ZAXIS = rotate(moved.z, EULER.z);
		moved.x = ROTATION_ABOUT_ZAXIS * moved.x;
		moved.y = ROTATION_ABOUT_ZAXIS * moved.y;

		return moved;
	}
}