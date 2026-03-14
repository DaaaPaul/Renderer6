#include <cassert>
#include <ctime>
#include <cmath>
#include <random>
#include <array>
#include "Common.h"
#include "Global.h"

namespace Global {
	namespace Engine {
		void load() {
			assert(Global::gLoaded);

			(void) getKillhouse();
			(void) getCurrentTransformation();

			gLoaded = true;
		}

		[[nodiscard]] ::Engine::Killhouse& getKillhouse() {
			assert(Engine::gHitmenInFlight != 0xFFFFFFFF);

			static ::Engine::Killhouse gKillhouse(gHitmenInFlight, Global::getDevices().getGraphicsQfIndex());
			return gKillhouse;
		}

		[[nodiscard]] Vertex::Transforms& getCurrentTransformation() {
			static Vertex::Transforms gCurrentTransformation(
				[]() -> Vertex::Transforms {
					Vertex::Transforms currentTransformation(
						glm::mat4{1.0f},
						glm::mat4{glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))},
						glm::mat4{glm::perspective(glm::radians(45.0f), static_cast<float>(Global::getSwapchain().getCurrentExtent().width) / static_cast<float>(Global::getSwapchain().getCurrentExtent().height), 0.1f, 1000.0f)}
					);
					currentTransformation.projection[1][1] *= -1.0f;

					return currentTransformation;
				}()
			);

