#pragma once

#include "../../vendor/glm/glm.hpp"

#include "../../util/logger.h"

#include <SDL.h>
#include "font.h"
#include "texture.h"
#include "renderer_2d.h"


namespace cotwin
{
	class Text
	{
	private:
		const char* text;
		Font font;
		SDL_Color color;
		glm::ivec2 position;
		glm::ivec2 size;

		Texture text_texture;

	public:
		Text(const char* s_text, const Font& s_font, SDL_Color s_color, const glm::ivec2& s_position, const glm::ivec2& s_size)
			: text(s_text), font(s_font), color(s_color), position(s_position), size(s_size)
		{
			//Render text surface
			SDL_Surface* textSurface = TTF_RenderText_Solid(font.font_handle, text, color);
			if (textSurface == NULL)
			{
				Logger::Error("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
			}
			else
			{
				//Create texture from surface pixels
				SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer2D::renderer, textSurface);
				if (texture == NULL)
				{
					Logger::Error("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
				}

				//Get rid of old surface
				SDL_FreeSurface(textSurface);

				int texture_width, texture_height;
				SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

				text_texture = Texture(texture, size.x, size.y);
			}
		}

		~Text()
		{
			SDL_DestroyTexture(text_texture.texture_handle);
		}

		friend Renderer2D;
	};
}