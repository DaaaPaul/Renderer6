#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanBackend.hpp"

class Device {
private:
    VulkanBackend* backend;

    const uint16_t WINDOW_WIDTH;
    const uint16_t WINDOW_HEIGHT;
    const std::string WINDOW_NAME;

    void arise();

public:
    Device();
    ~Device();

    DELETE_COPY_CONSTRUCTORS(Device)
    DELETE_MOVE_CONSTRUCTORS(Device)
};