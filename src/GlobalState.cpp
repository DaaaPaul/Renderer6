#include <cassert>
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
		Backend::Instance::CreateInfo createInfo(ENABLED_LAYERS, ENABLED_EXTENSIONS, APP_INFO, INSTANCE_CREATE_INFO);

		static Backend::Instance gBackend(&getWindow(), std::move(createInfo));

		return gBackend;
	}

	Backend::Devices& Core::getDevices() {
		assert(loaded);

		static Backend::Devices gDevices(
			&getInstance(),
			Backend::Devices::sGetConstructParameters(getInstance().instance)
		);

		return gDevices;
	}

	Backend::Swapchain& Core::getSwapchain() {
		assert(loaded);

		static Backend::Swapchain gSwapchainWrapper(
			&getDevices(),
			Backend::Swapchain::sGetConstructParameters(
				getInstance().instance,
				getDevices().mpPhysicalDevice,
				getInstance().WINDOW->glfwWindow,
				getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX
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
					DeviceMemory::Common::BufferInfo(sizeof(Vertex::Vertex) * 8, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX),
					DeviceMemory::Common::BufferInfo(sizeof(uint32_t) * 12, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX),
					DeviceMemory::Common::BufferInfo(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX),
					DeviceMemory::Common::BufferInfo(getKtxTexture2()->dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX)
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
						vkGetPhysicalDeviceProperties(getDevices().mpPhysicalDevice, &arguement);
						return arguement;
					}()
				);

				const DeviceMemory::DeviceLocal::CreateInfo ARGUEMENT(
					{ 
						DeviceMemory::Common::BufferInfo(sizeof(Vertex::Vertex) * 8, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX),
						DeviceMemory::Common::BufferInfo(sizeof(uint32_t) * 12, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX)
					},
					{
						DeviceMemory::Common::ImageInfo(
							VK_IMAGE_TYPE_2D,
							static_cast<VkFormat>(getKtxTexture2()->vkFormat),
							VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1),
							1,
							VK_SAMPLE_COUNT_1_BIT,
							VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
							getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX,
							VK_IMAGE_LAYOUT_UNDEFINED,
							DeviceMemory::Common::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, static_cast<VkFormat>(getKtxTexture2()->vkFormat), VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1))
						),
						DeviceMemory::Common::ImageInfo(
							VK_IMAGE_TYPE_2D,
							VK_FORMAT_D32_SFLOAT,
							VkExtent3D(getSwapchain().mParameters.mSwapchainKHRCreateInfo.imageExtent.width, getSwapchain().mParameters.mSwapchainKHRCreateInfo.imageExtent.height, 1),
							1,
							VK_SAMPLE_COUNT_1_BIT,
							VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
							getDevices().mGRAPHICS_QUEUE_FAMILY_INDEX,
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