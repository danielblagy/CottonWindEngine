#pragma once

#define MINIAUDIO_IMPLEMENTATION
#include "../vendor/miniaudio/miniaudio.h"

#include <vector>

#include "audio.h"

#define SAMPLE_FORMAT	ma_format_f32
#define CHANNEL_COUNT	2
#define SAMPLE_RATE		48000


namespace cotwin
{
	class AudioPlayback
	{
	private:
		std::vector<Audio> audio_to_play;
		ma_device_config device_config;

	public:
		static void add(Audio audio)
		{
			get_instance().audio_to_play.push_back(audio);
		}
	
	private:
		static AudioPlayback& get_instance()
		{
			static AudioPlayback instance;

			return instance;
		}
		
		AudioPlayback()
		{
			/* Create only a single device. The decoders will be mixed together in the callback. In this example the data format needs to be the same as the decoders. */
			device_config = ma_device_config_init(ma_device_type_playback);
			device_config.playback.format = SAMPLE_FORMAT;
			device_config.playback.channels = CHANNEL_COUNT;
			device_config.sampleRate = SAMPLE_RATE;
			device_config.dataCallback = this::data_callback;
			device_config.pUserData = NULL;

			if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
				for (iDecoder = 0; iDecoder < g_decoderCount; ++iDecoder) {
					ma_decoder_uninit(&g_pDecoders[iDecoder]);
				}
				free(g_pDecoders);
				free(g_pDecodersAtEnd);

				printf("Failed to open playback device.\n");
				return -3;
			}

			/*
			We can't stop in the audio thread so we instead need to use an event. We wait on this thread in the main thread, and signal it in the audio thread. This
			needs to be done before starting the device. We need a context to initialize the event, which we can get from the device. Alternatively you can initialize
			a context separately, but we don't need to do that for this example.
			*/
			ma_event_init(&g_stopEvent);

			/* Now we start playback and wait for the audio thread to tell us to stop. */
			if (ma_device_start(&device) != MA_SUCCESS) {
				ma_device_uninit(&device);
				for (iDecoder = 0; iDecoder < g_decoderCount; ++iDecoder) {
					ma_decoder_uninit(&g_pDecoders[iDecoder]);
				}
				free(g_pDecoders);
				free(g_pDecodersAtEnd);

				printf("Failed to start playback device.\n");
				return -4;
			}

		}

		~AudioPlayback()
		{
			// the device
		}

		AudioPlayback(const AudioPlayback&) = delete;
		AudioPlayback& operator= (const AudioPlayback&) = delete;

		ma_bool32 are_all_decoders_at_end()
		{
			for (Audio& audio : audio_to_play)
			{
				if (audio.at_end == MA_FALSE)
				{
					return MA_FALSE;
				}
			}
			
			return MA_TRUE;
		}
		
		ma_uint32 read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float* pOutputF32, ma_uint32 frameCount)
		{
			/*
			The way mixing works is that we just read into a temporary buffer, then take the contents of that buffer and mix it with the
			contents of the output buffer by simply adding the samples together. You could also clip the samples to -1..+1, but I'm not
			doing that in this example.
			*/
			float temp[4096];
			ma_uint32 tempCapInFrames = ma_countof(temp) / CHANNEL_COUNT;
			ma_uint32 totalFramesRead = 0;
			
			while (totalFramesRead < frameCount)
			{
				ma_uint32 iSample;
				ma_uint32 framesReadThisIteration;
				ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
				ma_uint32 framesToReadThisIteration = tempCapInFrames;
				if (framesToReadThisIteration > totalFramesRemaining) {
					framesToReadThisIteration = totalFramesRemaining;
				}
			
				framesReadThisIteration = (ma_uint32)ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration);
				if (framesReadThisIteration == 0) {
					break;
				}
			
				/* Mix the frames together. */
				for (iSample = 0; iSample < framesReadThisIteration * CHANNEL_COUNT; ++iSample) {
					pOutputF32[totalFramesRead * CHANNEL_COUNT + iSample] += temp[iSample];
				}
			
				totalFramesRead += framesReadThisIteration;
			
				if (framesReadThisIteration < framesToReadThisIteration) {
					break;  /* Reached EOF. */
				}
			}
			
			return totalFramesRead;
		}

		void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
		{
			float* pOutputF32 = (float*)pOutput;
			ma_uint32 i;

			MA_ASSERT(pDevice->playback.format == SAMPLE_FORMAT);   /* <-- Important for this example. */

			for (Audio& audio : audio_to_play)
			{
				if (!audio.at_end) {
					ma_uint32 framesRead = read_and_mix_pcm_frames_f32(&audio.decoder, pOutputF32, frameCount);
					if (framesRead < frameCount)
					{
						audio.at_end = MA_TRUE;
					}
				}
			}

			// TODO : maybe check and remove individual audio objects ??
			if (are_all_decoders_at_end())
			{
				audio_to_play.clear();
			}

			(void)pInput;
		}
	};
}