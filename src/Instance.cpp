#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "Instance.h"
#include "Util.h"

namespace Backend {
	namespace Instance {
		void init() {
			initExtensions();
			checkHaveLayers(gLayers);
			checkHaveExtensions(gExtensions);
			createInstance();
		}

		void deInit() {
			destroyInstance();
		}

		void initExtensions() {
			gExtensions = Util::Window::getVkWindowExtensions();
			#ifdef __APPLE__
			gExtensions.push_back("VK_KHR_portability_enumeration");
			#endif
		}

		void checkHaveExtensions(std::vector<const char*> const& EXTENSIONS) {
			uint32_t haveExtensionsCount{};
			vkEnumerateInstanceExtensionProperties(nullptr, &haveExtensionsCount, nullptr);
			std::vector<VkExtensionProperties> haveExtensions(haveExtensionsCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &haveExtensionsCount, haveExtensions.data());

			std::vector<std::string> haveExtensionsNames{};
			for(VkExtensionProperties const& HAVE : haveExtensions) {
				haveExtensionsNames.emplace_back(HAVE.extensionName);
			}

			if(!Util::containsAll(haveExtensionsNames, Util::toStringVector(EXTENSIONS))) {
				throw std::runtime_error("Your GPU does not have the required VkInstance extensions");
			}
		}

		void checkHaveLayers(std::vector<const char*> const& LAYERS) {
			uint32_t haveLayersCount{};
			vkEnumerateInstanceLayerProperties(&haveLayersCount, nullptr);
			std::vector<VkLayerProperties> haveLayers(haveLayersCount);
			vkEnumerateInstanceLayerProperties(&haveLayersCount, haveLayers.data());

			std::vector<std::string> haveLayersNames{};
			for(VkLayerProperties const& HAVE : haveLayers) {
				haveLayersNames.emplace_back(HAVE.layerName);
			}

			if(!Util::containsAll(haveLayersNames, Util::toStringVector(LAYERS))) {
				throw std::runtime_error("Your GPU does not have the required Vulkan layers");
			}
		}

		void createInstance() {
			VkInstanceCreateFlags instanceCreateFlags = 0;
			#ifdef __APPLE__
			instanceCreateFlags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
			#endif
			VkApplicationInfo appInfo{
				.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.apiVersion = VK_API_VERSION_1_3,
			};
			VkInstanceCreateInfo instanceCreate{
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.flags = instanceCreateFlags,
				.pApplicationInfo = &appInfo,
				.enabledLayerCount = UINT32(gLayers.size()),
				.ppEnabledLayerNames = gLayers.data(),
				.enabledExtensionCount = UINT32(gExtensions.size()),
				.ppEnabledExtensionNames = gExtensions.data(),
			};

			CHECK_VK_SUCCESS(vkCreateInstance(&instanceCreate, nullptr, &gInstance), "Failed to create instance")
		}
		
		void destroyInstance() {
			vkDestroyInstance(gInstance, nullptr);
		}
	}
}
