#pragma once

#include "CameraComponent.hpp"
#include "engine/Entity.hpp"

namespace EntityManager {
	inline Entity g_camera;

	void init();
	void destroy();
}