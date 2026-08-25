#include "Gui.h"

namespace Gui {
	void set_io_context(glm::vec2 display_size, ImGuiConfigFlags config_flags, ImGuiBackendFlags backend_flags) {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = display_size.x;
		io.DisplaySize.y = display_size.y;
		io.ConfigFlags = config_flags;
		io.BackendFlags = backend_flags;
	}
}