
#include <core/core.h>
#include <core/core.c>

#define CORE_IMPL
#include <core/net.h>

#include "score.h"

// { "ABCD", 255 },
// { {'B', 'A', 'B', 'E'}, 255 },
// { {'M', 'A', 'T', 'T'}, 255 },
struct {
	u32 count;
	highscore scores[64];
} data = {0};

void add_score(highscore score) {
	FOR (i, data.count) {
		if (score.value > data.scores[i].value) {
			print("adding score at index %i", i);
			copy_memory(data.scores + i, data.scores + i + 1, array_size(data.scores)-i-1);
			data.scores[i] = score;
			++data.count;
			break;
		}
	}

	if (!data.count) {
		data.scores[0] = score;
		++data.count;
	}

	file_t score_file = file_open("scores.dat");
	file_write(score_file, 0, &data, sizeof(data));
	file_close(score_file);
}

int main() {
	print("Score Server");

	allocator_t allocator = create_allocator(NULL, MB(1));

	file_t score_file = file_open("scores.dat");
	print("file %i", score_file);
	if (file_valid(score_file)) {
		file_read(score_file, 0, &data, sizeof(data));
		file_close(score_file);
	} else {
		score_file = file_create("scores.dat");
		print("file %i", score_file);
		file_close(score_file);
	}

	char hostname[64];
	gethostname(hostname, 64);
	print("Hostname: %s", hostname);

	netsocket sock = net_server(6000, NET_STREAM);

	for (;;) {
		print("Waiting for connection...");
		netsocket conn = net_accept(sock);
		print("Got connection from: %s", conn.ipstr);
		u8 buffer[1024] = {0};
		int size;
		while ((size = net_receive(conn, buffer, 1024))) {
			// print_inline("%s", buffer);
			
			packetheader header = *(packetheader*)buffer;

			if (header.key != PACKET_KEY_CODE) {
				print_error("invalid packet: %x, %u", header.key, header.size);
				continue;
			}
			if (header.size != size) {
				print_error("Sizes don't match");
				continue;
			}
			if (header.command == PACKET_SUBMIT_SCORE) {

				if (header.size != sizeof(scorepacket)) {
					print_error("invalid packet: %x, %u", header.key, header.size);
					continue;
				}

				scorepacket score;
				score = *(scorepacket*)buffer;
				print("score submitted: %c%c%c%c, %u", score.score.name[0], score.score.name[1], score.score.name[2], score.score.name[3], score.score.value);
				add_score(score.score);
			}
			if (header.command == PACKET_REQUEST_SCORES) {
				print("scores requested");
				print("count: %i", data.count);

				int size = sizeof(packetheader)+(sizeof(highscore)*data.count);
				packetheader* header = alloc_memory_in(&allocator, size);
				header->key = PACKET_KEY_CODE;
				header->size = size;
				header->command = PACKET_SCORES;
				copy_memory(header+1, data.scores, data.count*sizeof(highscore));
				net_send(conn, header, size);
				// print("memory usage: %i", allocator.)
				free_memory_in(&allocator, header);
			}
		}
	}
	
	return 0;
}