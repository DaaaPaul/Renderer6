#include <iostream>
#include "Instance.hpp"

namespace Backend {
	void Instance::checkHaveExtensions(std::vector<const char*> const& NECESSARY_EXTENSIONS) {
		if(NECESSARY_EXTENSIONS.empty()) {
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
		for(const char* const& CHECK_ME_NAME : NECESSARY_EXTENSIONS) {
			checkHaveMeNames.push_back(CHECK_ME_NAME);
		}

		CHECK_CONTAINS_ALL(instanceExtensionNames, checkHaveMeNames, "Your vulkan installation does not have the required instance extensions")
	}

	void Instance::checkHaveLayers(std::vector<const char*> const& NECESSARY_LAYERS) {
		if(NECESSARY_LAYERS.empty()) {
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
		for (const char* const& CHECK_ME_NAME : NECESSARY_LAYERS) {
			checkHaveMeNames.push_back(CHECK_ME_NAME);
		}

		CHECK_CONTAINS_ALL(loaderLayerNames, checkHaveMeNames, "Your vulkan installation does not have the required loader layers")
	}

	Instance::Instance(Window* pGivenWindow, CreateInfo&& givenCreateInfo) :
		pInstance{},
		pWindow{ pGivenWindow },
		CREATE_INFO{ std::move(givenCreateInfo) } {

		Instance::checkHaveExtensions(CREATE_INFO.extensions);
		Instance::checkHaveLayers(CREATE_INFO.layers);

		CHECK_VK_SUCCESS(
			vkCreateInstance(&CREATE_INFO.createInfo, nullptr, &pInstance),
			"Failed to create instance"
		)
	}

	Instance::~Instance() {
		vkDestroyInstance(pInstance, nullptr);
	}
}
