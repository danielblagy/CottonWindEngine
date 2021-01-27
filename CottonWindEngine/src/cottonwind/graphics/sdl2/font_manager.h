#pragma once

#include <SDL_ttf.h>

#include "font.h"

#include "../../util/logger.h"

#include <string>
#include <unordered_map>


namespace cotwin
{
	class FontManager
	{
	private:
		std::unordered_map<std::string, Font> fonts;

	public:
		static FontManager& get_instance()
		{
			static FontManager instance;
			return instance;
		}

		static Font& load_font(const char* filepath, int point_size)
		{
			return get_instance().load_font_internal(filepath, point_size);
		}

		static Font& get_font(const char* filepath)
		{
			return get_instance().get_font_internal(filepath);
		}

	private:
		Font& load_font_internal(const char* filepath, int point_size)
		{
			//Open the font
			TTF_Font* font = TTF_OpenFont(filepath, point_size);
			if (font == NULL)
			{
				printf("Failed to load the font SDL_ttf Error: %s\n", TTF_GetError());
			}

			fonts[filepath] = Font(font, point_size);
			return fonts[filepath];
		}

		Font& get_font_internal(const char* filepath)
		{
			if (fonts.count(filepath))
			{
				return fonts[filepath];
			}
			else
			{
				return load_font_internal(filepath, 16);
			}
		}

		FontManager()
		{
			//Initialize SDL_ttf
			if (TTF_Init() == -1)
			{
				Logger::Critical("CottonWind\t Unable to initialize SDL_ttf: %s", TTF_GetError());
			}
		}

		~FontManager()
		{
			// TODO : will unordered_map destructor call destructor for Font ??
			for (auto i : fonts)
				TTF_CloseFont(i.second.font_handle);

			TTF_Quit();
		}

		// explicitly disallow copying
		FontManager(const FontManager&) = delete;
		FontManager& operator= (const FontManager&) = delete;
	};
}