//
//  Created by Matt Hartley on 20/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//


#ifndef __CORE_WAVE_HEADER__
#define __CORE_WAVE_HEADER__


#include <stdint.h>

#include "core.h"
#include "sysaudio.h"


#define WAVE_FORMAT_TAG_PCM 0x01
#define WAVE_FORMAT_TAG_PCM_FLOAT 0x03

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

audio_buffer_t* sys_decode_wave(allocator_t* allocator, file_data_t* fileData);


#	ifdef CORE_IMPL


void _downmix_24bit_samples(int numChannels, int numSamples, audio_sample32_t* output, uint8_t* data) {
	if (numChannels > 5) {
		print("6 or more channels \n");
		FOR(i, numSamples) {
			int32_t ch0 = *((int32_t*)(data+0)) & 0x00FFFFFF;
			int32_t ch1 = *((int32_t*)(data+3)) & 0x00FFFFFF;
			int32_t ch2 = *((int32_t*)(data+6)) & 0x00FFFFFF;
			int32_t ch4 = *((int32_t*)(data+12)) & 0x00FFFFFF;
			int32_t ch5 = *((int32_t*)(data+15)) & 0x00FFFFFF;
			double ch0f = ((ch0 & 0x800000) ? ch0 | 0xFF000000 : ch0) * 256.0;
			double ch1f = ((ch1 & 0x800000) ? ch1 | 0xFF000000 : ch1) * 256.0;
			double ch2f = ((ch2 & 0x800000) ? ch2 | 0xFF000000 : ch2) * 256.0;
			double ch4f = ((ch4 & 0x800000) ? ch4 | 0xFF000000 : ch4) * 256.0;
			double ch5f = ((ch5 & 0x800000) ? ch5 | 0xFF000000 : ch5) * 256.0;
			
			output[i].left = ch0f + (ch2f*0.7071) + (ch4f*0.7071);
			output[i].right = ch1f + (ch2f*0.7071) + (ch5f*0.7071);
			data += numChannels*3;
		}
	} else if (numChannels >= 2) {
		print("2 - 5 channels \n");
		FOR(i, numSamples) {
			int32_t ch0 = *((int32_t*)(data+0)) & 0x00FFFFFF;
			int32_t ch1 = *((int32_t*)(data+3)) & 0x00FFFFFF;
			double ch0f = ((ch0 & 0x800000) ? ch0 | 0xFF000000 : ch0) * 256.0;
			double ch1f = ((ch1 & 0x800000) ? ch1 | 0xFF000000 : ch1) * 256.0;
			output[i].left = ch0f;
			output[i].right = ch1f;
			data += numChannels*3;
		}
	} else if (numChannels == 1) {
		print("1 channel \n");
		FOR(i, numSamples) {
			int32_t ch0 = *((int32_t*)(data+0)) & 0x00FFFFFF;
			double ch0f = ((ch0 & 0x800000) ? ch0 | 0xFF000000 : ch0) * 256.0;
			output[i].left = ch0f;
			output[i].right = ch0f;
			data += numChannels*3;
		}
	}
}

