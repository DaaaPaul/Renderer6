#include <iostream>
#include "Camera.hpp"
#include "Swapchain.h"

namespace Engine {
	OrthonormalBasis normalize(OrthonormalBasis const& B) {
		return OrthonormalBasis{ glm::normalize(B.x), glm::normalize(B.y), glm::normalize(B.z) };
	}

	bool isNormalized(OrthonormalBasis const& B) {
		OrthonormalBasis normalized = normalize(B);
		return Util::equal(normalized.x, B.x) && Util::equal(normalized.y, B.y) && Util::equal(normalized.z, B.z);
	}

	bool isNormalized(glm::vec3 const& V) {
		return Util::equal(glm::normalize(V), V);
	}

	Camera::Camera(glm::vec3 const& POS, glm::vec3 const& WORLD_Y, EulerAngles const& ROTATION) :
		pos(POS), worldY(WORLD_Y), basis(applyRotation(OrthonormalBasis{}, ROTATION)), zoom{ DA_PI / 4.0f } {}

	void Camera::update() {
		glm::vec3 move = keyboardInputMove(Backend::Window::gGlfwWindow);
		glm::vec3 xMove = basis.x * move.x;
		glm::vec3 yMove = basis.y * move.y;
		glm::vec3 zMove = basis.z * move.z;

		std::cout << "basis.x:\n";
		std::cout << basis.x.x << " " << basis.x.y << " " << basis.x.z << "\n";
		std::cout << "xMove:\n";
		std::cout << xMove.x << " " << xMove.y << " " << xMove.z << "\n";

		pos += xMove;
		pos += yMove;
		pos += zMove;
		
		zoom = DA_PI / 4.0f + gZoomAdd;
		basis = applyRotation(OrthonormalBasis{}, gRotateAdd);
	}

	glm::mat4 convertViewMatrix(Camera const& CAMERA) {
		return glm::mat4(
			CAMERA.basis.x.x, CAMERA.basis.y.x, CAMERA.basis.z.x, 0.0f,
			CAMERA.basis.x.y, CAMERA.basis.y.y, CAMERA.basis.z.y, 0.0f,
			CAMERA.basis.x.z, CAMERA.basis.y.z, CAMERA.basis.z.z, 0.0f,
			-(CAMERA.basis.x.x * CAMERA.pos.x + CAMERA.basis.x.y * CAMERA.pos.y + CAMERA.basis.x.z * CAMERA.pos.z), -(CAMERA.basis.y.x * CAMERA.pos.x + CAMERA.basis.y.y * CAMERA.pos.y + CAMERA.basis.y.z * CAMERA.pos.z), -(CAMERA.basis.z.x * CAMERA.pos.x + CAMERA.basis.z.y * CAMERA.pos.y + CAMERA.basis.z.z * CAMERA.pos.z), 1.0f
		);
	}

	glm::mat4 convertProjMatrix(Camera const& CAMERA) {
		return glm::perspective(CAMERA.zoom, static_cast<float>(Swapchain::gImageSize.width) / static_cast<float>(Swapchain::gImageSize.height), 0.1f, 100.0f);
	}

	glm::mat3 rotate(glm::vec3 const& AXIS, float const& ANGLE_CC) {
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

	OrthonormalBasis applyRotation(OrthonormalBasis const& BASIS, EulerAngles const& EULER) {
		assert(isNormalized(BASIS));
		
		OrthonormalBasis moved(BASIS);
		
		glm::mat3 aboutX = rotate(moved.x, EULER.x);
		moved.y = aboutX * moved.y;
		moved.z = aboutX * moved.z;

		glm::mat3 aboutY = rotate(moved.y, EULER.y);
		moved.x = aboutY * moved.x;
		moved.z = aboutY * moved.z;

		glm::mat3 aboutZ = rotate(moved.z, EULER.z);
		moved.x = aboutZ * moved.x;
		moved.y = aboutZ * moved.y;

		return moved;
	}

	void glfwScrollCallback(GLFWwindow* window, double x, double y) {
		gZoomAdd += y / 20.0;
	}

	void glfwMouseMovedCallback(GLFWwindow* window, double x, double y) {
		static bool first = true;
		static float lastX = 0.0f;
		static float lastY = 0.0f;

		if(first) {
			lastX = x;
			lastY = y;
			first = false;
		}

		float xOff = x - lastX;
		float yOff = y - lastY;
		lastX = x;
		lastY = y;

		gRotateAdd.x += yOff / 500.0f;
		gRotateAdd.y += xOff / 500.0f;
	}

	glm::vec3 keyboardInputMove(GLFWwindow* window) {
		glm::vec3 posChange(0.0f, 0.0f, 0.0f);

		if(PRESSED(GLFW_KEY_W)) {
			posChange.z -= Camera::MOVE;
		}
		if(PRESSED(GLFW_KEY_S)) {
			posChange.z += Camera::MOVE;
		}
		if(PRESSED(GLFW_KEY_A)) {
			posChange.x -= Camera::MOVE;
		}
		if(PRESSED(GLFW_KEY_D)) {
			posChange.x += Camera::MOVE;
		}

		return posChange;
	}
}