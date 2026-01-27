#include <iostream>
#include "VulkanBackendWrapper.hpp"
#include "VulkanPFNs.h"
#include "GlobalCreateInfos.h"

VulkanBackendWrapper::VulkanBackendWrapper(GlfwWindowWrapper* givenGlfwWindowWrapper, VkInstanceCreateInfo const* GIVEN_INSTANCE_CREATE_INFO) :
    mInstance{},
    mGlfwWindowWrapper{ givenGlfwWindowWrapper },
    mINSTANCE_CREATE_INFO { GIVEN_INSTANCE_CREATE_INFO } {

    std::cout << "SET VULKAN BACKEND CREATE PARAMETERS:\n";

    std::cout << "\t-INSTANCE API VERSION: " << (UINT32_TO_VK_API_VERSION_CSTR(mINSTANCE_CREATE_INFO->pApplicationInfo->apiVersion)) << '\n';

    std::vector<std::string> vectorStringInstanceExtensionNames{};
    for(int i = 0; i < mINSTANCE_CREATE_INFO->enabledExtensionCount; i++) {
        vectorStringInstanceExtensionNames.push_back(std::string(mINSTANCE_CREATE_INFO->ppEnabledExtensionNames[i]));
        std::cout << "\t-INSTANCE EXTENSION: " << vectorStringInstanceExtensionNames[i] << "\n";
    }

    std::vector<std::string> vectorStringLoaderLayerNames{};

    for(int i = 0; i < mINSTANCE_CREATE_INFO->enabledLayerCount; i++) {
        vectorStringLoaderLayerNames.push_back(std::string(mINSTANCE_CREATE_INFO->ppEnabledLayerNames[i]));
        std::cout << "\t-LOADER LAYER: " << vectorStringLoaderLayerNames[i] << "\n";
    }

    checkHaveInstanceExtensions(vectorStringInstanceExtensionNames);
    checkHaveLoaderLayers(vectorStringLoaderLayerNames);
    arise();
}

VulkanBackendWrapper::~VulkanBackendWrapper() {
    std::cout << "Destroying VulkanBackendWrapper...\n";

    VulkanPFNs::gpVkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    VulkanPFNs::gpVkDestroyInstance(mInstance, nullptr);

    std::cout << "Destroyed VulkanBackendWrapper\n";
}

void VulkanBackendWrapper::arise() {
    std::cout << "Creating VulkanBackendWrapper...\n";

    CHECK_VK_SUCCESS(VulkanPFNs::gpVkCreateInstance(mINSTANCE_CREATE_INFO, nullptr, &mInstance))
    CHECK_VK_SUCCESS(glfwCreateWindowSurface(mInstance, mGlfwWindowWrapper->mGlfwWindow, nullptr, &mSurface))

    setVulkanPFNsInstanceInUseToInstanceMember();

    std::cout << "Created VulkanBackendWrapper\n";
}
    
void VulkanBackendWrapper::checkHaveInstanceExtensions(std::vector<std::string> const& checkHaveMe) const {
    if(checkHaveMe.empty()) {
        return;
    }

    uint32_t instanceExtensionsCount{};
    VulkanPFNs::gpVkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, nullptr);
    std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionsCount);
    VulkanPFNs::gpVkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, instanceExtensionProperties.data());

    std::vector<std::string> instanceExtensionNames{};
    for(VkExtensionProperties const& singleExtensionProperties : instanceExtensionProperties) {
        instanceExtensionNames.push_back(std::string(singleExtensionProperties.extensionName));
    }

    CHECK_CONTAINS_ALL(instanceExtensionNames, checkHaveMe)
}

void VulkanBackendWrapper::checkHaveLoaderLayers(std::vector<std::string> const& checkHaveMe) const {
    if(checkHaveMe.empty()) {
        return;
    }

    uint32_t loaderLayerCount{};
    VulkanPFNs::gpVkEnumerateInstanceLayerProperties(&loaderLayerCount, nullptr);
    std::vector<VkLayerProperties> loaderLayerProperties(loaderLayerCount);
    VulkanPFNs::gpVkEnumerateInstanceLayerProperties(&loaderLayerCount, loaderLayerProperties.data());

    std::vector<std::string> loaderLayerNames{};
    for(VkLayerProperties const& singleLayerProperties : loaderLayerProperties) {
        loaderLayerNames.push_back(std::string(singleLayerProperties.layerName));
    }

    CHECK_CONTAINS_ALL(loaderLayerNames, checkHaveMe)
}

void VulkanBackendWrapper::setVulkanPFNsInstanceInUseToInstanceMember() const {
    VulkanPFNs::fSetInstance(mInstance);
}