#include "Camera.hpp"

namespace Engine {
	glm::mat3 rotate(glm::vec3 const& AXIS, float const& ANGLE_CC) noexcept {
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

	OrthonormalBasis applyEuler(OrthonormalBasis const& BASIS, EulerAngles const& EULER) noexcept {
		OrthonormalBasis moved(BASIS);
			
		const glm::mat3 ROTATION_ABOUT_XAXIS = rotate(moved.xAxis, EULER.x);
		moved.yAxis = ROTATION_ABOUT_XAXIS * moved.yAxis;
		moved.zAxis = ROTATION_ABOUT_XAXIS * moved.zAxis;

		const glm::mat3 ROTATION_ABOUT_YAXIS = rotate(moved.yAxis, EULER.y);
		moved.xAxis = ROTATION_ABOUT_YAXIS * moved.xAxis;
		moved.zAxis = ROTATION_ABOUT_YAXIS * moved.zAxis;

		const glm::mat3 ROTATION_ABOUT_ZAXIS = rotate(moved.zAxis, EULER.z);
		moved.xAxis = ROTATION_ABOUT_ZAXIS * moved.xAxis;
		moved.yAxis = ROTATION_ABOUT_ZAXIS * moved.yAxis;

		return moved;
	}

	OrthonormalBasis normalize(OrthonormalBasis const& BASIS) noexcept {
		return OrthonormalBasis{ glm::normalize(BASIS.xAxis), glm::normalize(BASIS.yAxis), glm::normalize(BASIS.zAxis) };
	}
}