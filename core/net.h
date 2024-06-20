//
//  net.h
//  Core Networking
//
//  Created by Matt Hartley on 20/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#ifndef __CORE_NET_HEADER__
#define __CORE_NET_HEADER__


#include <string.h>
#include <errno.h>
// #include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "core.h"


typedef struct {
	int fd;
	struct addrinfo* servinfo;
} netsocket;

netsocket net_socket_create(u16 port) {
	netsocket sock;

	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	char port_str[6] = {0};
	int length = snprintf(port_str, 6, "%hu", port);
	if (length > 5) {
		print_error("Port is too large");
		return (netsocket){0};
	}

	// struct addrinfo* servinfo;
	if (getaddrinfo(NULL, port_str, &hints, &sock.servinfo)) {
		print_error("getaddrinfo() failed");
		return (netsocket){0};
	}

	struct addrinfo* addr = sock.servinfo;
	while (addr) {
		char* ip;
		char ipstr[INET6_ADDRSTRLEN];
		inet_ntop(addr->ai_family, addr, ipstr, sizeof(ipstr));
		print("ip %s", ipstr);
		addr = addr->ai_next;
	}

	sock.fd = socket(sock.servinfo->ai_family, sock.servinfo->ai_socktype, sock.servinfo->ai_protocol);
	if (sock.fd == -1) {
		print_error("socket() failed");
		return (netsocket){0};
	}

	return sock;
}

void net_socket_listen(netsocket sock) {
	int opt = 1;
	if (setsockopt(sock.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
		print_error("setsockopt() failed");
	}

#ifdef __WIN32__
	int timeout = 5000; // Windows only, linux needs timeval
#elif defined(__POSIX__)
	struct timeval timeout = {5, 0};
#endif
	if (setsockopt(sock.fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
		print_error("SO_RCVTIMEO %s", strerror(errno));
	}

	if (bind(sock.fd, sock.servinfo->ai_addr, sock.servinfo->ai_addrlen) == -1) {
		print_error("bind() %s", strerror(errno));
	}

	freeaddrinfo(sock.servinfo);
}


#endif