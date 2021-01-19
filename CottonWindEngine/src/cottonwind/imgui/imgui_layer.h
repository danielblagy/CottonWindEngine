#include <glad/glad.h>

#include "../vendor/imgui_sdl2_opengl3/imgui.h"
#include "../vendor/imgui_sdl2_opengl3/imgui_impl_sdl.h"
#include "../vendor/imgui_sdl2_opengl3/imgui_impl_opengl3.h"

#include <SDL.h>

#include "../layer/layer.h"

#include "../events/mouse_event.h"
#include "../events/keyboard_event.h"


namespace cotwin
{
	// A Special debug ImGui layer providede by the engine
	class ImGuiLayer : public Layer
	{
	private:
		SDL_Window* window;
		SDL_GLContext gl_context;
		const char* glsl_version;

		// TODO : Can io be initialized in constructor ??
		ImGuiIO* io;
	
	public:
		ImGuiLayer(SDL_Window* s_window, SDL_GLContext s_gl_context, const char* s_glsl_version)
			: Layer("imgui debug"), window(s_window), gl_context(s_gl_context), glsl_version(s_glsl_version)
		{
			
		}
		
		void on_attach() override
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();
			
			// Setup Platform/Renderer bindings
			ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
			ImGui_ImplOpenGL3_Init(glsl_version);
		}

		void on_detach() override
		{
			// imgui cleanup
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
		}

		void new_frame()
		{
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window);
			ImGui::NewFrame();
		}

		void render_frame()
		{
			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		void on_event(Event* event) override
		{
			//event->processed = ImGui_ImplSDL2_ProcessEvent(sdl_event);

			// MOVE TO TESTLAYER
			/*
			// basically do what ImGui_ImplSDL2_ProcessEvent does
			ImGuiIO& io = ImGui::GetIO();
			switch (event->type)
			{
			case MouseWheel:
			{
				Vector2 wheel = dynamic_cast<MouseWheelEvent*>(event)->wheel_scroll;
				if (wheel.x > 0) io.MouseWheelH += 1;
				if (wheel.x < 0) io.MouseWheelH -= 1;
				if (wheel.y > 0) io.MouseWheel += 1;
				if (wheel.y < 0) io.MouseWheel -= 1;
				event->processed = true;
			} break;
			case MouseButtonPress:
			{
				unsigned int button = dynamic_cast<MouseButtonEvent*>(event)->button_code;
				if (button == SDL_BUTTON_LEFT) set_g_mouse_pressed_state(0, true);
				if (button == SDL_BUTTON_RIGHT) set_g_mouse_pressed_state(1, true);
				if (button == SDL_BUTTON_MIDDLE) set_g_mouse_pressed_state(2, true);
				event->processed = true;
			} break;
			case TextInput:
			{
				const char* text = dynamic_cast<KeyboardTextInputEvent*>(event)->text;
				io.AddInputCharactersUTF8(text);
				event->processed = true;
			} break;
			case KeyPress:
			case KeyRelease:
			{
				KeyboardKeyEvent* keyboard_event = dynamic_cast<KeyboardKeyEvent*>(event);
				int key = keyboard_event->keycode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event->type == KeyPress);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
				io.KeySuper = false;
#else
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
				event->processed = true;
			} break;
			}
			*/
		}
	};
}