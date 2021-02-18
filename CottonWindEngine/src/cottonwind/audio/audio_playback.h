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
		
		AudioPlayback() {}

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