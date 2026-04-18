#include "MathHead.h"

namespace MathHead {
	glm::mat4 lookAt(glm::vec3 const& CAMERA_POS, glm::vec3 const& TARGET_POS, glm::vec3 const& WORLD_UP) noexcept {
		glm::vec3 zAxis = glm::normalize(CAMERA_POS - TARGET_POS);
		glm::vec3 xAxis = glm::normalize(glm::cross(WORLD_UP, zAxis));
		glm::vec3 yAxis = glm::normalize(glm::cross(zAxis, xAxis));

		glm::mat4 rotate = glm::mat4(1.0f);
		rotate[0][0] = xAxis.x;
		rotate[1][0] = xAxis.y;
		rotate[2][0] = xAxis.z;
		rotate[0][1] = yAxis.x;
		rotate[1][1] = yAxis.y;
		rotate[2][1] = yAxis.z;
		rotate[0][2] = zAxis.x;
		rotate[1][2] = zAxis.y;
		rotate[2][2] = zAxis.z;

		glm::mat4 translate = glm::mat4(1.0f);
		translate[3][0] = -CAMERA_POS.x;
		translate[3][1] = -CAMERA_POS.y;
		translate[3][2] = -CAMERA_POS.z;

		return rotate * translate;
	}
}