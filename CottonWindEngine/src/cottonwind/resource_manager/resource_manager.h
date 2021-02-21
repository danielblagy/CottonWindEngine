#pragma once

#include <SDL.h>

#include "../util/logger.h"

#include <string>
#include <unordered_map>

#include "../graphics/texture.h"
#include "../audio/audio.h"
#include "../graphics/font.h"


namespace cotwin
{
	// Here a separate class for Texture management is used, since SDL textures require an SDL_Renderer instance
	class ResourceManager
	{
	private:
		std::unordered_map<std::string, Audio> audio_resources;

		SDL_AudioSpec audio_format;
		SDL_AudioDeviceID device_id;

	public:
		static ResourceManager& get_instance()
		{
			static ResourceManager instance;
			return instance;
		}

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
			return get_instance().load_audio_internal(filepath);
		}

		static Audio& get_audio(const char* filepath)
		{
			return get_instance().get_audio_internal(filepath);
		}

		static Font& load_font(const char* filepath, int point_size)
		{
			return FontManager::load_font(filepath, point_size);
		}

		static Font& get_font(const char* filepath)
		{
			return FontManager::get_font(filepath);
		}

	private:
		Audio& load_audio_internal(const char* filepath)
		{
			Audio audio;
			if (SDL_LoadWAV(filepath, &audio.spec, &audio.audio_buffer, &audio.buffer_length) == NULL)
			{
				Logger::Error("Failed to load audio %s", filepath);
			}
			// TODO : change to OpenAudio to ensure that the same device is used across all the audio ??
			audio.device_id = device_id;
			audio_resources[std::string(filepath)] = audio;
			return audio_resources[filepath];
		}

		Audio& get_audio_internal(const char* filepath)
		{
			if (audio_resources.count(filepath))
			{
				return audio_resources[filepath];
			}
			else
			{
				return load_audio_internal(filepath);
			}
		}

		ResourceManager()
		{
			/* Set the audio format */
			audio_format.freq = 22050;
			audio_format.format = AUDIO_S16;
			audio_format.channels = 2;    /* 1 = mono, 2 = stereo */
			audio_format.samples = 1024;  /* Good low-latency value for callback */
			audio_format.callback = NULL;
			audio_format.userdata = NULL;

			device_id = SDL_OpenAudio(&audio_format, NULL);
		}

		~ResourceManager()
		{
			// free audio resources
			SDL_CloseAudioDevice(device_id);
			for (auto i : audio_resources)
			{
				i.second.free();
			}
		}

		// explicitly disallow copying
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator= (const ResourceManager&) = delete;
	};
}