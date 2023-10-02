
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
void wasapi_init_audio(audio_sample_t* output, CORE_AUDIO_MIXER_PROC mixer_proc);

typedef struct {
	// WAVEOUT
	HWAVEOUT hwaveout;

	// WASAPI
	CORE_AUDIO_MIXER_PROC mixer_proc;
	IAudioClient* audio_client;
	IAudioRenderClient* audio_render_client;

	// 64 concurrent sounds
	audio_sound_t sounds[64];
	int sounds_count;
	core_critical_section_t sounds_lock;

	b32 reload;
} core_audio_t;

b32 core_init_audio(core_audio_t* audio, CORE_AUDIO_MIXER_PROC mixer_proc, u32 flags) {
	// init audio
	core_init_critical_section(&audio->sounds_lock);
	wasapi_init_audio(audio, mixer_proc);
}

void core_play_sound(core_audio_t* audio, audio_buffer_t* buffer, float volume) {
	core_enter_critical_section(&audio->sounds_lock);
	if (audio->sounds_count < 64) {
		audio_sound_t sound = {
			.cursor = 0.0f,
			.buffer = buffer,
			.volume = volume,
		};
		audio->sounds[audio->sounds_count++] = sound;
	}
	core_exit_critical_section(&audio->sounds_lock);
}

void CORE_DEFAULT_AUDIO_MIXER_PROC(audio_sample_t* output, size_t sample_count, void* userp) {
	core_audio_t* audio = userp;
	memset(output, 0, sample_count * sizeof(audio_sample_t));

	int count = atomic_read32(&audio->sounds_count);
	FOR(i, count) {
		audio_sound_t* sound = audio->sounds + i;
		float speed_diff = (f32)sound->buffer->samples_per_second /
			(f32)CORE_AUDIO_SAMPLES_PER_SECOND;
		size_t sound_samples_remaining = (f32)(sound->buffer->sample_count-sound->cursor) * speed_diff;
		
		// size_t buffer_
		size_t samples_to_mix = imin(sample_count, sound_samples_remaining);
		if (samples_to_mix == 0) {
			int x = 0;
		}
		// size_t samples_to_mix = sample_count;
		if (sound->buffer->channels == 1) {
			FOR(isample, samples_to_mix) {
				u32 buffer_index = sound->cursor;
				audio_sample_t* sample = &sound->buffer->data[buffer_index];
				i16 amp = sample->left;
				output[isample].left += amp;
				output[isample].right += amp;
				// output[isample].left += sound->buffer->data[buffer_index].left;
				// output[isample].right += sound->buffer->data[buffer_index].right;
				sound->cursor += speed_diff * 0.5f;
			}
		} else {
			FOR(isample, samples_to_mix) {
				u32 buffer_index = sound->cursor;
				output[isample].left += (f32)sound->buffer->data[buffer_index].left * sound->volume;
				output[isample].right += (f32)sound->buffer->data[buffer_index].right * sound->volume;
				sound->cursor += speed_diff;
			}
		}
		// sound->cursor += samples_to_mix;

		core_enter_critical_section(&audio->sounds_lock);
		if (sound_samples_remaining < 1/*sound->cursor >= sound->buffer->sample_count*/) {
			int copy_count = audio->sounds_count - i - 1;
			memcpy(audio->sounds + i, audio->sounds + i + 1, copy_count * sizeof(audio_sound_t));
			--audio->sounds_count;
			--count;
			--i;
		}
		core_exit_critical_section(&audio->sounds_lock);
	}

	FOR(i, audio->sounds_count) {

	}
}

b32 cpp_wasapi_init_audio(core_audio_t* audio) {
	printf("initializing wasapi audio \n");
#define AssertSucceeded(hr)\
	if(!SUCCEEDED(hr)) {\
		printf("it broke \n");\
		return;\
	}

	// IAudioClient* audio_client;
	// IAudioRenderClient* audio_render_client;
	//
	// CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	// IMMDeviceEnumerator* device_enumerator;
	// HRESULT hresult;
	// if (!SUCCEEDED(CoCreateInstance(
	// 		&CLSID_MMDeviceEnumerator,
	// 		NULL,
	// 		CLSCTX_ALL,
	// 		&IID_IMMDeviceEnumerator,
	// 		&device_enumerator))) {
	// 	// printf();
	// }

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
	// core_error()
	return FALSE;
}

