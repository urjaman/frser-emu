/* This is an example of what libfrser expects main.h to contain. */

#include <avr/pgmspace.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <setjmp.h>

#define RAMSTART 0
#define RAMEND 0x4000

#define _delay_us(x)
