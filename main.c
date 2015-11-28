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
