
#include <core/core.h>
#include <core/core.c>

#define CORE_IMPL
#include <core/net.h>

#include "score.h"

int main() {
	print("Score Server");

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
			scorepacket score;
			score = *(scorepacket*)buffer;

			if (score.header.id != PACKET_ID) {
				print_error("invalid packet: %x, %u", score.header.id, score.header.size);
				continue;
			}
			if (score.header.size != sizeof(scorepacket)) {
				print_error("invalid packet: %x, %u", score.header.id, score.header.size);
				continue;
			}
			print("score: %c%c%c%c, %u", score.name[0], score.name[1], score.name[2], score.name[3], score.value);
		}
	}
	
	return 0;
}