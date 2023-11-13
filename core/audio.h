
// C interface for cpp wasapi stuff
#include "core.h"
#include "math.c"

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

b32 wasapi_init_audio(core_audio_t* audio);
DWORD wasapi_audio_thread(void* arg);
void CORE_AUDIO_DEFAULT_MIXER_PROC(audio_sample_t* output, size_t sample_count, void* userp);

u32 terminate_threads = 0;

b32 core_init_audio(core_audio_t* audio, CORE_AUDIO_MIXER_PROC mixer_proc, u32 flags) {
	core_init_critical_section(&audio->sounds_lock);

	if (mixer_proc) {
		audio->mixer_proc = mixer_proc;
	} else {
		audio->mixer_proc = CORE_AUDIO_DEFAULT_MIXER_PROC;
	}

	b32 wasapi_result = wasapi_init_audio(audio);

	if (wasapi_result) {
		CreateThread(0, 0, wasapi_audio_thread, audio, 0, 0);
		return TRUE;
	}
}

void core_play_sound(core_audio_t* audio, audio_buffer_t* buffer, float volume) {
	if (buffer) {
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
}

void CORE_AUDIO_DEFAULT_MIXER_PROC(audio_sample_t* output, size_t sample_count, void* userp) {
	core_audio_t* audio = userp;
	memset(output, 0, sample_count * sizeof(audio_sample_t));

	int count = atomic_read32(&audio->sounds_count);
	FOR(i, count) {
		audio_sound_t* sound = audio->sounds + i;
		float speed_diff = (f32)sound->buffer->samples_per_second /
			(f32)CORE_AUDIO_SAMPLES_PER_SECOND;
		size_t sound_samples_remaining = (f32)(sound->buffer->sample_count-sound->cursor) * speed_diff;
		
		size_t samples_to_mix = imin(sample_count, sound_samples_remaining);
		if (samples_to_mix == 0) {
			int x = 0;
		}

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

		core_enter_critical_section(&audio->sounds_lock);
		if (sound_samples_remaining < 1) {
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

b32 wasapi_init_audio(core_audio_t* audio) {
	IAudioClient* audio_client;
	IAudioRenderClient* audio_render_client;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	HRESULT hresult;

	IMMDeviceEnumerator* device_enumerator;
	hresult = CoCreateInstance(
			&CLSID_MMDeviceEnumerator,
			NULL,
			CLSCTX_ALL,
			&IID_IMMDeviceEnumerator,
			&device_enumerator);
	if (FAILED(hresult)) {
		core_win32_error(hresult, FALSE, "Failed to initialize device enumerator");
		goto done;
	}

	IMMDevice* audio_device;
	hresult = IMMDeviceEnumerator_GetDefaultAudioEndpoint(
			device_enumerator,
			eRender,
			eConsole,
			&audio_device);
	if (FAILED(hresult)) {
		core_win32_error(hresult, FALSE, "Failed to get default audio endpoint");
		goto done;
	}

	hresult = IMMDevice_Activate(
			audio_device,
			&IID_IAudioClient,
			CLSCTX_ALL,
			0,
			(void**)&audio_client);
	if (FAILED(hresult)) {
		core_win32_error(hresult, FALSE, "Failed to activate audio device");
		goto done;
	}

	REFERENCE_TIME device_period;
	hresult = IAudioClient_GetDevicePeriod(audio_client, 0, &device_period);
	if (FAILED(hresult)) {
		core_win32_error(hresult, FALSE, "Failed to get device period");
		goto done;
	}

	WAVEFORMATEX format = {0};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = CORE_AUDIO_CHANNELS;
	format.nSamplesPerSec = CORE_AUDIO_SAMPLES_PER_SECOND;
	format.wBitsPerSample = CORE_AUDIO_SAMPLE_BYTES*8;
	format.nBlockAlign = CORE_AUDIO_CHANNELS*CORE_AUDIO_SAMPLE_BYTES;
	format.nAvgBytesPerSec = CORE_AUDIO_SAMPLES_PER_SECOND * format.nBlockAlign;

	hresult = IAudioClient_Initialize(
			audio_client,
			AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			device_period,
			0,
			&format,
			0);
	if (FAILED(hresult)) {
		core_win32_error(hresult, FALSE, "Failed to initialize audio client");
		goto done;
	}

	hresult = IAudioClient_GetService(audio_client, &IID_IAudioRenderClient, &audio_render_client);
	if (FAILED(hresult)) {
		core_win32_error(hresult, FALSE, "Faield to initialize audio render client");
		goto done;
	}

	IMMDeviceEnumerator_Release(device_enumerator);
	IMMDevice_Release(audio_device);

	audio->audio_client = audio_client;
	audio->audio_render_client = audio_render_client;

	printf("WASAPI initialized \n");
	return TRUE;

done:
	printf("Continuing without audio...");
	return FALSE;
}

DWORD wasapi_audio_thread(void* arg) {
	core_audio_t* audio = (core_audio_t*)arg;
	HRESULT hr;
	// TODO sort out the error handling dude
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	HANDLE ready_event = CreateEvent(0, 0, 0, 0);
	hr = IAudioClient_SetEventHandle(audio->audio_client, ready_event);
	if (FAILED(hr)) {
		core_win32_error(hr, FALSE, "Failed to set audio client event handler");
		goto done;
	}

	u32 buffer_frames;
	hr = IAudioClient_GetBufferSize(audio->audio_client, &buffer_frames);
	if (FAILED(hr)) {
		core_win32_error(hr, FALSE, "Failed to get buffer size");
		goto done;
	}

	// u32 buffer_size = buffer_frames * 4;
	hr = IAudioClient_Start(audio->audio_client);
	if (FAILED(hr)) {
		core_win32_error(hr, FALSE, "Failed to start audio client");
		goto done;
	}

#if 0
	// Clear buffer to zero
	HRESULT hresult = audio_render_client->GetBuffer(buffer_frames, (BYTE**)&buffer);
	for(int i=0; i<buffer_frames; ++i) {
		buffer[i].s[0] = 0;
		buffer[i].s[1] = 0;
	}
	audio_render_client->ReleaseBuffer(buffer_frames, 0);
#endif
	
	audio_sample_t* buffer;
	for(;;) {
		if (atomic_read32(&terminate_threads)) {
			audio->reload = TRUE;
			break;
		}

		DWORD wait = WaitForSingleObject(ready_event, INFINITE);
		if(wait != WAIT_OBJECT_0) {
			printf("WAIT OBJECT %i \n", wait);
			exit(1);
		}

		u32 buffer_padding;
		hr = IAudioClient_GetCurrentPadding(audio->audio_client, &buffer_padding);
		if(FAILED(hr)) {
			printf("GetCurrentPadding failed \n");
			continue;
		}
		u32 frames_to_fill = buffer_frames - buffer_padding;
		if(!frames_to_fill) {
			continue;
		}

		hr = IAudioRenderClient_GetBuffer(
				audio->audio_render_client,
				frames_to_fill,
				(BYTE**)&buffer);
		if(SUCCEEDED(hr)) {
			// for(int i=0; i<frames_to_fill; ++i) {
			// 	float sample = sinf(tick * 0.1f) * 255.0f * 4.0f;
			// 	buffer[i].s[0] = sample;
			// 	buffer[i].s[1] = sample;
			// 	mod += 0.00001f;
			// 	tick += 1 + mod;
			// }
			audio->mixer_proc(buffer, frames_to_fill, audio);
		} else {
			printf("GetBuffer failed (%s) \n", "hresult");
			continue;
		}

		HRESULT release_result = IAudioRenderClient_ReleaseBuffer(
				audio->audio_render_client,
				frames_to_fill,
				0);
		if(FAILED(release_result)) {
			printf("ReleaseBuffer failed \n");
		}
	}

	printf("exiting audio thread \n");
	return 0;

done:
	printf("Continuing without audio... \n");
	return 1;
}

