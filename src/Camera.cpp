#include <iostream>
#include "Camera.hpp"
#include "Swapchain.h"

namespace Engine {
	bool Basis::isNormalized(glm::vec3 const& V) {
		return Util::equal(glm::normalize(V), V);
	}
	
	bool Basis::isNormalized(Basis const& B) {
		Basis normalized = normalize(B);
		return Util::equal(normalized.x, B.x) && Util::equal(normalized.y, B.y) && Util::equal(normalized.z, B.z);
	}
	
	Basis Basis::normalize(Basis const& B) {
		return Basis{ glm::normalize(B.x), glm::normalize(B.y), glm::normalize(B.z) };
	}

	glm::mat3 Basis::rotate(glm::vec3 const& AXIS, float const& ANGLE_CC) {
		assert(isNormalized(AXIS));

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

	Basis Basis::applyRotation(Basis const& BASIS, Angles const& EULER) {
		assert(isNormalized(BASIS));
		
		Basis moved(BASIS);
		
		const glm::mat3 ABOUT_X = rotate(moved.x, EULER.x);
		moved.y = ABOUT_X * moved.y;
		moved.z = ABOUT_X * moved.z;

		const glm::mat3 ABOUT_Y = rotate(moved.y, EULER.y);
		moved.x = ABOUT_Y * moved.x;
		moved.z = ABOUT_Y * moved.z;

		const glm::mat3 ABOUT_Z = rotate(moved.z, EULER.z);
		moved.x = ABOUT_Z * moved.x;
		moved.y = ABOUT_Z * moved.y;

		return moved;
	}

	Camera::Camera(glm::vec3 const& POS, Angles const& ROTATION, float const& ZOOM) :
		pos(POS), basis(Basis::applyRotation(DEFAULT_BASIS, ROTATION)), zoom{ DEFAULT_ZOOM + ZOOM } {}

	void Camera::updateKeyboard(float const& DELTA) {
		const float MOVE = SPEED * DELTA;

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

	void Camera::updateMouse(float const& X, float const& Y) {
		basis = Basis::applyRotation(basis, Angles{Y * SENSITIVITY, X * SENSITIVITY, 0.0f});
	}

	void Camera::updateScrolled(float const& Y) {
		zoom += Y * SCROLL_SENSITIVITY;
	}

	void Camera::mouseMoved(GLFWwindow* window, double x, double y) {
		static bool first = true;
		static float lastX{};
		static float lastY{};

		if(first) {
			lastX = x;
			lastY = y;
			first = false;
		}

		float xOff = x - lastX;
		float yOff = y - lastY;
		lastX = x;
		lastY = y;

		gCamera.updateMouse(xOff, yOff);
	}

	void Camera::scrolled(GLFWwindow* window, double x, double y) {
		gCamera.updateScrolled(y);
	}

	glm::mat4 Camera::view(Camera const& CAMERA) {
		return glm::mat4(
			CAMERA.basis.x.x, CAMERA.basis.y.x, CAMERA.basis.z.x, 0.0f,
			CAMERA.basis.x.y, CAMERA.basis.y.y, CAMERA.basis.z.y, 0.0f,
			CAMERA.basis.x.z, CAMERA.basis.y.z, CAMERA.basis.z.z, 0.0f,
			-(CAMERA.basis.x.x * CAMERA.pos.x + CAMERA.basis.x.y * CAMERA.pos.y + CAMERA.basis.x.z * CAMERA.pos.z), -(CAMERA.basis.y.x * CAMERA.pos.x + CAMERA.basis.y.y * CAMERA.pos.y + CAMERA.basis.y.z * CAMERA.pos.z), -(CAMERA.basis.z.x * CAMERA.pos.x + CAMERA.basis.z.y * CAMERA.pos.y + CAMERA.basis.z.z * CAMERA.pos.z), 1.0f
		);
	}

	glm::mat4 Camera::proj(Camera const& CAMERA) {
		return glm::perspective(CAMERA.zoom, Backend::Window::gAspectRatio, 0.1f, 100.0f);
	}
}