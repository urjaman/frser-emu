#include "main.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include "uart.h"

#define TBUFSZ 4096

static int server_fd = -1;
static int session_fd = -1;

static uint8_t rxbuf[TBUFSZ];
static int rxbuf_dsz=0;
static int rxbuf_off=0;

static uint8_t txbuf[TBUFSZ];
static int txbuf_dsz=0;

static int server_accept(void) {
	if (session_fd>=0) {
		close(session_fd);
		session_fd = -1;
	}
	do {
		session_fd = accept(server_fd,0,0);
		if (session_fd==-1) {
			if (errno==EINTR) continue;
			return -4;
		}
	} while (session_fd==-1);
	int flag=1;
	if (setsockopt(session_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int))) {
		fprintf(stderr,"TCP_NODELAY setting failed\n");
	}
	rxbuf_dsz = 0;
	rxbuf_off = 0;
	txbuf_dsz = 0;
	return 0;
}

int server_init(uint16_t port)
{
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_fd==-1) {
		return -1;
	}

	int reuseaddr=1;
	setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr));
	
	if (bind(server_fd,(struct sockaddr*)&addr,sizeof(struct sockaddr_in))==-1) {
		return -2;
	}

	if (listen(server_fd, SOMAXCONN)) {
		return -3;
	}

	return server_accept();
}

void server_tx_flush(void) {
	int txbuf_off = 0;
	do {
		ssize_t rv = write(session_fd, txbuf + txbuf_off, txbuf_dsz - txbuf_off);
		if (rv < 0) {
			if (errno==EAGAIN) continue;
			if (server_accept()) {
				fprintf(stderr,"Cannot accept new connection (%d)\n", errno);
				exit(1);
			}
			continue;
		}
		txbuf_off += rv;
	} while (txbuf_dsz - txbuf_off);
	txbuf_dsz = 0;
}

uint8_t uart_recv(void) {
	if (rxbuf_off<rxbuf_dsz) {
		return rxbuf[rxbuf_off++];
	}
	if (txbuf_dsz) server_tx_flush();
	rxbuf_off = 0;
	rxbuf_dsz = 0;
	do {
		errno = 0;
		ssize_t rv = read(session_fd, rxbuf, TBUFSZ);
		if (rv <= 0) {
			if (errno==EAGAIN) continue;
			if (server_accept()) {
				fprintf(stderr,"Cannot accept new connection (%d)\n", errno);
				exit(1);
			}
			continue;
		}
		rxbuf_dsz = rv;
	} while (!rxbuf_dsz);
	return rxbuf[rxbuf_off++];
}


void uart_send(uint8_t d) {
	txbuf[txbuf_dsz++] = d;
	if (txbuf_dsz>=TBUFSZ) server_tx_flush();
}
