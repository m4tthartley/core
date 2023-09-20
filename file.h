
#pragma pack(push, 1)
typedef struct {
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
} bmp_header;
#pragma pack(pop)

typedef struct {
	uint32_t* data;
	bmp_header* header;
} bmp;

bmp loadBmp(char* filename) {
	FILE* fontFile;
	long fileSize;
	void* fontData;
	bmp_header* header;
	uint32_t* palette;
	char* data;
	int rowSize;
	uint32_t* image;
	bmp result;

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
	palette = (uint32_t*)((char*)fontData+14+header->headerSize);
	data = (char*)fontData+header->offset;
	rowSize = ((header->colorDepth*header->bitmapWidth+31) / 32) * 4;
	
	image = (uint32_t*)malloc(sizeof(uint32_t)*header->bitmapWidth*header->bitmapHeight);
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
				uint32_t* chunk = (uint32_t*)((char*)fontData+header->offset+(row*rowSize)+(bitIndex/8));
				uint32_t pi = *chunk;
				if(header->colorDepth<8) {
					pi >>= (header->colorDepth-(bitIndex%8));
				}
				pi &= (((int64_t)1<<header->colorDepth)-1);
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
	
	result.data = image;
	result.header = header;
	return result;
}

