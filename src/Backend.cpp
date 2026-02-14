#include <iostream>
#include "Backend.hpp"

namespace Backend {
	Backend::Backend() :
		mpInstance{},
		mpWindow{},
		mParameters{} {}

	Backend::Backend(Window* givenWindow, BackendConstructParameters const& GIVEN_VULKAN_BACKEND_CREATE_PARAMETERS) :
		mpInstance{},
		mpWindow{ givenWindow },
		mParameters{ GIVEN_VULKAN_BACKEND_CREATE_PARAMETERS } {

		// route mCreateInfo's pointers properly
		mParameters.mCreateInfo.pApplicationInfo = &mParameters.mAPP_INFO;
		mParameters.mCreateInfo.enabledExtensionCount = static_cast<uint32_t>(mParameters.mENABLED_EXTENSIONS.size());
		mParameters.mCreateInfo.ppEnabledExtensionNames = mParameters.mENABLED_EXTENSIONS.data();
		mParameters.mCreateInfo.enabledLayerCount = static_cast<uint32_t>(mParameters.mENABLED_LOADER_LAYERS.size());
		mParameters.mCreateInfo.ppEnabledLayerNames = mParameters.mENABLED_LOADER_LAYERS.data();

		// checks
		Backend::sCheckHaveInstanceExtensions(mParameters.mENABLED_EXTENSIONS);
		Backend::sCheckHaveLoaderLayers(mParameters.mENABLED_LOADER_LAYERS);

		// construct the VkInstance
		CHECK_VK_SUCCESS(
			vkCreateInstance(&mParameters.mCreateInfo, nullptr, &mpInstance),
			"Failed to create instance"
		)
	}

	Backend::~Backend() {
		vkDestroyInstance(mpInstance, nullptr);
	}

	[[nodiscard]] Backend::BackendConstructParameters Backend::sGetConstructParameters() {
		const std::vector<const char*> ENABLED_LOADER_LAYERS{ "VK_LAYER_KHRONOS_validation" };
		std::vector<const char*> enabledExtensions(Window::sGetGlfwWindowExtensions());
		#ifdef __APPLE__
		enabledExtensions.push_back("VK_KHR_portability_enumeration");
		#endif
		const VkApplicationInfo APP_INFO{
				.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.pNext = nullptr,
				.pApplicationName = nullptr,
				.applicationVersion = 0,
				.pEngineName = nullptr,
				.engineVersion = 0,
				.apiVersion = VK_API_VERSION_1_3,
		};
		const VkInstanceCreateInfo INSTANCE_CREATE_INFO{
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pNext = nullptr,
				.flags =
				#ifdef _WIN32 
				0,
				#endif
				#ifdef __APPLE__
				VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
				#endif
				.pApplicationInfo = nullptr, // must route later
				.enabledLayerCount = 0,
				.ppEnabledLayerNames = nullptr, // must route later
				.enabledExtensionCount = 0,
				.ppEnabledExtensionNames = nullptr, // must route later
		};
    
		return BackendConstructParameters(ENABLED_LOADER_LAYERS, enabledExtensions, APP_INFO, INSTANCE_CREATE_INFO);
	}
    
	void Backend::sCheckHaveInstanceExtensions(std::vector<const char*> const& CHECK_HAVE_ME) {
		if(CHECK_HAVE_ME.empty()) {
			return;
		}

		uint32_t instanceExtensionsCount{};
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, nullptr);
		std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, instanceExtensionProperties.data());

		std::vector<std::string> instanceExtensionNames{};
		for(VkExtensionProperties const& SINGLE_EXTENSION_PROPERTIES : instanceExtensionProperties) {
			instanceExtensionNames.push_back(SINGLE_EXTENSION_PROPERTIES.extensionName);
		}
		std::vector<std::string> checkHaveMeNames{};
		for(const char* const& CHECK_ME_NAME : CHECK_HAVE_ME) {
			checkHaveMeNames.push_back(CHECK_ME_NAME);
		}

		CHECK_CONTAINS_ALL(instanceExtensionNames, checkHaveMeNames, "Your vulkan installation does not have the required instance extensions")
	}

	void Backend::sCheckHaveLoaderLayers(std::vector<const char*> const& CHECK_HAVE_ME) {
		if(CHECK_HAVE_ME.empty()) {
			return;
		}

		uint32_t loaderLayerCount{};
		vkEnumerateInstanceLayerProperties(&loaderLayerCount, nullptr);
		std::vector<VkLayerProperties> loaderLayerProperties(loaderLayerCount);
		vkEnumerateInstanceLayerProperties(&loaderLayerCount, loaderLayerProperties.data());

		std::vector<std::string> loaderLayerNames{};
		for(VkLayerProperties const& singleLayerProperties : loaderLayerProperties) {
			loaderLayerNames.push_back(std::string(singleLayerProperties.layerName));
		}
		std::vector<std::string> checkHaveMeNames{};
		for (const char* const& CHECK_ME_NAME : CHECK_HAVE_ME) {
			checkHaveMeNames.push_back(CHECK_ME_NAME);
		}

		CHECK_CONTAINS_ALL(loaderLayerNames, checkHaveMeNames, "Your vulkan installation does not have the required loader layers")
	}
}
