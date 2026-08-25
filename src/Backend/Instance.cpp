#include "backend/Instance.h"

namespace Instance {
	void init() {
		set_layers();
		set_extensions();
		set_create_info();
		set_instance();
	}

	void destroy() {
		destroy_instance();
	}

	void set_layers() {
		g_layers = { "VK_LAYER_KHRONOS_validation" };

		check_have_layers(&g_layers);
	}
	void set_extensions() {
		std::vector<const char*> extensions(Window::get_instance_window_extensions());
		#ifdef __APPLE__
		extensions.push_back("VK_KHR_portability_enumeration");
		#endif

		g_extensions = extensions;

		check_have_extensions(&g_extensions);
	}

	void check_have_layers(const std::vector<const char*>* p_REQUIRED_LAYERS) {
		uint32_t have_count{};
		vkEnumerateInstanceLayerProperties(&have_count, nullptr);
		std::vector<VkLayerProperties> layers(have_count);
		vkEnumerateInstanceLayerProperties(&have_count, layers.data());

		std::vector<std::string> layer_names{};
		for(const VkLayerProperties& layer : layers) {
			layer_names.emplace_back(layer.layerName);
		}

		if(!Utility::contains_all(layer_names, Utility::to_string(p_REQUIRED_LAYERS))) {
			throw std::runtime_error("check_have_layers: does not have the required VkInstance layers");
		}
	}

	void check_have_extensions(const std::vector<const char*>* p_REQUIRED_EXTENSIONS) {
		uint32_t have_count{};
		vkEnumerateInstanceExtensionProperties(nullptr, &have_count, nullptr);
		std::vector<VkExtensionProperties> extensions(have_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &have_count, extensions.data());

		std::vector<std::string> extension_names{};
		for(const VkExtensionProperties& extension : extensions) {
			extension_names.emplace_back(extension.extensionName);
		}

		if(!Utility::contains_all(extension_names, Utility::to_string(p_REQUIRED_EXTENSIONS))) {
			throw std::runtime_error("check_have_extensions: does not have the required VkInstance extensions");
		}
	}
}
