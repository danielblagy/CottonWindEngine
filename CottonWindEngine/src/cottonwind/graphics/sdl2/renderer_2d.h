#pragma once

#include <SDL.h>

#include "../../vendor/glm/glm.hpp"

#include "texture.h"

#include "find_sdl_ttf.h"

#ifdef CW_SDL_TTF_AVAILABLE
#include "font.h"
#endif


namespace cotwin
{
	// Whatever graphics implementation, each Renderer class should have clear() and flush() functions
	// that follow the same function declaration
	//
	// static void clear(const glm::vec4& color)
	// 
	// static void flush()

	// SDL2 requires SDL_Render instance to render stuff
	class Renderer2D
	{
	private:
		static SDL_Renderer* renderer;
		static glm::vec4 clear_color;
	
	public:
		static void set_render_instance(SDL_Renderer* s_renderer)
		{
			renderer = s_renderer;
		}

		static void set_clear_color(const glm::vec4& s_clear_color)
		{
			clear_color = s_clear_color;
		}

		static void set_clear_color(const glm::u8vec4 s_clear_color)
		{
			clear_color = { 
				(float)s_clear_color.r / 255.0f,
				(float)s_clear_color.g / 255.0f,
				(float)s_clear_color.b / 255.0f,
				(float)s_clear_color.a / 255.0f 
			};
		}
		
		static void clear()
		{
			SDL_SetRenderDrawColor(
				renderer,
				unsigned int(clear_color.r * 255.0f),
				unsigned int(clear_color.g * 255.0f),
				unsigned int(clear_color.b * 255.0f),
				unsigned int(clear_color.a * 255.0f)
			);
			SDL_RenderClear(renderer);
		}

		static void flush()
		{
			SDL_RenderPresent(renderer);
		}

		static void draw_point(int x, int y, const glm::u8vec4& draw_color)
		{
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawPoint(renderer, x, y);
		}

		static void draw_point(const glm::ivec2& coordinates, const glm::u8vec4& draw_color)
		{
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawPoint(renderer, coordinates.x, coordinates.y);
		}

		static void draw_line(int x1, int y1, int x2, int y2, const glm::u8vec4& draw_color)
		{
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
		}

		static void draw_line(const glm::ivec2& point1, const glm::ivec2& point2, const glm::u8vec4& draw_color)
		{
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawLine(renderer, point1.x, point1.y, point2.x, point2.y);
		}

		// coordinates[0] & [1] are x & y of one point, coordinates [2] & [3] - of another point
		static void draw_line(const glm::ivec2& coordinates, const glm::u8vec4& draw_color)
		{
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawLine(renderer, coordinates[0], coordinates[1], coordinates[2], coordinates[3]);
		}
		
		static void draw_rect(const glm::ivec4& rect, const glm::u8vec4& draw_color)
		{
			SDL_Rect sdl_rect = { rect.x, rect.y, rect.z, rect.w };
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderDrawRect(renderer, &sdl_rect);
		}

		static void fill_rect(const glm::ivec4& rect, const glm::u8vec4& draw_color)
		{
			SDL_Rect sdl_rect = { rect.x, rect.y, rect.z, rect.w };
			SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			SDL_RenderFillRect(renderer, &sdl_rect);
		}

		static void render_texture(const Texture& texture, const glm::ivec4& bounds)
		{
			SDL_Rect rect = { bounds[0], bounds[1], bounds[2], bounds[3] };
			SDL_RenderCopy(renderer, texture.texture_handle, NULL, &rect);
		}

		static void render_texture(const Texture& texture, const glm::ivec4& texture_rect, const glm::ivec2& position, const glm::ivec2& size)
		{
			SDL_Rect texture_area_rect = { texture_rect[0], texture_rect[1], texture_rect[2], texture_rect[3] };
			SDL_Rect sprite_rect = { position.x, position.y, size.x, size.y };
			SDL_RenderCopy(renderer, texture.texture_handle, &texture_area_rect, &sprite_rect);
		}

#ifdef CW_SDL_TTF_AVAILABLE
		static void render_text(const char* text, const Font& font, SDL_Color color, const glm::ivec2& position)
		{
			//Render text surface
			SDL_Surface* textSurface = TTF_RenderText_Solid(font.font_handle, text, color);
			if (textSurface == NULL)
			{
				printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
			}
			else
			{
				//Create texture from surface pixels
				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
				if (texture == NULL)
				{
					printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
				}

				//Get rid of old surface
				SDL_FreeSurface(textSurface);

				int texture_width, texture_height;
				SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);
				
				Texture text_texture = Texture(texture, texture_width, texture_height);
				
				render_texture(
					text_texture,
					{ 0, 0, texture_width, texture_height },
					{ position.x, position.y }, { texture_width, texture_height	}
				);

				SDL_DestroyTexture(text_texture.texture_handle);
			}
		}
#endif
	};

	SDL_Renderer* Renderer2D::renderer = 0;
	// TODO : make Renderer2D a singleton, put this init into constructor
	glm::vec4 Renderer2D::clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };	// black by default
}