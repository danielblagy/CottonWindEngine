#pragma once

#include <SDL.h>

#include "events/event.h"
#include "events/keyboard_event.h"
#include "events/mouse_event.h"
#include "events/window_event.h"

#include "layer/layer_stack.h"


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
		//bool fullscreen, centered, resizable, borderless;
		WindowPropertiesFlags flags;

		WindowProperties(const char* s_title, int s_left, int s_top, int s_width, int s_height, unsigned int s_flags)
			: title(s_title), left(s_left), top(s_top), width(s_width), height(s_height), flags(static_cast<WindowPropertiesFlags>(s_flags))
		{}
	};
	
	class Game
	{
	protected:
		SDL_Window* window;
		SDL_Renderer* renderer;

		// Game will stop update loop, execute on_destroy() and clean up when running is set to false
		bool running = false;
		// Renderer renderer;

		LayerStack layer_stack;

	private:
		// all delta time used in this class is in SECONDS
		double target_delta = 0.0;
	
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

				if (target_delta > accumulated_delta)
				{
					continue;
				}

				double fps = 1.0 / accumulated_delta;
				SDL_Log("CottonWind: delta %lf s (%lf fps)", accumulated_delta, fps);
				
				handle_sdl_events();

				// update and render for each layer from the bottom to the top
				for (Layer* layer : layer_stack)
					layer->on_update(accumulated_delta);

				SDL_RenderClear(renderer);
				SDL_RenderPresent(renderer);

				accumulated_delta = 0.0;
			}

			stop();
		}

		void stop()
		{
			on_destroy();
			
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		virtual void on_init() = 0;
		virtual void on_destroy() = 0;

		bool is_running()
		{
			return running;
		}

		// uncapped by default
		void set_target_delta(double delta_in_seconds)
		{
			target_delta = delta_in_seconds;
		}

		// uncapped by default
		void set_target_fps(unsigned int fps)
		{
			target_delta = 1.0 / fps;
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

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			if (renderer == NULL) {
				SDL_Log("CottonWind: Could not create renderer: %s\n", SDL_GetError());
				return false;
			}

			SDL_SetRenderDrawColor(renderer, 180, 50, 100, 255);

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
					default:
						type = Unsupported;
					}

					if (type != Unsupported)
					{
						KeyboardEvent event(e.key.keysym.scancode, SDL_GetScancodeName(e.key.keysym.scancode), e.key.repeat);
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
					default: {}
					}
				}
				else if (e.type == SDL_QUIT)
				{
					Event event;
					event.category = EventCategoryWindow;
					event.type = ApplicationQuit;
					on_event(&event);
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
					default: {}
					}
				}
			}
		}
	};
}