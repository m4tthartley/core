//
//  Created by Matt Hartley on 20/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_WAVE_HEADER__
#define __CORE_WAVE_HEADER__

#ifndef __EDITOR__
#	ifndef __CORE_HEADER__
#		error "core.h must be included before wave.h"
#	endif
#else
#	include "sys.h"
#	include "core.h"
#	include "sysaudio.h"
#endif


#include <stdint.h>


typedef struct __attribute((packed)) {
	char ChunkId[4];
	uint32_t ChunkSize;
	char WaveId[4];
} WavHeader;
typedef struct __attribute((packed)) {
	uint8_t id[4];
	uint32_t size;
	uint16_t formatTag;
	uint16_t channels;
	uint32_t samplesPerSec;
	uint32_t bytesPerSec;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	uint16_t cbSize;
	int16_t validBitsPerSample;
	int32_t channelMask;
	uint8_t subFormat[16];
} WavFormatChunk;
typedef struct __attribute((packed)) {
	char id[4];
	uint32_t size;
	void *data;
	char padByte;
} WavDataChunk;

audio_buffer_t* sys_decode_wave(allocator_t* allocator, file_data_t* fileData) {
	u8* data = fileData->data;
	WavHeader *header = (WavHeader*)data;
	WavFormatChunk *format = NULL;
	WavDataChunk *dataChunk = NULL;
	char *f = (char*)(header + 1);

	if (data) {
		// Parse file and collect structures
		while (f < (char*)data + fileData->stat.size) {
			int id = *(int*)f;
			u32 size = *(u32*)(f+4);
			if (id == (('f'<<0)|('m'<<8)|('t'<<16)|(' '<<24))) {
				format = (WavFormatChunk*)f;
			}
			if (id == (('d'<<0)|('a'<<8)|('t'<<16)|('a'<<24))) {
				dataChunk = (WavDataChunk*)f;
				dataChunk->data = f + 8;
			}
			f += size + 8;
		}

		if (format && dataChunk) {
			assert(format->channels <= 2);
			assert(format->bitsPerSample == 16);
			// assert(dataChunk->size ==);
			// Possibly check whether to alloc or push
			audio_buffer_t* wave;
			if(format->channels == 1) {
				// TODO this is temporary solution
				wave = alloc_memory(allocator, sizeof(audio_buffer_t) + dataChunk->size*2);
				wave->channels = 2;
				wave->sampleRate = format->samplesPerSec;
				wave->sampleSize = format->bitsPerSample / 8;
				wave->sampleCount = dataChunk->size / (wave->channels * wave->sampleSize);
				i16* raw_data = dataChunk->data;
				audio_sample16_t* output = (audio_sample16_t*)(wave + 1);
				FOR(i, wave->sampleCount) {
					output[i].left = raw_data[i];
					output[i].right = raw_data[i];
				}
			} else {
				wave = alloc_memory(allocator, sizeof(audio_buffer_t) + dataChunk->size);
				memcpy(wave+1, dataChunk->data, dataChunk->size);
				wave->channels = format->channels;
				wave->sampleRate = format->samplesPerSec;
				wave->sampleSize = format->bitsPerSample / 8;
				wave->sampleCount = dataChunk->size / (wave->channels * wave->sampleSize);
			}
			return wave;
		}
	}

	return NULL;
}


#endif
