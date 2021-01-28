#pragma once

#include <SDL_ttf.h>


namespace cotwin
{
	class FontManager;
	class Renderer2D;
	class Text;

	class Font
	{
	private:
		TTF_Font* font_handle;
		int point_size;

	public:
		Font()
			: font_handle(0), point_size(0)
		{}

		Font(TTF_Font* s_font_handle, int s_point_size)
			: font_handle(s_font_handle), point_size(s_point_size)
		{}

		friend FontManager;
		friend Renderer2D;
		friend Text;
	};
}