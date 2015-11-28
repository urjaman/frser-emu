#include "main.h"
#include "pm49fl004t.h"

static uint8_t chipmem[512*1024];

enum cmd_chip_state {
	S_CYC_0,
	S_CYC_1,
	S_CYC_2,
	S_CYC_ER1,
	S_CYC_ER2,
	S_CYC_ER3,
	S_CYC_BP,
	S_CYC_PID
} s = S_CYC_0;

static uint8_t block_lock_regs[8] = { 0x1,0x1,0x1,0x1, 0x1,0x1,0x1,0x1 };

uint8_t pm49fl004t_read(uint32_t addr) {
	if (addr < 0xFFF80000) {
		if ((addr & 0xFFF8FFFF) == 0xFFB80002) {
			uint8_t block = ((addr >> 16) & 0xF) - 8;
			return block_lock_regs[block];
		}
		return 0xFF;
	}
	uint32_t ladd = addr - 0xFFF80000;
	const uint8_t id_data[4] = { 0x9D, 0x6E, 0x7F, 0x7F };
	if (s == S_CYC_PID) {
		return id_data[ladd&3];
	}
	uint32_t block = ladd & ~(65535);
	uint8_t lr = block_lock_regs[block >> 16];
	s = S_CYC_0;
	if (lr & 4) return 0xFF; /* Read Lock */
	return chipmem[ladd];
}

void pm49fl004t_write(uint32_t addr, uint8_t data)
{
	if (addr < 0xFFF80000) {
		if ((addr & 0xFFF8FFFF) == 0xFFB80002) {
			uint8_t block = ((addr >> 16) & 0xF) - 8;
			uint8_t lr = block_lock_regs[block];
			if (lr & 2) return;
			block_lock_regs[block] = data & 0x7;
		}
		return;
	}
	uint16_t sdpa = addr & 0xFFFF;
	uint32_t ladd = addr - 0xFFF80000;
	uint32_t block = ladd & ~(65535);
	uint8_t lr = block_lock_regs[block >> 16];
	switch (s) {
		case S_CYC_0:
			if ((sdpa == 0x5555) && (data == 0xAA)) {
				s = S_CYC_1;
				break;
			}
			break;
		case S_CYC_1:
			if ((sdpa == 0x2AAA) && (data == 0x55)) {
				s = S_CYC_2;
				break;
			}
			s = S_CYC_0;
			break;
		case S_CYC_2:
			if (sdpa == 0x5555) {
				if (data == 0x80) {
					s = S_CYC_ER1;
					break;
				}
				if (data == 0xA0) {
					s = S_CYC_BP;
					break;
				}
				if (data == 0x90) {
					s = S_CYC_PID;
					break;
				}
				if (data == 0xAA) {
					s = S_CYC_1;
					break;
				}
			}
			s = S_CYC_0;
			break;
		case S_CYC_BP:
			if (lr & 1) { /* Write Locked */
				s = S_CYC_0;
				break;
			}
			chipmem[ladd] &= data;
			s = S_CYC_0;
			break;
		case S_CYC_ER1:
			if ((sdpa == 0x5555) && (data == 0xAA)) {
				s = S_CYC_ER2;
				break;
			}
			s = S_CYC_0;
			break;
		case S_CYC_ER2:
			if ((sdpa == 0x2AAA) && (data == 0x55)) {
				s = S_CYC_ER3;
				break;
			}
			s = S_CYC_0;
			break;
		case S_CYC_ER3:
			if (lr & 1) { /* Write lock */
				s = S_CYC_0;
				break;
			}
			if (data == 0x30) {
				/* Sector Erase */
				uint32_t sector = ladd & ~(4095);
				memset(chipmem+sector, 0xFF, 4096);
				s = S_CYC_0;
				break;
			}
			if (data == 0x50) {
				/* Block Erase */
				memset(chipmem+block, 0xFF, 64*1024);
				s = S_CYC_0;
				break;
			}
			s = S_CYC_0;
			break;
		case S_CYC_PID:
			if (data == 0xF0) {
				s = S_CYC_0;
			}
			break;
	}			
}
