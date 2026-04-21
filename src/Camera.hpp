#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Util.h"

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
	};

	OrthonormalBasis normalize(OrthonormalBasis const&);
	bool isNormalized(OrthonormalBasis const&);
	bool isNormalized(glm::vec3 const&);

	class Camera {
		static constexpr float MOVE = 0.003f;

		glm::vec3 pos{};
		glm::vec3 worldY{};
		OrthonormalBasis basis{};
		float zoom{};

		public:
		explicit Camera(glm::vec3 const& POS, glm::vec3 const& WORLD_Y, EulerAngles const& ROTATION);
		void update();

		friend glm::mat4 convertViewMatrix(Camera const&);
		friend glm::mat4 convertProjMatrix(Camera const&);
		friend void glfwMouseMovedCallback(GLFWwindow* window, double x, double y);
		friend void glfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
		friend glm::vec3 keyboardInputMove(GLFWwindow* window);
	};

	inline Camera gCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), EulerAngles{});

	glm::mat4 convertViewMatrix(Camera const&);
	glm::mat4 convertProjMatrix(Camera const&);

	glm::mat3 rotate(glm::vec3 const&, float const&);
	OrthonormalBasis applyRotation(OrthonormalBasis const&, EulerAngles const&);
	
	inline float gZoomAdd{};
	void glfwScrollCallback(GLFWwindow* window, double x, double y);
	inline EulerAngles gRotateAdd{};
	void glfwMouseMovedCallback(GLFWwindow* window, double x, double y);
	glm::vec3 keyboardInputMove(GLFWwindow* window);
}