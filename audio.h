
// C interface for cpp wasapi stuff
#include "core.h"
#include "math.c"

#include <mmdeviceapi.h>
#include <audioclient.h>

#define CORE_AUDIO_SAMPLES_PER_SECOND 48000

void _mix_samples_proc(i16* output, u32 num_samples);

typedef struct {
	audio_buffer_t* buffer;
	// Cursor relative to the buffer's samples
	f32 cursor;
	float volume;
} audio_sound_t;

typedef void (*CORE_AUDIO_MIXER_PROC)(audio_sample_t* output, size_t sample_count, void* userp);
void wasapi_init_audio(CORE_AUDIO_MIXER_PROC mixer_proc);

// 64 concurrent sounds
audio_sound_t _sounds[64];
int sounds_count = 0;
core_critical_section_t sounds_lock = {};

b32 core_init_audio(CORE_AUDIO_MIXER_PROC mixer_proc, u32 flags) {
	// init audio
	core_init_critical_section(&sounds_lock);
	wasapi_init_audio(mixer_proc);
}

void core_play_sound(audio_buffer_t* buffer, float volume) {
	core_enter_critical_section(&sounds_lock);
	if (sounds_count < 64) {
		audio_sound_t sound = {
			.cursor = 0.0f,
			.buffer = buffer,
			.volume = volume,
		};
		_sounds[sounds_count++] = sound;
	}
	core_exit_critical_section(&sounds_lock);
}

void CORE_DEFAULT_AUDIO_MIXER_PROC(audio_sample_t* output, size_t sample_count, void* userp) {
	memset(output, 0, sample_count * sizeof(audio_sample_t));

	int count = atomic_read32(&sounds_count);
	FOR(i, count) {
		audio_sound_t* sound = _sounds + i;
		float speed_diff = (f32)sound->buffer->samples_per_second /
			(f32)CORE_AUDIO_SAMPLES_PER_SECOND;
		size_t sound_samples_remaining = (f32)(sound->buffer->sample_count-sound->cursor) * speed_diff;
		
		// size_t buffer_
		size_t samples_to_mix = imin(sample_count, sound_samples_remaining);
		// size_t samples_to_mix = sample_count;
		if (sound->buffer->channels == 1) {
			FOR(isample, samples_to_mix) {
				u32 buffer_index = sound->cursor;
				output[isample].left += sound->buffer->data[buffer_index].left;
				output[isample].right += sound->buffer->data[buffer_index].left;
				sound->cursor += speed_diff * 0.5f;
			}
		} else {
			FOR(isample, samples_to_mix) {
				u32 buffer_index = sound->cursor;
				output[isample].left += sound->buffer->data[buffer_index].left;
				output[isample].right += sound->buffer->data[buffer_index].right;
				sound->cursor += speed_diff;
			}
		}
		// sound->cursor += samples_to_mix;

		core_enter_critical_section(&sounds_lock);
		if (sound_samples_remaining < 1/*sound->cursor >= sound->buffer->sample_count*/) {
			int copy_count = sounds_count - i - 1;
			memcpy(_sounds + i, _sounds + i + 1, copy_count * sizeof(audio_sound_t));
			--sounds_count;
			--i;
		}
		core_exit_critical_section(&sounds_lock);
	}

	FOR(i, sounds_count) {

	}
}

b32 cpp_wasapi_init_audio() {
	printf("initializing wasapi audio \n");
#define AssertSucceeded(hr)\
	if(!SUCCEEDED(hr)) {\
		printf("it broke \n");\
		return;\
	}

	IAudioClient* audio_client;
	IAudioRenderClient* audio_render_client;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	IMMDeviceEnumerator* device_enumerator;
	HRESULT hresult;
	if (!SUCCEEDED(CoCreateInstance(
			&CLSID_MMDeviceEnumerator,
			NULL,
			CLSCTX_ALL,
			&IID_IMMDeviceEnumerator,
			&device_enumerator)) {
		printf();
	}
	// AssertSucceeded(hresult);
	//
	// IMMDevice* audio_device;
	// hresult = device_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audio_device);
	// AssertSucceeded(hresult);
	//
	// audio_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, 0, (void**)&audio_client);
	// AssertSucceeded(hresult);
	//
	// REFERENCE_TIME device_period;
	// audio_client->GetDevicePeriod(0, &device_period);
	// AssertSucceeded(hresult);
	//
	// WAVEFORMATEX format = {0};
	// format.wFormatTag = WAVE_FORMAT_PCM;
	// format.nChannels = 2;
	// format.nSamplesPerSec = CORE_AUDIO_SAMPLES_PER_SECOND;
	// format.wBitsPerSample = 16;
	// format.nBlockAlign = 4;
	// format.nAvgBytesPerSec = CORE_AUDIO_SAMPLES_PER_SECOND * format.nBlockAlign;
	//
	// hresult = audio_client->Initialize(
	// 		AUDCLNT_SHAREMODE_SHARED,
	// 		AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
	// 		device_period,
	// 		0,
	// 		&format,
	// 		0);
	// AssertSucceeded(hresult);
	//
	// AssertSucceeded(audio_client->GetService(IID_PPV_ARGS(&audio_render_client)));
	//
	// device_enumerator->Release();
	//
	// CreateThread(0, 0, wasapi_audio_thread, NULL, 0, 0);

	return TRUE;

done:
	core_error()
	return FALSE
}

