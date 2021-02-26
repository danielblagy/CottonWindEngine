#pragma once

#include "../graphics/texture.h"
#include "../graphics/font.h"
#include "../audio/audio_manager.h"


namespace cotwin
{
	// TODO : remove load_ functions, add unload_ functions
	class ResourceManager
	{
	public:
		static Texture& load_texture(const char* filepath)
		{
			return TextureManager::load_texture(filepath);
		}

		static Texture& get_texture(const char* filepath)
		{
			return TextureManager::get_texture(filepath);
		}

		static Audio& load_audio(const char* filepath)
		{
			return AudioManager::load_audio(filepath);
		}

		static Audio& get_audio(const char* filepath)
		{
			return AudioManager::get_audio(filepath);
		}

		static Font& load_font(const char* filepath, int point_size)
		{
			return FontManager::load_font(filepath, point_size);
		}

		static Font& get_font(const char* filepath)
		{
			return FontManager::get_font(filepath);
		}
	};
}