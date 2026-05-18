#include <cstdint>
#include <cmath>
#include "Geometry/Vertex.hpp"
#include "OldResources.h"
#include "Utility/Utility.h"
#include "Backend/Window.h"

namespace Resources {
	void load_model() {
		Utility::load_gltf_model(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", g_model_vertices, g_model_indices);
		g_vertex_buffer_size = g_model_vertices.size() * sizeof(Vertex);
		g_index_buffer_size = g_model_indices.size() * sizeof(uint32_t);

		g_texture = Utility::load_ktx_image(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)");
	}
}