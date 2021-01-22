#pragma once

#include "graphics/opengl/opengl_graphics.h"

#include "events/event.h"
#include "events/keyboard_event.h"
#include "events/mouse_event.h"
#include "events/window_event.h"

#include "layer/layer_stack.h"

#include "graphics/renderer.h"

#include "imgui/imgui_layer.h"

#include "logger.h"


namespace cotwin
{
	class Game
	{
	protected:
		// Game will stop update loop, execute on_destroy() and clean up when running is set to false
		bool running = false;

	private:
#ifdef CW_GRAPHICS_SDL2
		SDLGraphics graphics;
#elif defined CW_GRAPHICS_OPENGL
		OpenGLGraphics graphics;
#endif
		
		LayerStack layer_stack;
		
		Vector4f clear_color;
		double delta_cap = 0.0;

		unsigned int vertex_array, vertex_buffer, index_buffer;

	
	public:
		Game(WindowProperties window_properties)
		{
			running = init(window_properties);
		}

		virtual ~Game() = default;

		void start()
		{
			on_init();
			
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
				graphics.clear_screen(&clear_color);
				
				// update and render for each layer from the bottom to the top
				for (Layer* layer : layer_stack)
					layer->on_update(accumulated_delta);

				// update screen with rendering
				graphics.present();

				//accumulated_delta = 0.0;
				accumulated_delta -= delta_cap;
			}

			stop();
		}

		void stop()
		{
			on_destroy();
			
			graphics.destroy();
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

		void set_render_clear_color(Vector4f s_clear_color)
		{
			clear_color = s_clear_color;
		}
		
		void set_render_clear_color(Vector4u8 s_clear_color)
		{
			clear_color = { (float)s_clear_color.r / 255.0f, (float)s_clear_color.g / 255.0f, (float)s_clear_color.b / 255.0f, (float)s_clear_color.a / 255.0f };
		}

		Graphics* get_graphics()
		{
			return &graphics;
		}

		Renderer get_renderer()
		{
			return Renderer();
		}

	private:
		bool init(WindowProperties window_properties)
		{
			// set render clear color to black by default
			Vector4u8 black_color = { 0, 0, 0, 0 };
			set_render_clear_color(black_color);

			return graphics.init(&window_properties);
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