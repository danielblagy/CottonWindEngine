#pragma once

#include <queue>

#include <SDL.h>

#include "events/event.h"


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

	private:
		std::queue<Event> event_queue;
		
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
				
				SDL_Event e;
				while (SDL_PollEvent(&e))
				{
					// TODO : decide whether or not to give up control over SDL_QUIT handling to the user
					if (e.type == SDL_QUIT)
					{
						running = false;
						continue;
					}
					
					// TODO : convert SDL_Event to cotwin::Event type
					EventCategory category;
					if (e.type >= SDL_KEYDOWN && e.type <= SDL_KEYMAPCHANGED)
						category = EventCategoryKeyboard;
					else if (e.type == SDL_WINDOWEVENT)
						category = EventCategoryWindow;
					
					SDL_QUIT;
					event_queue.emplace(e);
				}

				on_update(accumulated_delta);

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
		virtual void on_update(double delta) = 0;
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

		bool poll_event(Event* event)
		{
			if (event_queue.empty())
				return false;

			*event = event_queue.front();
			event_queue.pop();

			return true;
		}

		void clear_event_queue()
		{
			while (!event_queue.empty())
				event_queue.pop();
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
	};
}