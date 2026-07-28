#include "EntityManager.h"
#include "backend/Window.h"

namespace EntityManager {
	void init() {
		g_camera.add<CameraComponent>(&g_camera, glm::vec3(0.0f, 0.0f, 10.0f), Angles{0.0f, 0.0f, 0.0f}, 0.0f);

		Window::g_window_user_pointer->camera_component = g_camera.get<CameraComponent>();
	}

	void destroy() {
		g_camera.remove<CameraComponent>();
	}
}