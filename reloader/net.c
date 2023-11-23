
#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct addrinfo addrinfo;
typedef struct sockaddr_storage sockaddr_storage;

#define PORT "53535"

void server() {
	WSADATA wsa_data;
	if(WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
		printf("WSAStartup failed \n");
		exit(1);
	}

	addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	addrinfo* servinfo;
	if (getaddrinfo(NULL, PORT, &hints, &servinfo)) {
		printf("getaddrinfo() failed \n");
		exit(1);
	}

	addrinfo* addr = servinfo;
	while (addr) {
		char* ip;
		char ipstr[INET6_ADDRSTRLEN];
		inet_ntop(addr->ai_family, addr, ipstr, sizeof(ipstr));
		printf("ip %s \n", ipstr);
		addr = addr->ai_next;
	}

	int sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (sock == SOCKET_ERROR) {
		printf("socket() failed \n");
		exit(1);
	}

	if (bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) == SOCKET_ERROR) {
		printf("bind() failed \n");
		exit(1);
	}

	int opt;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == SOCKET_ERROR) {
		printf("setsockopt() failed \n");
		exit(1);
	}

	if (listen(sock, 10) == SOCKET_ERROR) {
		printf("listen() failed \n");
		exit(1);
	}

	sockaddr_storage conn_addr;
	int addrlen = sizeof(conn_addr);
	SOCKET conn = accept(sock, &conn_addr, &addrlen);

	freeaddrinfo(servinfo);
}

void client() {
	// connect()
}

