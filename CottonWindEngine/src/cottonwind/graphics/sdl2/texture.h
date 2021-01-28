#pragma once

#include <SDL.h>

#include "../../vendor/glm/glm.hpp"


namespace cotwin
{
	class TextureManager;
	class Renderer2D;
	class Text;

	class Texture
	{
	private:
		SDL_Texture* texture_handle;
		int width, height;

	public:
		Texture()
			: texture_handle(0), width(0), height(0)
		{}

		Texture(SDL_Texture* s_texture_handle, int s_width, int s_height)
			: texture_handle(s_texture_handle), width(s_width), height(s_height)
		{}

		inline int get_width()
		{
			return width;
		}

		inline int get_height()
		{
			return height;
		}

		inline glm::ivec2 get_size()
		{
			return { width, height };
		}

		friend TextureManager;
		friend Renderer2D;
		friend Text;
	};
}