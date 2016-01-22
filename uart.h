#ifndef _UART_H_
#define _UART_H_

uint8_t uart_recv(void);
void uart_send(uint8_t val);


#define RECEIVE() uart_recv()
#define SEND(n) uart_send(n)

/* For the protocol window size */
#define UART_BUFLEN 4096

int server_init(uint16_t port);

#endif
