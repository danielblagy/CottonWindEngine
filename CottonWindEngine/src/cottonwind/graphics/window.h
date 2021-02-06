#pragma once

#include <SDL.h>


namespace cotwin
{
	class Graphics;
	
	class Window
	{
	private:
		SDL_Window* window = 0;

	public:
		static glm::ivec2 get_window_size()
		{
			glm::ivec2 size;
			SDL_GetWindowSize(get_instance().window, &size.x, &size.y);
			return size;
		}

	private:
		static Window& get_instance()
		{
			static Window instance;
			return instance;
		}

		static void set_window(SDL_Window* s_window)
		{
			get_instance().window = s_window;
		}

		Window()
		{}

		~Window()
		{}

		// explicitly disallow copying
		Window(const Window&) = delete;
		Window& operator= (const Window&) = delete;

		friend Graphics;
	};
}