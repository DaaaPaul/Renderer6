#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "Backend/Instance.h"
#include "Backend/Window.h"
#include "Utility/Utility.h"
#include "Utility/Vulkan.h"

namespace Instance {
	std::vector<const char*> init_extensions() {
		std::vector<const char*> extensions{};

		extensions = Window::get_instance_window_extensions();
		#ifdef __APPLE__
		extensions.push_back("VK_KHR_portability_enumeration");
		#endif

		return extensions;
	}
	
	void init() {
		check_have_layers(g_layers);
		check_have_extensions(g_extensions);
		g_instance = create_instance();
	}

	void destroy() {
		vkDestroyInstance(g_instance, nullptr);
	}

	void check_have_extensions(const std::vector<const char*>& needed_extensions) {
		uint32_t have_count{};
		vkEnumerateInstanceExtensionProperties(nullptr, &have_count, nullptr);
		std::vector<VkExtensionProperties> extensions(have_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &have_count, extensions.data());

		std::vector<std::string> extension_names{};
		for(const VkExtensionProperties& extension : extensions) {
			extension_names.emplace_back(extension.extensionName);
		}

		if(!Utility::contains_all(extension_names, Utility::to_string(needed_extensions))) {
			THROW_RUNTIME("check_have_extensions: does not have the required VkInstance extensions")
		}
	}

	void check_have_layers(const std::vector<const char*>& needed_layers) {
		uint32_t have_count{};
		vkEnumerateInstanceLayerProperties(&have_count, nullptr);
		std::vector<VkLayerProperties> layers(have_count);
		vkEnumerateInstanceLayerProperties(&have_count, layers.data());

		std::vector<std::string> layer_names{};
		for(const VkLayerProperties& layer : layers) {
			layer_names.emplace_back(layer.layerName);
		}

		if(!Utility::contains_all(layer_names, Utility::to_string(needed_layers))) {
			THROW_RUNTIME("check_have_layers: does not have the required VkInstance layers")
		}
	}

	VkInstance create_instance() {
		VkInstance instance{};

		constexpr VkInstanceCreateFlags flags = 
		#ifdef __APPLE__
		VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#else
		VK_NO_FLAGS;
		#endif

		VkApplicationInfo app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.apiVersion = VK_API_VERSION_1_3,
		};

		VkInstanceCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.flags = flags,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = UINT32(g_layers.size()),
			.ppEnabledLayerNames = g_layers.data(),
			.enabledExtensionCount = UINT32(g_extensions.size()),
			.ppEnabledExtensionNames = g_extensions.data(),
		};

		VK_CHECK(vkCreateInstance(&create, nullptr, &instance), "Failed to create instance")

		return instance;
	}
}
