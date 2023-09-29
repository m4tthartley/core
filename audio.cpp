
// Win32 audio using WASAPI

#include <stdio.h>
#include <math.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <stdint.h>

// #include "core.h"
// extern "C" {
// #include "audio.h"
// }

typedef __int64  i64;
typedef unsigned __int64  u64;
typedef __int32  i32;
typedef unsigned __int32  u32;
typedef __int16  i16;
typedef unsigned __int16  u16;
typedef __int8  i8;
typedef unsigned __int8  u8;

typedef float f32;
typedef double f64;

#define CORE_AUDIO_SAMPLES_PER_SECOND 48000

typedef struct {
	union {
		i16 channels[2];
		struct {
			i16 left;
			i16 right;
		};
	};
} audio_sample_t;
typedef void (*CORE_AUDIO_MIXER_PROC)(audio_sample_t* output, size_t sample_count, void* userp);


// void _mix_samples_proc(i16* output, u32 num_samples);

extern "C" char* _win32_hresult_string(HRESULT hresult);

// typedef struct {
// 	i16 s[2];
// } sample_t;
// sample_t* buffer;

CORE_AUDIO_MIXER_PROC _audio_mixer_proc = NULL;

IAudioClient* audio_client;
IAudioRenderClient* audio_render_client;

float tick = 0;
float mod = 0;

wchat_t _win32_error_buffer[1024];
char* _win32_hresult_string(HRESULT hresult) {
	FormatMessage(
		/*FORMAT_MESSAGE_ALLOCATE_BUFFER |*/ FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hresult,
		0,
		(LPWSTR)_win32_error_buffer,
		sizeof(_win32_error_buffer),
		NULL);
	return _win32_error_buffer;
}

DWORD wasapi_audio_thread(void* arg) {
	// TODO sort out the error handling dude
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	HANDLE ready_event = CreateEvent(0, 0, 0, 0);
	audio_client->SetEventHandle(ready_event);
	u32 buffer_frames;
	audio_client->GetBufferSize(&buffer_frames);
	u32 buffer_size = buffer_frames * 4;
	audio_client->Start();

	audio_sample_t* buffer;
#if 0
	HRESULT hresult = audio_render_client->GetBuffer(buffer_frames, (BYTE**)&buffer);
	for(int i=0; i<buffer_frames; ++i) {
		buffer[i].s[0] = 0;
		buffer[i].s[1] = 0;
	}
	audio_render_client->ReleaseBuffer(buffer_frames, 0);
#endif
	
	for(;;) {
		DWORD wait = WaitForSingleObject(ready_event, INFINITE);
		if(wait != WAIT_OBJECT_0) {
			printf("WAIT OBJECT %i \n", wait);
			exit(1);
		}
		u32 buffer_padding;
		HRESULT hr = audio_client->GetCurrentPadding(&buffer_padding);
		if(!SUCCEEDED(hr)) {
			printf("GetCurrentPadding failed \n");
			exit(1);
		}
		u32 frames_to_fill = buffer_frames - buffer_padding;
		if(!frames_to_fill) {
			continue;
		}
		HRESULT hresult = audio_render_client->GetBuffer(frames_to_fill, (BYTE**)&buffer);
		if(SUCCEEDED(hresult)) {
			
			// for(int i=0; i<frames_to_fill; ++i) {
			// 	float sample = sinf(tick * 0.1f) * 255.0f * 4.0f;
			// 	buffer[i].s[0] = sample;
			// 	buffer[i].s[1] = sample;
			// 	mod += 0.00001f;
			// 	tick += 1 + mod;
			// }
			_audio_mixer_proc(buffer, frames_to_fill, NULL);

		} else {
			printf("GetBuffer failed (%s) \n", _win32_hresult_string(hresult));
		}
		HRESULT release_result = audio_render_client->ReleaseBuffer(frames_to_fill, 0);
		if(!SUCCEEDED(release_result)) {
			printf("ReleaseBuffer failed \n");
		}
	}
}

void cpp_wasapi_init_audio() {
	printf("initializing wasapi audio \n");
#define AssertSucceeded(hr)\
	if(!SUCCEEDED(hr)) {\
		printf("it broke \n");\
		return;\
	}

	CoInitialize(0);
	IMMDeviceEnumerator* device_enumerator;
	HRESULT hresult;
	hresult = CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, IID_PPV_ARGS(&device_enumerator));
	AssertSucceeded(hresult);

	// IMMEnumerator
	IMMDevice* audio_device;
	hresult = device_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audio_device);
	AssertSucceeded(hresult);

	// IAudioClient* audio_client;
	audio_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, 0, (void**)&audio_client);
	AssertSucceeded(hresult);

	REFERENCE_TIME device_period;
	audio_client->GetDevicePeriod(0, &device_period);
	AssertSucceeded(hresult);

	WAVEFORMATEX format = {0};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.nSamplesPerSec = CORE_AUDIO_SAMPLES_PER_SECOND;
	format.wBitsPerSample = 16;
	format.nBlockAlign = 4;
	format.nAvgBytesPerSec = CORE_AUDIO_SAMPLES_PER_SECOND * format.nBlockAlign;

	hresult = audio_client->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			device_period,
			// 1000*1000*10,
			0,
			&format,
			0);
	AssertSucceeded(hresult);

	AssertSucceeded(audio_client->GetService(IID_PPV_ARGS(&audio_render_client)));

	device_enumerator->Release();

	CreateThread(0, 0, wasapi_audio_thread, NULL, 0, 0);

	return;
}

extern "C" {
	void wasapi_init_audio(CORE_AUDIO_MIXER_PROC mixer_proc) {
		_audio_mixer_proc = mixer_proc;
		cpp_wasapi_init_audio();
	}
}
