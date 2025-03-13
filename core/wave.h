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


#endif


#ifdef CORE_IMPL
#	ifndef __CORE_WAVE_HEADER_IMPL__
#	define __CORE_WAVE_HEADER_IMPL__


void _downmix_24bit_samples(int numChannels, int numSamples, audio_sample32_t* output, uint8_t* data) {
	if (numChannels > 5) {
		print_inline("6 or more channels \n");
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
			// 4294967295 / 16777215 = 256 ish
			
			output[i].left = ch0f + (ch2f*0.7071) + (ch4f*0.7071);
			output[i].right = ch1f + (ch2f*0.7071) + (ch5f*0.7071);
			data += numChannels*3;
		}
	} else if (numChannels >= 2) {
		print_inline("2 - 5 channels \n");
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
		print_inline("1 channel \n");
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

		// assert(format->formatTag==WAVE_FORMAT_TAG_PCM || format->formatTag==WAVE_FORMAT_TAG_PCM_FLOAT);
		assert(
			(format->formatTag==WAVE_FORMAT_TAG_PCM &&
			(format->bitsPerSample==8 || format->bitsPerSample==16 || format->bitsPerSample==24 || format->bitsPerSample==32))
			||
			(format->formatTag==WAVE_FORMAT_TAG_PCM_FLOAT && format->bitsPerSample==32)
		);

		audio_buffer_t* wave = alloc_memory(allocator, sizeof(audio_buffer_t) + dataChunk->size*2);

		if (format && dataChunk) {
			// assert(format->channels <= 2);
			// assert(format->bitsPerSample == 16);
			// assert(dataChunk->size ==);
			// Possibly check whether to alloc or push
			
			wave->channels = 2;
			wave->sampleRate = format->samplesPerSec;
			wave->sampleSize = format->bitsPerSample / 8;
			wave->sampleCount = dataChunk->size / (wave->channels * wave->sampleSize);

			// if(format->channels == 1) {
			// 	// TODO this is temporary solution
			// 	if (wave->sampleSize == 1) {
			// 		uint8_t* data = dataChunk->data;
			// 		audio_sample8_t* output = (audio_sample8_t*)(wave + 1);
			// 		FOR(i, wave->sampleCount) {
			// 			output[i].left = data[i];
			// 			output[i].right = data[i];
			// 		}
			// 	}
			// 	if (wave->sampleSize == 2) {
			// 		i16* raw_data = dataChunk->data;
			// 		audio_sample16_t* output = (audio_sample16_t*)(wave + 1);
			// 		FOR(i, wave->sampleCount) {
			// 			output[i].left = raw_data[i];
			// 			output[i].right = raw_data[i];
			// 		}
			// 	}
			// 	if (wave->sampleSize == 3) {
			// 		uint8_t* raw_data = dataChunk->data;
			// 		audio_sample32_t* output = (audio_sample32_t*)(wave + 1);
			// 		FOR(i, wave->sampleCount) {
			// 			output[i].left = *((int32_t*)(raw_data+i*3)) & 0x00FFFFFF;
			// 			output[i].right = *((int32_t*)(raw_data+i*3)) & 0x00FFFFFF;
			// 		}
			// 	}
			// 	if (wave->sampleSize == 4) {
			// 		i32* raw_data = dataChunk->data;
			// 		audio_sample32_t* output = (audio_sample32_t*)(wave + 1);
			// 		FOR(i, wave->sampleCount) {
			// 			output[i].left = raw_data[i];
			// 			output[i].right = raw_data[i];
			// 		}
			// 	}
			// } else {
			// 	// wave = alloc_memory(allocator, sizeof(audio_buffer_t) + dataChunk->size);
			// 	memcpy(wave+1, dataChunk->data, dataChunk->size);
			// 	wave->channels = format->channels;
			// 	wave->sampleRate = format->samplesPerSec;
			// 	wave->sampleSize = format->bitsPerSample / 8;
			// 	wave->sampleCount = dataChunk->size / (wave->channels * wave->sampleSize);
			// }

#define		down_mix_channels()\
			if (format->channels > 5) {\
				print_inline("6 or more channels \n");\
				FOR(i, wave->sampleCount) {\
					output[i].left = data[0] + (data[2]*0.7071f) + (data[4]*0.7071f);\
					output[i].right = data[1] + (data[2]*0.7071f) + (data[5]*0.7071f);\
					data += format->channels;\
				}\
			} else if (format->channels >= 2) {\
				print_inline("2 - 5 channels \n");\
				FOR(i, wave->sampleCount) {\
					output[i].left = data[0];\
					output[i].right = data[1];\
					data += format->channels;\
				}\
			} else if (format->channels == 1) {\
				print_inline("1 channel \n");\
				FOR(i, wave->sampleCount) {\
					output[i].left = data[0];\
					output[i].right = data[0];\
					data += format->channels;\
				}\
			}

// #define		down_mix_channels_24bit()\
// 			if (format->channels > 5) {\
// 			   FOR(i, wave->sampleCount) {\
// 				   output[i].left = data[0] + (data[2]*0.7071f) + (data[4]*0.7071f);\
// 				   output[i].right = data[1] + (data[2]*0.7071f) + (data[5]*0.7071f);\
// 				   data += format->channels;\
// 			   }\
// 		   } else if (format->channels >= 2) {\
// 			   FOR(i, wave->sampleCount) {\
// 				   output[i].left = data[0];\
// 				   output[i].right = data[1];\
// 				   data += format->channels;\
// 			   }\
// 		   } else if (format->channels == 1) {\
// 			   FOR(i, wave->sampleCount) {\
// 				   output[i].left = data[0];\
// 				   output[i].right = data[0];\
// 				   data += format->channels;\
// 			   }\
// 		   }
			
			if (wave->sampleSize == 1) {
				uint8_t* data = dataChunk->data;
				audio_sample8_t* output = (audio_sample8_t*)(wave + 1);
				// FOR(i, wave->sampleCount) {
				// 	output[i].left = data[i];
				// 	output[i].right = data[i];
				// }
				print_inline("downmixing 8bit, ");
				down_mix_channels();
			}
			if (wave->sampleSize == 2) {
				i16* data = dataChunk->data;
				audio_sample16_t* output = (audio_sample16_t*)(wave + 1);

				print_inline("downmixing 16bit, ");
				down_mix_channels();
			}
			if (wave->sampleSize == 3) {
				uint8_t* raw_data = dataChunk->data;
				audio_sample32_t* output = (audio_sample32_t*)(wave + 1);
				// FOR(i, wave->sampleCount) {
				// 	output[i].left = *((int32_t*)(raw_data+i*3)) & 0x00FFFFFF;
				// 	output[i].right = *((int32_t*)(raw_data+i*3)) & 0x00FFFFFF;
				// }
				print_inline("downmixing 24bit, ");
				_downmix_24bit_samples(format->channels, wave->sampleCount, output, raw_data);
			}
			if (wave->sampleSize == 4) {
				i32* raw_data = dataChunk->data;
				audio_sample32_t* output = (audio_sample32_t*)(wave + 1);
				// FOR(i, wave->sampleCount) {
				// 	output[i].left = raw_data[i];
				// 	output[i].right = raw_data[i];
				// }
				print_inline("downmixing 32bit, ");
				down_mix_channels();
			}

			return wave;
		}
	}

	return NULL;
}


#	endif
#endif
