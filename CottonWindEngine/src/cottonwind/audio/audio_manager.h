#pragma once

#include <unordered_map>

#include <SDL.h>

#include "audio.h"
#include "../util/logger.h"


namespace cotwin
{
	class AudioManager
	{
	private:
		std::unordered_map<std::string, Audio> audio_resources;

		SDL_AudioSpec audio_format;
		SDL_AudioDeviceID device_id;

	public:
		static AudioManager& get_instance()
		{
			static AudioManager instance;
			return instance;
		}

		static Audio& load_audio(const char* filepath)
		{
			return get_instance().load_audio_internal(filepath);
		}

		static Audio& get_audio(const char* filepath)
		{
			return get_instance().get_audio_internal(filepath);
		}

	private:
		Audio& load_audio_internal(const char* filepath)
		{
			Audio audio;
			if (SDL_LoadWAV(filepath, &audio.spec, &audio.audio_buffer, &audio.buffer_length) == NULL)
			{
				Logger::Error("Failed to load audio %s", filepath);
			}
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

		AudioManager()
		{
			audio_format.freq = 48000;
			audio_format.format = AUDIO_S16LSB;
			audio_format.channels = 2;
			audio_format.samples = 4096;
			audio_format.callback = NULL;
			audio_format.userdata = NULL;
			
			device_id = SDL_OpenAudioDevice(NULL, 0, &audio_format, NULL, 0);
		}

		~AudioManager()
		{
			// free audio resources
			SDL_CloseAudioDevice(device_id);
			for (auto i : audio_resources)
			{
				i.second.free();
			}
		}

		// explicitly disallow copying
		AudioManager(const AudioManager&) = delete;
		AudioManager& operator= (const AudioManager&) = delete;

		friend Audio;
	};
}