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
	}

	ktxTexture2 const* Core::getKtxTexture2() {
		static ktxTexture2 const*const gKTX_TEXTURE2 = DeviceMemory::Common::loadKtxImage(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)");
		
		return gKTX_TEXTURE2;
	}

	std::pair<std::vector<Vertex::Vertex>, std::vector<uint32_t>>& Core::getGltfModel() {
		static std::pair<std::vector<Vertex::Vertex>, std::vector<uint32_t>> uniqueVerticesAndVertexIndices{};

		if(uniqueVerticesAndVertexIndices.first.empty() && uniqueVerticesAndVertexIndices.second.empty()) {
			DeviceMemory::Common::loadGltfModel(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", uniqueVerticesAndVertexIndices.first, uniqueVerticesAndVertexIndices.second);
		}

		return uniqueVerticesAndVertexIndices;
	}

	Backend::Window& Core::getWindow() {
		static Backend::Window gWindowWrapper({
			.width = 800,
			.height = 600,
			.NAME = "Renderer6"
		});

		return gWindowWrapper;
	}

	Backend::Instance& Core::getInstance() {
		static Backend::Instance gBackend(&getWindow(), 
			[]() -> Backend::Instance::CreateInfo {
				std::vector<const char*> enabledLayers{ "VK_LAYER_KHRONOS_validation" };
				#ifdef _WIN32
				std::vector<const char*> enabledExtensions(Backend::Window::getInstanceRequiredWindowExtensions());
				#endif
				#ifdef __APPLE__
				std::vector<const char*> enabledExtensions(
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
					.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
					.ppEnabledLayerNames = enabledLayers.data(),
					.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
					.ppEnabledExtensionNames = enabledExtensions.data(),
				};

				return Backend::Instance::CreateInfo(std::move(enabledLayers), std::move(enabledExtensions), APP_INFO, INSTANCE_CREATE_INFO);
			}()
		);

		return gBackend;
	}

	Backend::Devices& Core::getDevices() {
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

				std::vector<const char*> extensions{
					"VK_KHR_swapchain",
					"VK_KHR_synchronization2",
					"VK_KHR_spirv_1_4",
					#ifdef __APPLE__
					"VK_KHR_portability_subset"
					#endif
				};

				std::vector<std::vector<float>> queuePriorities{
					{0.5f}
				};
				std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos{
					VkDeviceQueueCreateInfo{
						.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
						.queueFamilyIndex = UINT32_MAX, // this will be set properly later (*)
						.queueCount = 1,
						.pQueuePriorities = queuePriorities[0].data()
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
					if ((queueFamilyInfos[0].queueFamilyIndex = getPhysicalDeviceGraphicsQfIndex(systemPhysicalDevices[i], queueFamilyInfos[0].queueCount) /* (*) */) == UINT32_MAX) {
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
					for (int i = 0; i < extensions.size(); i++) {
						logicalDeviceExtensionNames.push_back(extensions[i]);
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
					.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
					.ppEnabledExtensionNames = extensions.data(),
				};

				return Backend::Devices::CreateInfo(std::move(finalSelection), DEVICE_INFO, std::move(queueFamilyInfos), std::move(queuePriorities), std::move(extensions), EXTENDED_DYNAMIC_STATE, DYNAMIC_RENDERING, SYNC2, FEATURES);
			}()
		);

		return gDevices;
	}

	Backend::Swapchain& Core::getSwapchain() {
		static Backend::Swapchain gSwapchainWrapper(
			&getDevices(),
			[]() -> Backend::Swapchain::CreateInfo {
				VkSurfaceKHR returnSurface{};
				CHECK_VK_SUCCESS(
					glfwCreateWindowSurface(getInstance().getInstance(), getWindow().getGlfwWindow(), nullptr, &returnSurface),
					"Failed to create surface"
				)

				VkSurfaceCapabilitiesKHR surfaceCapabilities{};
				CHECK_VK_SUCCESS(
					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getDevices().getPhysicalDevice(), returnSurface, &surfaceCapabilities),
					"Failed to get physical device surface capabilities"
				)

				VkExtent2D surfaceExtentInPixels{};
				if (surfaceCapabilities.currentExtent.width == UINT32_MAX && surfaceCapabilities.currentExtent.height == UINT32_MAX) {
					glfwGetFramebufferSize(getWindow().getGlfwWindow(), reinterpret_cast<int*>(&surfaceExtentInPixels.width), reinterpret_cast<int*>(&surfaceExtentInPixels.height));
				} else {
					surfaceExtentInPixels = VkExtent2D(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
				}

				std::vector<uint32_t> accessorGfxQf{ getDevices().getGraphicsQfIndex() };
    
				const VkSwapchainCreateInfoKHR SWAPCHAIN_INFO{
					.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
					.pNext = nullptr,
					.flags = 0,
					.surface = nullptr, // reroute needed
					.minImageCount = 4,
					.imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
					.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
					.imageExtent = surfaceExtentInPixels,
					.imageArrayLayers = 1,
					.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = static_cast<uint32_t>(accessorGfxQf.size()),
					.pQueueFamilyIndices = accessorGfxQf.data(),
					.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
					.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
					.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
					.clipped = VK_TRUE,
					.oldSwapchain = VK_NULL_HANDLE,
				};

				return Backend::Swapchain::CreateInfo(std::move(returnSurface), SWAPCHAIN_INFO, std::move(accessorGfxQf));
			}()
		);

		return gSwapchainWrapper;
	}

	DeviceMemory::HostVisible& Core::getHostVisibleMemory() {
		const static uint32_t VERTEX_BUFFER_SIZE = getGltfModel().first.size() * sizeof(Vertex::Vertex);
		const static uint32_t INDEX_BUFFER_SIZE = getGltfModel().second.size() * sizeof(uint32_t);

		static auto gPopulate = [](DeviceMemory::HostVisible& hostVisibleMemory) -> void {
			hostVisibleMemory.writeToBuffer(0, getGltfModel().first.data(), VERTEX_BUFFER_SIZE);
			hostVisibleMemory.writeToBuffer(1, getGltfModel().second.data(), INDEX_BUFFER_SIZE);
			hostVisibleMemory.writeToBuffer(3, getKtxTexture2()->pData, getKtxTexture2()->dataSize);
			hostVisibleMemory.updateDescriptorSetBuffer(0, 0, {2});
		};

		static DeviceMemory::HostVisible gHostVisibleMemory(
			&getDevices(),
			DeviceMemory::HostVisible::CreateInfo(
				{
					DeviceMemory::Common::BufferInfo(VERTEX_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::Common::BufferInfo(INDEX_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::Common::BufferInfo(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::Common::BufferInfo(getKtxTexture2()->dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex())
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
		const static uint32_t VERTEX_BUFFER_SIZE = getHostVisibleMemory().getCreateInfo().bufferInfos[0].size;
		const static uint32_t INDEX_BUFFER_SIZE = getHostVisibleMemory().getCreateInfo().bufferInfos[1].size;

		static auto gPopulate = [](DeviceMemory::DeviceLocal& self) -> void {
			self.copyBufferToBuffer(0, getHostVisibleMemory().getBuffers()[0], {VkBufferCopy(0, 0, VERTEX_BUFFER_SIZE)});
			self.copyBufferToBuffer(1, getHostVisibleMemory().getBuffers()[1], {VkBufferCopy(0, 0, INDEX_BUFFER_SIZE)});
			// assumes buffer is tightly packed row by row
			self.copyBufferToImage(0, getHostVisibleMemory().getBuffers()[3], {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1))});
			self.updateDescriptorSetCombinedImageSampler(0, 0, {0});
		};

		static DeviceMemory::DeviceLocal gDeviceLocalMemory(
			&getDevices(),
			[]() -> DeviceMemory::DeviceLocal::CreateInfo {
				const VkPhysicalDeviceProperties PHYSICAL_DEVICE_PROPERTIES(
					[]() -> const VkPhysicalDeviceProperties {
						VkPhysicalDeviceProperties arguement{};
						vkGetPhysicalDeviceProperties(getDevices().getPhysicalDevice(), &arguement);
						return arguement;
					}()
				);

				return DeviceMemory::DeviceLocal::CreateInfo(
					{ 
						DeviceMemory::Common::BufferInfo(VERTEX_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getDevices().getGraphicsQfIndex()),
						DeviceMemory::Common::BufferInfo(INDEX_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getDevices().getGraphicsQfIndex())
					},
					{
						// texture image
						DeviceMemory::Common::ImageInfo(
							VK_IMAGE_TYPE_2D,
							static_cast<VkFormat>(getKtxTexture2()->vkFormat),
							VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1),
							1,
							VK_SAMPLE_COUNT_1_BIT,
							VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
							getDevices().getGraphicsQfIndex(),
							VK_IMAGE_LAYOUT_UNDEFINED,
							DeviceMemory::Common::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, static_cast<VkFormat>(getKtxTexture2()->vkFormat), VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1))
						),
						// depth image
						DeviceMemory::Common::ImageInfo(
							VK_IMAGE_TYPE_2D,
							VK_FORMAT_D32_SFLOAT,
							VkExtent3D(getSwapchain().getCreateInfo().createInfo.imageExtent.width, getSwapchain().getCreateInfo().createInfo.imageExtent.height, 1),
							1,
							VK_SAMPLE_COUNT_1_BIT,
							VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
							getDevices().getGraphicsQfIndex(),
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
		static Engine::GraphicsPipeline gGraphicsPipeline(
			&getDevices(),
			[]() -> Engine::GraphicsPipeline::CreateInfo {
				VkGraphicsPipelineCreateInfo pipelineCreateInfo{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					// reroute needed for everything
				};

				std::vector<VkFormat> colorAttachmentFormats{ VK_FORMAT_R8G8B8A8_SRGB };
				VkPipelineRenderingCreateInfo rendering{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
					.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size()),
					.pColorAttachmentFormats = colorAttachmentFormats.data(),
					.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
					.stencilAttachmentFormat = {},
				};

				std::vector<char> sprivFileBytes(Common::fLoadSprivFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\shaders.spv)"));
				VkShaderModuleCreateInfo shaderModuleForEverythingInfo{
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.codeSize = static_cast<uint32_t>(sprivFileBytes.size()),
					.pCode = reinterpret_cast<uint32_t const*>(sprivFileBytes.data())
				};
				VkShaderModule shaderModuleForEverything{};
				CHECK_VK_SUCCESS(
					vkCreateShaderModule(getDevices().getLogicalDevice(), &shaderModuleForEverythingInfo, nullptr, &shaderModuleForEverything),
					"Failed to create shader module"
				)
				std::vector<VkPipelineShaderStageCreateInfo> stages{
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = shaderModuleForEverything,
						.pName = "vertexShader"
					},
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = shaderModuleForEverything,
						.pName = "fragmentShader"
					}
				};
				pipelineCreateInfo.stageCount = static_cast<uint32_t>(stages.size());
				pipelineCreateInfo.pStages = stages.data();

				std::vector<VkVertexInputBindingDescription> vertexBindings{ Vertex::Vertex::sGetInputBindingDescription() };
				std::vector<VkVertexInputAttributeDescription> vertexAttributes{ Vertex::Vertex::sGetInputAttributeDescriptions() };
				VkPipelineVertexInputStateCreateInfo vertexInput{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size()),
					.pVertexBindingDescriptions = vertexBindings.data(),
					.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size()),
					.pVertexAttributeDescriptions = vertexAttributes.data(),
				};

				VkPipelineInputAssemblyStateCreateInfo inputAssembly{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
					.primitiveRestartEnable = VK_FALSE
				};

				// UNUSED CURRENTLY
				VkPipelineTessellationStateCreateInfo tessellation{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
				};

				VkPipelineViewportStateCreateInfo viewport{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
					.viewportCount = 1,
					.pViewports = nullptr, // will dynamically set
					.scissorCount = 1,
					.pScissors = nullptr // will dynamically set
				};

				VkPipelineRasterizationStateCreateInfo rasterization{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.depthClampEnable = VK_FALSE,
					.rasterizerDiscardEnable = VK_FALSE,
					.polygonMode = VK_POLYGON_MODE_FILL,
					.cullMode = VK_CULL_MODE_NONE,
					.frontFace = VK_FRONT_FACE_CLOCKWISE,
					.depthBiasEnable = VK_FALSE,
					.depthBiasConstantFactor = 0.0f,
					.depthBiasClamp = 0.0f,
					.depthBiasSlopeFactor = 1.0f,
					.lineWidth = 1.0f
				};

				// UNUSED CURRENTLY
				VkPipelineMultisampleStateCreateInfo multisampling{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
					.sampleShadingEnable = VK_FALSE,
					.alphaToCoverageEnable = VK_FALSE,
					.alphaToOneEnable = VK_FALSE,
				};

				VkPipelineDepthStencilStateCreateInfo depthStencil{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.depthTestEnable = VK_TRUE,
					.depthWriteEnable = VK_TRUE,
					.depthCompareOp = VK_COMPARE_OP_LESS,
					.depthBoundsTestEnable = VK_FALSE,
					.stencilTestEnable = VK_FALSE,
				};

				// UNUSED CURRENTLY
				std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{
					VkPipelineColorBlendAttachmentState{
						.blendEnable = VK_FALSE,
						.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT 
					}
				};
				VkPipelineColorBlendStateCreateInfo colorBlend{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.logicOpEnable = VK_FALSE,	
					.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),
					.pAttachments = colorBlendAttachments.data()
				};

				std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
				VkPipelineDynamicStateCreateInfo dynamicStateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
					.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
					.pDynamicStates = dynamicStates.data(),
				};

				std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ getHostVisibleMemory().getDescriptorSetLayouts() };
				descriptorSetLayouts.insert(descriptorSetLayouts.end(), getDeviceLocalMemory().getDescriptorSetLayouts().begin(), getDeviceLocalMemory().getDescriptorSetLayouts().end());
				VkPipelineLayoutCreateInfo layoutInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
					.pSetLayouts = descriptorSetLayouts.data()
				};
				CHECK_VK_SUCCESS(
					vkCreatePipelineLayout(getDevices().getLogicalDevice(), &layoutInfo, nullptr, &pipelineCreateInfo.layout),
					"Failed to create pipeline layout"
				)

				return Engine::GraphicsPipeline::CreateInfo(
					pipelineCreateInfo,
					rendering, std::move(colorAttachmentFormats),
					std::move(stages),
					vertexInput, std::move(vertexBindings), std::move(vertexAttributes),
					inputAssembly, 
					tessellation,
					viewport,
					rasterization,
					multisampling,
					depthStencil,
					colorBlend, std::move(colorBlendAttachments),
					dynamicStateInfo, std::move(dynamicStates)
				);
			}()
		);

		return gGraphicsPipeline;
	}
}