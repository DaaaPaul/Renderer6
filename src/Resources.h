#pragma once

#include <ktx.h>
#include <cstdint>
#include <vector>
#include "Particle.hpp"
#include "Vertex.hpp"

namespace Resources {
	inline std::vector<Vertex::Vertex> gModelVertices{};
	inline uint32_t gModelVertexBufferSize = UINT32_MAX; /* (aa) */
	inline std::vector<uint32_t> gModelIndices{};
	inline uint32_t gModelIndexBufferSize = UINT32_MAX; /* (ab) */

	inline ktxTexture2* gTexture{};

	inline constexpr uint32_t gPARTICLES_COUNT = 256;
	inline constexpr uint32_t gPARTICLES_BUFFER_SIZE = gPARTICLES_COUNT * sizeof(Particle::Particle);
	inline std::vector<Particle::Particle> gParticles(gPARTICLES_COUNT, {});

	void load();

	void loadModelVertices();
	void loadKtxTexture2();
	void loadParticlesData();
}