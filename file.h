
#ifndef __CORE_FILE_HEADER__
#define __CORE_FILE_HEADER__

#include "core.h"
#include <mmreg.h>

typedef void* f_handle;
typedef struct {
	u64 created;
	u64 modified;
	size_t size;
} f_info;

f_handle f_open(char* path) {
	assert(sizeof(HANDLE)<=sizeof(f_handle));
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, /*OPEN_EXISTING*/OPEN_ALWAYS, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		LPTSTR msg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
					  FORMAT_MESSAGE_FROM_SYSTEM|
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, error,
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&msg, 0, NULL);
		// uiMessage("%i, %s", GetLastError(), msg);
		return 0;
	}
	return handle;
}

int f_read(f_handle file, int offset, void* output, size_t size) { // 32bit only?
	DWORD bytesRead;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = ReadFile(file, output, size, &bytesRead, &overlapped);
	if(!result || bytesRead!=size) {
		char path[64];
		GetFinalPathNameByHandleA(file, path, 64, FILE_NAME_OPENED);
		MessageBox(NULL, s_format("Failed to read file: %s", path), "File Error", MB_OK);
		return 0;
	} else {
		return 1;
	}
}

void f_write(f_handle file, int offset, void* data, size_t size) { // 32bit only?
	DWORD bytesWritten;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = WriteFile(file, data, size, &bytesWritten, &overlapped);
	if(!result || bytesWritten!=size) {
		MessageBox(NULL, "Failed to write file", "Error", MB_OK);
	}
}

f_info f_stat(f_handle file) {
	f_info result = {0};
	BY_HANDLE_FILE_INFORMATION info = {0};
	if(GetFileInformationByHandle(file, &info)) {
		result.created = info.ftCreationTime.dwLowDateTime;
		result.created |= (u64)info.ftCreationTime.dwHighDateTime<<32;
		result.modified = info.ftLastWriteTime.dwLowDateTime;
		result.modified |= (u64)info.ftLastWriteTime.dwHighDateTime<<32;
		result.size = info.nFileSizeLow;
	} else {
		MessageBox(NULL, "Failed to stat file", "Error", MB_OK);
	}
	return result;
}

void f_close(f_handle file) {
	if(file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
	}
}


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

bitmap_t* f_load_bitmap(memory_arena* arena, char* filename) {
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
		printf("cant open file\n");
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
	bitmap_t* result = m_alloc(arena, sizeof(bitmap_t) + sizeof(u32)*header->bitmapWidth*header->bitmapHeight);
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

bitmap_t* f_load_font_file(m_arena* arena, char*filename) {
	bitmap_t* bitmap = f_load_bitmap(arena, filename);
	u32* pixels = bitmap + 1;
	FOR (i, bitmap->width * bitmap->height) {
		u32 pixel = pixels[i];
		if (pixel != 0xFF000000) {
			pixels[i] = 0;
		} else {
			pixels[i] = 0xFFFFFFFF;
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

wave_t* f_load_wave_from_memory(m_arena* arena, u8* data, size_t file_size) {
	WavHeader *header = (WavHeader*)data;
	WavFormatChunk *format = NULL;
	WavDataChunk *dataChunk = NULL;
	char *f = (char*)(header + 1);

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

	WAVEFORMAT* win32_format = &format->formatTag;
	WAVEFORMATEXTENSIBLE* win32_extended_format = &format->formatTag;

	if (format && dataChunk) {
		assert(format->channels <= 2);
		assert(format->bitsPerSample == 16);
		// assert(dataChunk->size ==);
		// Possibly check whether to alloc or push
		wave_t* wave;
		if(format->channels == 1) {
			// TODO this is temporary solution
			wave = m_alloc(arena, sizeof(wave_t) + dataChunk->size*2);
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
			wave = m_alloc(arena, sizeof(wave_t) + dataChunk->size);
			memcpy(wave+1, dataChunk->data, dataChunk->size);
			wave->channels = format->channels;
			wave->samples_per_second = format->samplesPerSec;
			wave->bytes_per_sample = format->bitsPerSample / 8;
			wave->sample_count = dataChunk->size / (wave->channels * wave->bytes_per_sample);
		}
		return wave;
	}

	return NULL;
}

wave_t* f_load_wave(memory_arena* arena, char* filename) {
	FILE* file;
	file = fopen(filename, "r");
	if(!file) {
		core_error(FALSE, "Unable to open file: %s", filename);
	}
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	u8* data = malloc(size);
	rewind(file);
	fread(data, 1, size, file);
	fclose(file);

	wave_t* wave = f_load_wave_from_memory(arena, data, size);
	free(data);
	return wave;
}

#endif
