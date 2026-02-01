#include <iostream>
#include "VulkanPFNs.h"
#include "GlfwWindowWrapper.hpp"
#include "Common.h"

GlfwWindowWrapper::GlfwWindowWrapper(GlfwWindowWrapperParameters const& GIVEN_PARAMETERS) :
    mpGlfwWindow{},
    mPARAMETERS{ GIVEN_PARAMETERS } {
    // print parameters
    std::cout << "SET WINDOW CREATE PARAMETERS:\n"
        "\t-width: " << mPARAMETERS.mWIDTH << "\n"
        "\t-height: " << mPARAMETERS.mHEIGHT << "\n"
        "\t-name: " << mPARAMETERS.mNAME << "\n";

    // construct the GLFWwindow*
    std::cout << "Creating GlfwWindowWrapper...\n";

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mpGlfwWindow = glfwCreateWindow(mPARAMETERS.mWIDTH, mPARAMETERS.mHEIGHT, mPARAMETERS.mNAME, nullptr, nullptr);

    CHECK_NULLPTR(mpGlfwWindow, "glfwCreateWindow failed");

    std::cout << "Created GlfwWindowWrapper\n";
}

GlfwWindowWrapper::~GlfwWindowWrapper() {
    std::cout << "Destroying GlfwWindowWrapper...\n";

    glfwDestroyWindow(mpGlfwWindow);
    glfwTerminate();

    std::cout << "Destroyed GlfwWindowWrapper\n";
}

[[nodiscard]] std::vector<const char*> GlfwWindowWrapper::getGlfwWindowExtensions() {
    glfwInit();

    uint32_t requiredGlfwExtensionsCount{};
    const char** requiredGlfwExtensionsNames = glfwGetRequiredInstanceExtensions(&requiredGlfwExtensionsCount);
    std::vector<const char*> requiredGlfwExtensionsNamesVector{};

    if (!requiredGlfwExtensionsNames) {
        #ifdef __APPLE__
        requiredGlfwExtensionsNamesVector.push_back("VK_KHR_surface");
        requiredGlfwExtensionsNamesVector.push_back("VK_EXT_metal_surface");
        #endif

        #ifdef _WIN32
        CHECK_NULLPTR(requiredGlfwExtensionsNames, "glfwGetRequiredInstanceExtensions failed")
        #endif
    }

    for (int i = 0; i < requiredGlfwExtensionsCount; i++) {
        requiredGlfwExtensionsNamesVector.push_back(requiredGlfwExtensionsNames[i]);
    }

    return requiredGlfwExtensionsNamesVector;
}