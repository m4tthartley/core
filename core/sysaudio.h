//
//  Created by Matt Hartley on 22/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_AUDIO_HEADER__
#define __CORE_AUDIO_HEADER__


#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>


#undef _True
#undef _False
#define _True ((_Bool)1)
#define _False ((_Bool)0)


#define CORE_AUDIO_SAMPLES_PER_SECOND 48000
#define CORE_AUDIO_SAMPLE_BYTES 2
#define CORE_AUDIO_CHANNELS 2

typedef struct {
	union {
		uint8_t channels[2];
		struct {
			uint8_t left;
			uint8_t right;
		};
	};
} audio_sample8_t;
typedef struct {
	union {
		int16_t channels[2];
		struct {
			int16_t left;
			int16_t right;
		};
	};
} audio_sample16_t;
typedef struct {
	union {
		int32_t channels[2];
		struct {
			int32_t left;
			int32_t right;
		};
	};
} audio_sample32_t;
typedef struct {
	union {
		float channels[2];
		struct {
			float left;
			float right;
		};
	};
} audio_sample_float32_t;
typedef struct {
	union {
		double channels[2];
		struct {
			double left;
			double right;
		};
	};
} audio_sample_float64_t;

typedef enum {
	SYSAUDIO_FORMAT_NONE =  0x00,
	SYSAUDIO_FORMAT_INT8 =  0x01,
	SYSAUDIO_FORMAT_INT16 = 0x02,
	SYSAUDIO_FORMAT_INT24 = 0x03,
	SYSAUDIO_FORMAT_INT32 = 0x04,
	SYSAUDIO_FORMAT_FLOAT32 = 0x05,
	SYSAUDIO_FORMAT_FLOAT64 = 0x06,
} audio_buffer_format_t;

typedef struct {
	audio_buffer_format_t format;
	int channels;
	int sampleRate;
	int sampleSize;
	size_t sampleCount;
	uint8_t data[];
} audio_buffer_t;

typedef struct {
	audio_buffer_t* buffer;
	double cursor;
	float volume;
} audio_sound_t;

typedef void (*SYSAUDIO_MIXER_PROC)(void* sysaudio, void* output, size_t sample_count);

typedef enum {
	SYSAUDIO_SAMPLE_RATE_8K = 8000,
	SYSAUDIO_SAMPLE_RATE_11K = 11025,
	SYSAUDIO_SAMPLE_RATE_16K = 16000,
	SYSAUDIO_SAMPLE_RATE_22K = 22050,
	SYSAUDIO_SAMPLE_RATE_44K = 44100,
	SYSAUDIO_SAMPLE_RATE_48K = 48000,
	SYSAUDIO_SAMPLE_RATE_88K = 88200,
	SYSAUDIO_SAMPLE_RATE_96K = 96000,
} sysaudio_sample_rate_t;

typedef struct {
	audio_buffer_format_t format;
	sysaudio_sample_rate_t sampleRate;
} sysaudio_spec_t;

typedef struct {
#ifdef __WIN32__
	_Bool thread_lock;
	// WAVEOUT
	HWAVEOUT hwaveout;
	// WASAPI
	CORE_AUDIO_MIXER_PROC mixer_proc;
	IAudioClient* audio_client;
	IAudioRenderClient* audio_render_client;
#endif
#ifdef __APPLE__
	void* outputUnit;
#endif
	
	_Atomic SYSAUDIO_MIXER_PROC mixer;
	void* dataForMixer;

	audio_buffer_format_t format;
	sysaudio_sample_rate_t sampleRate;
	int sampleSize;
	
	audio_sound_t music;
	_Bool musicFade;
	audio_sound_t sounds[64];
	int sounds_count;
	
	_Bool reload;
} sysaudio_t;

#define SYSAUDIO_DEFAULT_SPEC ((sysaudio_spec_t){ .sampleRate=176400, .format=SYSAUDIO_FORMAT_INT32 })

void _sys_audio_print(char* str);
_Bool sys_init_audio(sysaudio_t* audio, sysaudio_spec_t spec);
void sys_start_audio(sysaudio_t* audio);
void sys_stop_audio(sysaudio_t* audio);
void sys_set_audio_callback(sysaudio_t* audio, SYSAUDIO_MIXER_PROC mixer);
void sys_play_sound(sysaudio_t* audio, audio_buffer_t* buffer, float volume);
void sysaudio_default_mixer(void* sysaudio, void* output, size_t sample_count);


