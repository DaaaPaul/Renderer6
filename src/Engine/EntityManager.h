#pragma once

#include "CameraComponent.hpp"
#include "Utility/IdPool.hpp"

namespace EntityManager {
	inline Entity g_camera;

	void init();
	void destroy() noexcept;
}