
#include <core/core.h>

// 0x0123456789ABCDEF
// 0xCAFEBABE
#define PACKET_ID 0xC0FFEE

typedef struct {
	u32 id;
	u32 size;
} packetheader;

typedef struct {
	packetheader header;
	u32 value;
	char name[4];
} scorepacket;