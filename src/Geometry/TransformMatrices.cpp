#include "Utility/Vulkan.h"
#include "TransformMatrices.hpp"

std::ostream& operator<<(std::ostream& os, TransformMatrices const& transform_matrices) {
	return os << 
		"Model Matrix:\n" << transform_matrices.model_matrix << "\n\n" <<
		"View Matrix:\n" << transform_matrices.view_matrix << "\n\n" <<
		"Projection Matrix:\n" << transform_matrices.projection_matrix;
}