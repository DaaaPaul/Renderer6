#include <iostream>
#include <stdexcept>
#include "Engine.h"
#include "FrameData.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "MemoryDevice.h"
#include "MemoryHost.h"
#include "Window.h"
#include "Pipelines.h"
#include "PipelineLayouts.h"
#include "Resources.h"

int main() {
    try {

	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cerr << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}