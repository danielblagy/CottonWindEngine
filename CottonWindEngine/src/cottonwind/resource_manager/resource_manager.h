#pragma once

#include <SDL.h>

#include "../util/logger.h"

#include <string>
#include <unordered_map>

#ifdef CW_MODERN_OPENGL
// TODO : CW_MODERN_OPENGL textures support
#else
#include "../graphics/sdl2/texture_resource_manager.h"
#endif


namespace cotwin
{
	class ResourceManager
	{
	private:
		TextureManager texture_manager;
	
	public:
		ResourceManager(TextureManager s_texture_manager)
			: texture_manager(s_texture_manager)
		{}

		~ResourceManager()
		{}

		Texture& load_texture(const char* filepath)
		{
			return texture_manager.load_texture(filepath);
		}

		Texture& get_texture(const char* filepath)
		{
			return texture_manager.get_texture(filepath);
		}
	};
}