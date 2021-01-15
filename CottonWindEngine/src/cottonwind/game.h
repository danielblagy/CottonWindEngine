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

		bool running = false;
		// Renderer renderer;
	
	public:
		Game(WindowProperties window_properties)
		{
			running = init(window_properties);
		}

		virtual ~Game() = default;

		void start()
		{
			on_init();
			
			// The window is open: could enter program loop here (see SDL_PollEvent())

			while (running)
			{
				// TODO : make an event system and pass event handling to the api user
				SDL_Event e;
				if (SDL_PollEvent(&e))
				{
					if (e.type == SDL_QUIT)
					{
						running = false;
					}
				}

				on_update();

				SDL_RenderClear(renderer);
				SDL_RenderPresent(renderer);
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
		virtual void on_update() = 0;
		virtual void on_destroy() = 0;

		bool is_running()
		{
			return running;
		}

	private:
		bool init(WindowProperties window_properties)
		{
			int sdl_init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

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