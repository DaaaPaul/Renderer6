#include <ktx.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <fstream>
#include <random>	
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "Util.h"
#include "Vertex.hpp"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace Util {
	std::vector<std::string> toStringVector(std::vector<const char*> const& C_STRS) {
		std::vector<std::string> strings{};

		for(const char* const& C : C_STRS) {
			strings.emplace_back(C);
		}

		return strings;
	}

	bool containsAll(std::vector<std::string> const& HAVE, std::vector<std::string> const& CHECK) {
		if(CHECK.empty()) {
			return true;
		} else {
			std::vector<std::string> bigCopy(HAVE);
			std::vector<std::string> smallCopy(CHECK);

			std::sort(bigCopy.begin(), bigCopy.end());
			std::sort(smallCopy.begin(), smallCopy.end());

			return std::ranges::includes(bigCopy, smallCopy);
		}
	}

	std::vector<char> getFileBytes(std::string const& PATH) {
		std::ifstream fileIn(PATH, std::ios::binary | std::ios::ate);
		if(!fileIn.good()) {
			throw std::runtime_error("Failure reading file at " + std::string(PATH));
		}

		uint32_t fileSize = fileIn.tellg();
		std::vector<char> bytes(fileSize);

		fileIn.seekg(0);
		fileIn.read(bytes.data(), fileSize);

		return bytes;
	}

	float random() {
		static std::default_random_engine gEngine(static_cast<unsigned>(time(nullptr)));
		static std::uniform_real_distribution gRange(0.0f, 1.0f);

		return gRange(gEngine);
	}

	namespace Vulkan {
		void beginOneTimeCommandBuffer(VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer, uint32_t const& QUEUE_FAMILY_INDEX) {
			VkCommandPoolCreateInfo poolCreate{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
				.queueFamilyIndex = QUEUE_FAMILY_INDEX
			};
			CHECK_VK_SUCCESS(vkCreateCommandPool(gDevice, &poolCreate, nullptr, &cmdPool), "Failed to create temporary command pool")

			VkCommandBufferAllocateInfo commandBufferCreate{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = cmdPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};
			CHECK_VK_SUCCESS(vkAllocateCommandBuffers(gDevice, &commandBufferCreate, &cmdBuffer), "Failed to create temporary command buffer")

			constexpr VkCommandBufferBeginInfo BEGIN{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
			CHECK_VK_SUCCESS(vkBeginCommandBuffer(cmdBuffer, &BEGIN), "Failed to begin temporary command buffer recording")
		}

		void endOneTimeCommandBuffer(VkQueue queue, VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer) {
			CHECK_VK_SUCCESS(vkEndCommandBuffer(cmdBuffer), "Failed to end temporary command buffer recording")

			VkFence cmdBufferDone{};
			constexpr VkFenceCreateInfo CREATE{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			CHECK_VK_SUCCESS(vkCreateFence(gDevice, &CREATE, nullptr, &cmdBufferDone), "Failed to create copy command done fence")

			VkSubmitInfo commandBufferSubmit{
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.commandBufferCount = 1,
				.pCommandBuffers = &cmdBuffer,
			};
			CHECK_VK_SUCCESS(vkQueueSubmit(queue, 1, &commandBufferSubmit, cmdBufferDone), "Failed to submit temporary command buffer")
			CHECK_VK_SUCCESS(vkWaitForFences(gDevice, 1, &cmdBufferDone, VK_TRUE, UINT64_MAX), "Failed to wait for copy command done fence")

			vkDestroyFence(gDevice, cmdBufferDone, nullptr);
			vkFreeCommandBuffers(gDevice, cmdPool, 1, &cmdBuffer);
			vkDestroyCommandPool(gDevice, cmdPool, nullptr);
		}

		void transitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageSubresourceRange const& SUBRESOURCE_RANGE, VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX) {
			VkImageMemoryBarrier2 imageBarrier{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = SRC_STAGE,
				.srcAccessMask = SRC_ACCESS,
				.dstStageMask = DST_STAGE,
				.dstAccessMask = DST_ACCESS,
				.oldLayout = OLD_LAYOUT,
				.newLayout = NEW_LAYOUT,
				.srcQueueFamilyIndex = GRAPHICS_QF_INDEX,
				.dstQueueFamilyIndex = GRAPHICS_QF_INDEX,
				.image = image,
				.subresourceRange = SUBRESOURCE_RANGE,
			};

			VkDependencyInfo dependencyInfo{
				.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
				.imageMemoryBarrierCount = 1,
				.pImageMemoryBarriers = &imageBarrier,
			};

			vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
		}
	}

	namespace Resources {
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
					tinygltf::Accessor const* pTEXCOORD_ACCESSOR = nullptr;
					tinygltf::BufferView const* pTEXCOORD_BUFFER_VIEW = nullptr;
					tinygltf::Buffer const* pTEXCOORD_BUFFER = nullptr;

					if (HAS_TEXCOORDS) {
						pTEXCOORD_ACCESSOR = &model.accessors[PRIMITIVE.attributes.at("TEXCOORD_0")];
						pTEXCOORD_BUFFER_VIEW = &model.bufferViews[pTEXCOORD_ACCESSOR->bufferView];
						pTEXCOORD_BUFFER = &model.buffers[pTEXCOORD_BUFFER_VIEW->buffer];
					}

					// Process vertices
					for (size_t i = 0; i < POSITION_ACCESSOR.count; i++) {
						Vertex::Vertex vertex{};

						// Get position
						float const* pPOSITION = reinterpret_cast<float const*>(&POSITION_BUFFER.data[POSITION_BUFFER_VIEW.byteOffset + POSITION_ACCESSOR.byteOffset + i * 12]);
						vertex.position = {pPOSITION[0], pPOSITION[1], pPOSITION[2], 1.0f};

						// Get texture coordinates if available
						if (HAS_TEXCOORDS) {
							float const* pTEXCOORD = reinterpret_cast<float const*>(&pTEXCOORD_BUFFER->data[pTEXCOORD_BUFFER_VIEW->byteOffset + pTEXCOORD_ACCESSOR->byteOffset + i * 8]);
							vertex.texCoord = {pTEXCOORD[0], pTEXCOORD[1]};
						} else {
							vertex.texCoord = {0.0f, 0.0f};
						}

						// Add vertex if unique
						if (!uniqueVertices.contains(vertex)) {
							uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
							vertices.push_back(vertex);
						}
					}

					// Process indices
					unsigned char const* pINDEX_DATA = &INDEX_BUFFER.data[INDEX_BUFFER_VIEW.byteOffset + INDEX_ACCESSOR.byteOffset];

					// Handle different index component types
					if (INDEX_ACCESSOR.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
						uint16_t const* pUI16 = reinterpret_cast<const uint16_t*>(pINDEX_DATA);
						for (size_t i = 0; i < INDEX_ACCESSOR.count; i++) {
							Vertex::Vertex vertex = vertices[pUI16[i]];
							indices.push_back(uniqueVertices[vertex]);
						}
					} else if (INDEX_ACCESSOR.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
						uint32_t const* pUI32 = reinterpret_cast<const uint32_t*>(pINDEX_DATA);
						for (size_t i = 0; i < INDEX_ACCESSOR.count; i++) {
							Vertex::Vertex vertex = vertices[pUI32[i]];
							indices.push_back(uniqueVertices[vertex]);
						}
					} else if (INDEX_ACCESSOR.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
						uint8_t const* pUI8 = reinterpret_cast<const uint8_t*>(pINDEX_DATA);
						for (size_t i = 0; i < INDEX_ACCESSOR.count; i++) {
							Vertex::Vertex vertex = vertices[pUI8[i]];
							indices.push_back(uniqueVertices[vertex]);
						}
					}
				}
			}
		}

		ktxTexture2* loadKtxImage(const char* const& PATH) {
			ktxTexture2* pKtxTexture{};
			ktx_error_code_e error{};

			if((error = ktxTexture_CreateFromNamedFile(PATH, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, reinterpret_cast<ktxTexture**>(&pKtxTexture))) != KTX_SUCCESS) {
				throw std::runtime_error("Failed to load ktx texture from " + std::string(PATH) + ". Error code is " + std::to_string(error));
			}

			if(ktxTexture2_NeedsTranscoding(pKtxTexture)) {
				constexpr ktx_transcode_fmt_e TARGET_FORMAT = KTX_TTF_BC7_RGBA;

				if(ktxTexture2_TranscodeBasis(pKtxTexture, TARGET_FORMAT, 0) != KTX_SUCCESS) {
					throw std::runtime_error("Failed to transcode ktx texture to ktx_transcode_fmt " + std::to_string(TARGET_FORMAT));
				}
			}

			return pKtxTexture;
		}
	}

	namespace Window {
		std::vector<const char*> getVkWindowExtensions() {
			glfwInit();

			uint32_t requiredCount{};
			const char** required = glfwGetRequiredInstanceExtensions(&requiredCount);
			std::vector<const char*> requiredVector{};

			#ifdef __APPLE__
			if (!required) {
				requiredVector.push_back("VK_KHR_surface");
				requiredVector.push_back("VK_EXT_metal_surface");
			}
			#endif
			for (int i = 0; i < requiredCount; i++) {
				requiredVector.push_back(required[i]);
			}

			return requiredVector;
		}
	}

	namespace Memory {
		VkImageView createImageView(VkImageViewCreateInfo const& IMAGE_VIEW_INFO) {
			VkImageView view{};
		
			CHECK_VK_SUCCESS(vkCreateImageView(gDevice, &IMAGE_VIEW_INFO, nullptr, &view), "Failed to create image view")
	
			return view;
		}

		VkDeviceSize alignNextHighest(VkDeviceSize const& N, VkDeviceSize const& ALIGNMENT) {
			return (N + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
		}

		VkDeviceSize alignNextLowest(VkDeviceSize const& N, VkDeviceSize const& ALIGNMENT) {
			return N & ~(ALIGNMENT - 1);
		}

		std::pair<VkDeviceSize, std::vector<VkDeviceSize>> doMemoryCalculations(std::vector<VkMemoryRequirements> const& ITEM_REQUIREMENTS, std::vector<ItemType> const& TYPES, VkDeviceSize const& BI_GRANULARITY) {
			assert(ITEM_REQUIREMENTS.size() == TYPES.size());
			std::vector<VkDeviceSize> beginnings{};

			VkDeviceSize beginningByte = 0;
			VkDeviceSize endingByte = 0;
			VkDeviceSize nextOpenSpace = 0;

			for(int i = 0; i < ITEM_REQUIREMENTS.size(); i++) {
				beginningByte = alignNextHighest(beginningByte, ITEM_REQUIREMENTS[i].alignment);

				if(i > 0) {
					bool linearFollowedByNonLinear = TYPES[i] == ItemType::LINEAR && TYPES[i - 1] == ItemType::NON_LINEAR;
					bool nonLinearFollowedByLinear = TYPES[i] == ItemType::NON_LINEAR && TYPES[i - 1] == ItemType::LINEAR;

					if(linearFollowedByNonLinear || nonLinearFollowedByLinear) {
						nextOpenSpace = alignNextHighest(endingByte, BI_GRANULARITY);

						if(beginningByte < nextOpenSpace) {
							beginningByte = nextOpenSpace;
						}
					}
				}

				beginnings.push_back(beginningByte);
				beginningByte += ITEM_REQUIREMENTS[i].size;
				endingByte = beginningByte - 1;
			}

			return { beginningByte, beginnings };
		}

		uint32_t getMemoryTypeIndex(std::vector<VkMemoryRequirements> const& REQUIREMENTS, VkMemoryPropertyFlags const& WANTED_PROPERTIES) {
			uint32_t suitableMemoryMask = UINT32_MAX;
			for (VkMemoryRequirements const& REQ : REQUIREMENTS) {
				suitableMemoryMask &= REQ.memoryTypeBits;
			}

			VkPhysicalDeviceMemoryProperties memoryProperties{};
			vkGetPhysicalDeviceMemoryProperties(Backend::PhysicalDevice::gPhysicalDevice, &memoryProperties);

			uint32_t suitableMemoryTypeIndex = UINT32_MAX;
			bool suitableMemoryCondition = false;
			for (int i = 0; i < memoryProperties.memoryTypeCount && suitableMemoryTypeIndex == UINT32_MAX; i++) {
				suitableMemoryCondition = (suitableMemoryMask & suitableMemoryMask << i) && (memoryProperties.memoryTypes[i].propertyFlags & WANTED_PROPERTIES);

				if(suitableMemoryCondition) {
					suitableMemoryTypeIndex = i;
				}
			};

			return suitableMemoryTypeIndex;
		}
	}

	namespace FrameData {
		VkCommandPool createCmdPool(VkCommandPoolCreateFlags const& FLAGS, uint32_t const& QF_INDEX) {
			VkCommandPool cmdPool{};

			VkCommandPoolCreateInfo cmdPoolCreate{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = FLAGS,
				.queueFamilyIndex = QF_INDEX
			};

			CHECK_VK_SUCCESS(vkCreateCommandPool(gDevice, &cmdPoolCreate, nullptr, &cmdPool), "Failed to create command pool")

			return cmdPool;
		}

		VkCommandBuffer createCmdBuffer(VkCommandPool cmdPool, VkCommandBufferLevel const& LEVEL) {
			VkCommandBuffer cmdBuffer{};

			VkCommandBufferAllocateInfo cmdBufferCreate{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = cmdPool,
				.level = LEVEL,
				.commandBufferCount = 1
			};

			CHECK_VK_SUCCESS(vkAllocateCommandBuffers(gDevice, &cmdBufferCreate, &cmdBuffer), "Failed to create command buffer")

			return cmdBuffer;
		}

		VkFence createFence(VkFenceCreateFlags const& FLAGS) {
			VkFence fence{};

			VkFenceCreateInfo fenceCreate{
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = FLAGS
			};

			CHECK_VK_SUCCESS(vkCreateFence(gDevice, &fenceCreate, nullptr, &fence), "Failed to create fence")

			return fence;
		}

		VkSemaphore createSemaphore(VkSemaphoreTypeCreateInfo const& TYPE) {
			VkSemaphore semaphore{};

			VkSemaphoreCreateInfo semaphoreCreate{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = &TYPE
			};

			CHECK_VK_SUCCESS(vkCreateSemaphore(gDevice, &semaphoreCreate, nullptr, &semaphore), "Failed to create semaphore")

			return semaphore;
		}
	}
}