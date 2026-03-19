#include <cassert>
#include <ctime>
#include <cmath>
#include <array>
#include <random>
#include <iostream>
#include "Util.h"
#include "Global.h"
#include "FeatureChain.hpp"

namespace Global {
	namespace Engine {
		void asserts() noexcept {
			assert(Global::gLoaded);
			assert(gFrameIndex == 0);
			assert(gFramesInFlight != UINT32_MAX);
		}

		void load() {
			(void) getFrames();
			(void) getCurrentTransformation();

			gLoaded = true;
		}

		::Engine::Frames& getFrames() {
			static ::Engine::Frames gFrames(gFramesInFlight, Global::getDevices().getGraphicsQfIndex());

			return gFrames;
		}

		Vertex::Transforms& getCurrentTransformation() {
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

	void asserts() noexcept {
		assert(gPARTICLES_COUNT % 256 == 0);
	}

	void load() {
		(void) getSwapchain();
		(void) getHostVisibleMemory();
		(void) getDeviceLocalMemory();
		(void) getModelPipelineLayout();
		(void) getComputePipelineLayout();
		(void) getModelShaderModule();
		(void) getParticleShaderModule();
		(void) getModelGraphicsPipeline();
		(void) getParticlesGraphicsPipeline();
		(void) getComputePipeline();

		gLoaded = true;
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
				Engine::gFramesInFlight = SWAPCHAIN_INFO.minImageCount; /* (*) */

				return Backend::Swapchain::CreateInfo(std::move(pReturnSurface), SWAPCHAIN_INFO, std::move(accessorGfxQf));
			}()
		);

		return gSwapchainWrapper;
	}

