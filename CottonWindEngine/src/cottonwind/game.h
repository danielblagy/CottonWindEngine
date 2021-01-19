#pragma once

#include <cstring>

#include <SDL.h>

#include "events/event.h"
#include "events/keyboard_event.h"
#include "events/mouse_event.h"
#include "events/window_event.h"

#include "layer/layer_stack.h"

#include "render/renderer.h"

#include <glad/glad.h>          // Initialize with gladLoadGL()

#include "imgui/imgui_layer.h"


namespace cotwin
{
	enum WindowPropertiesFlags
	{
		None = 0,
		Fullscreen = 1 << 0,
		Centered = 1 << 1,
		Resizable = 1 << 2,
		Borderless = 1 << 3
	};
	
	struct WindowProperties
	{
		const char* title;
		int left, top, width, height;
		WindowPropertiesFlags flags;

		WindowProperties(const char* s_title, int s_left, int s_top, int s_width, int s_height, unsigned int s_flags)
			: title(s_title), left(s_left), top(s_top), width(s_width), height(s_height), flags(static_cast<WindowPropertiesFlags>(s_flags))
		{}
	};
	
	class Game
	{
	protected:
		// Game will stop update loop, execute on_destroy() and clean up when running is set to false
		bool running = false;

	private:
		SDL_Window * window;
		SDL_GLContext gl_context;
		const char* glsl_version;
		
		LayerStack layer_stack;
		ImGuiLayer* imgui_layer;
		
		Vector4ui8 clear_color;
		double delta_cap = 0.0;

	
	public:
		Game(WindowProperties window_properties)
		{
			running = init(window_properties);
		}

		virtual ~Game() = default;

		void start()
		{
			on_init();

			imgui_layer = new ImGuiLayer(window, gl_context, glsl_version);
			attach_layer(imgui_layer);
			
			Uint32 last_time = SDL_GetTicks();
			double accumulated_delta = 0.0;

			while (running)
			{
				Uint32 current_time = SDL_GetTicks();
				Uint32 ms_passed = current_time - last_time;
				last_time = current_time;

				// time delta in seconds
				double current_frame_delta = ms_passed / 1000.0;

				accumulated_delta += current_frame_delta;

				if (delta_cap > accumulated_delta)
				{
					continue;
				}
				
				handle_sdl_events();

				// TODO : have a global WindowData struct which is updated when window events come up
				//										(either handle that in the engine, or require it from the user)
				// clear screen
				int window_w, window_h;
				SDL_GetWindowSize(window, &window_w, &window_h);
				glViewport(0, 0, window_w, window_h);
				glClearColor((float) clear_color.x / 255.0f, (float) clear_color.y / 255.0f, (float) clear_color.z / 255.0f, (float) clear_color.w / 255.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				
				// update and render for each layer from the bottom to the top
				for (Layer* layer : layer_stack)
					layer->on_update(accumulated_delta);

				// render imgui
				imgui_layer->new_frame();
				
				for (Layer* layer : layer_stack)
					layer->on_imgui_render();
				
				imgui_layer->render_frame();

				// update screen with rendering
				SDL_GL_SwapWindow(window);

				//accumulated_delta = 0.0;
				accumulated_delta -= delta_cap;
			}

			stop();
		}

		void stop()
		{
			on_destroy();
			
			SDL_GL_DeleteContext(gl_context);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void attach_layer(Layer* layer)
		{
			layer_stack.push_layer(layer);
			layer->on_attach();
		}

		virtual void on_init() = 0;
		virtual void on_destroy() = 0;

		bool is_running()
		{
			return running;
		}

		void enable_vsync()
		{
			delta_cap = 0.0;
			SDL_GL_SetSwapInterval(1);
		}

		void disable_vsync()
		{
			SDL_GL_SetSwapInterval(0);
		}
		
		// uncapped by default (if vsync is off)
		void set_delta_cap(double delta_in_seconds)
		{
			delta_cap = delta_in_seconds;
		}

		// uncapped by default (if vsync is off)
		void set_fps_cap(unsigned int fps)
		{
			delta_cap = 1.0 / fps;
		}

		void set_render_clear_color(Vector4ui8 s_clear_color)
		{
			clear_color = s_clear_color;
		}

		Renderer get_renderer()
		{
			return Renderer();
		}

	private:
		bool init(WindowProperties window_properties)
		{
			int sdl_init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

			if (sdl_init_result != 0)
			{
				SDL_Log("CottonWind: Unable to initialize SDL: %s", SDL_GetError());
				return false;
			}

			// TODO : mac support (opengl)

			// GL 3.0 + GLSL 130
			glsl_version = "#version 130";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

			// Create window with graphics context
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

			if (window_properties.flags & Centered)
			{
				window_properties.left = SDL_WINDOWPOS_CENTERED;
				window_properties.top = SDL_WINDOWPOS_CENTERED;
			}

			Uint32 window_flags = SDL_WINDOW_OPENGL;
			if (window_properties.flags & Fullscreen)
			{
				window_flags = window_flags | SDL_WINDOW_FULLSCREEN;
			}
			else
			{
				if (window_properties.flags & Resizable)
				{
					window_flags = window_flags | SDL_WINDOW_RESIZABLE;
				}

				if (window_properties.flags & Borderless)
				{
					window_flags = window_flags | SDL_WINDOW_BORDERLESS;
				}
			}

			window = SDL_CreateWindow(
				window_properties.title, window_properties.left, window_properties.top, window_properties.width, window_properties.height, window_flags
			);

			if (window == NULL) {
				SDL_Log("CottonWind: Could not create window: %s\n", SDL_GetError());
				return false;
			}

			gl_context = SDL_GL_CreateContext(window);
			SDL_GL_MakeCurrent(window, gl_context);
			// TODO : deal with vsync
			//SDL_GL_SetSwapInterval(1); // Enable vsync

			// set render clear color to black by default
			Vector4ui8 black_color = { 0, 0, 0, 0 };
			set_render_clear_color(black_color);

			// Initialize OpenGL loader
			if (gladLoadGL() == 0)
			{
				SDL_Log("CottonWind: Failed to initialize OpenGL loader!");
				return false;
			}

			// display opengl version
			int opengl_major_version, opengl_minor_version;
			SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_major_version);
			SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_minor_version);
			SDL_Log("CottonWind: OpenGL version: %d.%d", opengl_major_version, opengl_minor_version);

			SDL_Log("CottonWind: Game was successfully initialized");

			return true;
		}

