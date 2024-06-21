
#include <core/core.h>
#include <core/core.c>

#define CORE_IMPL
#include <core/net.h>

int main() {
	print("Score Server");

	char hostname[64];
	gethostname(hostname, 64);
	print("Hostname: %s", hostname);

	netsocket sock = net_socket_create(6000, NET_STREAM);
	net_socket_listen(sock);

	for (;;) {
		print("Waiting for connection...");
		netsocket conn = net_conn_accept(sock);
		print("Got connection");
		u8 buffer[1024] = {0};
		int size;
		while ((size = net_receive(conn, buffer, 1024))) {
			print_inline("%s", buffer);
		}
	}
	
	return 0;
}