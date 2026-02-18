#include "Common.h"
#include "GlobalState.h"

namespace GlobalState {
	ktxTexture2 const* Core::getKtxTexture2() const {
		const char* TEXTURE_PATH = R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\textures\Lumberjack Sion Compressed.ktx2)";
		static ktxTexture2 const*const gKTX_TEXTURE2 = DeviceMemory::Common::fKtxLoadImage(TEXTURE_PATH);
		return gKTX_TEXTURE2;
	}

	Backend::Window& Core::getWindow() const {
		const Backend::Window::CreateInfo WINDOW_CREATE_INFO{
			.width = 800,
			.height = 600,
			.NAME = "Renderer6"
		};
		static Backend::Window gWindowWrapper(WINDOW_CREATE_INFO);
		return gWindowWrapper;
	}

	Backend::Instance& Core::getInstance() const {
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

	const DeviceMemory::Common::HostVisibleConstructArguements fGetHostVisibleMemoryConstructArguements() {
		static DeviceMemory::Common::HostVisibleConstructArguements gConstructArguements{
			.mBufferInfos{ 
				DeviceMemory::Common::BufferInfo(sizeof(Vertex::Vertex) * 8, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX),
				DeviceMemory::Common::BufferInfo(sizeof(uint32_t) * 12, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX),
				DeviceMemory::Common::BufferInfo(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX),
				DeviceMemory::Common::BufferInfo(getKtxTexture2()->dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX)
			},
			.mDescriptorSetInfos{
				DeviceMemory::Common::DescriptorSetInfo({Vertex::Transforms::sGetTransformationMatricesDescriptorSetLayoutBinding(0)})
			}
		};

		return gConstructArguements;
	}

	void fPopulateHostVisibleMemory(DeviceMemory::HostVisible& toBePopulated) {
		const std::vector<Vertex::Vertex> VERTICIES{
			Vertex::Vertex(
				glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), 
				glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 
				glm::vec2(0.0f, 0.0f)), // top left
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
				glm::vec2(0.0f, 0.0f)), // top left
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

		toBePopulated.writeToBuffer(0, VERTICIES.data(), sizeof(Vertex::Vertex) * 8);
		toBePopulated.writeToBuffer(1, INDICES.data(), sizeof(uint32_t) * 12);
		toBePopulated.writeToBuffer(3, getKtxTexture2()->pData, getKtxTexture2()->dataSize);
		toBePopulated.updateDescriptorSetBuffer(0, 0, {2});
	}

	const DeviceMemory::Common::DeviceLocalConstructArguements fGetDeviceLocalMemoryConstructArguements() {
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(fGetDevicesWrapper().mpPhysicalDevice, &properties);

		static DeviceMemory::Common::DeviceLocalConstructArguements gConstructArguements{
			.mBufferInfos{ 
				DeviceMemory::Common::BufferInfo(sizeof(Vertex::Vertex) * 8, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX),
				DeviceMemory::Common::BufferInfo(sizeof(uint32_t) * 12, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX)
			},
			.mImageInfos{
				DeviceMemory::Common::ImageInfo(
					VK_IMAGE_TYPE_2D,
					static_cast<VkFormat>(getKtxTexture2()->vkFormat),
					VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1),
					1,
					VK_SAMPLE_COUNT_1_BIT,
					VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX,
					VK_IMAGE_LAYOUT_UNDEFINED,
					DeviceMemory::Common::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, static_cast<VkFormat>(getKtxTexture2()->vkFormat), VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1))
				),
				DeviceMemory::Common::ImageInfo(
					VK_IMAGE_TYPE_2D,
					VK_FORMAT_D32_SFLOAT,
					VkExtent3D(fGetSwapchainWrapper().mParameters.mSwapchainKHRCreateInfo.imageExtent.width, fGetSwapchainWrapper().mParameters.mSwapchainKHRCreateInfo.imageExtent.height, 1),
					1,
					VK_SAMPLE_COUNT_1_BIT,
					VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX,
					VK_IMAGE_LAYOUT_UNDEFINED,
					DeviceMemory::Common::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D32_SFLOAT, VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1))
				),
			},
			.mSamplerInfos{
				DeviceMemory::Common::SamplerInfo(
					VK_FILTER_LINEAR,
					VK_FILTER_LINEAR,
					VK_SAMPLER_MIPMAP_MODE_LINEAR,
					VK_SAMPLER_ADDRESS_MODE_REPEAT,
					VK_SAMPLER_ADDRESS_MODE_REPEAT,
					0.0f,
					VK_TRUE,
					properties.limits.maxSamplerAnisotropy,
					0.0f,
					0.0f,
					VK_BORDER_COLOR_INT_OPAQUE_WHITE
				)
			},
			.mDescriptorSetInfos{
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
			},
		};

		return gConstructArguements;
	}

	void fPopulateDeviceLocalMemory(DeviceMemory::DeviceLocal& toBePopulated) {
		toBePopulated.copyBufferToBuffer(0, fGetHostVisibleMemory().mpBuffers[0], {VkBufferCopy(0, 0, sizeof(Vertex::Vertex) * 8)});
		toBePopulated.copyBufferToBuffer(1, fGetHostVisibleMemory().mpBuffers[1], {VkBufferCopy(0, 0, sizeof(uint32_t) * 12)});
		toBePopulated.copyBufferToImage(0, fGetHostVisibleMemory().mpBuffers[3], {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(getKtxTexture2()->baseWidth, getKtxTexture2()->baseHeight, 1))}); // buffer is tightly packed row by row
		toBePopulated.updateDescriptorSetCombinedImageSampler(0, 0, {0});
	}
}