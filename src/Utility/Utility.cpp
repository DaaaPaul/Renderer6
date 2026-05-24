#include <ktx.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <fstream>
#include <random>	
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "Utility/Utility.h"
#include "Geometry/Vertex.hpp"
#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"

namespace Utility {
	std::vector<std::string> to_string(const std::vector<const char*>& c_strings) {
		std::vector<std::string> strings;

		for(const char* c_string : c_strings) {
			strings.emplace_back(c_string);
		}

		return strings;
	}

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these) {
		if(contain_these.empty()) {
			return true;
		} else {
			std::sort(these_strings.begin(), these_strings.end());
			std::sort(contain_these.begin(), contain_these.end());

			return std::ranges::includes(these_strings, contain_these);
		}
	}

	float random() {
		static std::default_random_engine random_engine(static_cast<unsigned>(time(nullptr)));
		static std::uniform_real_distribution range(0.0f, 1.0f);

		return range(random_engine);
	}

	void load_gltf_model(const char* file_path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
		tinygltf::Model model{};
		tinygltf::TinyGLTF loader{};
		std::string error{};
		std::string warning{};

		if(!loader.LoadASCIIFromFile(&model, &error, &warning, file_path)) {
			throw std::runtime_error("load_gltf_model failed: " + error);
		}
		if(!warning.empty()) {
			PRINTLN("load_gltf_model warning: " << warning);
		}

		// process all meshes in the model
		std::unordered_map<Vertex, uint32_t> unique_vertices{};

		for (const tinygltf::Mesh& mesh : model.meshes) {
			for (const tinygltf::Primitive& primitive : mesh.primitives) {
				// get indices
				const tinygltf::Accessor& index_accessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& index_buffer_view = model.bufferViews[index_accessor.bufferView];
				const tinygltf::Buffer& index_buffer = model.buffers[index_buffer_view.buffer];

				// get vertex positions
				const tinygltf::Accessor& position_accessor = model.accessors[primitive.attributes.at("POSITION")];
				const tinygltf::BufferView& position_buffer_view = model.bufferViews[position_accessor.bufferView];
				const tinygltf::Buffer& position_buffer = model.buffers[position_buffer_view.buffer];

				// get texture coordinates if available
				bool has_tex_coords = primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end();
				const tinygltf::Accessor* p_texcoord_accessor = nullptr;
				const tinygltf::BufferView* p_texcoord_buffer_view = nullptr;
				const tinygltf::Buffer* p_texcoord_buffer = nullptr;

				if (has_tex_coords) {
					p_texcoord_accessor = &model.accessors[primitive.attributes.at("TEXCOORD_0")];
					p_texcoord_buffer_view = &model.bufferViews[p_texcoord_accessor->bufferView];
					p_texcoord_buffer = &model.buffers[p_texcoord_buffer_view->buffer];
				}

				// process vertices
				for (int i = 0; i < position_accessor.count; ++i) {
					Vertex vertex{};

					// get position
					const float* p_position = reinterpret_cast<const float*>(&position_buffer.data[position_buffer_view.byteOffset + position_accessor.byteOffset + i * 12]);
					vertex.position = {p_position[0], p_position[1], p_position[2], 1.0f};

					// get texture coordinates if available
					if (has_tex_coords) {
						const float* p_texcoord = reinterpret_cast<const float*>(&p_texcoord_buffer->data[p_texcoord_buffer_view->byteOffset + p_texcoord_accessor->byteOffset + i * 8]);
						vertex.tex_coord = {p_texcoord[0], p_texcoord[1]};
					} else {
						vertex.tex_coord = {0.0f, 0.0f};
					}

					// add vertex if unique
					if (!unique_vertices.contains(vertex)) {
						unique_vertices[vertex] = UINT32(vertices.size());
						vertices.push_back(vertex);
					}
				}

				// process indices
				const unsigned char* p_index_data = &index_buffer.data[index_buffer_view.byteOffset + index_accessor.byteOffset];

				// handle different index component types
				if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
					const uint16_t* p_indices = reinterpret_cast<const uint16_t*>(p_index_data);
					for (size_t i = 0; i < index_accessor.count; ++i) {
						indices.push_back(unique_vertices[vertices[p_indices[i]]]);
					}
				} else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
					const uint32_t* p_indices = reinterpret_cast<const uint32_t*>(p_index_data);
					for (size_t i = 0; i < index_accessor.count; ++i) {
						indices.push_back(unique_vertices[vertices[p_indices[i]]]);
					}
				} else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
					const uint8_t* p_indices = reinterpret_cast<const uint8_t*>(p_index_data);
					for (size_t i = 0; i < index_accessor.count; ++i) {
						indices.push_back(unique_vertices[vertices[p_indices[i]]]);
					}
				}
			}
		}
	}

	ktxTexture2* load_ktx_texture(const char* ktx_path) {
		ktxTexture2* p_ktx_texture{};
		KTX_error_code error = ktxTexture2_CreateFromNamedFile(ktx_path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &p_ktx_texture);

		if(error != KTX_SUCCESS) {
			throw std::runtime_error("load_ktx_texture: load failure");
		} else {
			if(ktxTexture2_NeedsTranscoding(p_ktx_texture)) {
				constexpr ktx_transcode_fmt_e TARGET_FORMAT = KTX_TTF_BC7_RGBA;

				if(ktxTexture2_TranscodeBasis(p_ktx_texture, TARGET_FORMAT, 0) != KTX_SUCCESS) {
					throw std::runtime_error("load_ktx_texture: compress failure");
				}
			}
		}

		return p_ktx_texture;
	}

	std::vector<char> get_file_bytes(const std::string& file_path) {
		std::ifstream file_stream_in(file_path, std::ios::binary | std::ios::ate);

		if(!file_stream_in.good()) {
			throw std::runtime_error("get_file_bytes: Failure reading file at " + file_path);
		}

		uint32_t file_size = file_stream_in.tellg();
		std::vector<char> file_bytes(file_size);

		file_stream_in.seekg(0);

		file_stream_in.read(file_bytes.data(), file_size);

		return file_bytes;
	}

	namespace Memory {
		VkImageView create_image_view(VkImageViewCreateInfo const& IMAGE_VIEW_INFO) {
			VkImageView view{};
		
			VK_CHECK(vkCreateImageView(g_device, &IMAGE_VIEW_INFO, nullptr, &view), "Failed to create image view")
	
			return view;
		}

		VkDeviceSize alignNextHighest(VkDeviceSize const& N, VkDeviceSize const& ALIGNMENT) {
			return (N + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
		}

		std::pair<VkDeviceSize, std::vector<VkDeviceSize>> doMemoryCalculations(std::vector<VkMemoryRequirements> const& ITEM_REQUIREMENTS, std::vector<ItemType> const& TYPES, VkDeviceSize const& BI_GRANULARITY) {
			assert(ITEM_REQUIREMENTS.size() == TYPES.size());
			std::vector<VkDeviceSize> beginnings{};

			VkDeviceSize beginningByte = 0;
			VkDeviceSize endingByte = 0;
			VkDeviceSize nextOpenSpace = 0;

			for(int i = 0; i < ITEM_REQUIREMENTS.size(); ++i) {
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
			vkGetPhysicalDeviceMemoryProperties(PhysicalDevice::g_physical_device, &memoryProperties);

			uint32_t suitableMemoryTypeIndex = UINT32_MAX;
			bool suitableMemoryCondition = false;
			for (int i = 0; i < memoryProperties.memoryTypeCount && suitableMemoryTypeIndex == UINT32_MAX; ++i) {
				suitableMemoryCondition = (suitableMemoryMask & suitableMemoryMask << i) && (memoryProperties.memoryTypes[i].propertyFlags & WANTED_PROPERTIES);

				if(suitableMemoryCondition) {
					suitableMemoryTypeIndex = i;
				}
			};

			return suitableMemoryTypeIndex;
		}
	}
}