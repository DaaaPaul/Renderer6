#include "UniformBufferBlock.hpp"

UniformBufferBlock::UniformBufferBlock(const CameraComponent& camera) :
	model(1.0f), 
	view(CameraComponent::to_view_matrix(camera)),
	projection(CameraComponent::to_projection_matrix(camera)), 
	light_positions{ glm::vec4(0.0f, 0.0f, 5.0f, 1.0f), glm::vec4(0.0f, 0.0f, -5.0f, 1.0f), glm::vec4(5.0f, 0.0f, 0.0f, 1.0f), glm::vec4(-5.0f, 0.0f, 0.0f, 1.0f) },
	light_colors{ glm::vec4(300.0f, 300.0f, 300.0f, 1.0f), glm::vec4(300.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 300.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 300.0f, 1.0f) },
	camera_pos(glm::vec4(camera.get_position(), 1.0f)),
	exposure{ 4.5f },
	gamma{ 2.2f },
	prefilteredCubeMipLevels{ 1.0f },
	scaleIBLAmbient{ 1.0f } {

}