		void on_event(Event* event)
		{
			// process event from the top to the bottom
			for (auto it = layer_stack.rbegin(); it	!= layer_stack.rend(); ++it)
			{
				if (event->processed)
					break;
				
				(*it)->on_event(event);
			}
		}

		void handle_sdl_events()
		{
			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				if (e.type >= SDL_KEYDOWN && e.type <= SDL_KEYMAPCHANGED)
				{
					EventType type;

					switch (e.type)
					{
					case SDL_KEYDOWN: {
						type = KeyPress;
					} break;
					case SDL_KEYUP: {
						type = KeyRelease;
					} break;
					case SDL_TEXTINPUT: {
						type = TextInput;
					} break;
					default:
						type = Unsupported;
					}

					if (type == KeyPress || type == KeyRelease)
					{
						KeyboardKeyEvent event(e.key.keysym.scancode, SDL_GetScancodeName(e.key.keysym.scancode), e.key.repeat);
						event.type = type;
						on_event(&event);
					}
					else if (type == TextInput)
					{
						KeyboardTextInputEvent event(e.text.text);
						event.type = type;
						on_event(&event);
					}
					else
					{
						Event event;
						event.category = EventCategoryNone;
						event.type = type;
						on_event(&event);
					}
				}
				else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MOUSEWHEEL)
				{
					switch (e.type)
					{
					case SDL_MOUSEMOTION: {
						MouseMoveEvent event({ e.motion.x, e.motion.y }, { e.motion.xrel, e.motion.yrel });
						on_event(&event);
					} break;
					case SDL_MOUSEBUTTONDOWN: {
						MouseButtonEvent event({ e.motion.x, e.motion.y }, e.button.button, e.button.clicks == 2);
						event.type = MouseButtonPress;
						on_event(&event);
					} break;
					case SDL_MOUSEBUTTONUP: {
						MouseButtonEvent event({ e.motion.x, e.motion.y }, e.button.button, e.button.clicks == 2);
						event.type = MouseButtonRelease;
						on_event(&event);
					} break;
					case SDL_MOUSEWHEEL: {
						MouseWheelEvent event({ e.wheel.x, e.wheel.y });
						on_event(&event);
					} break;
					default: {
						Event event;
						event.category = EventCategoryNone;
						event.type = Unsupported;
						on_event(&event);
					}
					}
				}
				else if (e.type == SDL_QUIT)
				{
					Event event;
					event.category = EventCategoryWindow;
					event.type = ApplicationQuit;
					on_event(&event);

					// stop the game
					running = false;
				}
				else if (e.type == SDL_WINDOWEVENT)
				{
					switch (e.window.event)
					{
					case SDL_WINDOWEVENT_CLOSE: {
						Event event;
						event.category = EventCategoryWindow;
						event.type = WindowClose;
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MINIMIZED: {
						Event event;
						event.category = EventCategoryWindow;
						event.type = WindowMinimize;
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MAXIMIZED: {
						Event event;
						event.category = EventCategoryWindow;
						event.type = WindowMaximize;
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_FOCUS_GAINED: {
						Event event;
						event.category = EventCategoryWindow;
						event.type = WindowFocusGained;
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_FOCUS_LOST: {
						Event event;
						event.category = EventCategoryWindow;
						event.type = WindowFocusLost;
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MOVED: {
						WindowMoveEvent event({ (int)e.window.data1, (int)e.window.data2 });
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_RESIZED: {
						WindowResizeEvent event({ (int)e.window.data1, (int)e.window.data2 });
						on_event(&event);
					} break;
					default: {
						Event event;
						event.category = EventCategoryNone;
						event.type = Unsupported;
						on_event(&event);
					}
					}
				}
			}
		}
	};
}