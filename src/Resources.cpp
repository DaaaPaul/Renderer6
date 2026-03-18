#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include <iostream>
#include "Resources.h"
#include "Window.h"

namespace Resources {
	void init() {
		loadModelVertices();
		loadKtxTexture2();
		loadParticlesData();
	}

	void loadModelVertices() {
		Util::loadGltfModel(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", gModelVertices, gModelVertexIndices);
		gModelVertexBufferSize = gModelVertices.size() * sizeof(Vertex::Vertex); /* (aa) */
		gModelIndexBufferSize = gModelVertexIndices.size() * sizeof(uint32_t); /* (ab) */
	}

	void loadKtxTexture2() {
		gpTexture = Util::loadKtxImage(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)");
	}

	void loadParticlesData() {
		float ratio = static_cast<float>(Backend::Window::gWINDOW_HEIGHT) / Backend::Window::gWINDOW_WIDTH;
		float r{}, theta{}, x{}, y{};

		for(Particle::Particle& p : gParticles) {
			r = sqrtf(::Util::random());
			theta = 2.0f * DA_PI * ::Util::random();

			x = r * cosf(theta) * ratio;
			y = r * sinf(theta);

			p.color = glm::vec4(::Util::random(), ::Util::random(), ::Util::random(), 1.0f);
			p.position = glm::vec2(x, y);
			p.velocity = normalize(p.position);
		}
	}

	namespace Util {
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

			std::cout << "Model at " << PATH << " unique vertices : " << vertices.size() << "\n";
			std::cout << "Model at " << PATH << " total vertices : " << indices.size() << "\n";
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
}