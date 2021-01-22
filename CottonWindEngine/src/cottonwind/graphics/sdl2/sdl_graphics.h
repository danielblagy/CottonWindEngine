#pragma once

#include "../graphics.h"

#include "../../logger.h"


namespace cotwin
{
	class SDLGraphics : public Graphics
	{
	private:
		SDL_Renderer* renderer;

	public:
		bool init(WindowProperties* window_properties) override
		{
			int sdl_init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

			if (sdl_init_result != 0)
			{
				LogCritical("CottonWind\t Unable to initialize SDL: %s", SDL_GetError());
				return false;
			}

			if (window_properties->flags & Centered)
			{
				window_properties->left = SDL_WINDOWPOS_CENTERED;
				window_properties->top = SDL_WINDOWPOS_CENTERED;
			}

			Uint32 window_flags = SDL_WINDOW_OPENGL;
			if (window_properties->flags & Fullscreen)
			{
				window_flags = window_flags | SDL_WINDOW_FULLSCREEN;
			}
			else
			{
				if (window_properties->flags & Resizable)
				{
					window_flags = window_flags | SDL_WINDOW_RESIZABLE;
				}

				if (window_properties->flags & Borderless)
				{
					window_flags = window_flags | SDL_WINDOW_BORDERLESS;
				}
			}

			window = SDL_CreateWindow(
				window_properties->title,
				window_properties->left, window_properties->top,
				window_properties->width, window_properties->height,
				window_flags
			);

			if (window == NULL) {
				LogError("CottonWind\t Could not create window: %s\n", SDL_GetError());
				return false;
			}

			LogTrace("CottonWind\t Initialized SDL2 and SDL_Window");

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			if (renderer == NULL) {
				SDL_Log("CottonWind: Could not create renderer: %s\n", SDL_GetError());
				return false;
			}
		}
		
		void destroy() override
		{
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		SDL_Renderer* get_sdl_renderer()
		{
			return renderer;
		}
	};
}