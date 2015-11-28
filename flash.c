#include "main.h"
#include "frser-flashapi.h"
#include "uart.h"
#include "pm49fl004t.h"

enum emu_chip {
	CHIP_NONE,
	CHIP_PM49FL004T
} chip = CHIP_PM49FL004T;

void flash_select_protocol(uint8_t proto) {
	(void)proto;
}

uint8_t flash_read(uint32_t addr) {
	addr |= 0xFF000000; /* for the time we still use SPv1 */
	switch (chip) {
		default:
			return 0xFF;
		case CHIP_PM49FL004T:
			return pm49fl004t_read(addr);
	}

}

void flash_write(uint32_t addr, uint8_t data) {
	addr |= 0xFF000000; /* for the time we still use SPv1 */
	switch (chip) {
		default:
			break;
		case CHIP_PM49FL004T:
			pm49fl004t_write(addr, data);
			break;
	}
}

void flash_readn(uint32_t addr, uint32_t len) {
	while (len--) SEND(flash_read(addr++));
}
	
