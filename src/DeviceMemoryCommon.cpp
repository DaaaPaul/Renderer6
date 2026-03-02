#include <iostream>
#include "DeviceMemoryCommon.h"
#include "Common.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

namespace DeviceMemory {
	namespace Common {
		void loadGltfModel(const char* const& PATH, std::vector<Vertex::Vertex>& vertices, std::vector<uint32_t>& indices) {
			tinygltf::Model model{};
			tinygltf::TinyGLTF loader{};
			std::string error{};
			std::string warning{};

			if(!loader.LoadASCIIFromFile(&model, &error, &warning, PATH)) {
				throw std::runtime_error("Failed to load gltf model: " + error);
			}
			if(!warning.empty()) {
				std::cerr << "Load gltf model attempt warning: " + warning << "\n";
			}

			// Process all meshes in the model
			std::unordered_map<Vertex::Vertex, uint32_t> uniqueVertices{};

			for (tinygltf::Mesh const& MESH : model.meshes) {
				for (tinygltf::Primitive const& PRIMITIVE : MESH.primitives) {
					// Get indices
					tinygltf::Accessor const& INDEX_ACCESSOR = model.accessors[PRIMITIVE.indices];
					tinygltf::BufferView const& INDEX_BUFFER_VIEW = model.bufferViews[INDEX_ACCESSOR.bufferView];
					tinygltf::Buffer const& INDEX_BUFFER = model.buffers[INDEX_BUFFER_VIEW.buffer];

					// Get vertex positions
					tinygltf::Accessor const& POSITION_ACCESSOR = model.accessors[PRIMITIVE.attributes.at("POSITION")];
					tinygltf::BufferView const& POSITION_BUFFER_VIEW = model.bufferViews[POSITION_ACCESSOR.bufferView];
					tinygltf::Buffer const& POSITION_BUFFER = model.buffers[POSITION_BUFFER_VIEW.buffer];

					// Get texture coordinates if available
					const bool HAS_TEXCOORDS = PRIMITIVE.attributes.find("TEXCOORD_0") != PRIMITIVE.attributes.end();
					tinygltf::Accessor const* TEXCOORD_ACCESSOR = nullptr;
					tinygltf::BufferView const* TEXCOORD_BUFFER_VIEW = nullptr;
					tinygltf::Buffer const* TEXCOORD_BUFFER = nullptr;

					if (HAS_TEXCOORDS) {
						TEXCOORD_ACCESSOR = &model.accessors[PRIMITIVE.attributes.at("TEXCOORD_0")];
						TEXCOORD_BUFFER_VIEW = &model.bufferViews[TEXCOORD_ACCESSOR->bufferView];
						TEXCOORD_BUFFER = &model.buffers[TEXCOORD_BUFFER_VIEW->buffer];
					}

					// Process vertices
					for (size_t i = 0; i < POSITION_ACCESSOR.count; i++) {
						Vertex::Vertex vertex{};

						// Get position
						float const* POSITION = reinterpret_cast<float const*>(&POSITION_BUFFER.data[POSITION_BUFFER_VIEW.byteOffset + POSITION_ACCESSOR.byteOffset + i * 12]);
						vertex.position = {POSITION[0], POSITION[1], POSITION[2], 1.0f};

						// Get texture coordinates if available
						if (HAS_TEXCOORDS) {
							float const* TEXCOORD = reinterpret_cast<float const*>(&TEXCOORD_BUFFER->data[TEXCOORD_BUFFER_VIEW->byteOffset + TEXCOORD_ACCESSOR->byteOffset + i * 8]);
							vertex.texCoord = {TEXCOORD[0], TEXCOORD[1]};
						} else {
							vertex.texCoord = {0.0f, 0.0f};
						}

						// Set default color
						vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};

						// Add vertex if unique
						if (!uniqueVertices.contains(vertex)) {
							uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
							vertices.push_back(vertex);
						}
					}

					// Process indices
					unsigned char const* INDEX_DATA = &INDEX_BUFFER.data[INDEX_BUFFER_VIEW.byteOffset + INDEX_ACCESSOR.byteOffset];

					// Handle different index component types
					if (INDEX_ACCESSOR.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
						uint16_t const* UI16 = reinterpret_cast<const uint16_t*>(INDEX_DATA);
						for (size_t i = 0; i < INDEX_ACCESSOR.count; i++) {
							Vertex::Vertex vertex = vertices[UI16[i]];
							indices.push_back(uniqueVertices[vertex]);
						}
					} else if (INDEX_ACCESSOR.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
						uint32_t const* UI32 = reinterpret_cast<const uint32_t*>(INDEX_DATA);
						for (size_t i = 0; i < INDEX_ACCESSOR.count; i++) {
							Vertex::Vertex vertex = vertices[UI32[i]];
							indices.push_back(uniqueVertices[vertex]);
						}
					} else if (INDEX_ACCESSOR.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
						uint8_t const* UI8 = reinterpret_cast<const uint8_t*>(INDEX_DATA);
						for (size_t i = 0; i < INDEX_ACCESSOR.count; i++) {
							Vertex::Vertex vertex = vertices[UI8[i]];
							indices.push_back(uniqueVertices[vertex]);
						}
					}
				}
			}