audio_buffer_t* sys_decode_wave(allocator_t* allocator, file_data_t* fileData) {
	u8* data = fileData->data;
	WavHeader *header = (WavHeader*)data;
	WavFormatChunk *format = NULL;
	WavDataChunk *dataChunk = NULL;
	char *f = (char*)(header + 1);

	if (!data) {
		return NULL;
	}

	// Parse chunks
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

	if (!format || !dataChunk) {
		return NULL;
	}

	// Parse data
	assert(
		(format->formatTag==WAVE_FORMAT_TAG_PCM &&
			(format->bitsPerSample==8 || format->bitsPerSample==16 || format->bitsPerSample==24 || format->bitsPerSample==32))
		||
		(format->formatTag==WAVE_FORMAT_TAG_PCM_FLOAT &&
			(format->bitsPerSample==32 || format->bitsPerSample==64))
	);

	audio_buffer_t w = {0};
	if (format->formatTag == WAVE_FORMAT_TAG_PCM) {
		w.format = SYSAUDIO_FORMAT_NONE + (format->bitsPerSample/8);
	} else {
		if (format->bitsPerSample == 32) {
			w.format = SYSAUDIO_FORMAT_FLOAT32;
		} else {
			w.format = SYSAUDIO_FORMAT_FLOAT64;
		}
	}
	w.channels = 2;
	w.sampleRate = format->samplesPerSec;
	w.sampleSize = format->bitsPerSample / 8;
	w.sampleCount = dataChunk->size / (w.channels * w.sampleSize);

	audio_buffer_t* wave = alloc_memory(allocator, sizeof(audio_buffer_t) + (w.sampleCount * w.sampleSize * 2));
	*wave = w;

	print("Wave: format %i, ch%i, %ibit, %ihz", format->formatTag, format->channels, format->bitsPerSample, format->samplesPerSec);

	for(int sindex=0; sindex<wave->sampleCount; ++sindex) {
		if (format->formatTag == WAVE_FORMAT_TAG_PCM) {
			int8_t* data = dataChunk->data;
			if (format->channels > 5) {
				int stride = wave->sampleSize*format->channels;
				int shiftAmount = (4-wave->sampleSize)*8;
				int32_t chScaled[6];
				for (int chindex=0; chindex<6; ++chindex) {
					int8_t* ch1Raw = data + sindex*stride+(wave->sampleSize*chindex);
					int32_t chSigned = *(int32_t*)ch1Raw - (wave->sampleSize==1 ? 128 : 0);
					chScaled[chindex] = chSigned << shiftAmount;
				}
				
				float normCoef = 1.0f + 0.7071f + 0.7071f;
				int32_t left = (chScaled[0] + (chScaled[2]*0.7071f) + (chScaled[4]*0.7071f)) / normCoef;
				int32_t right = (chScaled[1] + (chScaled[2]*0.7071f) + (chScaled[5]*0.7071f)) / normCoef;
				for (int byte=0; byte<wave->sampleSize; ++byte) {
					wave->data[sindex*wave->sampleSize*2 + byte] = (left >> (shiftAmount + byte*8)) & 0xFF;
					wave->data[sindex*wave->sampleSize*2 + wave->sampleSize+byte] = (right >> (shiftAmount + byte*8)) & 0xFF;
				}
			}
			else if (format->channels > 1) {
				for (int bi=0; bi<wave->sampleSize; ++bi) {
					uint8_t l = data[sindex*wave->sampleSize*format->channels + bi];
					uint8_t r = data[sindex*wave->sampleSize*format->channels + wave->sampleSize + bi];
					wave->data[sindex*wave->sampleSize*2 + bi] = l;
					wave->data[sindex*wave->sampleSize*2 + wave->sampleSize + bi] = r;
				}
			}
			else if (format->channels == 1) {
				for (int bi=0; bi<wave->sampleSize; ++bi) {
					uint32_t m = data[sindex*wave->sampleSize*format->channels + bi];
					wave->data[sindex*wave->sampleSize*2 + bi] = m;
					wave->data[sindex*wave->sampleSize*2 + wave->sampleSize + bi] = m;
				}
			}


		}

		if (format->formatTag == WAVE_FORMAT_TAG_PCM_FLOAT) {
			if (wave->sampleSize == 4) {
				audio_sample_float32_t* output = (audio_sample_float32_t*)wave->data;
				float* input = dataChunk->data;
				if (format->channels > 1) {
					output[sindex].left = input[sindex*format->channels];
					output[sindex].right = input[sindex*format->channels+1];
				}
				else {
					output[sindex].left = input[sindex*format->channels];
					output[sindex].right = input[sindex*format->channels];
				}
			}
			if (wave->sampleSize == 8) {
				audio_sample_float64_t* output = (audio_sample_float64_t*)wave->data;
				double* input = dataChunk->data;
				if (format->channels > 1) {
					output[sindex].left = input[sindex*format->channels];
					output[sindex].right = input[sindex*format->channels+1];
				}
				else {
					output[sindex].left = input[sindex*format->channels];
					output[sindex].right = input[sindex*format->channels];
				}
			}
		}
	}

	return wave;
}


#	endif
#endif
