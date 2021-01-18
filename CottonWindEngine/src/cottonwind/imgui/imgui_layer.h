#include "../vendor/glad_v3.2_core/include/glad/glad.h"

#include "../vendor/imgui_sdl2_opengl3/imgui.h"
#include "../vendor/imgui_sdl2_opengl3/imgui_impl_sdl.h"
#include "../vendor/imgui_sdl2_opengl3/imgui_impl_opengl3.h"

#include <SDL.h>

#include "../layer/layer.h"


namespace cotwin
{
	// A Special debug ImGui layer providede by the engine
	class ImGuiLayer : public Layer
	{
	public:
		void on_attach() override
		{
			IMGUI_CHECKVERSION();
		}

		void on_detach() override
		{}

		void on_update(double delta) override
		{}

		void on_event(Event* event) override
		{}
	};
}