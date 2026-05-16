#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utility.h"

struct Angles {
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};
};

struct Basis {
	glm::vec3 x{1.0f, 0.0f, 0.0f};
	glm::vec3 y{0.0f, 1.0f, 0.0f};
	glm::vec3 z{0.0f, 0.0f, 1.0f};
	
	static bool is_normalized(const glm::vec3& vec3);
	static glm::mat3 rotate(const glm::vec3& axis, const float& angle_cc);
	static Basis apply_rotation(const Basis& basis, const Angles& euler_angles);
};

class Camera {
	static constexpr float SPEED = 10.5f;
	static constexpr float SCROLL_SENSITIVITY = 1.0f / 25.0f;
	static constexpr float SENSITIVITY = 1.0f / 1000.0f;
	static constexpr float DEFAULT_ZOOM = DA_PI / 4.0f;
	static constexpr Basis DEFAULT_BASIS = Basis{};

	public:
	static void mouse_moved_callback(GLFWwindow* window, double x, double y);
	static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

	private:
	glm::vec3 pos{};
	Basis basis{};
	float zoom{};

	public:
	explicit Camera(const glm::vec3& pos, const Angles& starting_rotation, const float& starting_zoom);

	void update_position(const float& delta_time);
	void update_rotation(const float& x_offset, const float& y_offset);
	void update_zoom(const float& y_offset);
	static glm::mat4 to_view_matrix(const Camera& cam);
	static glm::mat4 to_projection_matrix(const Camera& cam);
};

inline Camera g_camera(glm::vec3(0.0f, 0.0f, 10.0f), Angles{}, 0.0f);
