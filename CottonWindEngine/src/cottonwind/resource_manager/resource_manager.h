#pragma once

#include <SDL.h>

#include "../util/logger.h"

#include <string>
#include <unordered_map>


namespace cotwin
{
	struct Texture
	{
		SDL_Texture* texture_handle;
		int width, height;

		Texture()
			: texture_handle(0), width(0), height(0)
		{}
		
		Texture(SDL_Texture* s_texture_handle, int s_width, int s_height)
			: texture_handle(s_texture_handle), width(s_width), height(s_height)
		{}
	};
	
	// TODO : CW_MODERN_OPENGL textures support
	
	class ResourceManager
	{
	private:
		SDL_Renderer* renderer_handle;
		std::unordered_map<std::string, Texture> textures;
	
	public:
		ResourceManager(SDL_Renderer* s_renderer_handle)
			: renderer_handle(s_renderer_handle)
		{}

		~ResourceManager()
		{
			// TODO : will unordered_map destructor call destructor for Texture ??
			for (auto i : textures)
				SDL_DestroyTexture(i.second.texture_handle);
		}

		Texture& load_texture(const char* filepath)
		{
			SDL_Surface* loading_surface = SDL_LoadBMP(filepath);
			if (!loading_surface)
			{
				Logger::Error("Failed to load texture %s", filepath);
				// TODO : generate an error texture (with SDL_CreateTexture)
				return textures["error"];
			}

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_handle, loading_surface);
			SDL_FreeSurface(loading_surface);

			int texture_width, texture_height;
			SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

			textures[std::string(filepath)] = Texture(texture, texture_width, texture_height);
			return textures[filepath];
		}

		Texture& get(const char* filepath)
		{
			std::string filepath_str = filepath;
			if (textures.count(filepath_str))
			{
				return textures[filepath_str];
			}
			else
			{
				return load_texture(filepath);
			}
		}
	};
}