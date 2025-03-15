//
//  Created by Matt Hartley on 11/03/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudioTypes/CoreAudioTypes.h>

#include "sysaudio.h"


#undef _True
#undef _False
#define _True ((_Bool)1)
#define _False ((_Bool)0)


void _sys_audio_print(char* str) {
	write(STDOUT_FILENO, str, strlen(str));
}

OSStatus _AURenderCallback(
	void* refCon,
	AudioUnitRenderActionFlags* flags,
	const AudioTimeStamp* timeStamp,
	UInt32 busNumber,
	UInt32 numFrames,
	AudioBufferList* data
) {
	sysaudio_t* audio = refCon;
	if (data->mBuffers[0].mData) {
		sysaudio_default_mixer(audio, data->mBuffers[0].mData, numFrames);
	} else {
		_sys_audio_print("AURenderCallback buffer is NULL \n");
	}

	return noErr;
}

_Bool sys_init_audio(sysaudio_t* audio, sysaudio_spec_t spec) {
	*audio = (sysaudio_t){0};
	audio->spec = spec;
	audio->spec.mixer = NULL;

	AudioComponentDescription desc = {
		.componentType = kAudioUnitType_Output,
		.componentSubType = kAudioUnitSubType_DefaultOutput,
		.componentManufacturer = kAudioUnitManufacturer_Apple,
	};

	AudioComponent outputComponent = AudioComponentFindNext(NULL, &desc);
	if (!outputComponent) {
		_sys_audio_print("Audio init failed: AudioComponentFindNext \n");
		goto init_audio_err;
	}

	AudioUnit outputUnit;
	OSStatus status = AudioComponentInstanceNew(outputComponent, &outputUnit);
	if (status != noErr) {
		_sys_audio_print("Audio init failed: AudioComponentInstanceNew \n");
		goto init_audio_err;
	}

	AudioStreamBasicDescription streamDesc = {
		.mSampleRate = 44100,
		.mFormatID = kAudioFormatLinearPCM,
		.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked,
		.mFramesPerPacket = 1,
		.mChannelsPerFrame = 2,
		.mBytesPerFrame = sizeof(float)*2,
		.mBytesPerPacket = sizeof(float)*2,
		.mBitsPerChannel = 32,
	};
	AudioUnitSetProperty(
		outputUnit,
		kAudioUnitProperty_StreamFormat,
		kAudioUnitScope_Input,
		0,
		&streamDesc,
		sizeof(streamDesc)
	);

	AURenderCallbackStruct callback;
	callback.inputProc = _AURenderCallback;
	callback.inputProcRefCon = audio;
	AudioUnitSetProperty(
		outputUnit,
		kAudioUnitProperty_SetRenderCallback,
		kAudioUnitScope_Input,
		0,
		&callback,
		sizeof(callback)
	);

	AudioUnitInitialize(outputUnit);
	AudioOutputUnitStart(outputUnit);
	audio->outputUnit = outputUnit;

	return _True;

init_audio_err:
	return _False;
}

void sys_set_audio_callback(sysaudio_t* audio, SYSAUDIO_MIXER_PROC mixer) {
	AudioUnit outputUnit = audio->outputUnit;

	AURenderCallbackStruct callback;
	callback.inputProc = _AURenderCallback;
	callback.inputProcRefCon = audio;
	AudioUnitSetProperty(
		outputUnit,
		kAudioUnitProperty_SetRenderCallback,
		kAudioUnitScope_Input,
		0,
		&callback,
		sizeof(callback)
	);
}

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