			std::cout << "Model at " << PATH << " unique vertices : " << vertices.size() << "\n";
			std::cout << "Model at " << PATH << " total vertices : " << indices.size() << "\n";
		}

		ktxTexture2* loadKtxImage(const char* const& FILE_PATH) {
			ktxTexture2* pKtxTexture{};
			ktx_error_code_e error{};

			if((error = ktxTexture_CreateFromNamedFile(FILE_PATH, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, reinterpret_cast<ktxTexture**>(&pKtxTexture))) != KTX_SUCCESS) {
				throw std::runtime_error("Failed to load ktx texture from " + std::string(FILE_PATH) + ". Error code is " + std::to_string(static_cast<int>(error)));
			}

			if(ktxTexture2_NeedsTranscoding(pKtxTexture)) {
				const ktx_transcode_fmt_e TARGET_FORMAT{ KTX_TTF_BC7_RGBA };

				if(ktxTexture2_TranscodeBasis(pKtxTexture, TARGET_FORMAT, 0) != KTX_SUCCESS) {
					throw std::runtime_error("Failed to transcode ktx texture to ktx_transcode_fmt " + std::to_string(TARGET_FORMAT));
				}
			}

			return pKtxTexture;
		}

		[[nodiscard]] VkBuffer createBuffer(VkLogicalDevice pLogicalDevice, BufferInfo const& INFO) {
			VkBuffer buffer{};

			const VkBufferCreateInfo BUFFER_CREATE{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = INFO.size,
				.usage = INFO.usage,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 1,
				.pQueueFamilyIndices = &INFO.graphicsQfIndex,
			};

			
			CHECK_VK_SUCCESS(
			vkCreateBuffer(pLogicalDevice, &BUFFER_CREATE, nullptr, &buffer),
			"Failed to create buffer"
			)

			return buffer;
		}

		[[nodiscard]] VkImage createImage(VkLogicalDevice pLogicalDevice, ImageInfo const& IMAGE_INFO) {
			VkImage image{};

			const VkImageCreateInfo IMAGE_CREATE{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.imageType = IMAGE_INFO.type,
				.format = IMAGE_INFO.format,
				.extent = IMAGE_INFO.extent,
				.mipLevels = IMAGE_INFO.mipLevelsCount,
				.arrayLayers = 1,
				.samples = IMAGE_INFO.sampleCount,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = IMAGE_INFO.usage,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 1,
				.pQueueFamilyIndices = &IMAGE_INFO.graphicsQfIndex,
				.initialLayout = IMAGE_INFO.initialLayout,
			};

			CHECK_VK_SUCCESS(
			vkCreateImage(pLogicalDevice, &IMAGE_CREATE, nullptr, &image),
			"Failed to create image"
			)

			return image;
		}

