#pragma once

#include <SDL.h>

#include "texture.h"

#include <string>
#include <unordered_map>

#include "../../util/logger.h"


namespace cotwin
{
	class SDLGraphics;
	
	class TextureManager
	{
	private:
		SDL_Renderer* renderer_handle = 0;
		std::unordered_map<std::string, Texture> textures;

	public:
		static TextureManager& get_instance()
		{
			static TextureManager instance;
			return instance;
		}

		static Texture& load_texture(const char* filepath)
		{
			return get_instance().load_texture_internal(filepath);
		}

		static Texture& get_texture(const char* filepath)
		{
			return get_instance().get_texture_internal(filepath);
		}

	private:
		Texture& load_texture_internal(const char* filepath)
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

			// TODO : allocate on the heap ??
			textures[std::string(filepath)] = Texture(texture, texture_width, texture_height);
			return textures[filepath];
		}

		Texture& get_texture_internal(const char* filepath)
		{
			std::string filepath_str = filepath;
			if (textures.count(filepath_str))
			{
				return textures[filepath_str];
			}
			else
			{
				return load_texture_internal(filepath);
			}
		}

		static void set_renderer_handle(SDL_Renderer* s_renderer_handle)
		{
			get_instance().renderer_handle = s_renderer_handle;
		}

		TextureManager() {}

		~TextureManager()
		{
			// TODO : will unordered_map destructor call destructor for Texture ??
			for (auto i : textures)
				SDL_DestroyTexture(i.second.texture_handle);
		}

		// explicitly disallow copying
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator= (const TextureManager&) = delete;

		friend SDLGraphics;
	};
}