#pragma once

#include <SDL.h>


namespace cotwin
{
	class ResourceManager;
	
	class Audio
	{
	private:
		SDL_AudioSpec spec;
		SDL_AudioDeviceID device_id;
		Uint8* audio_buffer;
		Uint32 buffer_length;

	public:
		// NOTE : ResourceManager must set the fields
		Audio()
		{}

		~Audio()
		{}

		void free()
		{
			SDL_CloseAudioDevice(device_id);
			SDL_FreeWAV(audio_buffer);
		}

		void play()
		{
			SDL_QueueAudio(device_id, audio_buffer, buffer_length);
			SDL_PauseAudioDevice(device_id, 0);
		}

		friend ResourceManager;
	};
}