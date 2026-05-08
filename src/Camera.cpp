#include "Camera.hpp"
#include "Swapchain.h"

bool Basis::is_normalized(const glm::vec3& vec3) {
	return Util::equal(glm::normalize(vec3), vec3);
}

glm::mat3 Basis::rotate(const glm::vec3& axis, const float& angle_cc) {
	assert(is_normalized(axis));

	const float COS = std::cos(angle_cc);
	const float SIN = std::sin(angle_cc);
	const float ONE_MINUS_COS = 1.0f - std::cos(angle_cc);

	return glm::mat3(
		COS + axis.x * axis.x * ONE_MINUS_COS,
		axis.y * axis.x * ONE_MINUS_COS - axis.z * SIN,
		axis.z * axis.x * ONE_MINUS_COS + axis.y * SIN,

		axis.x * axis.y * ONE_MINUS_COS + axis.z * SIN,
		COS + axis.y * axis.y * ONE_MINUS_COS,
		axis.z * axis.y * ONE_MINUS_COS - axis.x * SIN,

		axis.x * axis.z * ONE_MINUS_COS - axis.y * SIN,
		axis.y * axis.z * ONE_MINUS_COS + axis.x * SIN,
		COS + axis.z * axis.z * ONE_MINUS_COS
	);
}

Basis Basis::apply_rotation(const Basis& basis, const Angles& euler_angles) {
	assert(is_normalized(basis.x) && is_normalized(basis.y) && is_normalized(basis.z));
		
	Basis moved(basis);
		
	const glm::mat3 ABOUT_X = rotate(moved.x, euler_angles.x);
	moved.y = ABOUT_X * moved.y;
	moved.z = ABOUT_X * moved.z;

	const glm::mat3 ABOUT_Y = rotate(moved.y, euler_angles.y);
	moved.x = ABOUT_Y * moved.x;
	moved.z = ABOUT_Y * moved.z;

	const glm::mat3 ABOUT_Z = rotate(moved.z, euler_angles.z);
	moved.x = ABOUT_Z * moved.x;
	moved.y = ABOUT_Z * moved.y;

	return moved;
}

Camera::Camera(const glm::vec3& pos, const Angles& starting_rotation, const float& starting_zoom) :
	pos(pos), basis(Basis::apply_rotation(DEFAULT_BASIS, starting_rotation)), zoom{ DEFAULT_ZOOM + starting_zoom } {}

void Camera::update_position(const float& delta_time) {
	const float MOVE = SPEED * delta_time;

	if(PRESSED(GLFW_KEY_W)) {
		pos += -(MOVE * basis.z);
	}
	if(PRESSED(GLFW_KEY_S)) {
		pos += MOVE * basis.z;
	}
	if(PRESSED(GLFW_KEY_A)) {
		pos += -(MOVE * basis.x);
	}
	if(PRESSED(GLFW_KEY_D)) {
		pos += MOVE * basis.x;
	}
	if(PRESSED(GLFW_KEY_SPACE) && PRESSED(GLFW_KEY_LEFT_SHIFT)) {
		pos += -(MOVE * basis.y);
	} else if(PRESSED(GLFW_KEY_SPACE)) {
		pos += MOVE * basis.y;
	}
}

void Camera::update_rotation(const float& x_offset, const float& y_offset) {
	basis = Basis::apply_rotation(basis, Angles{y_offset * SENSITIVITY, x_offset * SENSITIVITY, 0.0f});
}

void Camera::update_zoom(const float& y_offset) {
	zoom += y_offset * SCROLL_SENSITIVITY;
}

void Camera::mouse_moved_callback(GLFWwindow* window, double x, double y) {
	static bool first_callback = true;
	static float previous_x{};
	static float previous_y{};

	if(first_callback) {
		previous_x = x;
		previous_y = y;
		first_callback = false;
	}

	float x_offset = x - previous_x;
	float y_offset = y - previous_y;
	previous_x = x;
	previous_y = y;

	g_camera.update_rotation(x_offset, y_offset);
}

void Camera::scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
	g_camera.update_zoom(y_offset);
}

glm::mat4 Camera::to_view_matrix(const Camera& cam) {
	return glm::mat4(
		cam.basis.x.x, cam.basis.y.x, cam.basis.z.x, 0.0f,
		cam.basis.x.y, cam.basis.y.y, cam.basis.z.y, 0.0f,
		cam.basis.x.z, cam.basis.y.z, cam.basis.z.z, 0.0f,
		-(cam.basis.x.x * cam.pos.x + cam.basis.x.y * cam.pos.y + cam.basis.x.z * cam.pos.z), -(cam.basis.y.x * cam.pos.x + cam.basis.y.y * cam.pos.y + cam.basis.y.z * cam.pos.z), -(cam.basis.z.x * cam.pos.x + cam.basis.z.y * cam.pos.y + cam.basis.z.z * cam.pos.z), 1.0f
	);
}

glm::mat4 Camera::to_projection_matrix(const Camera& cam) {
	return glm::perspective(cam.zoom, Window::gAspectRatio, 0.1f, 100.0f);
}
