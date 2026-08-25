#pragma once

#include <glm/fwd.hpp>
#include <GLFW/glfw3.h>
#include "utility/Utility.h"
#include "engine/Entity.hpp"
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
	
	static glm::mat3 rotate(glm::vec3 axis, float angle_cc);
	static Basis apply_rotation(Basis basis, Angles euler_angles);
};

class CameraComponent : public Component {
	public:
	explicit CameraComponent(Entity* entity, glm::vec3 position, Angles rotation, float zoom);

	glm::vec3 get_position() const {
		return position;
	}

	Basis get_basis() const {
		return basis;
	}

	float get_zoom() const {
		return zoom;
	}

	void set_position(glm::vec3 position) {
		this->position = position;
	}

	void set_basis(Basis basis) {
		this->basis = basis;
	}

	void set_zoom(float zoom) {
		this->zoom = zoom;
	}

	static void mouse_moved_callback(GLFWwindow* window, double x, double y);
	static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

	static glm::vec3 process_position(Basis basis, glm::vec3 vec3, float delta_time);
	static Basis process_rotation(Basis basis, float x_offset, float y_offset);
	static float process_zoom(float zoom, float y_offset);

	glm::mat4 view_matrix() const;
	glm::mat4 projection_matrix() const;

	private:
	static constexpr float SPEED = 10.5f;
	static constexpr float SCROLL_SENSITIVITY = 1.0f / 25.0f;
	static constexpr float SENSITIVITY = 1.0f / 1000.0f;

	static constexpr glm::vec3 DEFAULT_POSITION{0.0f, 0.0f, 0.0f};
	static constexpr Basis DEFAULT_BASIS = Basis{};
	static constexpr float DEFAULT_ZOOM = Utility::PI / 4.0f;

	static constexpr float FAR_PLANE = 100.0f;
	static constexpr float NEAR_PLANE = 0.1f;

	glm::vec3 position{};
	Basis basis{};
	float zoom{};
};