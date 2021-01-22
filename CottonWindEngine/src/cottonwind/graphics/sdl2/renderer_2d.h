#pragma once

#include <SDL.h>

#include "../../math.h"


namespace cotwin
{
	// Whatever graphics implementation, each Renderer class should have clear() and flush() functions
	// that follow the same function declaration

	// since SDL2 requires SDL_Render instance to render stuff,
	// I make this a simple class (methods aren't static)
	class Renderer2D
	{
	private:
		SDL_Renderer* renderer;
	
	public:
		void set_render_instance(SDL_Renderer* s_renderer)
		{
			renderer = s_renderer;
		}
		
		void clear(Vector4f* color)
		{
			SDL_SetRenderDrawColor(
				renderer,
				unsigned int(color->r * 255.0f),
				unsigned int(color->g * 255.0f),
				unsigned int(color->b * 255.0f),
				unsigned int(color->a * 255.0f)
			);
			SDL_RenderClear(renderer);
		}

		void flush()
		{
			SDL_RenderPresent(renderer);
		}
		
		void draw_rect(const Vector4i& rect, const Vector4u8& draw_color)
		{
			SDL_Rect sdl_rect = { rect.left, rect.top, rect.width, rect.height };
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawRect(renderer, &sdl_rect);
		}

		void fill_rect(const Vector4i& rect, const Vector4u8& draw_color)
		{
			SDL_Rect sdl_rect = { rect.left, rect.top, rect.width, rect.height };
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderFillRect(renderer, &sdl_rect);
		}
	};
}