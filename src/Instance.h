#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Backend {
	namespace Instance {
		inline VkInstance gpInstance{};

		inline std::vector<const char*> gLayers{
			"VK_LAYER_KHRONOS_validation"
		};
		inline std::vector<const char*> gExtensions(
			[]() -> std::vector<const char*> {
				std::vector<const char*> extensions(Util::Window::getRequiredWindowExtensionsForInstance());
				#ifdef __APPLE__
				extensions.push_back("VK_KHR_portability_enumeration");
				#endif

				return extensions;
			}()
		);

		void init();
		void deInit();

		void createInstance();
		void destroyInstance() noexcept;

		void checkHaveExtensions(std::vector<const char*> const& EXTENSIONS);
		void checkHaveLayers(std::vector<const char*> const& LAYERS);
	}
}