			return gCurrentTransformation;
		}
	}

	void load() {
		(void) getKtxTexture2();
		(void) getGltfModel();
		(void) getParticlesData();
		(void) getWindow();
		(void) getInstance();
		(void) getDevices();
		(void) getSwapchain();
		(void) getSwapchainImages();
		(void) getHostVisibleMemory();
		(void) getDeviceLocalMemory();
		(void) getGraphicsPipeline();

		gLoaded = true;
	}

	ktxTexture2 const* getKtxTexture2() {
		static ktxTexture2 const*const gpKTX_TEXTURE2 = DeviceMemory::loadKtxImage(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)");
		
		return gpKTX_TEXTURE2;
	}

	std::pair<std::vector<Vertex::Vertex>, std::vector<uint32_t>>& getGltfModel() {
		static std::pair<std::vector<Vertex::Vertex>, std::vector<uint32_t>> uniqueVerticesAndVertexIndices{};

		if(uniqueVerticesAndVertexIndices.first.empty() && uniqueVerticesAndVertexIndices.second.empty()) {
			DeviceMemory::loadGltfModel(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", uniqueVerticesAndVertexIndices.first, uniqueVerticesAndVertexIndices.second);
		
			gVertexBufferSize = uniqueVerticesAndVertexIndices.first.size() * sizeof(Vertex::Vertex); /* (*) */
			gIndexBufferSize = uniqueVerticesAndVertexIndices.second.size() * sizeof(uint32_t); /* (*) */
		}

		return uniqueVerticesAndVertexIndices;
	}

	Backend::Window& getWindow() {
		static Backend::Window gWindowWrapper({
			.width = 800,
			.height = 600,
			.NAME = "Renderer6"
		});

		return gWindowWrapper;
	}

	[[nodiscard]] std::vector<Particle::Particle> getParticlesData() {
		static auto gRandom = []() -> float {
			static std::default_random_engine gEngine(static_cast<unsigned>(time(nullptr)));
			static std::uniform_real_distribution gNormal(0.0f, 1.0f);

			return gNormal(gEngine);
		};

		static std::vector<Particle::Particle> particles(
			[]() -> std::vector<Particle::Particle> {
				std::vector<Particle::Particle> particles(gPARTICLES_COUNT, {});

				float r{}, theta{}, x{}, y{};
				for(Particle::Particle& p : particles) {
					r = sqrtf(gRandom());
					theta = 2.0f * 3.14159265358979323846f * gRandom();
					x = cosf(theta) * getWindow().getCreateInfo().height / getWindow().getCreateInfo().width;
					y = sinf(theta);

					p.position = glm::vec4(x, y, 1.0f, 1.0f);
					p.velocity = normalize(p.position) * 0.00025f;
					p.color = glm::vec4(x, y, r, 1.0f);
				}

				gParticleBufferSize = particles.size() * sizeof(Particle::Particle);

				return particles;
			}()
		);


		return particles;
	}

	Backend::Instance& getInstance() {
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

	Backend::Devices& getDevices() {
		static Backend::Devices gDevices(
			&getInstance(),
			[]() -> Backend::Devices::CreateInfo {
				VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extendedDynamicState{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
					.extendedDynamicState2 = true
				};
				VkPhysicalDeviceDynamicRenderingFeatures dynamicRendering{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
					.pNext = nullptr, // reroute needed
					.dynamicRendering = true
				};
				VkPhysicalDeviceSynchronization2Features sync2{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
					.pNext = nullptr, // reroute needed
					.synchronization2 = true
				};
				VkPhysicalDeviceTimelineSemaphoreFeatures timeline{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
					.pNext = nullptr, // reroute needed
					.timelineSemaphore = true
				};
				VkPhysicalDeviceFeatures2 features{
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

				auto getQfIndex = [](VkPhysicalDevice& pPhysicalDevice, uint16_t const& MINIMUM_QUEUES) -> uint32_t {
					uint32_t physicalDeviceQueueFamilyCount{};
					vkGetPhysicalDeviceQueueFamilyProperties(pPhysicalDevice, &physicalDeviceQueueFamilyCount, nullptr);
					std::vector<VkQueueFamilyProperties> queueFamilyProperties(physicalDeviceQueueFamilyCount);
					vkGetPhysicalDeviceQueueFamilyProperties(pPhysicalDevice, &physicalDeviceQueueFamilyCount, queueFamilyProperties.data());

					uint32_t graphicsQfIndex = UINT32_MAX;
					for (int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
						if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
							(queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
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

					// has graphics and compute queue family with enough queues check
					if ((queueFamilyInfos[0].queueFamilyIndex = getQfIndex(systemPhysicalDevices[i], queueFamilyInfos[0].queueCount) /* (*) */) == UINT32_MAX) {
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

					if (!Common::containsAll(physicalDeviceExtensionNames, logicalDeviceExtensionNames)) {
						systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
						continue;
					}

					// features check
					VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extendedDynamicStateStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT
					};
					VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
						.pNext = &extendedDynamicStateStatus
					};
					VkPhysicalDeviceSynchronization2Features sync2Status{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
						.pNext = &dynamicRenderingStatus
					};
					VkPhysicalDeviceTimelineSemaphoreFeatures timelineStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
						.pNext = &sync2Status,
					};
					VkPhysicalDeviceFeatures2 featuresStatus{
						.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
						.pNext = &timelineStatus
					};
					vkGetPhysicalDeviceFeatures2(systemPhysicalDevices[i], &featuresStatus);

					bool allNeeded = featuresStatus.features.samplerAnisotropy && 
						featuresStatus.features.textureCompressionBC && 
						timelineStatus.timelineSemaphore && 
						sync2Status.synchronization2 && 
						dynamicRenderingStatus.dynamicRendering && 
						extendedDynamicStateStatus.extendedDynamicState2;
					
					if (!allNeeded) {
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
				VkPhysicalDevice pFinalSelection = systemPhysicalDevices[SELECTED_PHYSICAL_DEVICE_INDEX];
				VkPhysicalDeviceProperties selectedPhysicalDeviceProperties{};
				vkGetPhysicalDeviceProperties(pFinalSelection, &selectedPhysicalDeviceProperties);
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

				return Backend::Devices::CreateInfo(std::move(pFinalSelection), DEVICE_INFO, std::move(queueFamilyInfos), std::move(queuePriorities), std::move(extensions), extendedDynamicState, dynamicRendering, sync2, timeline, features);
			}()
		);

		return gDevices;
	}

	Backend::Swapchain& getSwapchain() {
		static Backend::Swapchain gSwapchainWrapper(
			&getDevices(),
			[]() -> Backend::Swapchain::CreateInfo {
				VkSurfaceKHR pReturnSurface{};
				CHECK_VK_SUCCESS(
					glfwCreateWindowSurface(getInstance().getInstance(), getWindow().getGlfwWindow(), nullptr, &pReturnSurface),
					"Failed to create surface"
				)

				VkSurfaceCapabilitiesKHR surfaceCapabilities{};
				CHECK_VK_SUCCESS(
					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getDevices().getPhysicalDevice(), pReturnSurface, &surfaceCapabilities),
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
				Engine::gHitmenInFlight = SWAPCHAIN_INFO.minImageCount; /* (*) */

				return Backend::Swapchain::CreateInfo(std::move(pReturnSurface), SWAPCHAIN_INFO, std::move(accessorGfxQf));
			}()
		);

		return gSwapchainWrapper;
	}

	[[nodiscard]] std::vector<VkImage>& getSwapchainImages() {
		static std::vector<VkImage> images(
			[]() -> std::vector<VkImage> {
				uint32_t imageCount = 0xFFFFFFFF;
				vkGetSwapchainImagesKHR(Global::getDevices().getLogicalDevice(), Global::getSwapchain().getSwapchain(), &imageCount, nullptr);
				std::vector<VkImage> images(imageCount);
				vkGetSwapchainImagesKHR(Global::getDevices().getLogicalDevice(), Global::getSwapchain().getSwapchain(), &imageCount, images.data());

				return images;
			}()
		);

		return images;
	}

	DeviceMemory::HostVisible& getHostVisibleMemory() {
		/*
			Buffer 0 - Model vertices
			Buffer 1 - Model vertex indices
			Buffer 2 - Model texture data
			Buffer 3 to 6 - Model transform
			Buffer 7 to 10 - Particle SSBO
			Buffer 11 to 14 - Particle delta time

			Descriptor Set 0 to 3 - matches uniform buffers 3 to 6
			Descriptor Set 4 to 7 - matches uniform buffers 11 to 14
		*/
		static auto gPopulate = [](DeviceMemory::HostVisible& self) -> void {
			self.writeToBuffer(0, getGltfModel().first.data(), gVertexBufferSize);
			self.writeToBuffer(1, getGltfModel().second.data(), gIndexBufferSize);
			self.writeToBuffer(2, getKtxTexture2()->pData, getKtxTexture2()->dataSize);
			for(int i = 0; i < Engine::gHitmenInFlight; i++) {
				self.writeToBuffer(7 + i, getParticlesData().data(), gParticleBufferSize);
			}

			for(int i = 0; i < Engine::gHitmenInFlight; i++) {
				self.updateDescriptorSetBuffer(i, 0, {static_cast<unsigned long long>(3 + i)});
			}
			for(int i = 0; i < Engine::gHitmenInFlight; i++) {
				self.updateDescriptorSetBuffer(4 + i, 0, {static_cast<unsigned long long>(11 + i)});
			}
		};

		static std::vector<DeviceMemory::BufferInfo> gBufferInfos(
			[]() -> std::vector<DeviceMemory::BufferInfo> {
				std::vector<DeviceMemory::BufferInfo> lambdaReturn{
					DeviceMemory::BufferInfo(gVertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::BufferInfo(gIndexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::BufferInfo(getKtxTexture2()->dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex())
				};

				for(int i = 0; i < Engine::gHitmenInFlight; i++) {
					lambdaReturn.emplace_back(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, getDevices().getGraphicsQfIndex());
				}
				for(int i = 0; i < Engine::gHitmenInFlight; i++) {
					lambdaReturn.emplace_back(gParticleBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex());
				}
				for(int i = 0; i < Engine::gHitmenInFlight; i++) {
					lambdaReturn.emplace_back(sizeof(Particle::DeltaTime), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, getDevices().getGraphicsQfIndex());
				}

				return lambdaReturn;
			}()
		);

		static std::vector<DeviceMemory::DescriptorSetInfo> gDescriptorSetInfos(
			[]() -> std::vector<DeviceMemory::DescriptorSetInfo> {
				std::vector<DeviceMemory::DescriptorSetInfo> lambdaReturn{};

				for(int i = 0; i < Engine::gHitmenInFlight; i++) {
					lambdaReturn.emplace_back(std::vector<VkDescriptorSetLayoutBinding>{Vertex::Transforms::getDescriptorSetBinding(0)});
				}
				for(int i = 0; i < Engine::gHitmenInFlight; i++) {
					lambdaReturn.emplace_back(std::vector<VkDescriptorSetLayoutBinding>{Particle::DeltaTime::getDescriptorSetBinding(0)});
				}

				return lambdaReturn;
			}()
		);

		static DeviceMemory::HostVisible gHostVisibleMemory(
			&getDevices(),
			DeviceMemory::HostVisible::CreateInfo(std::move(gBufferInfos), std::move(gDescriptorSetInfos)),
			gPopulate
		);

		return gHostVisibleMemory;
	}

	DeviceMemory::DeviceLocal& getDeviceLocalMemory() {
		/*
			Buffer 0 - Model vertices
			Buffer 1 - Model vertex indices
			Buffer 2 to 5 - Particle SSBO

			Image 0 - Model texture
			Image 1 - Depth image

			Sampler 0 - Model texture sampler

			Descriptor Set 0 - Matches sampler 0
			Descriptor Set 1 to 4 - Matches buffers 2 to 5
		*/
		static auto gPopulate = [](DeviceMemory::DeviceLocal& self) -> void {
			self.copyBufferToBuffer(0, getHostVisibleMemory().getBuffers()[0], {VkBufferCopy(0, 0, gVertexBufferSize)});
			self.copyBufferToBuffer(1, getHostVisibleMemory().getBuffers()[1], {VkBufferCopy(0, 0, gIndexBufferSize)});
			for(int i = 0; i < Engine::gHitmenInFlight; i++) {
				self.copyBufferToBuffer(2 + i, getHostVisibleMemory().getBuffers()[7 + i], {VkBufferCopy(0, 0, gVertexBufferSize)});
			}

			self.copyBufferToImage(0, getHostVisibleMemory().getBuffers()[2], {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1))});
			
			self.updateDescriptorSetCombinedImageSampler(0, 0, {0});
			for(int i = 0; i < Engine::gHitmenInFlight; i++) {
				self.updateDescriptorSetBuffer(1 + i, 0, {static_cast<unsigned long long>(2 + i)});
			}
		};

		static std::vector<DeviceMemory::BufferInfo> gBufferInfos(
			[]() -> std::vector<DeviceMemory::BufferInfo> {
				std::vector<DeviceMemory::BufferInfo> lambdaReturn{
					DeviceMemory::BufferInfo(gVertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::BufferInfo(gIndexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getDevices().getGraphicsQfIndex()),
				};

				for(int i = 0; i < Engine::gHitmenInFlight; i++) {
					lambdaReturn.emplace_back(gParticleBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, getDevices().getGraphicsQfIndex());
				}

				return lambdaReturn;
			}()
		);

		static std::vector<DeviceMemory::ImageInfo> gImageInfos{
			DeviceMemory::ImageInfo(
				VK_IMAGE_TYPE_2D,
				static_cast<VkFormat>(getKtxTexture2()->vkFormat),
				VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1),
				DeviceMemory::calculateMipLevels(VkExtent2D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight)),
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				getDevices().getGraphicsQfIndex(),
				VK_IMAGE_LAYOUT_UNDEFINED,
				DeviceMemory::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, static_cast<VkFormat>(getKtxTexture2()->vkFormat), VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1))
			),
			DeviceMemory::ImageInfo(
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_D32_SFLOAT,
				VkExtent3D(getSwapchain().getCurrentExtent().width, getSwapchain().getCurrentExtent().height, 1),
				1,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				getDevices().getGraphicsQfIndex(),
				VK_IMAGE_LAYOUT_UNDEFINED,
				DeviceMemory::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D32_SFLOAT, VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1))
			),
		};

		static VkPhysicalDeviceProperties gPhysicalDeviceProperties(
			[]() -> VkPhysicalDeviceProperties {
				VkPhysicalDeviceProperties lambdaReturn{};
				vkGetPhysicalDeviceProperties(getDevices().getPhysicalDevice(), &lambdaReturn);
				return lambdaReturn;
			}()
		);
		static std::vector<DeviceMemory::SamplerInfo> gSamplerInfos{
				DeviceMemory::SamplerInfo(
					VK_FILTER_LINEAR,
					VK_FILTER_LINEAR,
					VK_SAMPLER_MIPMAP_MODE_LINEAR,
					VK_SAMPLER_ADDRESS_MODE_REPEAT,
					VK_SAMPLER_ADDRESS_MODE_REPEAT,
					0.0f,
					VK_TRUE,
					gPhysicalDeviceProperties.limits.maxSamplerAnisotropy,
					0.0f,
					0.0f,
					VK_BORDER_COLOR_INT_OPAQUE_WHITE
				)
		};

		static std::vector<DeviceMemory::DescriptorSetInfo> gDescriptorSetInfos(
			[]() -> std::vector<DeviceMemory::DescriptorSetInfo> {
				std::vector<DeviceMemory::DescriptorSetInfo> lambdaReturn{
					DeviceMemory::DescriptorSetInfo({
							VkDescriptorSetLayoutBinding{
								.binding = 0,
								.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								.descriptorCount = 1,
								.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
							}
					})
				};

				for(int i = 0; i < Engine::gHitmenInFlight; i++) {
					lambdaReturn.emplace_back(std::vector<VkDescriptorSetLayoutBinding>{Particle::Particle::getDescriptorSetBinding(0)});
				}

				return lambdaReturn;
			}()
		);

		static DeviceMemory::DeviceLocal gDeviceLocalMemory(
			&getDevices(),
			DeviceMemory::DeviceLocal::CreateInfo(std::move(gBufferInfos), std::move(gImageInfos), std::move(gSamplerInfos), std::move(gDescriptorSetInfos)),
			gPopulate
		);

		return gDeviceLocalMemory;
	}

	::Engine::GraphicsPipeline& getGraphicsPipeline() {
		static ::Engine::GraphicsPipeline gGraphicsPipeline(
			&getDevices(),
			[]() -> ::Engine::GraphicsPipeline::CreateInfo {
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

				std::vector<char> sprivFileBytes(Common::loadSprivFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\shaders.spv)"));
				VkShaderModuleCreateInfo shaderModuleForEverythingInfo{
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.codeSize = static_cast<uint32_t>(sprivFileBytes.size()),
					.pCode = reinterpret_cast<uint32_t const*>(sprivFileBytes.data())
				};
				VkShaderModule pShaderModuleForEverything{};
				CHECK_VK_SUCCESS(
					vkCreateShaderModule(getDevices().getLogicalDevice(), &shaderModuleForEverythingInfo, nullptr, &pShaderModuleForEverything),
					"Failed to create shader module"
				)
				std::vector<VkPipelineShaderStageCreateInfo> stages{
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = pShaderModuleForEverything,
						.pName = "vertexShader"
					},
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = pShaderModuleForEverything,
						.pName = "fragmentShader"
					}
				};
				pipelineCreateInfo.stageCount = static_cast<uint32_t>(stages.size());
				pipelineCreateInfo.pStages = stages.data();

				std::vector<VkVertexInputBindingDescription> binding{ Vertex::Vertex::getInputBinding() };
				std::vector<VkVertexInputAttributeDescription> attributes{ Vertex::Vertex::getInputAttributes() };
				VkPipelineVertexInputStateCreateInfo vertexInput{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.vertexBindingDescriptionCount = static_cast<uint32_t>(binding.size()),
					.pVertexBindingDescriptions = binding.data(),
					.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
					.pVertexAttributeDescriptions = attributes.data(),
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

				std::vector<VkDescriptorSetLayout> graphicsLayout{
					getHostVisibleMemory().getDescriptorSetLayouts()[3], // Model transform uniform buffer
					getHostVisibleMemory().getDescriptorSetLayouts()[0] // Combined image sampler
				};

				VkPipelineLayoutCreateInfo layoutInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.setLayoutCount = static_cast<uint32_t>(graphicsLayout.size()),
					.pSetLayouts = graphicsLayout.data()
				};
				CHECK_VK_SUCCESS(
					vkCreatePipelineLayout(getDevices().getLogicalDevice(), &layoutInfo, nullptr, &pipelineCreateInfo.layout),
					"Failed to create pipeline layout"
				)

				return ::Engine::GraphicsPipeline::CreateInfo(
					pipelineCreateInfo,
					rendering, std::move(colorAttachmentFormats),
					std::move(stages),
					vertexInput, std::move(binding), std::move(attributes),
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

	[[nodiscard]] ::Engine::ComputePipeline& getComputePipeline() {
		static ::Engine::ComputePipeline computePipeline(
			&getDevices(),
			[]() -> ::Engine::ComputePipeline::CreateInfo {
				VkComputePipelineCreateInfo create{
					.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO
				};

				std::vector<char> sprivFileBytes(Common::loadSprivFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\shaders.spv)"));
				VkShaderModuleCreateInfo shaderModuleInfo{
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.codeSize = static_cast<uint32_t>(sprivFileBytes.size()),
					.pCode = reinterpret_cast<uint32_t const*>(sprivFileBytes.data())
				};
				VkShaderModule pShaderModule{};
				CHECK_VK_SUCCESS(
					vkCreateShaderModule(getDevices().getLogicalDevice(), &shaderModuleInfo, nullptr, &pShaderModule),
					"Failed to create shader module"
				)

				create.stage = 	VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_COMPUTE_BIT,
					.module = pShaderModule,
					.pName = "computeShader"
				};

				std::vector<VkDescriptorSetLayout> computeLayouts{};
				computeLayouts.push_back(getHostVisibleMemory().getDescriptorSetLayouts()[11]); // Delta time uniform buffer
				computeLayouts.push_back(getDeviceLocalMemory().getDescriptorSetLayouts()[1]);  // PARTICLES_IN SSBO
				computeLayouts.push_back(getDeviceLocalMemory().getDescriptorSetLayouts()[1]); // particlesOut SSBO
				VkPipelineLayoutCreateInfo layoutInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.setLayoutCount = 3,
					.pSetLayouts = computeLayouts.data()
				};
				CHECK_VK_SUCCESS(
					vkCreatePipelineLayout(getDevices().getLogicalDevice(), &layoutInfo, nullptr, &create.layout),
					"Failed to create pipeline layout"
				)

				return ::Engine::ComputePipeline::CreateInfo(create);
			}()
		);

		return computePipeline;
	}
}