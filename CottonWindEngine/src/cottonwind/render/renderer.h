#pragma once

#include <SDL.h>

#include "../math.h"


namespace cotwin
{
	class Game;
	
	class Renderer
	{
	protected:
		SDL_Renderer* renderer;

	public:
		void draw_rect(const Vector4& rect, const Vector4ui8& draw_color)
		{
			SDL_Rect sdl_rect = { rect.left, rect.top, rect.width, rect.height };
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawRect(renderer, &sdl_rect);
		}

		void fill_rect(const Vector4& rect, const Vector4ui8& draw_color)
		{
			SDL_Rect sdl_rect = { rect.left, rect.top, rect.width, rect.height };
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderFillRect(renderer, &sdl_rect);
		}

		friend Game;

	protected:
		Renderer(SDL_Renderer* s_renderer)
			: renderer (s_renderer)
		{}
	};
}