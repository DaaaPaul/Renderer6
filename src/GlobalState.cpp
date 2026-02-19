#include <cassert>
#include <array>
#include "Common.h"
#include "GlobalState.h"

namespace GlobalState {
	void Core::load() {
		(void) getKtxTexture2();
		(void) getWindow();
		(void) getInstance();
		(void) getDevices();
		(void) getSwapchain();
		(void) getHostVisibleMemory();
		(void) getDeviceLocalMemory();
		(void) getGraphicsPipeline();

		loaded = true;
	}

	ktxTexture2 const* Core::getKtxTexture2() {
		assert(loaded);

		static ktxTexture2 const*const gKTX_TEXTURE2 = DeviceMemory::Common::fKtxLoadImage(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\textures\Lumberjack Sion Compressed.ktx2)");
		
		return gKTX_TEXTURE2;
	}

	Backend::Window& Core::getWindow() {
		assert(loaded);

		static Backend::Window gWindowWrapper({
			.width = 800,
			.height = 600,
			.NAME = "Renderer6"
		});

		return gWindowWrapper;
	}

	Backend::Instance& Core::getInstance() {
		assert(loaded);

		static Backend::Instance gBackend(&getWindow(), 
			[]() -> Backend::Instance::CreateInfo {
				const std::vector<const char*> ENABLED_LAYERS{ "VK_LAYER_KHRONOS_validation" };
				#ifdef _WIN32
				const std::vector<const char*> ENABLED_EXTENSIONS(Backend::Window::getInstanceRequiredWindowExtensions());
				#endif
				#ifdef __APPLE__
				const std::vector<const char*> ENABLED_EXTENSIONS(
				[]() -> const std::vector<const char*> {
					std::vector<const char*> init(Backend::Window::getInstanceRequiredWindowExtensions());
					init.push_back("VK_KHR_portability_enumeration");
					return init;
				}()
				);
				#endif

				const VkApplicationInfo APP_INFO{
						.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
						.apiVersion = VK_API_VERSION_1_3,
				};
				const VkInstanceCreateInfo INSTANCE_CREATE_INFO{
						.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
						.flags =
						#ifdef _WIN32 
						0,
						#endif
						#ifdef __APPLE__
						VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
						#endif
						.pApplicationInfo = nullptr, // reroute needed
						.enabledLayerCount = static_cast<uint32_t>(ENABLED_LAYERS.size()),
						.ppEnabledLayerNames = ENABLED_LAYERS.data(),
						.enabledExtensionCount = static_cast<uint32_t>(ENABLED_EXTENSIONS.size()),
						.ppEnabledExtensionNames = ENABLED_EXTENSIONS.data(),
				};

				return Backend::Instance::CreateInfo(ENABLED_LAYERS, ENABLED_EXTENSIONS, APP_INFO, INSTANCE_CREATE_INFO);
			}()
		);

		return gBackend;
	}

	Backend::Devices& Core::getDevices() {
		assert(loaded);

		static Backend::Devices gDevices(
			&getInstance(),
			[]() -> Backend::Devices::CreateInfo {
				const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT EXTENDED_DYNAMIC_STATE{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
					.extendedDynamicState2 = true
				};
				const VkPhysicalDeviceDynamicRenderingFeatures DYNAMIC_RENDERING{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
					.pNext = nullptr, // reroute needed
					.dynamicRendering = true
				};
				const VkPhysicalDeviceSynchronization2Features SYNC2{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
					.pNext = nullptr, // reroute needed
					.synchronization2 = true
				};
				const VkPhysicalDeviceFeatures2 FEATURES{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
					.pNext = nullptr, // reroute needed
					.features = {
						.samplerAnisotropy = true,
						.textureCompressionBC = true
					}
				};

				const std::vector<const char*> EXTENSIONS{
					"VK_KHR_swapchain",
					"VK_KHR_synchronization2",
					"VK_KHR_spirv_1_4",
					#ifdef __APPLE__
					"VK_KHR_portability_subset"
					#endif
				};

				const std::vector<std::vector<float>> QUEUE_PRIORITIES{
					{0.5f}
				};
				std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos{
					VkDeviceQueueCreateInfo{
						.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
						.queueFamilyIndex = UINT32_MAX, // this will be set properly later (*)
						.queueCount = 1,
						.pQueuePriorities = QUEUE_PRIORITIES[0].data()
					}
				};

				uint32_t physicalDeviceCount{};
				CHECK_VK_SUCCESS(
					vkEnumeratePhysicalDevices(getInstance().getInstance(), &physicalDeviceCount, nullptr),
					"Failed to enumerate physical devices on your instance"
				);
				std::vector<VkPhysicalDevice> systemPhysicalDevices(physicalDeviceCount);
				CHECK_VK_SUCCESS(
					vkEnumeratePhysicalDevices(getInstance().getInstance(), &physicalDeviceCount, systemPhysicalDevices.data()),
					"Failed to enumerate physical devices on your instance"
				);

				auto getPhysicalDeviceGraphicsQfIndex = [](VkPhysicalDevice& physicalDevice, uint16_t const& MINIMUM_QUEUES) -> uint32_t {
					uint32_t physicalDeviceQueueFamilyCount{};
					vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, nullptr);
					std::vector<VkQueueFamilyProperties> queueFamilyProperties(physicalDeviceQueueFamilyCount);
					vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, queueFamilyProperties.data());

					uint32_t graphicsQfIndex = UINT32_MAX;
					for (int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
						if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
							(queueFamilyProperties[i].queueCount >= MINIMUM_QUEUES)) {
							graphicsQfIndex = i;
						}
					}

					return graphicsQfIndex;
				};