#endif


#ifdef CORE_IMPL
#	ifndef __CORE_AUDIO_HEADER_IMPL__
#	define __CORE_AUDIO_HEADER_IMPL__


void sys_play_sound(sysaudio_t* audio, audio_buffer_t* buffer, float volume) {
	for (int i=0; i<64; ++i) {
		if (!audio->sounds[i].buffer) {
			audio->sounds[i] = (audio_sound_t){
				.buffer = buffer,
				.cursor = 0,
				.volume = volume,
			};
			return;
		}
	}
	_sys_audio_print("Out of sound slots");
}

void sys_play_music(sysaudio_t* audio, audio_buffer_t* buffer, float volume) {
	audio->music = (audio_sound_t){
		.buffer = buffer,
		.cursor = 0.0f,
		.volume = volume,
	};
	audio->musicFade = _False;
}

float _lerp_float(float a, float b, float t) {
	return a + (b-a) * t;
}

audio_sample_float64_t _lerp_float_sample(audio_sample_float64_t a, audio_sample_float64_t b, float t) {
	audio_sample_float64_t result = {
		.left = _lerp_float(a.left, b.left, t),
		.right = _lerp_float(a.right, b.right, t),
	};
	return result;
}

void _mix_sample(sysaudio_t* audio, uint8_t* output, audio_sound_t* sound) {
	audio_buffer_t* buffer = sound->buffer;

	int index0 = sound->cursor;
	int index1 = index0 + 1;
	float t = sound->cursor - floorf(sound->cursor);
	audio_sample_float64_t sample0;
	audio_sample_float64_t sample1;

	if (buffer->format < SYSAUDIO_FORMAT_FLOAT32) {
		int shiftAmount = (4-buffer->sampleSize)*8;
		int stride = buffer->sampleSize*2;

		uint8_t* left0Raw = buffer->data + index0*stride;
		uint8_t* right0Raw = buffer->data + index0*stride+buffer->sampleSize;
		uint8_t* left1Raw = buffer->data + index1*stride;
		uint8_t* right1Raw = buffer->data + index1*stride+buffer->sampleSize;

		int32_t left0Scaled;
		int32_t right0Scaled;
		int32_t left1Scaled;
		int32_t right1Scaled;

		if (sound->buffer->sampleSize==1) {
			left0Scaled = (*left0Raw - 128) << shiftAmount;
			right0Scaled = (*right0Raw - 128) << shiftAmount;
			left1Scaled = (*left1Raw - 128) << shiftAmount;
			right1Scaled = (*right1Raw - 128) << shiftAmount;
		} else {
			int32_t left0Signed = *(int32_t*)left0Raw;
			int32_t right0Signed = *(int32_t*)right0Raw;
			int32_t left1Signed = *(int32_t*)left1Raw;
			int32_t right1Signed = *(int32_t*)right1Raw;
			// for (int i=0; i<buffer->sampleSize; ++i) {
			// 	left0Signed |= left0Raw[i] << (i*8);
			// 	right0Signed |= right0Raw[i] << (i*8);
			// 	left1Signed |= left1Raw[i] << (i*8);
			// 	right1Signed |= right1Raw[i] << (i*8);
			// }

			left0Scaled = left0Signed << shiftAmount;
			right0Scaled = right0Signed << shiftAmount;
			left1Scaled = left1Signed << shiftAmount;
			right1Scaled = right1Signed << shiftAmount;
		}

		sample0 = (audio_sample_float64_t){
			.left = (int32_t)left0Scaled,
			.right = (int32_t)right0Scaled,
		};
		sample1 = (audio_sample_float64_t){
			.left = (int32_t)left1Scaled,
			.right = (int32_t)right1Scaled,
		};
	}
	else {
		if (buffer->sampleSize==4) {
			audio_sample_float32_t* input = (audio_sample_float32_t*)buffer->data;
			sample0.left = input[index0].left * 0x7FFFFFFF;
			sample0.right = input[index0].right * 0x7FFFFFFF;
			sample1.left = input[index1].left * 0x7FFFFFFF;
			sample1.right = input[index1].right * 0x7FFFFFFF;
		}
		if (buffer->sampleSize==8) {
			audio_sample_float64_t* input = (audio_sample_float64_t*)buffer->data;
			sample0.left = input[index0].left * 0x7FFFFFFF;
			sample0.right = input[index0].right * 0x7FFFFFFF;
			sample1.left = input[index1].left * 0x7FFFFFFF;
			sample1.right = input[index1].right * 0x7FFFFFFF;
		}
	}

	audio_sample_float64_t lerpedSample = _lerp_float_sample(sample0, sample1, t);

	lerpedSample.left *= sound->volume;
	lerpedSample.right *= sound->volume;

	if (audio->format < SYSAUDIO_FORMAT_FLOAT32) {
		int32_t resultLeft = (int32_t)lerpedSample.left;
		int32_t resultRight = (int32_t)lerpedSample.right;
		
		int outputShift = (4 - audio->sampleSize) * 8;
		for (int byte=0; byte<audio->sampleSize; ++byte) {
			output[byte] = (resultLeft >> (outputShift + byte*8)) & 0xFF;
			output[audio->sampleSize+byte] = (resultRight >> (outputShift + byte*8)) & 0xFF;
		}
	}
	else {
		if (audio->sampleSize==4) {
			float left = lerpedSample.left / 0x7FFFFFFF;
			float right = lerpedSample.right / 0x7FFFFFFF;
			float* out = (float*)output;
			out[0] = left;
			out[1] = right;
		}
		if (audio->sampleSize==8) {
			double left = lerpedSample.left / 0x7FFFFFFF;
			double right = lerpedSample.right / 0x7FFFFFFF;
			double* out = (double*)output;
			out[0] = left;
			out[1] = right;
		}
	}
}

