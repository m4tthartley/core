//
//  audio.h
//  Core
//
//  Created by Matt Hartley on 22/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "core.h"
// #include "math.c"

#define COBJMACROS
#include <initguid.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#define CORE_AUDIO_SAMPLES_PER_SECOND 48000
#define CORE_AUDIO_SAMPLE_BYTES 2
#define CORE_AUDIO_CHANNELS 2

typedef struct {
	union {
		i16 channels[2];
		struct {
			i16 left;
			i16 right;
		};
	};
} audio_sample_t;

typedef struct {
	int channels;
	int samples_per_second;
	int bytes_per_sample;
	size_t sample_count;
	audio_sample_t data[];
} audio_buffer_t;
typedef audio_buffer_t wave_t;

typedef struct {
	audio_buffer_t* buffer;
	f32 cursor;
	float volume;
} audio_sound_t;

typedef void (*CORE_AUDIO_MIXER_PROC)(audio_sample_t* output, size_t sample_count, void* userp);

typedef struct {
	// WAVEOUT
	HWAVEOUT hwaveout;

	// WASAPI
	CORE_AUDIO_MIXER_PROC mixer_proc;
	IAudioClient* audio_client;
	IAudioRenderClient* audio_render_client;

	audio_sound_t sounds[64];
	int sounds_count;
	core_critical_section_t sounds_lock;

	b32 reload;
} core_audio_t;

b32 core_init_audio(core_audio_t* audio, CORE_AUDIO_MIXER_PROC mixer_proc, u32 flags);
void core_play_sound(core_audio_t* audio, audio_buffer_t* buffer, float volume);
b32 wasapi_init_audio(core_audio_t* audio);
DWORD wasapi_audio_thread(void* arg);
void CORE_AUDIO_DEFAULT_MIXER_PROC(audio_sample_t* output, size_t sample_count, void* userp);
