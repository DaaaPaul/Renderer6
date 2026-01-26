#include <iostream>
#include "VulkanBackend.hpp"
#include "VulkanPFNs.h"

VulkanBackend::VulkanBackend(VkInstanceCreateInfo const& GIVEN_INSTANCE_CREATE_INFO) :
    INSTANCE_CREATE_INFO { GIVEN_INSTANCE_CREATE_INFO } {

    std::cout << "SET VULKAN BACKEND CREATE PARAMETERS:\n";

    std::vector<std::string> vectorStringInstanceExtensionNames{};
    for(int i = 0; i < INSTANCE_CREATE_INFO.enabledExtensionCount; i++) {
        vectorStringInstanceExtensionNames.push_back(std::string(INSTANCE_CREATE_INFO.ppEnabledExtensionNames[i]));
        std::cout << "\t-INSTANCE EXTENSION: " << vectorStringInstanceExtensionNames[i] << "\n";
    }

    std::vector<std::string> vectorStringLoaderLayerNames{};
    for(int i = 0; i < INSTANCE_CREATE_INFO.enabledLayerCount; i++) {
        vectorStringInstanceExtensionNames.push_back(std::string(INSTANCE_CREATE_INFO.ppEnabledLayerNames[i]));
        std::cout << "\t-LOADER LAYER: " << vectorStringLoaderLayerNames[i] << "\n";
    }

    checkHaveInstanceExtensions(vectorStringInstanceExtensionNames);
    checkHaveLoaderLayers(vectorStringLoaderLayerNames);
    arise();
}

VulkanBackend::~VulkanBackend() {
    std::cout << "Destroying Vulkan backend...\n";

    pVkDestroyInstance(instance, nullptr);

    std::cout << "Destroyed Vulkan backend\n";
}

void VulkanBackend::arise() {
    std::cout << "Creating Vulkan backend...\n";

    CHECK_VK_SUCCESS(pVkCreateInstance(&INSTANCE_CREATE_INFO, nullptr, &instance))

    setInstance(instance);

    std::cout << "Created Vulkan backend\n";
}
    
void VulkanBackend::checkHaveInstanceExtensions(std::vector<std::string> const& checkHaveMe) const {
    if(checkHaveMe.empty()) {
        return;
    }

    uint32_t instanceExtensionsCount{};
    pVkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, nullptr);
    std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionsCount);
    pVkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, instanceExtensionProperties.data());

    std::vector<std::string> instanceExtensionNames{};
    for(VkExtensionProperties const& singleExtensionProperties : instanceExtensionProperties) {
        instanceExtensionNames.push_back(std::string(singleExtensionProperties.extensionName));
    }

    CHECK_CONTAINS_ALL(instanceExtensionNames, checkHaveMe)
}

void VulkanBackend::checkHaveLoaderLayers(std::vector<std::string> const& checkHaveMe) const {
    if(checkHaveMe.empty()) {
        return;
    }

    uint32_t loaderLayerCount{};
    pVkEnumerateInstanceLayerProperties(&loaderLayerCount, nullptr);
    std::vector<VkLayerProperties> loaderLayerProperties(loaderLayerCount);
    pVkEnumerateInstanceLayerProperties(&loaderLayerCount, loaderLayerProperties.data());

    std::vector<std::string> loaderLayerNames{};
    for(VkLayerProperties const& singleLayerProperties : loaderLayerProperties) {
        loaderLayerNames.push_back(std::string(singleLayerProperties.layerName));
    }

    CHECK_CONTAINS_ALL(loaderLayerNames, checkHaveMe)
}

std::vector<const char*> VulkanBackend::getGlfwWindowExtensions() {
    uint32_t requiredGlfwExtensionsCount{};
	const char** requiredGlfwExtensionsNames = glfwGetRequiredInstanceExtensions(&requiredGlfwExtensionsCount);

    std::vector<const char*> requiredGlfwExtensionsNamesVector{};
    for(int i = 0; i < requiredGlfwExtensionsCount; i++) {
        requiredGlfwExtensionsNamesVector.push_back(requiredGlfwExtensionsNames[i]);
    }

    return requiredGlfwExtensionsNamesVector;
}