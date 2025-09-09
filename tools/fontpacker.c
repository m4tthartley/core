//
//  Created by Matt Hartley on 08/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <alloca.h>
#define CORE_IMPL
#include <core/core.h>

typedef struct __attribute((packed)) {
	char header[2];
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset;
	
	// Windows BITMAPINFOHEADER
	uint32_t headerSize;
	int32_t bitmapWidth;
	int32_t bitmapHeight;
	uint16_t colorPlanes;
	uint16_t colorDepth;
	uint32_t compression;
	uint32_t imageSize;
	int32_t hres;
	int32_t vres;
	uint32_t paletteSize;
	uint32_t importantColors;
} bmp_header_t;

typedef struct __attribute((packed)) {
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint32_t data[];
} bitmap_t;

bitmap_t* R_LoadBitmap(allocator_t* allocator, char* filename) {
	file_t file = sys_open(filename);
	if (file == -1) {
		print("Failed to load bitmap: %s", filename);
		return NULL;
	}
	stat_t fileinfo = sys_stat(file);
	int fileSize = fileinfo.size;
	void* fileData = alloc_memory(allocator, fileSize);
	sys_read(file, 0, fileData, fileSize);
	sys_close(file);

	bmp_header_t* header = (bmp_header_t*)fileData;
	u32* palette = (u32*)((char*)fileData+14+header->headerSize);
	u8* data = (u8*)fileData+header->offset;
	int rowSize = ((header->colorDepth*header->bitmapWidth+31) / 32) * 4;

	// Possibly check whether to alloc or push
	bitmap_t* result = alloc_memory(allocator, sizeof(bitmap_t) + sizeof(u32)*header->bitmapWidth*header->bitmapHeight);
	result->size = header->size;
	result->width = header->bitmapWidth;
	result->height = header->bitmapHeight;
	
	u32* image = (u32*)(result + 1);
	for(int row=0; row<header->bitmapHeight; ++row) {
		int bitIndex=0;
		for(int pixel=0; pixel<header->bitmapWidth; ++pixel) {
			u32* chunk = (u32*)((char*)fileData+header->offset+(row*rowSize)+(bitIndex/8));
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

			image[row*header->bitmapWidth+pixel] |= 0xFF << 24;

			if(image[row*header->bitmapWidth+pixel]==0xFF000000 ||
				image[row*header->bitmapWidth+pixel]==0xFFFF00FF) {
				image[row*header->bitmapWidth+pixel] = 0;
			}

			bitIndex += header->colorDepth;
		}
	}

	return result;
}

int main(int argc, char** argv)
{
	print("Font Packer \n");

	char* inputFilename = NULL;
	char* outputFilename = NULL;
	char* outputDataName = "FONT_DATA";

	for (int argi=1; argi<argc; ++argi) {
		if (strcompare(argv[argi], "-o")) {
			++argi;
			if (argi==argc) {
				print("Invalid output argument \n");
				exit(1);
			}
			outputFilename = argv[argi];
		}
		else if (strcompare(argv[argi], "-name")) {
			++argi;
			if (argi==argc) {
				print("Invalid name argument \n");
				exit(1);
			}
			outputDataName = argv[argi];
		}
		else {
			if (!inputFilename) {
				inputFilename = argv[argi];
			} else {
				print("Invalid argument: %s \n", argv[argi]);
				exit(1);
			}
		}
	}

	if (!inputFilename || !outputFilename) {
		print("Usage: fontpacker $inputFile $outputFile \n");
		exit(1);
	}

	print("%s -> %s \n", inputFilename, outputFilename);

	allocator_t allocator = virtual_heap_allocator(MB(10), 0);

	bitmap_t* bitmap = R_LoadBitmap(&allocator, inputFilename);

	file_t outputFile = sys_create(outputFilename);
	sys_truncate(outputFile, 0);

	if (outputFile) {
		char* str = strformat("#include <stdint.h>\nuint32_t %s[] = {\n", outputDataName);
		sys_write_seq(outputFile, str, strsize(str));
		for (int pixel=0; pixel<bitmap->width*bitmap->height; ++pixel) {
			char* value = strformat("%u,", bitmap->data[pixel]);
			sys_write_seq(outputFile, value, strsize(value));
			if (pixel && pixel % 16 == 0) {
				sys_write_seq(outputFile, "\n", 1);
			}
		}
		str = "\n};\n";
		sys_write_seq(outputFile, str, strsize(str));
		sys_close(outputFile);
	}

	return 0;
}
