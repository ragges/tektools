
CFLAGS=-g -fsigned-char -Wall -Wextra -pedantic -l gpib -L/usr/local/lib

getcaldata : getcaldata.c
#	gcc $(CFLAGS) getcaldata.c -o getcaldata

.PHONY : clean
clean :
	-rm -fv getcaldata
