#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include "Window.h"
#include "utility/Vulkan.h"

namespace Instance {
	inline VkInstance g_instance{};

	inline std::vector<const char*> g_layers;
	inline std::vector<const char*> g_extensions;

	inline VkApplicationInfo g_app_info{};
	inline VkInstanceCreateInfo g_create_info{};

	void init();
	void destroy();

	void set_layers();
	void set_extensions();
	void check_have_layers(const std::vector<const char*>* p_REQUIRED_LAYERS);
	void check_have_extensions(const std::vector<const char*>* p_REQUIRED_EXTENSIONS);
	inline std::vector<const char*>* get_layers() {
		return &g_layers;
	}
	inline std::vector<const char*>* get_extensions() {
		return &g_extensions;
	}

	inline void set_create_info() {
		g_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		g_app_info.apiVersion = VK_API_VERSION_1_3;

		g_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		#ifdef __APPLE__
		g_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#endif
		g_create_info.pApplicationInfo = &g_app_info;
		g_create_info.enabledLayerCount = static_cast<uint32_t>(g_layers.size());
		g_create_info.ppEnabledLayerNames = g_layers.data();
		g_create_info.enabledExtensionCount = static_cast<uint32_t>(g_extensions.size());
		g_create_info.ppEnabledExtensionNames = g_extensions.data();
	}
	inline VkInstanceCreateInfo* get_create_info() {
		return &g_create_info;
	}

	inline void set_instance() {
		Vulkan::check(vkCreateInstance(&g_create_info, nullptr, &g_instance), "set_instance: failed");
	}
	inline VkInstance get_instance() {
		return g_instance;
	}
	inline void destroy_instance() {
		vkDestroyInstance(g_instance, nullptr);
	}
}
