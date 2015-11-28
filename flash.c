#include "main.h"
#include "frser-flashapi.h"
#include "uart.h"

void flash_select_protocol(uint8_t proto) {
	(void)proto;
}

uint8_t flash_read(uint32_t addr) {
	return 0xFF;
}

void flash_write(uint32_t addr, uint8_t data) {
}

void flash_readn(uint32_t addr, uint32_t len) {
	while (len--) SEND(0xFF);
}
	
