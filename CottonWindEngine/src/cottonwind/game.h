#pragma once

#include "events/event.h"
#include "events/keyboard_event.h"
#include "events/mouse_event.h"
#include "events/window_event.h"

#include "layer/layer_stack.h"

#include "util/logger.h"

#include "graphics/graphics.h"
#include "graphics/renderer.h"
#include "graphics/texture.h"


namespace cotwin
{
	class Game
	{
	protected:
		// Game will stop update loop, execute on_destroy() and clean up when running is set to false
		bool running = false;

	private:
// TODO : Simply use generic Graphics class with new ...() for graphics instance, since it will only be created once
#ifdef CW_MODERN_OPENGL
		OpenGLGraphics graphics;
#else
		SDLGraphics graphics;
#endif
	
		LayerStack layer_stack;
		
		glm::vec4 clear_color;
		float delta_cap = 0.0;

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
			float accumulated_delta = 0.0;
			Uint32 time_passed = 0;
			int fps_count = 0;

			while (running)
			{
				Uint32 current_time = SDL_GetTicks();
				Uint32 ms_passed = current_time - last_time;
				last_time = current_time;

				// time delta in seconds
				float current_frame_delta = ms_passed / 1000.0f;

				accumulated_delta += current_frame_delta;

				time_passed += ms_passed;

				if (delta_cap > accumulated_delta)
				{
					continue;
				}

				if (time_passed >= 1000)
				{
					Logger::Debug("fps: %d", fps_count);
					fps_count = 0;
					time_passed -= 1000;
				}

				fps_count++;
				
				handle_sdl_events();

				// TODO : have a global WindowData struct which is updated when window events come up
				//										(either handle that in the engine, or require it from the user)
				
				// clear screen
				Renderer2D::clear(clear_color);
				
				// update and render for each layer from the bottom to the top
				for (Layer* layer : layer_stack)
					layer->on_update(accumulated_delta);

				// update screen with rendering
				Renderer2D::flush();

				if (delta_cap)
				{
					accumulated_delta -= delta_cap;
				}
				else
				{
					accumulated_delta = 0.0f;
				}
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
			graphics.enable_vsync(&delta_cap);
		}

		void disable_vsync()
		{
			graphics.disable_vsync(&delta_cap);
		}
		
		// uncapped by default (if vsync is off)
		void set_delta_cap(float delta_in_seconds)
		{
			delta_cap = delta_in_seconds;
		}

		// uncapped by default (if vsync is off)
		void set_fps_cap(unsigned int fps)
		{
			delta_cap = 1.0f / fps;
		}

		void set_render_clear_color(glm::vec4 s_clear_color)
		{
			clear_color = s_clear_color;
		}
		
		void set_render_clear_color(glm::u8vec4 s_clear_color)
		{
			clear_color = { (float)s_clear_color.r / 255.0f, (float)s_clear_color.g / 255.0f, (float)s_clear_color.b / 255.0f, (float)s_clear_color.a / 255.0f };
		}

		Graphics* get_graphics()
		{
			return &graphics;
		}

	private:
		bool init(WindowProperties window_properties)
		{
			// set render clear color to black by default
			glm::vec4 black_color = { 0.0f, 0.0f, 0.0f, 0.0f };
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
						KeyboardEvent event(type, e.key.keysym.scancode, SDL_GetScancodeName(e.key.keysym.scancode), e.key.repeat);
						on_event(&event);
					}
					else if (type == TextInput)
					{
						KeyboardEvent event(e.text.text);
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
						MouseEvent event({ e.motion.x, e.motion.y }, { e.motion.xrel, e.motion.yrel });
						on_event(&event);
					} break;
					case SDL_MOUSEBUTTONDOWN: {
						MouseEvent event(MouseButtonPress, { e.motion.x, e.motion.y }, e.button.button, e.button.clicks == 2);
						on_event(&event);
					} break;
					case SDL_MOUSEBUTTONUP: {
						MouseEvent event(MouseButtonRelease, { e.motion.x, e.motion.y }, e.button.button, e.button.clicks == 2);
						on_event(&event);
					} break;
					case SDL_MOUSEWHEEL: {
						MouseEvent event({ e.wheel.x, e.wheel.y });
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
						WindowEvent event(WindowClose);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MINIMIZED: {
						WindowEvent event(WindowMinimize);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MAXIMIZED: {
						WindowEvent event(WindowMaximize);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_FOCUS_GAINED: {
						WindowEvent event(WindowFocusGained);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_FOCUS_LOST: {
						WindowEvent event(WindowFocusLost);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MOVED: {
						WindowEvent event(WindowMove, { (int)e.window.data1, (int)e.window.data2 });
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_RESIZED: {
						WindowEvent event(WindowResize, { (int)e.window.data1, (int)e.window.data2 });
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