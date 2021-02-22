#pragma once

#include <SDL.h>
#include <cstdlib>


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
		// NOTE : ResourceManager must set the fields
		Audio()
		{}

		~Audio()
		{}

		void free()
		{
			//SDL_CloseAudioDevice(device_id);
			SDL_FreeWAV(audio_buffer);
		}

		void play()
		{
			//SDL_QueueAudio(device_id, audio_buffer, buffer_length);
			
			// assuming every audio file uses the same device
			// NOTE : this returns a "snapshot" of how much data is queued in the moment, so it can shrink at any time
			int queued_data_size = SDL_GetQueuedAudioSize(device_id);

			int data_length;
			// TODO : will audio come out OK if this is true ?? hmm...
			if (queued_data_size >= buffer_length)
				data_length = queued_data_size;
			else
				data_length = buffer_length;

			// no need for sizeof, since both the queued_data_size and buffer_length are in bytes
			Uint8* data = (Uint8*) std::malloc(data_length);
			for (int i = 0; i < data_length; i++)
				data[i] = 0;
			
			Uint32 bytes_actually_dequeued = SDL_DequeueAudio(device_id, data, queued_data_size);
			//if ()

			// TODO : assert that audio spec.format is the same as the queued audio audio format
			SDL_MixAudioFormat(data, audio_buffer, spec.format, data_length, SDL_MIX_MAXVOLUME);

			SDL_QueueAudio(device_id, data, data_length);

			std::free(data);
			
			SDL_PauseAudioDevice(device_id, 0);
		}

		friend AudioManager;
	};
}