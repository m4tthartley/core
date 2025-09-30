/*
	Created by Matt Hartley on 29/09/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __CORE_BMP_HEADER__
#define __CORE_BMP_HEADER__

#include <stdint.h>


typedef struct __attribute__((packed)) {
	char header[2];
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset;
	
	// Windows BITMAPINFOHEADER
	uint32_t headerSize;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t depth;
	uint32_t compression;
	uint32_t imageSize;
	int32_t hres;
	int32_t vres;
	uint32_t colorsUsed;
	uint32_t colorsImportant;
} bmp_header_t;

typedef struct __attribute__((packed)) {
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint32_t data[];
} bitmap_t;

typedef enum {
	BMP_FORMAT_32BIT = 1,
	BMP_FORMAT_24BIT,
	BMP_FORMAT_16BIT,
	BMP_FORMAT_8BIT, // PALETTE
	BMP_FORMAT_4BIT, // PALETTE
	BMP_FORMAT_1BIT, // PALETTE
	// BMP_FORMAT_PALETTE,
} bmp_format_t;

typedef struct {
	bmp_format_t format;
	uint32_t width;
	uint32_t height;
	// bmp_header_t* header;
} bmp_info_t;


#ifdef CORE_IMPL


bmp_info_t bmp_get_info(void* input)
{
	bmp_header_t* header = (bmp_header_t*)input;
	// u32* palette = (u32*)((char*)fileData+14+header->headerSize);
	// u8* data = (u8*)fileData+header->offset;
	// uint32_t rowSize = ((header->colorDepth*header->bitmapWidth+31) / 32) * 4;

	bmp_info_t result = {0};
	result.format = header->depth;
	result.width = header->width;
	result.height = header->height;

	return result;
}

uint32_t bmp_get_size(void* input)
{
	bmp_header_t* header = (bmp_header_t*)input;
	return header->size;
}

// Loads bitmap data and transforms to 32bit RGBA
void bmp_load_rgba32(void* input, void* output) {
	// bmp_header_t* header = (bmp_header_t*)fileData;
	// u32* palette = (u32*)((char*)fileData+14+header->headerSize);
	// u8* data = (u8*)fileData+header->offset;
	// int rowSize = ((header->colorDepth*header->bitmapWidth+31) / 32) * 4;

	// // Possibly check whether to alloc or push
	// bitmap_t* result = alloc_memory(allocator, sizeof(bitmap_t) + sizeof(u32)*header->bitmapWidth*header->bitmapHeight);
	// result->size = header->size;
	// result->width = header->bitmapWidth;
	// result->height = header->bitmapHeight;

	bmp_header_t* header = (bmp_header_t*)input;
	// bmp_info_t info = bmp_load_info(input);
	uint32_t width = header->width;
	uint32_t height = header->height;
	uint8_t* imageData = input + header->offset;
	uint32_t* palette = (uint32_t*)(input + 14 + header->headerSize);

	uint32_t dataOffset = header->offset;
	uint16_t colorDepth = header->depth;
	uint32_t rowSize = ((header->depth*width+31) / 32) * 4;
	
	// u32* image = (u32*)(result + 1);
	uint32_t* image = output;

	for(int row=0; row<height; ++row) {
		int bitIndex=0;
		for(int pixel=0; pixel<width; ++pixel) {
			uint32_t* chunk = (uint32_t*)(imageData + (row*rowSize) + (bitIndex/8));
			uint32_t pi = *chunk;
			if(colorDepth < 8) {
				pi >>= (colorDepth-(bitIndex%8));
			}
			pi &= (((i64)1<<colorDepth)-1);
			if(colorDepth>8) {
				image[row*width+pixel] = pi;
			} else {
				image[row*width+pixel] = palette[pi];
			}

			image[row*width+pixel] |= 0xFF << 24;

			if(image[row*width+pixel]==0xFF000000 ||
				image[row*width+pixel]==0xFFFF00FF) {
				image[row*width+pixel] = 0;
			}

			bitIndex += header->depth;
		}
	}
}


#endif
#endif
