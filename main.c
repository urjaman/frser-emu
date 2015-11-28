/*
 * This file is part of the frser-emu project.
 *
 * Copyright (C) 2015 Urja Rannikko <urjaman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "main.h"
#include <stdio.h>
#include <errno.h>
#include "frser.h"
#include "uart.h"

int main(void) {
	int r;
	const uint16_t port = 9876;
	printf("Trying to listen on port %d\n", port);
	if ((r=server_init(port))) {
		fprintf(stderr,"Opening TCP server failed (%d, %d)\n", r, errno);
		return 1;
	}
	frser_main();
	return 0;
}
