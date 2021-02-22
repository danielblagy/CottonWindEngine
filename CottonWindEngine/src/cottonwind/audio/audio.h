#pragma once

#include <SDL.h>


namespace cotwin
{
	class AudioManager;

	class Audio
	{
	private:
		SDL_AudioSpec spec;
		SDL_AudioDeviceID device_id;
		Uint8* audio_buffer;
		Uint32 buffer_length;

	public:
		Audio()
		{}

		~Audio()
		{}

		void free()
		{
			SDL_FreeWAV(audio_buffer);
		}

		void play()
		{
			SDL_QueueAudio(device_id, audio_buffer, buffer_length);
			SDL_PauseAudioDevice(device_id, 0);
		}

		friend AudioManager;
	};
}