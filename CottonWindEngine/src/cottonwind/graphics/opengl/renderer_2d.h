#pragma once

#include <SDL.h>

#include "../../math.h"


namespace cotwin
{
	// Whatever graphics implementation, each Renderer class should have clear() and flush() functions
	// that follow the same function declaration

	class Renderer2D
	{
	public:
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
	};
}