	DeviceMemory::HostVisible& getHostVisibleMemory() {
		// Buffer 0 - Model vertices
		// Buffer 1 - Model vertex indices
		// Buffer 2 - Model texture data
		// Buffer 3 to 6 - Model transform
		// Buffer 7 to 10 - Particle SSBO
		// Buffer 11 to 14 - Particle delta time
		static auto gPopulate = [](DeviceMemory::HostVisible& self) -> void {
			self.writeToBuffer(0, loadModelVertices().first.data(), gVertexBufferSize);
			self.writeToBuffer(1, loadModelVertices().second.data(), gIndexBufferSize);
			self.writeToBuffer(2, loadKtxTexture2()->pData, loadKtxTexture2()->dataSize);

			for(int i = 0; i < Engine::gFramesInFlight; i++) {
				self.writeToBuffer(3 + i, &Engine::getCurrentTransformation(), sizeof(Vertex::Transforms));
			}
			for(int i = 0; i < Engine::gFramesInFlight; i++) {
				self.writeToBuffer(7 + i, loadParticlesData().data(), gPARTICLES_BUFFER_SIZE);
			}
		};

		static std::vector<DeviceMemory::BufferInfo> gBufferInfos(
			[]() -> std::vector<DeviceMemory::BufferInfo> {
				std::vector<DeviceMemory::BufferInfo> lambdaReturn{
					DeviceMemory::BufferInfo(gVertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::BufferInfo(gIndexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::BufferInfo(loadKtxTexture2()->dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex())
				};

				for(int i = 0; i < Engine::gFramesInFlight; i++) {
					lambdaReturn.emplace_back(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, getDevices().getGraphicsQfIndex());
				}
				for(int i = 0; i < Engine::gFramesInFlight; i++) {
					lambdaReturn.emplace_back(gPARTICLES_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, getDevices().getGraphicsQfIndex());
				}
				for(int i = 0; i < Engine::gFramesInFlight; i++) {
					lambdaReturn.emplace_back(sizeof(float), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, getDevices().getGraphicsQfIndex());
				}

				return lambdaReturn;
			}()
		);

		static DeviceMemory::HostVisible gHostVisibleMemory(
			&getDevices(),
			DeviceMemory::HostVisible::CreateInfo(std::move(gBufferInfos), {}),
			gPopulate
		);

		return gHostVisibleMemory;
	}

	DeviceMemory::DeviceLocal& getDeviceLocalMemory() {
		// Buffer 0 - Model vertices
		// Buffer 1 - Model vertex indices
		// Buffer 2 to 5 - Particle SSBO

		// Image 0 - Model texture
		// Image 1 - Depth image

		// Sampler 0 - Model texture sampler

		// Descriptor Set 0 - Matches sampler 0
		static auto gPopulate = [](DeviceMemory::DeviceLocal& self) -> void {
			self.copyBufferToBuffer(0, getHostVisibleMemory().getBuffers()[0], {VkBufferCopy(0, 0, gVertexBufferSize)});
			self.copyBufferToBuffer(1, getHostVisibleMemory().getBuffers()[1], {VkBufferCopy(0, 0, gIndexBufferSize)});
			for(int i = 0; i < Engine::gFramesInFlight; i++) {
				self.copyBufferToBuffer(2 + i, getHostVisibleMemory().getBuffers()[7 + i], {VkBufferCopy(0, 0, gPARTICLES_BUFFER_SIZE)});
			}

			self.copyBufferToImage(0, getHostVisibleMemory().getBuffers()[2], {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(loadKtxTexture2()->baseWidth, loadKtxTexture2()->baseHeight, 1))});
			
			self.descriptorSetBindingToCombinedImageSampler(0, 0, {0});
		};

		static std::vector<DeviceMemory::BufferInfo> gBufferInfos(
			[]() -> std::vector<DeviceMemory::BufferInfo> {
				std::vector<DeviceMemory::BufferInfo> lambdaReturn{
					DeviceMemory::BufferInfo(gVertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getDevices().getGraphicsQfIndex()),
					DeviceMemory::BufferInfo(gIndexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getDevices().getGraphicsQfIndex()),
				};

				for(int i = 0; i < Engine::gFramesInFlight; i++) {
					lambdaReturn.emplace_back(gPARTICLES_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, getDevices().getGraphicsQfIndex());
				}

				return lambdaReturn;
			}()
		);

		static std::vector<DeviceMemory::ImageInfo> gImageInfos{
			DeviceMemory::ImageInfo(
				VK_IMAGE_TYPE_2D,
				static_cast<VkFormat>(loadKtxTexture2()->vkFormat),
				VkExtent3D(loadKtxTexture2()->baseWidth, loadKtxTexture2()->baseHeight, 1),
				DeviceMemory::calculateMipLevels(VkExtent2D(loadKtxTexture2()->baseWidth, loadKtxTexture2()->baseHeight)),
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				getDevices().getGraphicsQfIndex(),
				VK_IMAGE_LAYOUT_UNDEFINED,
				DeviceMemory::ImageViewInfo(VK_IMAGE_VIEW_TYPE_2D, static_cast<VkFormat>(loadKtxTexture2()->vkFormat), VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1))
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

	::Engine::PipelineLayout& getModelPipelineLayout() {
		static ::Engine::PipelineLayout gLayout(&getDevices(), 
		std::vector<VkDescriptorSetLayout>{
			getDeviceLocalMemory().getDescriptorSetLayouts()[0], // combined image sampler (set 0)
		},
		std::vector<VkPushConstantRange>{
			VkPushConstantRange{
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				.offset = 0,
				.size = POINTER_SIZE(1)
			}
		});

		return gLayout;
	}

	::Engine::PipelineLayout& getEmptyPipelineLayout() {
		static ::Engine::PipelineLayout gEmptyLayout(&getDevices(), {}, {});
		
		return gEmptyLayout;
	}

	::Engine::PipelineLayout& getComputePipelineLayout() {
		static ::Engine::PipelineLayout gLayout(&getDevices(), 
		std::vector<VkDescriptorSetLayout>{}, 
		std::vector<VkPushConstantRange>{
			VkPushConstantRange{
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.offset = 0,
				.size = POINTER_SIZE(3)
			}
		});

		return gLayout;
	}

	::Engine::ShaderModule& getModelShaderModule() {
		static constexpr const char* gPATH = R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\shaders.spv)";
		static ::Engine::ShaderModule gModelShaderModule(&getDevices(), Util::getFileBytes(gPATH));

		return gModelShaderModule;
	}

	::Engine::ShaderModule& getParticleShaderModule() {
		static constexpr const char* gPATH = R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\particleShaders.spv)";
		static ::Engine::ShaderModule gParticleShaderModule(&getDevices(), Util::getFileBytes(gPATH));

		return gParticleShaderModule;
	}

	::Engine::GraphicsPipeline& getModelGraphicsPipeline() {
		static ::Engine::GraphicsPipeline gModelPipeline(
			&getDevices(),
			[]() -> ::Engine::GraphicsPipeline::CreateInfo {
				VkGraphicsPipelineCreateInfo graphicsPipelineCreate{
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

				std::vector<VkPipelineShaderStageCreateInfo> stages{
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = getModelShaderModule().getShaderModule(),
						.pName = "vertexShader"
					},
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = getModelShaderModule().getShaderModule(),
						.pName = "fragmentShader"
					}
				};
				graphicsPipelineCreate.stageCount = static_cast<uint32_t>(stages.size());
				graphicsPipelineCreate.pStages = stages.data();

				std::vector<VkVertexInputBindingDescription> binding{ Vertex::Vertex::getInputBinding() };
				std::vector<VkVertexInputAttributeDescription> attributes(Vertex::Vertex::getInputAttributes());
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
					.cullMode = VK_CULL_MODE_BACK_BIT,
					.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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

				graphicsPipelineCreate.layout = getModelPipelineLayout().getLayout();

				return ::Engine::GraphicsPipeline::CreateInfo(
					graphicsPipelineCreate,
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

		return gModelPipeline;
	}

	::Engine::GraphicsPipeline& getParticlesGraphicsPipeline() {
		static ::Engine::GraphicsPipeline gParticlesPipeline(
			&getDevices(),
			[]() -> ::Engine::GraphicsPipeline::CreateInfo {
				VkGraphicsPipelineCreateInfo particlesPipelineCreate{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					// reroute needed for everything
				};

				std::vector<VkFormat> colorAttachmentFormats{ VK_FORMAT_R8G8B8A8_SRGB };
				VkPipelineRenderingCreateInfo rendering{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
					.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size()),
					.pColorAttachmentFormats = colorAttachmentFormats.data(),
				};

				std::vector<VkPipelineShaderStageCreateInfo> stages{
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = getParticleShaderModule().getShaderModule(),
						.pName = "vertexShader"
					},
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = getParticleShaderModule().getShaderModule(),
						.pName = "fragmentShader"
					}
				};
				particlesPipelineCreate.stageCount = static_cast<uint32_t>(stages.size());
				particlesPipelineCreate.pStages = stages.data();

				std::vector<VkVertexInputBindingDescription> binding{ Particle::getInputBinding() };
				std::vector<VkVertexInputAttributeDescription> attributes(Particle::getInputAttributes());
				VkPipelineVertexInputStateCreateInfo vertexInput{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.vertexBindingDescriptionCount = static_cast<uint32_t>(binding.size()),
					.pVertexBindingDescriptions = binding.data(),
					.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
					.pVertexAttributeDescriptions = attributes.data(),
				};

				VkPipelineInputAssemblyStateCreateInfo inputAssembly{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
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

				// UNUSED CURRENTLY
				VkPipelineDepthStencilStateCreateInfo depthStencil{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.depthTestEnable = VK_FALSE,
					.depthWriteEnable = VK_FALSE,
				};

				std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{
					VkPipelineColorBlendAttachmentState{
						.blendEnable = VK_TRUE,
						.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
						.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
						.colorBlendOp = VK_BLEND_OP_ADD,
						.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
						.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
						.alphaBlendOp = VK_BLEND_OP_ADD,
						.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT 
					}
				};
				VkPipelineColorBlendStateCreateInfo colorBlend{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.logicOpEnable = VK_FALSE,
					.logicOp = VK_LOGIC_OP_COPY,
					.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),
					.pAttachments = colorBlendAttachments.data()
				};

				std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
				VkPipelineDynamicStateCreateInfo dynamicStateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
					.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
					.pDynamicStates = dynamicStates.data(),
				};

				particlesPipelineCreate.layout = getEmptyPipelineLayout().getLayout();

				return ::Engine::GraphicsPipeline::CreateInfo(
					particlesPipelineCreate,
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

		return gParticlesPipeline;
	}

	::Engine::ComputePipeline& getComputePipeline() {
		static ::Engine::ComputePipeline computePipeline(
			&getDevices(),
			[]() -> ::Engine::ComputePipeline::CreateInfo {
				VkComputePipelineCreateInfo computePipelineCreate{
					.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO
				};

				computePipelineCreate.stage = 	VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_COMPUTE_BIT,
					.module = getParticleShaderModule().getShaderModule(),
					.pName = "computeShader"
				};

				computePipelineCreate.layout = getComputePipelineLayout().getLayout();

				return ::Engine::ComputePipeline::CreateInfo(computePipelineCreate);
			}()
		);

		return computePipeline;
	}
}