#ifndef _UART_H_
#define _UART_H_

uint8_t uart_recv(void);
void uart_send(uint8_t val);


#define BAUD 115200

#define RECEIVE() uart_recv()
#define SEND(n) uart_send(n)
#define UART_BUFLEN 2048
#define UARTTX_BUFLEN 0

int server_init(uint16_t port);

#endif
