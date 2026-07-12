#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <cmath>
#include "Utility/Vulkan.h"
#include "Backend/Window.h"
#include "Engine/CameraComponent.hpp"

bool Basis::is_normalized(const glm::vec3& vec3) {
	return glm::normalize(vec3) == vec3;
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
		
	glm::mat3 about_x = rotate(moved.x, euler_angles.x);
	moved.y = about_x * moved.y;
	moved.z = about_x * moved.z;

	glm::mat3 about_y = rotate(moved.y, euler_angles.y);
	moved.x = about_y * moved.x;
	moved.z = about_y * moved.z;

	glm::mat3 about_z = rotate(moved.z, euler_angles.z);
	moved.x = about_z * moved.x;
	moved.y = about_z * moved.y;

	return moved;
}

CameraComponent::CameraComponent(IdPool<Component>* entity, const glm::vec3& position, const Angles& rotation, float zoom) :
	Component{ entity },
	position(DEFAULT_POSITION + position), 
	basis(Basis::apply_rotation(DEFAULT_BASIS, rotation)), 
	zoom{ DEFAULT_ZOOM + zoom } {
	
}

glm::vec3 CameraComponent::process_position(const Basis& basis, const glm::vec3& vec3, float delta_time) {
	float move_amount = SPEED * delta_time;
	glm::vec3 change{};

	if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_W)) {
		change -= move_amount * basis.z;
	}
	if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_S)) {
		change += move_amount * basis.z;
	}
	if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_A)) {
		change -= move_amount * basis.x;
	}
	if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_D)) {
		change += move_amount * basis.x;
	}
	if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_SPACE) && glfwGetKey(Window::g_glfw_window, GLFW_KEY_LEFT_SHIFT)) {
		change -= move_amount * basis.y;
	} else if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_SPACE)) {
		change += move_amount * basis.y;
	}

	return vec3 + change;
}

Basis CameraComponent::process_rotation(const Basis& basis, float x_offset, float y_offset) {
	return Basis::apply_rotation(basis, Angles{y_offset * SENSITIVITY, x_offset * SENSITIVITY, 0.0f});
}

float CameraComponent::process_zoom(const float& zoom, float y_offset) {
	return zoom + y_offset * SCROLL_SENSITIVITY;
}

void CameraComponent::mouse_moved_callback(GLFWwindow* glfw_window, double x, double y) {
	CameraComponent* camera_component = Window::g_window_user_pointer->camera_component;
	CHECK_NULLPTR(camera_component, "mouse_moved_callback: camera_component is a nullptr")

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

	camera_component->basis = process_rotation(camera_component->basis, x_offset, y_offset);
}

void CameraComponent::scroll_callback(GLFWwindow* glfw_window, double x_offset, double y_offset) {
	CameraComponent* camera_component = Window::g_window_user_pointer->camera_component;
	CHECK_NULLPTR(camera_component, "scroll_callback: camera_component is a nullptr")
	
	camera_component->zoom = process_zoom(camera_component->zoom, y_offset);
}

glm::mat4 CameraComponent::to_view_matrix(const CameraComponent& cam) {
	return glm::mat4(
		cam.basis.x.x, cam.basis.y.x, cam.basis.z.x, 0.0f,
		cam.basis.x.y, cam.basis.y.y, cam.basis.z.y, 0.0f,
		cam.basis.x.z, cam.basis.y.z, cam.basis.z.z, 0.0f,
		-(cam.basis.x.x * cam.position.x + cam.basis.x.y * cam.position.y + cam.basis.x.z * cam.position.z), -(cam.basis.y.x * cam.position.x + cam.basis.y.y * cam.position.y + cam.basis.y.z * cam.position.z), -(cam.basis.z.x * cam.position.x + cam.basis.z.y * cam.position.y + cam.basis.z.z * cam.position.z), 1.0f
	);
}

glm::mat4 CameraComponent::to_projection_matrix(const CameraComponent& cam) {
	return glm::perspective(cam.zoom, Window::get_aspect_ratio(), 0.1f, 100.0f);
}
