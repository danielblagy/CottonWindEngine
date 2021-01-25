#pragma once

#include <SDL.h>

#include "../../vendor/glm/glm.hpp"


namespace cotwin
{
	// Whatever graphics implementation, each Renderer class should have clear() and flush() functions
	// that follow the same function declaration

	// SDL2 requires SDL_Render instance to render stuff
	class Renderer2D
	{
	private:
		static SDL_Renderer* renderer;
	
	public:
		static void set_render_instance(SDL_Renderer* s_renderer)
		{
			renderer = s_renderer;
		}
		
		static void clear(glm::vec4& color)
		{
			SDL_SetRenderDrawColor(
				renderer,
				unsigned int(color.r * 255.0f),
				unsigned int(color.g * 255.0f),
				unsigned int(color.b * 255.0f),
				unsigned int(color.a * 255.0f)
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
	};

	SDL_Renderer* Renderer2D::renderer = 0;
}