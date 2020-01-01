CC=/cygdrive/c/mingw/bin/gcc
CFLAGS=-Wextra -Wall -O2 -Wno-unused -ggdb
M68K=m68k-elf-
M68KCC=$(M68K)gcc
M68CCKFLAGS=-Wall -Wextra -Os -ggdb
M68KOBJCOPY=$(M68K)objcopy
M68KNM=$(M68K)nm

all: tekfwtool.exe target.bin


tekfwtool.exe:	tekfwtool.c target-procs.h
		$(CC) $(CFLAGS) ./gpib-32.obj -o $@ tekfwtool.c

target.bin:	target.elf
		$(M68KOBJCOPY) -O binary $< $@

target.elf:	target.o target.ld
		$(M68KCC) -nostdlib -Wl,-T target.ld -o $@ target.o

target-procs.h:	target.elf gen-procs.sh
		./gen-procs.sh >$@

target.o:	target.c
		$(M68KCC) $(M68KCCFLAGS) -c -o $@ $<
clean:
		rm -f tekfwtool.exe target.o target.elf target.bin
