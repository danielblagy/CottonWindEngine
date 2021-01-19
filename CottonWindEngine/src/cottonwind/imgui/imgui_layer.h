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
	class ImGuiDebugLayer : public Layer
	{
	private:
		SDL_Window* window;
		SDL_GLContext gl_context;
		const char* glsl_version;

		// TODO : Can io be initialized in constructor ??
		ImGuiIO* io;
		
		// gui state
		ImVec4 clear_color;
		bool show_demo_window;
		bool show_another_window;
	
	public:
		ImGuiDebugLayer(SDL_Window* s_window, SDL_GLContext s_gl_context, const char* s_glsl_version)
			: Layer("imgui debug"), window(s_window), gl_context(s_gl_context), glsl_version(s_glsl_version)
		{
			
		}

		~ImGuiDebugLayer()
		{
			destroy_imgui();
		}
		
		void on_attach() override
		{
			clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			show_demo_window = true;
			show_another_window = false;
			
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io_ref = ImGui::GetIO(); (void)io_ref;
			io = &io_ref;
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
			destroy_imgui();
		}

		void on_update(double delta) override
		{
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window);
			ImGui::NewFrame();

			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			// 3. Show another simple window.
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}

			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		// basically does what ImGui_ImplSDL2_ProcessEvent does
		void on_event(Event* event)
		{
			//event->processed = ImGui_ImplSDL2_ProcessEvent(sdl_event);

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
		}

	private:
		void destroy_imgui()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
		}
	};
}