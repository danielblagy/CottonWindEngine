#pragma once

#include <SDL.h>
#include <glad/glad.h>

#include "../../math.h"


namespace cotwin
{
	// Whatever graphics implementation, each Renderer class should have clear() and flush() functions
	// that follow the same function declaration

	// OpenGL with SDL2 requires SDL_Window instance to clear/flush
	class Renderer2D
	{
	private:
		static SDL_Window* window;
	
	public:
		static void set_window_instance(SDL_Window* s_window)
		{
			window = s_window;
		}
		
		static void clear(Vector4f* color)
		{
			int window_w, window_h;
			SDL_GetWindowSize(window, &window_w, &window_h);
			glViewport(0, 0, window_w, window_h);
			glClearColor(color->r, color->g, color->b, color->a);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		static void flush()
		{
			SDL_GL_SwapWindow(window);
		}
	};

	SDL_Window* Renderer2D::window = 0;
}