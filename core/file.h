//
//  file.h
//  Core
//
//  Created by Matt Hartley on 20/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_FILE_HEADER__
#define __CORE_FILE_HEADER__

#include "core.h"
#ifdef __WIN32__
#	include <mmreg.h>
#endif


#pragma pack(push, 1)
typedef struct {
	char header[2];
	u32 size;
	u16 reserved1;
	u16 reserved2;
	u32 offset;
	
	// Windows BITMAPINFOHEADER
	u32 headerSize;
	i32 bitmapWidth;
	i32 bitmapHeight;
	u16 colorPlanes;
	u16 colorDepth;
	u32 compression;
	u32 imageSize;
	i32 hres;
	i32 vres;
	u32 paletteSize;
	u32 importantColors;
} bmp_header;
#pragma pack(pop)

typedef struct {
	u32* data;
	bmp_header* header;
} bmp;

bitmap_t* f_load_bitmap(char* filename) {
	// TODO replace with platform functions
	FILE* fontFile;
	long fileSize;
	void* fontData;
	bmp_header* header;
	u32* palette;
	char* data;
	int rowSize;

	fontFile = fopen(filename, "r"); // todo: this stuff crashes when file not found
	if(!fontFile) {
		core_error("Unable to open file: %s", filename);
		return NULL;
	}
	fseek(fontFile, 0, SEEK_END);
	fileSize = ftell(fontFile);
	fontData = malloc(fileSize);
	rewind(fontFile);
	fread(fontData, 1, fileSize, fontFile);
	fclose(fontFile);
	
	header = (bmp_header*)fontData;
	palette = (u32*)((char*)fontData+14+header->headerSize);
	data = (char*)fontData+header->offset;
	rowSize = ((header->colorDepth*header->bitmapWidth+31) / 32) * 4;

	// Possibly check whether to alloc or push
	bitmap_t* result = core_alloc(sizeof(bitmap_t) + sizeof(u32)*header->bitmapWidth*header->bitmapHeight);
	result->size = header->size;
	result->width = header->bitmapWidth;
	result->height = header->bitmapHeight;
	
	u32* image = result + 1;
	// image = (u32*)malloc(sizeof(u32)*header->bitmapWidth*header->bitmapHeight);
	//{for(int w=0; w<header.bitmapHeight}
	{
		int row;
		int pixel;
		for(row=0; row<header->bitmapHeight; ++row) {
			int bitIndex=0;
			//printf("row %i \n", row);
// 			if(row==255) {
// 				DebugBreak();
// 			}
			for(pixel=0; pixel<header->bitmapWidth; ++pixel) {//while((bitIndex/8) < rowSize) {
				u32* chunk = (u32*)((char*)fontData+header->offset+(row*rowSize)+(bitIndex/8));
				u32 pi = *chunk;
				if(header->colorDepth<8) {
					pi >>= (header->colorDepth-(bitIndex%8));
				}
				pi &= (((i64)1<<header->colorDepth)-1);
				if(header->colorDepth>8) {
					image[row*header->bitmapWidth+pixel] = pi;
				} else {
					image[row*header->bitmapWidth+pixel] = palette[pi];
				}
				if(/*image[row*header->bitmapWidth+pixel]==0xFF000000 ||*/
				   image[row*header->bitmapWidth+pixel]==0xFFFF00FF) {
					image[row*header->bitmapWidth+pixel] = 0;
				}
// 				if(pixel==120) {
// 					int asd = 0;
// 				}
				bitIndex += header->colorDepth;
			}
		}
	}

	free(fontData);
	
	// result.data = image;
	// result.header = header;
	return result;
}

bitmap_t* f_load_font_file(char*filename) {
	bitmap_t* bitmap = f_load_bitmap(filename);
	if (bitmap) {
		u32* pixels = bitmap + 1;
		FOR (i, bitmap->width * bitmap->height) {
			u32 pixel = pixels[i];
			if (pixel != 0xFF000000) {
				pixels[i] = 0;
			} else {
				pixels[i] = 0xFFFFFFFF;
			}
		}
	}

	return bitmap;
}

#pragma pack(push, 1)
typedef struct {
	char ChunkId[4];
	u32 ChunkSize;
	char WaveId[4];
} WavHeader;
typedef struct {
	u8 id[4];
	u32 size;
	u16 formatTag;
	u16 channels;
	u32 samplesPerSec;
	u32 bytesPerSec;
	u16 blockAlign;
	u16 bitsPerSample;
	u16 cbSize;
	i16 validBitsPerSample;
	i32 channelMask;
	u8 subFormat[16];
} WavFormatChunk;
typedef struct {
	char id[4];
	u32 size;
	void *data;
	char padByte;
} WavDataChunk;
#pragma pack(pop)

wave_t* f_load_wave_from_memory(u8* data, size_t file_size) {
	WavHeader *header = (WavHeader*)data;
	WavFormatChunk *format = NULL;
	WavDataChunk *dataChunk = NULL;
	char *f = (char*)(header + 1);

	if (data) {
		// Parse file and collect structures
		while (f < (char*)data + file_size) {
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

#ifdef __WIN32__
		WAVEFORMAT* win32_format = &format->formatTag;
		WAVEFORMATEXTENSIBLE* win32_extended_format = &format->formatTag;
#endif

		if (format && dataChunk) {
			assert(format->channels <= 2);
			assert(format->bitsPerSample == 16);
			// assert(dataChunk->size ==);
			// Possibly check whether to alloc or push
			wave_t* wave;
			if(format->channels == 1) {
				// TODO this is temporary solution
				wave = core_alloc(sizeof(wave_t) + dataChunk->size*2);
				wave->channels = 2;
				wave->samples_per_second = format->samplesPerSec;
				wave->bytes_per_sample = format->bitsPerSample / 8;
				wave->sample_count = dataChunk->size / (wave->channels * wave->bytes_per_sample);
				i16* raw_data = dataChunk->data;
				audio_sample_t* output = wave + 1;
				FOR(i, wave->sample_count) {
					output[i].left = raw_data[i];
					output[i].right = raw_data[i];
				}
			} else {
				wave = core_alloc(sizeof(wave_t) + dataChunk->size);
				memcpy(wave+1, dataChunk->data, dataChunk->size);
				wave->channels = format->channels;
				wave->samples_per_second = format->samplesPerSec;
				wave->bytes_per_sample = format->bitsPerSample / 8;
				wave->sample_count = dataChunk->size / (wave->channels * wave->bytes_per_sample);
			}
			return wave;
		}
	}

	return NULL;
}

wave_t* f_load_wave(char* filename) {
	FILE* file;
	file = fopen(filename, "r");
	if(!file) {
		core_error("Unable to open file: %s", filename);
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	u8* data = malloc(size);
	rewind(file);
	fread(data, 1, size, file);
	fclose(file);

	wave_t* wave = f_load_wave_from_memory(data, size);
	free(data);
	return wave;
}

#endif
