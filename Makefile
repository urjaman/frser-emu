PROJECT=frser-emu
DEPS=uart.h main.h Makefile
SOURCES=main.c server.c flash.c pm49fl004t.c
CC=gcc

CFLAGS=-Os -Wl,--relax -g -Wall -W -pipe -flto -fwhole-program

include libfrser/Makefile.frser

all: $(PROJECT).out

$(PROJECT).out: $(SOURCES) $(DEPS)
	$(CC) $(CFLAGS) -I. -o $(PROJECT).out $(SOURCES)

clean:
	rm -f $(PROJECT).bin
	rm -f $(PROJECT).out
	rm -f $(PROJECT).hex
	rm -f $(PROJECT).s
	rm -f *.o

objdump: $(PROJECT).out
	objdump -xdC $(PROJECT).out | less

