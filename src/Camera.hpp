#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Util.h"

struct Angles {
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};
};

struct Basis {
	glm::vec3 x{1.0f, 0.0f, 0.0f};
	glm::vec3 y{0.0f, 1.0f, 0.0f};
	glm::vec3 z{0.0f, 0.0f, 1.0f};
	
	static bool isNormalized(glm::vec3 const&);
	static glm::mat3 rotate(glm::vec3 const&, float const&);
	static Basis applyRotation(Basis const&, Angles const&);
};

inline float gZoom{};
inline Angles gRotate{};

class Camera {
	static constexpr float SPEED = 10.5f;
	static constexpr float SCROLL_SENSITIVITY = 1.0f / 25.0f;
	static constexpr float SENSITIVITY = 1.0f / 1000.0f;
	static constexpr float DEFAULT_ZOOM = DA_PI / 4.0f;
	static constexpr Basis DEFAULT_BASIS = Basis{};

	static void mouseMoved(GLFWwindow* window, double x, double y);
	static void scrolled(GLFWwindow* window, double xOffset, double yOffset);

	friend void Window::setWindowCallbacks();

	glm::vec3 pos{};
	Basis basis{};
	float zoom{};

	public:
	explicit Camera(glm::vec3 const& POS, Angles const& ROTATION, float const& ZOOM);

	void updateKeyboard(float const&);
	void updateMouse(float const&, float const&);
	void updateScrolled(float const&);
	static glm::mat4 view(Camera const&);
	static glm::mat4 proj(Camera const&);
};

inline Camera gCamera(glm::vec3(0.0f, 0.0f, 10.0f), Angles{}, 0.0f);