		[[nodiscard]] VkImageView createImageView(VkLogicalDevice pLogicalDevice, VkImage image, ImageViewInfo const& IMAGE_VIEW_INFO) {
			VkImageView imageView{};

			const VkImageViewCreateInfo IMAGE_VIEW_CREATE{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = image,
				.viewType = IMAGE_VIEW_INFO.type,
				.format = IMAGE_VIEW_INFO.format,
				.subresourceRange = IMAGE_VIEW_INFO.subresourceRange,
			};

			CHECK_VK_SUCCESS(
			vkCreateImageView(pLogicalDevice, &IMAGE_VIEW_CREATE, nullptr, &imageView),
			"Failed to create image view"
			)

			return imageView;
		}

		[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> getMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS) {
			std::pair<VkDeviceSize, std::vector<VkDeviceSize>> allocationSizeAndBufferOffsets{};
			uint32_t buffersCount = static_cast<uint32_t>(BUFFER_MEMORY_REQUIREMENTS.size());
			allocationSizeAndBufferOffsets.second.resize(buffersCount, UINT64_MAX);

			for (int i = 0; i < buffersCount; i++) {
				while (allocationSizeAndBufferOffsets.first % BUFFER_MEMORY_REQUIREMENTS[i].alignment != 0) {
					allocationSizeAndBufferOffsets.first++;
				}

				allocationSizeAndBufferOffsets.second[i] = allocationSizeAndBufferOffsets.first;

				allocationSizeAndBufferOffsets.first += BUFFER_MEMORY_REQUIREMENTS[i].size;
			}

			return allocationSizeAndBufferOffsets;
		}

