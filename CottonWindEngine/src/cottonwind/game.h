#pragma once

#include <SDL.h>


namespace cotwin
{
	struct WindowProperties
	{
		const char* title;
		int left, top, width, height;
		bool fullscreen, centered, resizable, borderless;
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
				
				// TODO : make an event system and pass event handling to the api user
				SDL_Event e;
				if (SDL_PollEvent(&e))
				{
					if (e.type == SDL_QUIT)
					{
						running = false;
					}
				}

				on_update(accumulated_delta);

				SDL_RenderClear(renderer);
				SDL_RenderPresent(renderer);

				accumulated_delta = 0.0;
			}
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

	private:
		bool init(WindowProperties window_properties)
		{
			int sdl_init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

			if (sdl_init_result != 0)
			{
				SDL_Log("CottonWind: Unable to initialize SDL: %s", SDL_GetError());
				return false;
			}

			if (window_properties.centered)
			{
				window_properties.left = SDL_WINDOWPOS_CENTERED;
				window_properties.top = SDL_WINDOWPOS_CENTERED;
			}

			Uint32 window_flags = SDL_WINDOW_OPENGL;
			
			if (window_properties.fullscreen)
			{
				window_flags = window_flags | SDL_WINDOW_FULLSCREEN;
			}
			else
			{
				if (window_properties.resizable)
				{
					window_flags = window_flags | SDL_WINDOW_RESIZABLE;
				}

				if (window_properties.borderless)
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