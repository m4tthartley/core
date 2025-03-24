//
//  net.h
//  Core Networking
//
//  Created by Matt Hartley on 20/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

// TODO: Flesh out more next time I do some networking


#ifndef __CORE_NET_HEADER__
#define __CORE_NET_HEADER__


// #include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "core.h"


enum {
	NET_STREAM = 0x1,
	NET_DATAGRAM = 0x2,
};

typedef struct {
	int value;
	char str[6];
} netport;

typedef struct {
	int fd;
	int flags;
	struct addrinfo* servinfo;
	char ipstr[INET6_ADDRSTRLEN];
	// char portstr[6];
	netport port;
} netsocket;


#	ifdef CORE_IMPL


// netsocket net_socket_datagram_create(u16 port);

netport __net_prepare_port(int port) {
	netport result = {
		.value = port,
	};
	int length = snprintf(result.str, 6, "%hu", (unsigned short)port);
	if (length > 5) {
		print_err("Port is too large");
		return (netport){0};
	}
	return result;
}

void __net_print_socket_ips(netsocket sock) {
	struct addrinfo* addr = sock.servinfo;
	while (addr) {
		char ipstr[INET6_ADDRSTRLEN] = {0};
		if (addr->ai_family == AF_INET) {
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr->ai_addr;
			inet_ntop(addr->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
			print("IPv4 %s:%s", ipstr, sock.port.str);
		}
		if (addr->ai_family == AF_INET6) {
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)addr->ai_addr;
			inet_ntop(addr->ai_family, &(ipv6->sin6_addr), ipstr, sizeof(ipstr));
			print("IPv6 %s:%s", ipstr, sock.port.str);
		}
		addr = addr->ai_next;
	}
}

void __net_print_conn_ip() {
	
}

b32 net_socket_valid(netsocket sock) {
	return sock.fd != -1;
}

