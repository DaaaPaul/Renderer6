#pragma once

#include "CameraComponent.hpp"
#include "Utility/IdPool.hpp"

namespace EntityManager {
	inline IdPool<Component> g_camera;

	void init();
	void destroy() noexcept;
}