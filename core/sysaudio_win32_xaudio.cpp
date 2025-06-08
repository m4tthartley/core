//
//  Created by Matt Hartley on 05/06/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"
#include "sys.h"
#include "sysaudio.h"

#include <xaudio2.h>


void _win_print_hr_err(HRESULT hr)
{
    char str[24];
    snprintf(str, 24, "0x%08X", hr);
    sys_print_err(str);
    // std::string str = std::hex << hr << "\n";
}

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