		[[nodiscard]] uint32_t getMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES) {
			uint32_t finalMemoryRequirementsMask = UINT32_MAX;
			for (VkMemoryRequirements const& BUFFER_MEMORY_REQUIREMENT : BUFFER_MEMORY_REQUIREMENTS) {
				finalMemoryRequirementsMask &= BUFFER_MEMORY_REQUIREMENT.memoryTypeBits;
			}

			VkPhysicalDeviceMemoryProperties memoryProperties{};
			vkGetPhysicalDeviceMemoryProperties(pPhysicalDevice, &memoryProperties);

			uint32_t memoryTypeIndexReturn{ UINT32_MAX };
			for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
				if ((finalMemoryRequirementsMask & (1 << i)) &&
					((memoryProperties.memoryTypes[i].propertyFlags & MEMORY_PROPERTIES) == MEMORY_PROPERTIES)) {
					memoryTypeIndexReturn = i;
				}
			};

			return memoryTypeIndexReturn;
		}

		[[nodiscard]] VkDescriptorPool createDescriptorPool(VkLogicalDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO) {
			VkDescriptorPool pReturnDescriptorPool{};
		
			// create pool sizes (ASSUMING UNIQUE DESCRIPTOR TYPE PER ITS OWN UNIQUE BINDING)
			std::vector<VkDescriptorPoolSize> poolSizes{};
			for(Common::DescriptorSetInfo const& CUSTOM_SET_INFO : INFO) {
				const std::vector<VkDescriptorSetLayoutBinding> BINDINGS{ CUSTOM_SET_INFO.layoutBindings };

				for(VkDescriptorSetLayoutBinding const& BINDING : BINDINGS) {
					poolSizes.emplace_back(BINDING.descriptorType, BINDING.descriptorCount);
				}
			}

			// create descriptor pool
			const VkDescriptorPoolCreateInfo DESCRIPTOR_POOL_INFO{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
				.maxSets = static_cast<uint32_t>(INFO.size()),
				.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
				.pPoolSizes = poolSizes.data(),
			};
			CHECK_VK_SUCCESS(
				vkCreateDescriptorPool(pLogicalDevice, &DESCRIPTOR_POOL_INFO, nullptr, &pReturnDescriptorPool),
				"Failed to create descriptor pool"
			)

			return pReturnDescriptorPool;
		}

		void createBeginOneTimeCommandBuffer(VkLogicalDevice& rpDevice, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf, uint32_t const& GRAPHICS_QF_INDEX) {
			// create transient command pool
			{
				const VkCommandPoolCreateInfo COMMAND_POOL_INFO{
					.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
					.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
					.queueFamilyIndex = GRAPHICS_QF_INDEX
				};
				CHECK_VK_SUCCESS(
					vkCreateCommandPool(rpDevice, &COMMAND_POOL_INFO, nullptr, &rpCmdPool),
					"Failed to create temporary command pool"
				)
			}

			// create command buffer
			{
				const VkCommandBufferAllocateInfo COMMAND_BUFFER_INFO{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
					.commandPool = rpCmdPool,
					.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
					.commandBufferCount = 1,
				};
				CHECK_VK_SUCCESS(
					vkAllocateCommandBuffers(rpDevice, &COMMAND_BUFFER_INFO, &rpCmdBuf),
					"Failed to create temporary command buffer"
				)
			}

			// begin recording
			{
				const VkCommandBufferBeginInfo ONE_TIME_SUBMIT_BEGIN(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
				CHECK_VK_SUCCESS(
					vkBeginCommandBuffer(rpCmdBuf, &ONE_TIME_SUBMIT_BEGIN),
					"Failed to begin temporary command buffer recording"
				)
			}
		}

		void endSubmitDestroyOneTimeCommandBuffer(VkLogicalDevice& rpDevice, VkQueue& rpQueue, VkCommandPool& rpCmdPool, VkCommandBuffer& rpCmdBuf) {
			// end command buffer
			{
				CHECK_VK_SUCCESS(
					vkEndCommandBuffer(rpCmdBuf),
					"Failed to end temporary command buffer recording"
				)
			}

			// create fence to wait on
			VkFence copyCommandDone{};
			{
				const VkFenceCreateInfo FENCE_INFO(VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, 0);
				CHECK_VK_SUCCESS(
					vkCreateFence(rpDevice, &FENCE_INFO, nullptr, &copyCommandDone),
					"Failed to create copy command done fence"
				)
			}

			// submit it
			{
				const VkSubmitInfo ONE_TIME_SUBMIT_INFO{
					.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.commandBufferCount = 1,
					.pCommandBuffers = &rpCmdBuf,
				};

				CHECK_VK_SUCCESS(
					vkQueueSubmit(rpQueue, 1, &ONE_TIME_SUBMIT_INFO, copyCommandDone),
					"Failed to submit temporary command buffer"
				)
			}

			CHECK_VK_SUCCESS(
				vkWaitForFences(rpDevice, 1, &copyCommandDone, VK_TRUE, UINT64_MAX),
				"Failed to wait for copy command done fence"
			)

			vkDestroyFence(rpDevice, copyCommandDone, nullptr);
			vkFreeCommandBuffers(rpDevice, rpCmdPool, 1, &rpCmdBuf);
			vkDestroyCommandPool(rpDevice, rpCmdPool, nullptr);
		}

		void transitionImageLayout(VkCommandBuffer pCmdBuf, VkImage const& pIMAGE, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
		VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
		VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX) {
			const VkImageMemoryBarrier2 IMAGE_MEMORY_BARRIER2{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = SRC_STAGE,
				.srcAccessMask = SRC_ACCESS,
				.dstStageMask = DST_STAGE,
				.dstAccessMask = DST_ACCESS,
				.oldLayout = OLD_LAYOUT,
				.newLayout = NEW_LAYOUT,
				.srcQueueFamilyIndex = GRAPHICS_QF_INDEX,
				.dstQueueFamilyIndex = GRAPHICS_QF_INDEX,
				.image = pIMAGE,
				.subresourceRange = SUBRESOURCE_RANGE,
			};

			const VkDependencyInfo PARENT_MEMORY_BARRIER2{
				.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
				.imageMemoryBarrierCount = 1,
				.pImageMemoryBarriers = &IMAGE_MEMORY_BARRIER2,
			};

			vkCmdPipelineBarrier2(pCmdBuf, &PARENT_MEMORY_BARRIER2);
		}
	}
}
