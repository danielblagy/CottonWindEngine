#pragma once

#define MINIAUDIO_IMPLEMENTATION
#include "../vendor/miniaudio/miniaudio.h"

#include "../util/logger.h"

#define SAMPLE_FORMAT	ma_format_f32
#define CHANNEL_COUNT	2
#define SAMPLE_RATE		48000


namespace cotwin
{
	class AudioPlayback;
	
	class Audio
	{
	private:
		ma_decoder_config decoder_config;
		ma_decoder decoder;
		ma_bool32 at_end = false;

	public:
		// NOTE : ResourceManager must set the fields
		Audio(const char* filepath)
		{
			/* All decoders need to have the same output format for simple mixing. */
			decoder_config = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE);
			ma_result result = result = ma_decoder_init_file(filepath, &decoder_config, &decoder);
			if (result != MA_SUCCESS) {
				ma_decoder_uninit(&decoder);

				Logger::Error("Failed to load %s", filepath);
			}
			at_end = MA_FALSE;
		}

		~Audio()
		{}

		void free()
		{
			ma_decoder_uninit(&decoder);
		}

		void play()
		{
			// add to AudioPlayback
			AudioPlayback::add_to_play
		}

		friend AudioPlayback;
	};
}