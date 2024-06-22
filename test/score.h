
#include <core/core.h>
#include <core/net.h>

// 0x0123456789ABCDEF
// 0xCAFEBABE
#define PACKET_KEY_CODE 0xC0FFEE

typedef enum {
	PACKET_SUBMIT_SCORE,
	PACKET_REQUEST_SCORES,
	PACKET_SCORES,
} packetcommand;

typedef struct {
	u32 key;
	u32 size;
	packetcommand command;
} packetheader;

typedef struct {
	char name[4];
	u32 value;
} highscore;

typedef struct {
	packetheader header;
	highscore score;
} scorepacket;

typedef struct {
	highscore* scores;
	u32 count;
} highscorelist;

void submit_score(char name[4], u32 value) {
	netsocket sock = net_connect("localhost", 6000);
	packetheader header = { PACKET_KEY_CODE, sizeof(scorepacket), PACKET_SUBMIT_SCORE };
	scorepacket packet = {
		.header = header,
		.score = { { 'a', 'b', 'c', 'd' }, 255 },
	};
	net_send(sock, &packet, header.size);
	net_close(sock);
}

highscorelist request_high_scores(allocator_t* allocator) {
	netsocket sock = net_connect("localhost", 6000);
	packetheader pack = {
		.key = PACKET_KEY_CODE,
		.size = sizeof(packetheader),
		.command = PACKET_REQUEST_SCORES,
	};
	net_send(sock, &pack, sizeof(pack));
	u8 buffer[1024];
	net_receive(sock, buffer, 1024);
	packetheader* header = (packetheader*)buffer;
	highscorelist list;
	if (header->key == PACKET_KEY_CODE && header->command==PACKET_SCORES) {
		list.count = (header->size-sizeof(header)) / sizeof(highscore);
		list.scores = alloc_memory_in(allocator, list.count*sizeof(highscore));
		copy_memory(list.scores, header+1, list.count*sizeof(highscore));
	}
	net_close(sock);
	FOR (i, list.count) {
		print("- %.4s %u", list.scores[i].name, list.scores[i].value);
	}
	return list;
}
