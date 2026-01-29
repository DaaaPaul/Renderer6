#include <iostream>
#include "VulkanBackendWrapper.hpp"
#include "VulkanPFNs.h"
#include "GlobalCreateInfos.h"

VulkanBackendWrapper::VulkanBackendWrapper(GlfwWindowWrapper* givenGlfwWindowWrapper, VulkanBackendWrapperConstructParameters const& GIVEN_VULKAN_BACKEND_CREATE_PARAMETERS) :
    mInstance{},
    mGlfwWindowWrapper{ givenGlfwWindowWrapper },
    mParameters{ GIVEN_VULKAN_BACKEND_CREATE_PARAMETERS } {

    // route mCreateInfo's pointers properly
    mParameters.mCreateInfo.pApplicationInfo = &mParameters.mAPP_INFO;
    mParameters.mCreateInfo.enabledExtensionCount = static_cast<uint32_t>(mParameters.mENABLED_EXTENSIONS.size());
    mParameters.mCreateInfo.ppEnabledExtensionNames = mParameters.mENABLED_EXTENSIONS.data();
    mParameters.mCreateInfo.enabledLayerCount = static_cast<uint32_t>(mParameters.mENABLED_LOADER_LAYERS.size());
    mParameters.mCreateInfo.ppEnabledLayerNames = mParameters.mENABLED_LOADER_LAYERS.data();

    // print parameters
    std::cout << "SET VULKAN BACKEND CREATE PARAMETERS:\n";
    std::cout << "\t-INSTANCE API VERSION: " << UINT32_TO_VK_API_VERSION_CSTR(mParameters.mCreateInfo.pApplicationInfo->apiVersion) << "\n";
    for(int i = 0; i < mParameters.mCreateInfo.enabledExtensionCount; i++) {
        std::cout << "\t-INSTANCE EXTENSION: " << mParameters.mCreateInfo.ppEnabledExtensionNames[i] << "\n";
    }
    for(int i = 0; i < mParameters.mCreateInfo.enabledLayerCount; i++) {
        std::cout << "\t-LOADER LAYER: " << mParameters.mCreateInfo.ppEnabledLayerNames[i] << "\n";
    }

    VulkanBackendWrapper::checkHaveInstanceExtensions(vectorStringInstanceExtensionNames);
    VulkanBackendWrapper::checkHaveLoaderLayers(vectorStringLoaderLayerNames);
    arise();
}

VulkanBackendWrapper::~VulkanBackendWrapper() {
    std::cout << "Destroying VulkanBackendWrapper...\n";

    VulkanPFNs::gpVkDestroyInstance(mInstance, nullptr);

    std::cout << "Destroyed VulkanBackendWrapper\n";
}

void VulkanBackendWrapper::arise() {
    std::cout << "Creating VulkanBackendWrapper...\n";

    CHECK_VK_SUCCESS(
    VulkanPFNs::gpVkCreateInstance(mINSTANCE_CREATE_INFO, nullptr, &mInstance),
    "Failed to create instance"
    )

    setVulkanPFNsInstanceInUseToInstanceMember();

    std::cout << "Created VulkanBackendWrapper\n";
}

VulkanBackendWrapper::VulkanBackendWrapperConstructParameters VulkanBackendWrapper::getConstructParameters() {
    const std::vector<const char*> ENABLED_LOADER_LAYERS{ "VK_LAYER_KHRONOS_validation" };
    std::vector<const char*> enabledExtensions(GlfwWindowWrapper::getGlfwWindowExtensions());
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
    
    return VulkanBackendWrapperConstructParameters(ENABLED_LOADER_LAYERS, enabledExtensions, APP_INFO, INSTANCE_CREATE_INFO);
}
    
void VulkanBackendWrapper::checkHaveInstanceExtensions(std::vector<const char*> const& CHECK_HAVE_ME) {
    if(CHECK_HAVE_ME.empty()) {
        return;
    }

    uint32_t instanceExtensionsCount{};
    VulkanPFNs::gpVkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, nullptr);
    std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionsCount);
    VulkanPFNs::gpVkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, instanceExtensionProperties.data());

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

void VulkanBackendWrapper::checkHaveLoaderLayers(std::vector<const char*> const& CHECK_HAVE_ME) {
    if(CHECK_HAVE_ME.empty()) {
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
    std::vector<std::string> checkHaveMeNames{};
    for (const char* const& CHECK_ME_NAME : CHECK_HAVE_ME) {
        checkHaveMeNames.push_back(CHECK_ME_NAME);
    }

    CHECK_CONTAINS_ALL(loaderLayerNames, checkHaveMeNames, "Your vulkan installation does not have the required loader layers")
}

void VulkanBackendWrapper::setVulkanPFNsInstanceInUseToInstanceMember() const {
    VulkanPFNs::fSetInstance(mInstance);
}