netsocket net_server(u16 port, int flags) {
	netsocket sock = {0};

	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// int length = snprintf(sock.portstr, 6, "%hu", port);
	// if (length > 5) {
	// 	print_err("Port is too large");
	// 	return (netsocket){0};
	// }
	sock.port = __net_prepare_port(port);
	if (!sock.port.value) {
		return (netsocket){0};
	}

	// struct addrinfo* servinfo;
	if (getaddrinfo(NULL, sock.port.str, &hints, &sock.servinfo)) {
		print_err("getaddrinfo() failed");
		return (netsocket){0};
	}

	// __net_print_socket_ips(sock);

	struct addrinfo* addr = sock.servinfo;
	while (addr) {
		sock.fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sock.fd == -1) {
			print_err("socket() failed");
			continue;
		}

		int opt = 1;
		if (setsockopt(sock.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
			print_err("setsockopt() failed");
		}

		if (/*setting for setting timeout*/ FALSE) {
	#ifdef __WIN32__
			int timeout = 5000;
	#elif defined(__POSIX__)
			struct timeval timeout = {5, 0};
	#endif
			if (setsockopt(sock.fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
				print_err("SO_RCVTIMEO %s", strerror(errno));
			}
		}

		if (bind(sock.fd, addr->ai_addr, addr->ai_addrlen) == -1) {
			print_err("bind() %s", strerror(errno));
			continue;
		}

		break;
	}

	freeaddrinfo(sock.servinfo);

	if (listen(sock.fd, 10) == -1) {
		print_err("listen() failed");
	}

	return sock;
}

// void net_socket_listen(netsocket sock) {
// 	int opt = 1;
// 	if (setsockopt(sock.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
// 		print_err("setsockopt() failed");
// 	}

// #ifdef __WIN32__
// 	int timeout = 5000;
// #elif defined(__POSIX__)
// 	struct timeval timeout = {5, 0};
// #endif
// 	if (setsockopt(sock.fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
// 		print_err("SO_RCVTIMEO %s", strerror(errno));
// 	}

// 	if (bind(sock.fd, sock.servinfo->ai_addr, sock.servinfo->ai_addrlen) == -1) {
// 		print_err("bind() %s", strerror(errno));
// 	}

// 	freeaddrinfo(sock.servinfo);

// 	if (listen(sock.fd, 10) == -1) {
// 		print_err("listen() failed");
// 	}
// }

netsocket net_accept(netsocket sock) {
	netsocket conn = {0};

	struct sockaddr_storage conn_addr;
	socklen_t addrlen = sizeof(conn_addr);
	conn.fd = accept(sock.fd, (struct sockaddr*)&conn_addr, &addrlen);
	if (conn.fd == -1) {
		print_err("accept failed");
	}

	struct sockaddr_storage addr = {0};
	socklen_t addr_len = sizeof(addr);
	if (getpeername(conn.fd, (struct sockaddr*)&addr, &addr_len) != -1) {
		// char buffer[INET6_ADDRSTRLEN];
		if (addr.ss_family == AF_INET) {
			struct sockaddr_in ipv4 = *(struct sockaddr_in*)&addr;
			inet_ntop(AF_INET, &ipv4, conn.ipstr, sizeof(conn.ipstr));
			// print("ipv4 connection from %s", conn.ipstr);
		}
		if (addr.ss_family == AF_INET6) {
			struct sockaddr_in6 ipv6 = *(struct sockaddr_in6*)&addr;
			inet_ntop(AF_INET6, &ipv6, conn.ipstr, sizeof(conn.ipstr));
			// print("ipv6 connection from %s", conn.ipstr);
		}
	} else {
		print_err("getpeername error:");
	}

	return conn;
}

void net_conn_close(netsocket sock) {
#ifdef __WIN32__
	closesocket(sock);
#endif
#ifdef __POSIX__
	close(sock.fd);
#endif
}

size_t net_receive(netsocket conn, void* buffer, size_t buf_size) {
	// char buffer[0x10000] = {0};
	// int written = 0;
	int bytes = recv(conn.fd, buffer, buf_size, 0);
	if (bytes == -1) {
		print("recv() failed with the error: %s", strerror(errno));
		bytes = 0;
	}
	return bytes;
	// while ((bytes = recv(conn, buffer+written, sizeof(buffer)-written, 0)) > 0) {
	// 	written += bytes;
	// 	if (s_find(buffer, "\r\n\r\n", NULL)) {
	// 		http_response(conn, buffer);
	// 		goto next;
	// 	}
	// }

	// if (!s_find(buffer, "\r\n\r\n", NULL)) {
	// 	core_print("Incomplete request");
	// }
}

netsocket net_connect(char* address, int port) {
	netsocket sock = {0};

	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	sock.port = __net_prepare_port(port);

	if (getaddrinfo(address, sock.port.str, &hints, &sock.servinfo)) {
		print_err("getaddrinfo() failed");
		return (netsocket){0};
	}

	// __net_print_socket_ips(sock);
	sock.port = __net_prepare_port(port);

	struct addrinfo* addr;
	for (addr = sock.servinfo; addr; addr = addr->ai_next) {
		sock.fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sock.fd == -1) {
			print_err("socket() failed");
			continue;
		}

		if (connect(sock.fd, addr->ai_addr, addr->ai_addrlen) == -1) {
			print_err("connect() failed");
			continue;
		}

		break;
	}

	if (!addr) {
		sock.fd = -1;
		print_err("Failed to connect to %s", address);
	}

	return sock;
}

b32 net_send(netsocket conn, void* data, u32 size) {
	if (net_socket_valid(conn)) {
		int result = send(conn.fd, data, size, 0);
		if (result == -1) {
			print_err("send() failed");
			return FALSE;
		}
	} else {
		print_err("trying to send() on invalid socket");
		return FALSE;
	}

	return TRUE;
}

void net_close(netsocket sock) {
	close(sock.fd);
}


#	endif
#endif
