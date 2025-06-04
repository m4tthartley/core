//
//  Created by Matt Hartley on 11/03/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudioTypes/CoreAudioTypes.h>
#include <stdatomic.h>

#include "sysaudio.h"


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

	SYSAUDIO_MIXER_PROC mixer = atomic_load(&audio->mixer);
	if (!mixer) {
		return noErr;
	}

	if (data->mBuffers[0].mData) {
		memset(data->mBuffers[0].mData, 0, audio->sampleSize*2 * numFrames);
		audio->mixer(audio, data->mBuffers[0].mData, numFrames);
	} else {
		_sys_audio_print("AURenderCallback buffer is NULL \n");
	}

	return noErr;
}

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

	AudioComponentDescription desc = {
		.componentType = kAudioUnitType_Output,
		.componentSubType = kAudioUnitSubType_DefaultOutput,
		.componentManufacturer = kAudioUnitManufacturer_Apple,
		.componentFlags = 0,
		.componentFlagsMask = 0,
	};

	AudioComponent outputComponent = AudioComponentFindNext(NULL, &desc);
	if (!outputComponent) {
		_sys_audio_print("Audio init failed: AudioComponentFindNext \n");
		goto init_audio_err;
	}

	AudioUnit outputUnit = NULL;
	OSStatus status = AudioComponentInstanceNew(outputComponent, &outputUnit);
	if (status != noErr) {
		_sys_audio_print("Audio init failed: AudioComponentInstanceNew \n");
		goto init_audio_err;
	}

	AudioFormatFlags formatFlag = audio->format<SYSAUDIO_FORMAT_FLOAT32 ? kAudioFormatFlagIsSignedInteger : kAudioFormatFlagIsFloat;
	AudioStreamBasicDescription streamDesc = {
		.mSampleRate = audio->sampleRate,
		.mFormatID = kAudioFormatLinearPCM,
		.mFormatFlags = formatFlag | kAudioFormatFlagIsPacked,
		.mFramesPerPacket = 1,
		.mChannelsPerFrame = 2,
		.mBytesPerFrame = audio->sampleSize*2,
		.mBytesPerPacket = audio->sampleSize*2,
		.mBitsPerChannel = audio->sampleSize * 8,
	};
	AudioUnitSetProperty(
		outputUnit,
		kAudioUnitProperty_StreamFormat,
		kAudioUnitScope_Input,
		0,
		&streamDesc,
		sizeof(streamDesc)
	);

	audio->outputUnit = outputUnit;

	return _True;

init_audio_err:
	return _False;
}

void sys_start_audio(sysaudio_t* audio) {
	AURenderCallbackStruct callback;
	callback.inputProc = _AURenderCallback;
	callback.inputProcRefCon = audio;
	AudioUnitSetProperty(
		audio->outputUnit,
		kAudioUnitProperty_SetRenderCallback,
		kAudioUnitScope_Input,
		0,
		&callback,
		sizeof(callback)
	);

	AudioUnitInitialize(audio->outputUnit);

	AudioOutputUnitStart(audio->outputUnit);
}

void sys_stop_audio(sysaudio_t* audio) {
	AudioOutputUnitStop(audio->outputUnit);
	AudioUnitUninitialize(audio->outputUnit);
}

void sys_set_audio_callback(sysaudio_t* audio, SYSAUDIO_MIXER_PROC mixer) {
	atomic_exchange(&audio->mixer, mixer);
}