void _mix_sine_wave32(int32_t* buffer, int sampleCount) {
	static int cursor = 0;
	for (int i=0; i<sampleCount; ++i) {
		double wave = sinf(440.0f * (3.14159265359f*2.0f) * (double)cursor / 44100.0f) * 0.1f;
		int32_t sample = wave * (double)0x7FFFFFFF;
		// int32_t sample = rand() / 10;
		buffer[i*2] += sample;
		buffer[i*2+1] += sample;
		++cursor;
	}
}

void sysaudio_default_mixer(void* sysaudio, void* buffer, size_t sampleCount) {
	sysaudio_t* audio = sysaudio;

	if (audio->music.buffer) {
		int musicSamples = audio->music.buffer->sampleCount - (int)audio->music.cursor;
		int samplesToMix = sampleCount;
		if (musicSamples < samplesToMix) {
			samplesToMix = musicSamples;
		}

		double sampleRateRatio = (double)audio->music.buffer->sampleRate / (double)audio->sampleRate;

		for (int i=0; i<samplesToMix; ++i) {
			uint8_t* output = buffer + (i*audio->sampleSize*2);

			_mix_sample(audio, output, &audio->music);
			audio->music.cursor += sampleRateRatio * 1.0f;
		}

		if (audio->musicFade) {
			audio->music.volume -= 0.5f / ((float)audio->sampleRate / (float)sampleCount);
		}

		if ((int)audio->music.cursor >= (audio->music.buffer->sampleCount) || audio->music.volume <= 0.0f) {
			audio->music.buffer = NULL;
		}
	}

	for (int isound=0; isound<64; ++isound) {
		audio_sound_t* sound = audio->sounds + isound;
		if (sound->buffer) {
			int waveSamples = sound->buffer->sampleCount - (int)sound->cursor;
			int samplesToMix = sampleCount;
			if (waveSamples < samplesToMix) {
				samplesToMix = waveSamples;
			}

			float sampleRateRatio = (float)sound->buffer->sampleRate / (float)audio->sampleRate;

			for (int i=0; i<samplesToMix; ++i) {
				uint8_t* output = (uint8_t*)buffer + (i*audio->sampleSize*2);

				_mix_sample(audio, (uint8_t*)output, sound);

				sound->cursor += sampleRateRatio * 1.0f;
			}

			if ((int)sound->cursor >= (sound->buffer->sampleCount)) {
				sound->buffer = NULL;
			}
		}
	}
}


#ifdef __APPLE__
#	include "sysaudio_osx.m"
#endif
#ifdef __LINUX__
#	include "sysaudio_linux.h"
#endif
#ifdef __WIN32__
#	include "sysaudio_win32.h"
#endif


#	endif
#endif
