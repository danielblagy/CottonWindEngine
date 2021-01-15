#pragma once

#include <SDL.h>


namespace cotwin
{
	class Game
	{
	private:
		SDL_Window * window;
		SDL_Renderer* renderer;

		bool running = false;
	
	public:
		Game(
			const char* window_title, int window_width, int window_height,
			bool fullscreen
		)
		{
			running = init(window_title, 0, 0, window_width, window_height, fullscreen, true, true, false);
		}

		Game(
			const char* window_title, int window_width, int window_height,
			bool window_resizable, bool window_borderless
		)
		{
			running = init(window_title, 0, 0, window_width, window_height, false, true, window_resizable, window_borderless);
		}
		
		Game(
			const char* window_title, int window_x, int window_y, int window_width, int window_height,
			bool window_centered, bool window_resizable, bool window_borderless
		)
		{
			running = init(window_title, window_x, window_y, window_width, window_height, false, window_centered, window_resizable, window_borderless);
		}

		~Game()
		{
			destroy();
		}

		bool init(
			const char* window_title,
			int window_x, int window_y, int window_width, int window_height,
			bool fullscreen, bool window_centered, bool window_resizable, bool window_borderless
		)
		{
			int sdl_init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

			if (sdl_init_result != 0)
			{
				SDL_Log("CottonWind: Unable to initialize SDL: %s", SDL_GetError());
				return false;
			}

			if (window_centered)
			{
				window_x = SDL_WINDOWPOS_CENTERED;
				window_y = SDL_WINDOWPOS_CENTERED;
			}

			Uint32 window_flags = SDL_WINDOW_OPENGL;
			
			if (fullscreen)
			{
				window_flags = window_flags | SDL_WINDOW_FULLSCREEN;
			}
			else
			{
				if (window_resizable)
				{
					window_flags = window_flags | SDL_WINDOW_RESIZABLE;
				}

				if (window_borderless)
				{
					window_flags = window_flags | SDL_WINDOW_BORDERLESS;
				}
			}

			window = SDL_CreateWindow(window_title, window_x, window_y, window_width, window_height, window_flags);

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

		void update()
		{
			// The window is open: could enter program loop here (see SDL_PollEvent())

			SDL_Event e;
			if (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
				{
					running = false;
				}
			}

			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
		}

		void destroy()
		{
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		bool is_running()
		{
			return running;
		}
	};
}