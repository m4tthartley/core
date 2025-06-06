//
//  Created by Matt Hartley on 05/06/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xaudio2.h>
#include <iostream>

#include "core.h"
#include "sysaudio.h"


// _Bool sys_init_audio(sysaudio_t* audio, sysaudio_spec_t spec) {
// 	*audio = (sysaudio_t){0};
// 	assert(spec.format <= SYSAUDIO_FORMAT_FLOAT64);

// 	audio->format = spec.format;
// 	audio->sampleRate = spec.sampleRate;
// 	if (audio->format < SYSAUDIO_FORMAT_FLOAT32) {
// 		audio->sampleSize = audio->format;
// 	} else {
// 		if (audio->format == SYSAUDIO_FORMAT_FLOAT32) {
// 			audio->sampleSize = 4;
// 		} else {
// 			audio->sampleSize = 8;
// 		}
// 	}

// 	return _True;

// init_audio_err:
// 	return _False;
// }


void _win_print_hr_err(HRESULT hr)
{
    char str[24];
    snprintf(str, 24, "0x%08X", hr);
    sys_print_err(str);
    // std::string str = std::hex << hr << "\n";
}


#ifdef __cplusplus
extern "C" {
#endif


_Bool xaudio_init_audio(sysaudio_t* audio, sysaudio_spec_t spec)
{
    HRESULT hr;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    hr = XAudio2Create(&audio->xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        _win_print_hr_err(hr);
        return _False;
    }

    return _True;
}

void xaudio_start_audio(sysaudio_t* audio)
{

}

void xaudio_stop_audio(sysaudio_t* audio)
{

}


#ifdef __cplusplus
}
#endif