				// iterate over physical devices one by one, weeding out the downs
				for (int i = 0; i < systemPhysicalDevices.size(); i++) {
					VkPhysicalDeviceProperties physicalDeviceProperties{};
					vkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);

					// api version check
					if (!(physicalDeviceProperties.apiVersion >= VK_API_VERSION_1_3)) {
						systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
						continue;
					}

					// has graphics queue family with enough queues check
					if (getPhysicalDeviceGraphicsQfIndex(systemPhysicalDevices[i], queueFamilyInfos[0].queueCount) == UINT32_MAX) {
						systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
						continue;
					}

					// extensions check
					uint32_t physicalDeviceExtensionsCount{};
					vkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, nullptr);
					std::vector<VkExtensionProperties> physicalDeviceExtensions(physicalDeviceExtensionsCount);
					vkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, physicalDeviceExtensions.data());

					std::vector<std::string> physicalDeviceExtensionNames{};
					for (VkExtensionProperties const& physicalDeviceExtension : physicalDeviceExtensions) {
						physicalDeviceExtensionNames.push_back(physicalDeviceExtension.extensionName);
					}
					std::vector<std::string> logicalDeviceExtensionNames{};
					for (int i = 0; i < EXTENSIONS.size(); i++) {
						logicalDeviceExtensionNames.push_back(EXTENSIONS[i]);
					}

					if (!Common::fContainsAll(physicalDeviceExtensionNames, logicalDeviceExtensionNames)) {
						systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
						continue;
					}

					// features check
					VkPhysicalDeviceExtendedDynamicState2FeaturesEXT physicalDeviceExtendedDynamicStateFeaturesStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT
					};
					VkPhysicalDeviceDynamicRenderingFeatures physicalDeviceDynamicRenderingFeaturesStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
						.pNext = &physicalDeviceExtendedDynamicStateFeaturesStatus
					};
					VkPhysicalDeviceSynchronization2Features physicalDeviceSyncFeaturesStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
						.pNext = &physicalDeviceDynamicRenderingFeaturesStatus
					};
					VkPhysicalDeviceFeatures2 physicalDeviceFeaturesStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
						.pNext = &physicalDeviceSyncFeaturesStatus
					};
					vkGetPhysicalDeviceFeatures2(systemPhysicalDevices[i], &physicalDeviceFeaturesStatus);

					if (!(physicalDeviceFeaturesStatus.features.samplerAnisotropy && physicalDeviceFeaturesStatus.features.textureCompressionBC && physicalDeviceSyncFeaturesStatus.synchronization2 && physicalDeviceDynamicRenderingFeaturesStatus.dynamicRendering && physicalDeviceExtendedDynamicStateFeaturesStatus.extendedDynamicState2)) {
						systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
						continue;
					}
				}

				if (systemPhysicalDevices.empty()) {
					throw std::runtime_error("There is no suitable GPU on your computer for this application");
				}

				// perform nice-to-have checks on the remaining physical devices
				// index 0 : are you a discrete GPU?
				std::vector<std::array<uint16_t, 1>> niceToHavesBySystemPhysicalDevice(systemPhysicalDevices.size(), { 0 });
				for (int i = 0; i < systemPhysicalDevices.size(); i++) {
					VkPhysicalDeviceProperties physicalDeviceProperties{};
					vkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);

					if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
						niceToHavesBySystemPhysicalDevice[i][0] = 1;
					}
				}

				// sum nice-to-have counts up
				std::vector<uint16_t> niceToHaveSumsBySystemPhysicalDevice(systemPhysicalDevices.size(), 0);
				for (int i = 0; i < systemPhysicalDevices.size(); i++) {
					for (uint16_t const& niceToHaveStatus : niceToHavesBySystemPhysicalDevice[i]) {
						niceToHaveSumsBySystemPhysicalDevice[i] += niceToHaveStatus;
					}
				}

				const size_t SELECTED_PHYSICAL_DEVICE_INDEX = std::distance(niceToHaveSumsBySystemPhysicalDevice.begin(), std::max_element(niceToHaveSumsBySystemPhysicalDevice.begin(), niceToHaveSumsBySystemPhysicalDevice.end()));
				VkPhysicalDevice finalSelection = systemPhysicalDevices[SELECTED_PHYSICAL_DEVICE_INDEX];
				VkPhysicalDeviceProperties selectedPhysicalDeviceProperties{};
				vkGetPhysicalDeviceProperties(finalSelection, &selectedPhysicalDeviceProperties);
				std::cout << "SELECTED PHYSICAL DEVICE: " << selectedPhysicalDeviceProperties.deviceName << "\n";
				std::cout << "DISCRETE GPU? " << ((niceToHavesBySystemPhysicalDevice[SELECTED_PHYSICAL_DEVICE_INDEX][0]) ? "Yes\n" : "No\n");

				const VkDeviceCreateInfo DEVICE_INFO{
					.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
					.pNext = nullptr, // reroute needed
					.flags = 0,
					.queueCreateInfoCount = static_cast<uint32_t>(queueFamilyInfos.size()),
					.pQueueCreateInfos = queueFamilyInfos.data(),
					.enabledExtensionCount = static_cast<uint32_t>(EXTENSIONS.size()),
					.ppEnabledExtensionNames = EXTENSIONS.data(),
				};

				return Backend::Devices::CreateInfo(std::move(finalSelection), DEVICE_INFO, )
			}()
		);

		return gDevices;
	}

	Backend::Swapchain& Core::getSwapchain() {
		assert(loaded);

		static Backend::Swapchain gSwapchainWrapper(
			&getDevices(),
			Backend::Swapchain::sGetConstructParameters(
				getInstance().instance,
				getDevices().physicalDevice,
				getInstance().WINDOW->glfwWindow,
				getDevices().GRAPHICS_QF_INDEX
			)
		);

		return gSwapchainWrapper;
	}

	DeviceMemory::HostVisible& Core::getHostVisibleMemory() {
		assert(loaded);

		static auto gPopulate = [](DeviceMemory::HostVisible& self) -> void {
			const std::vector<Vertex::Vertex> VERTICIES{
				Vertex::Vertex(
					glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), 
					glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 
					glm::vec2(0.0f, 0.0f)
				), // top left
				Vertex::Vertex(
					glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), 
					glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 
					glm::vec2(1.0f, 0.0f)
				), // top right
				Vertex::Vertex(
					glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), 
					glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 
					glm::vec2(0.0f, 1.0f)
				), // bottom left
				Vertex::Vertex(
					glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), 
					glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 
					glm::vec2(1.0f, 1.0f)
				), // bottom right

				Vertex::Vertex(
					glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), 
					glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 
					glm::vec2(0.0f, 0.0f)
				), // top left
				Vertex::Vertex(
					glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), 
					glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 
					glm::vec2(1.0f, 0.0f)
				), // top right
				Vertex::Vertex(
					glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f), 
					glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 
					glm::vec2(0.0f, 1.0f)
				), // bottom left
				Vertex::Vertex(
					glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), 
					glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 
					glm::vec2(1.0f, 1.0f)
				), // bottom right
			};
			const std::vector<uint32_t> INDICES{
				0 + 4, 1 + 4, 3 + 4,
				0 + 4, 3 + 4, 2 + 4,

				0, 1, 3,
				0, 3, 2,
			};

			self.writeToBuffer(0, VERTICIES.data(), sizeof(Vertex::Vertex) * 8);
			self.writeToBuffer(1, INDICES.data(), sizeof(uint32_t) * 12);
			self.writeToBuffer(3, getKtxTexture2()->pData, getKtxTexture2()->dataSize);
			self.updateDescriptorSetBuffer(0, 0, {2});
		};

		static DeviceMemory::HostVisible gHostVisibleMemory(
			&getDevices(),
			DeviceMemory::HostVisible::CreateInfo(
				{ 
					DeviceMemory::Common::BufferInfo(sizeof(Vertex::Vertex) * 8, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().GRAPHICS_QF_INDEX),
					DeviceMemory::Common::BufferInfo(sizeof(uint32_t) * 12, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().GRAPHICS_QF_INDEX),
					DeviceMemory::Common::BufferInfo(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, getDevices().GRAPHICS_QF_INDEX),
					DeviceMemory::Common::BufferInfo(getKtxTexture2()->dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().GRAPHICS_QF_INDEX)
				},
				{
					DeviceMemory::Common::DescriptorSetInfo({Vertex::Transforms::sGetTransformationMatricesDescriptorSetLayoutBinding(0)})
				}
			),
			gPopulate
		);

		return gHostVisibleMemory;
	}

	DeviceMemory::DeviceLocal& Core::getDeviceLocalMemory() {
		assert(loaded);

		static auto gPopulate = [](DeviceMemory::DeviceLocal& self) -> void {
			self.copyBufferToBuffer(0, getHostVisibleMemory().buffers[0], {VkBufferCopy(0, 0, sizeof(Vertex::Vertex) * 8)});
			self.copyBufferToBuffer(1, getHostVisibleMemory().buffers[1], {VkBufferCopy(0, 0, sizeof(uint32_t) * 12)});
			self.copyBufferToImage(0, getHostVisibleMemory().buffers[3], {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1))}); // buffer is tightly packed row by row
			self.updateDescriptorSetCombinedImageSampler(0, 0, {0});
		};

		static DeviceMemory::DeviceLocal gDeviceLocalMemory(
			&getDevices(),
			[]() -> DeviceMemory::DeviceLocal::CreateInfo {
				const VkPhysicalDeviceProperties PHYSICAL_DEVICE_PROPERTIES(
					[]() -> const VkPhysicalDeviceProperties {
						VkPhysicalDeviceProperties arguement{};
						vkGetPhysicalDeviceProperties(getDevices().physicalDevice, &arguement);
						return arguement;
					}()
				);

				const DeviceMemory::DeviceLocal::CreateInfo ARGUEMENT(
					{ 
						DeviceMemory::Common::BufferInfo(sizeof(Vertex::Vertex) * 8, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getDevices().GRAPHICS_QF_INDEX),
						DeviceMemory::Common::BufferInfo(sizeof(uint32_t) * 12, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getDevices().GRAPHICS_QF_INDEX)
					},
					{
						DeviceMemory::Common::ImageInfo(
							VK_IMAGE_TYPE_2D,
							static_cast<VkFormat>(getKtxTexture2()->vkFormat),
							VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1),
							1,
							VK_SAMPLE_COUNT_1_BIT,
							VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
							getDevices().GRAPHICS_QF_INDEX,
							VK_IMAGE_LAYOUT_UNDEFINED,
							DeviceMemory::Common::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, static_cast<VkFormat>(getKtxTexture2()->vkFormat), VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1))
						),
						DeviceMemory::Common::ImageInfo(
							VK_IMAGE_TYPE_2D,
							VK_FORMAT_D32_SFLOAT,
							VkExtent3D(getSwapchain().CREATE_INFO.mSwapchainKHRCreateInfo.imageExtent.width, getSwapchain().CREATE_INFO.mSwapchainKHRCreateInfo.imageExtent.height, 1),
							1,
							VK_SAMPLE_COUNT_1_BIT,
							VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
							getDevices().GRAPHICS_QF_INDEX,
							VK_IMAGE_LAYOUT_UNDEFINED,
							DeviceMemory::Common::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D32_SFLOAT, VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1))
						),
					},
					{
						DeviceMemory::Common::SamplerInfo(
							VK_FILTER_LINEAR,
							VK_FILTER_LINEAR,
							VK_SAMPLER_MIPMAP_MODE_LINEAR,
							VK_SAMPLER_ADDRESS_MODE_REPEAT,
							VK_SAMPLER_ADDRESS_MODE_REPEAT,
							0.0f,
							VK_TRUE,
							PHYSICAL_DEVICE_PROPERTIES.limits.maxSamplerAnisotropy,
							0.0f,
							0.0f,
							VK_BORDER_COLOR_INT_OPAQUE_WHITE
						)
					},
					{
						DeviceMemory::Common::DescriptorSetInfo(
							{
								VkDescriptorSetLayoutBinding{
									.binding = 0,
									.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
									.descriptorCount = 1,
									.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
								}
							}
						)
					}
				);
			}(),
			gPopulate
		);

		return gDeviceLocalMemory;
	}

	Engine::GraphicsPipeline& Core::getGraphicsPipeline() {
		assert(loaded);

		static const std::vector<VkDescriptorSetLayout> gDESCRIPTOR_SET_LAYOUTS{
			[&]() -> std::vector<VkDescriptorSetLayout> {
				std::vector<VkDescriptorSetLayout> initialValue{ getHostVisibleMemory().descriptorSetLayouts };
				initialValue.insert(initialValue.end(), getDeviceLocalMemory().descriptorSetLayouts.begin(), getDeviceLocalMemory().descriptorSetLayouts.end());
				return initialValue;
			}()
		};

		static Engine::GraphicsPipeline gGraphicsPipeline(
			&getDevices(),
			Engine::GraphicsPipeline::sGetConstructParameters(gDESCRIPTOR_SET_LAYOUTS)
		);

		return gGraphicsPipeline;
	}
}