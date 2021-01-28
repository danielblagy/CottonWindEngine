#pragma once

#include "../../vendor/glm/glm.hpp"

#include "../../util/logger.h"

#include <SDL.h>
#include "font.h"
#include "texture.h"

#include <string>


namespace cotwin
{
	class Renderer2D;
	
	class Text
	{
	private:
		std::string text;
		Font font;
		glm::ivec4 color;
		glm::ivec2 position;
		glm::ivec2 size;

		Texture text_texture;

	public:
		Text()
			: text("")
		{}
		
		Text(std::string s_text, const Font& s_font, const glm::ivec4& s_color, const glm::ivec2& s_position)
			: text(s_text), font(s_font), color(s_color), position(s_position), size(), text_texture()
		{}
		
		Text(std::string s_text, const Font& s_font, const glm::ivec4& s_color, const glm::ivec2& s_position, const glm::ivec2& s_size)
			: text(s_text), font(s_font), color(s_color), position(s_position), size(s_size), text_texture()
		{}

		~Text()
		{
			free_texture();
		}

		inline std::string get_text() { return text; }
		inline Font get_font() { return font; }
		inline glm::ivec4 get_color() { return color; }
		inline glm::ivec2 get_position() { return position; }
		inline glm::ivec2 get_size() { return size; }

		void set_text(std::string s_text)
		{
			text = s_text;
			free_texture();
		}

		void set_font(const Font& s_font)
		{
			font = s_font;
			free_texture();
		}

		void set_color(const glm::ivec4& s_color)
		{
			color = s_color;
			free_texture();
		}

		inline void set_position(const glm::ivec2& s_position)
		{
			position = s_position;
		}

		inline void set_size(const glm::ivec2& s_size)
		{
			size = s_size;
		}

	private:
		void free_texture()
		{
			SDL_DestroyTexture(text_texture.texture_handle);
			text_texture = Texture();
		}

		friend Renderer2D;
	};
}