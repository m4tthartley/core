//
//  Created by Matt Hartley on 05/06/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "core.h"
#include "sysaudio.h"
#include "targetconditionals.h"


#ifdef __WIN32__
// #   include "sysaudio_win32_wasapi.h"
// #   include "sysaudio_win32_xaudio.cpp"
_Bool xaudio_init_audio(sysaudio_t* audio, sysaudio_spec_t spec);
#endif


_Bool sys_init_audio(sysaudio_t* audio, sysaudio_spec_t spec) {
	*audio = (sysaudio_t){0};
	assert(spec.format <= SYSAUDIO_FORMAT_FLOAT64);

	audio->format = spec.format;
	audio->sampleRate = spec.sampleRate;
	if (audio->format < SYSAUDIO_FORMAT_FLOAT32) {
		audio->sampleSize = audio->format;
	} else {
		if (audio->format == SYSAUDIO_FORMAT_FLOAT32) {
			audio->sampleSize = 4;
		} else {
			audio->sampleSize = 8;
		}
	}

#ifdef __WIN32__
    // b32 wasapi_result = wasapi_init_audio(audio);
    // if (!wasapi_result) {
    //     goto init_audio_err;
	// }
    // CreateThread(0, 0, _wasapi_audio_thread, audio, 0, 0);

    xaudio_init_audio(audio, spec);
#endif

	return _True;

init_audio_err:
	return _False;
}
