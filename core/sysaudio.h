//
//  Created by Matt Hartley on 22/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_AUDIO_HEADER__
#define __CORE_AUDIO_HEADER__


// #include "core.h"
// #include "math.c"
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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
} audio_sample_float_t;

typedef struct {
	int channels;
	int sampleRate;
	int sampleSize;
	size_t sampleCount;
	uint8_t data[];
} audio_buffer_t;
typedef audio_buffer_t wave_t;

typedef struct {
	audio_buffer_t* buffer;
	float cursor;
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

// A single channels sample size in bytes
typedef enum {
	SYSAUDIO_SAMPLE_SIZE_1 = 1,
	SYSAUDIO_SAMPLE_SIZE_2 = 2,
	SYSAUDIO_SAMPLE_SIZE_3 = 3,
	SYSAUDIO_SAMPLE_SIZE_4 = 4,
} sysaudio_sample_size_t;

typedef struct {
	SYSAUDIO_MIXER_PROC mixer;
	sysaudio_sample_rate_t sampleRate;
	sysaudio_sample_size_t sampleSize;
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
	
	// SYSAUDIO_MIXER_PROC mixer;
	sysaudio_spec_t spec;
	void* dataForMixer;
	
	audio_sound_t sounds[64];
	int sounds_count;
	
	_Bool reload;
} sysaudio_t;

#define SYSAUDIO_DEFAULT_SPEC ((sysaudio_spec_t){ .mixer=sysaudio_default_mixer, .sampleRate=44100/*176400*/, .sampleSize=4 })

_Bool sys_init_audio(sysaudio_t* audio, sysaudio_spec_t spec);
void sys_set_audio_callback(sysaudio_t* audio, SYSAUDIO_MIXER_PROC mixer);
void sys_play_sound(sysaudio_t* audio, audio_buffer_t* buffer, float volume);
void sysaudio_default_mixer(void* sysaudio, void* output, size_t sample_count);


#endif


#ifdef CORE_IMPL
#	ifndef __CORE_AUDIO_HEADER_IMPL__
#	define __CORE_AUDIO_HEADER_IMPL__


audio_sample32_t _mix_sample32_from_sample16(audio_sample16_t* data, float cursor) {
	// int cursorInt
	audio_sample16_t sample1 = data[(int)cursor];
	audio_sample16_t sample2 = data[(int)cursor + 1];
	float t = cursor - floorf(cursor);

	float scaler = 65535;
	// double sample0 = (double)data[0].left * scaler;
	// double sample1 = (double)data[0].right * scaler;
	float left = sample1.left + (sample2.left-sample1.left) * t;
	float right = sample1.right + (sample2.right-sample1.right) * t;
	audio_sample32_t output = {
		.left = left * scaler,
		.right = right * scaler,
	};
	return output;
}

float _lerp_float(float a, float b, float t) {
	return a + (b-a) * t;
}

audio_sample_float_t _lerp_float_sample(audio_sample_float_t a, audio_sample_float_t b, float t) {
	audio_sample_float_t result = {
		.left = _lerp_float(a.left, b.left, t),
		.right = _lerp_float(a.right, b.right, t),
	};
	return result;
}

audio_sample32_t _mix_sample32(audio_buffer_t* buffer, float cursor) {
	int shiftAmount = (4-buffer->sampleSize)*8;
	int index0 = cursor;
	int index1 = index0 + 1;
	int stride = buffer->sampleSize*2*2;
	float t = cursor - floorf(cursor);

	int32_t* left0Raw = (int32_t*)(buffer->data + index0*stride);
	int32_t* right0Raw = (int32_t*)(buffer->data + index0*stride+buffer->sampleSize);
	int32_t* left1Raw = (int32_t*)(buffer->data + index1*stride);
	int32_t* right1Raw = (int32_t*)(buffer->data + index1*stride+buffer->sampleSize);

	audio_sample_float_t sample0 = {
		.left = *left0Raw << shiftAmount,
		.right = *right0Raw << shiftAmount,
	};
	audio_sample_float_t sample1 = {
		.left = *left1Raw << shiftAmount,
		.right = *right1Raw << shiftAmount,
	};

	audio_sample_float_t lerpedSample = _lerp_float_sample(sample0, sample1, t);
	
	audio_sample32_t output = {
		.left = lerpedSample.left,
		.right = lerpedSample.right,
		// .left = *left0Raw,
		// .right = *right0Raw,
	};
	return output;
}

void sysaudio_default_mixer(void* sysaudio, void* buffer, size_t sampleCount) {
	sysaudio_t* audio = sysaudio;
	// int32_t* buffer = output;

	for (int z=0; z<sampleCount; ++z) {
		// buffer[z*2] = 0;
		// buffer[z*2+1] = 0;
		memset(buffer, 0, sizeof(audio_sample32_t)*sampleCount);
	}

	// static int cursor = 0;
	// for (int i=0; i<sampleCount; ++i) {
	// 	// double wave = sinf(440.0f * (3.14159265359f*2.0f) * (double)cursor / 44100.0f) * 0.1f;
	// 	// int32_t sample = wave * (double)0x7FFFFFFF;
	// 	int32_t sample = rand() / 10;
	// 	buffer[i*2] += sample;
	// 	buffer[i*2+1] += sample;
	// 	++cursor;
	// }

	for (int isound=0; isound<64/*array_size(audio->sounds)*/; ++isound) {
		audio_sound_t* sound = audio->sounds + isound;
		if (sound->buffer) {
			int waveSamples = sound->buffer->sampleCount - (int)sound->cursor;
			int samplesToMix = sampleCount;
			if (waveSamples < samplesToMix) {
				samplesToMix = waveSamples;
			}

			float sampleRateRatio = (float)sound->buffer->sampleRate / (float)audio->spec.sampleRate;

			// int32_t* soundData = sound->buffer->data;
			// for (int i=0; i<samplesToMix; ++i) {
			// 	int32_t* output = buffer;
			// 	int32_t left = soundData[(int)sound->cursor] / 10 /*(float)0x7FFF*/;
			// 	int32_t right = soundData[(int)sound->cursor+1] / 10 /*(float)0x7FFF*/;
			// 	output[i*2] += left * sound->volume;
			// 	output[i*2+1] += right * sound->volume;
			// 	sound->cursor += 2.0f;
			// }

			audio_sample16_t* soundData = sound->buffer->data;
			for (int i=0; i<samplesToMix; ++i) {
				// audio_sample32_t sample = _mix_sample32_from_sample16(soundData, sound->cursor);
				audio_sample32_t sample = _mix_sample32(sound->buffer, sound->cursor);
				audio_sample32_t* output = buffer;
				output[i].left += sample.left * sound->volume;
				output[i].right += sample.right * sound->volume;
				sound->cursor += sampleRateRatio;
			}

			if ((int)sound->cursor >= (sound->buffer->sampleCount*sound->buffer->channels)) {
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
