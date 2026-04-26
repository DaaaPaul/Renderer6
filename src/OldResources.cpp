#include <cstdint>
#include <cmath>
#include "Vertex.hpp"
#include "OldResources.h"
#include "Util.h"
#include "Window.h"
#include "Particle.hpp"

namespace Resources {
	void loadModel() {
		loadModelVertices();
		loadKtxTexture2();
	}

	void loadParticles() {
		loadParticlesData();
	}
	
	void loadModelVertices() {
		Util::Resources::loadGltfModel(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", gModelVertices, gModelIndices);
		gModelVertexBufferSize = gModelVertices.size() * sizeof(Vertex::Vertex); /* (aa) */
		gModelIndexBufferSize = gModelIndices.size() * sizeof(uint32_t); /* (ab) */
	}

	void loadKtxTexture2() {
		gTexture = Util::Resources::loadKtxImage(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)");
	}

	void loadParticlesData() {
		float ratio = Backend::Window::gAspectRatio;
		float r{}, theta{}, x{}, y{};

		for(Particle::Particle& p : gParticles) {
			r = sqrtf(Util::random());
			theta = 2.0f * DA_PI * Util::random();

			x = r * cosf(theta) * ratio;
			y = r * sinf(theta);

			p.color = glm::vec4(Util::random(), Util::random(), Util::random(), 1.0f);
			p.position = glm::vec2(x, y);
			p.velocity = normalize(p.position);
		}
	}
}