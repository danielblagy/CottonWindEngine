#pragma once

#define MA_IMPLEMENTATION
#include "../vendor/miniaudio/miniaudio.h"
#include "../util/logger.h"

#include <vector>

#define SAMPLE_FORMAT	ma_format_f32
#define CHANNEL_COUNT	2
#define SAMPLE_RATE		48000


namespace cotwin
{
	class Audio
	{
	private:
		ma_decoder_config decoder_config;
		ma_decoder decoder;
		ma_bool32 at_end = false;

	public:
		
		// a default constructor for unordered_map (used in ResourceManager)
		Audio()
		{}

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
			AudioPlayback::add(*this);
		}
		
		friend class AudioPlayback;
		
		class AudioPlayback
		{
		private:
			std::vector<Audio> audio_to_play;
			ma_device_config device_config;
			ma_device device;
		
		public:
			static void add(Audio audio)
			{
				get_instance().audio_to_play.push_back(audio);
			}
		
			static void play()
			{
				AudioPlayback& instance = get_instance();
		
				if (instance.audio_to_play.size() > 0)
				{
					if (ma_device_start(&instance.device) != MA_SUCCESS) {
						ma_device_uninit(&instance.device);
						Logger::Error("Failed to start playback device.");
					}
				}
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
				device_config.dataCallback = data_callback;
				device_config.pUserData = NULL;
		
				if (ma_device_init(NULL, &device_config, &device) != MA_SUCCESS)
				{
					Logger::Error("Failed to open playback device.");
				}
			}
		
			~AudioPlayback()
			{
				ma_device_uninit(&device);
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
		
			static ma_uint32 read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float* pOutputF32, ma_uint32 frameCount)
			{
				/*
				The way mixing works is that we just read into a temporary buffer, then take the contents of that buffer and mix it with the
				contents of the output buffer by simply adding the samples together. You could also clip the samples to -1..+1, but I'm not
				doing that in this example.
				*/
				const uint32_t size = 4096;
				float temp[size];
				ma_uint32 tempCapInFrames = size / CHANNEL_COUNT;
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
		
			// needs to be static (or a non-oop function) to be passed as device_config.dataCallback
			static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
			{
				AudioPlayback& instance = get_instance();
		
				float* pOutputF32 = (float*)pOutput;
		
				assert(pDevice->playback.format == SAMPLE_FORMAT);   /* <-- Important for simple mixing. */
		
				for (Audio& audio : instance.audio_to_play)
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
				if (instance.are_all_decoders_at_end())
				{
					instance.audio_to_play.clear();
				}
		
				(void)pInput;
			}
		};
	};
}