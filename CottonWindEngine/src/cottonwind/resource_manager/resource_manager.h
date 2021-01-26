#pragma once

#include <SDL.h>

#include "../util/logger.h"

#include <string>
#include <unordered_map>

#include "../graphics/texture.h"


namespace cotwin
{
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

	private:
		ResourceManager() {}
	};
}