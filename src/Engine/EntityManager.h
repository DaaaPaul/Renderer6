#pragma once

#include "CameraComponent.hpp"
#include "Utility/Entity.hpp"

namespace EntityManager {
	inline Entity g_camera;

	void init();
	void destroy();
}