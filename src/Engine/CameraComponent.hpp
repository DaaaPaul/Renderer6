#pragma once

#include <glm/fwd.hpp>
#include <GLFW/glfw3.h>
#include "Utility/Utility.h"
#include "Utility/IdPool.hpp"
#include "Component.hpp"

struct Angles {
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};
};

struct Basis {
	glm::vec3 x{1.0f, 0.0f, 0.0f};
	glm::vec3 y{0.0f, 1.0f, 0.0f};
	glm::vec3 z{0.0f, 0.0f, 1.0f};
	
	static glm::mat3 rotate(const glm::vec3& axis, const float& angle_cc);
	static Basis apply_rotation(const Basis& basis, const Angles& euler_angles);
};

class CameraComponent : public Component {
	static constexpr float SPEED = 10.5f;
	static constexpr float SCROLL_SENSITIVITY = 1.0f / 25.0f;
	static constexpr float SENSITIVITY = 1.0f / 1000.0f;

	static constexpr glm::vec3 DEFAULT_POSITION{0.0f, 0.0f, 0.0f};
	static constexpr Basis DEFAULT_BASIS = Basis{};
	static constexpr float DEFAULT_ZOOM = Utility::PI / 4.0f;

	public:
	static void mouse_moved_callback(GLFWwindow* window, double x, double y);
	static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

	private:
	glm::vec3 position{};
	Basis basis{};
	float zoom{};

	public:
	explicit CameraComponent(Entity* entity, const glm::vec3& position, const Angles& rotation, float zoom);

	glm::vec3 get_position() const {
		return position;
	}

	Basis get_basis() const {
		return basis;
	}

	float get_zoom() const {
		return zoom;
	}

	void set_position(const glm::vec3& position) {
		this->position = position;
	}

	void set_basis(const Basis& basis) {
		this->basis = basis;
	}

	void set_zoom(float zoom) {
		this->zoom = zoom;
	}

	static glm::vec3 process_position(const Basis& basis, const glm::vec3& vec3, float delta_time);
	static Basis process_rotation(const Basis& basis, float x_offset, float y_offset);
	static float process_zoom(const float& zoom, float y_offset);

	static glm::mat4 to_view_matrix(const CameraComponent& cam);
	static glm::mat4 to_projection_matrix(const CameraComponent& cam);
};