
CFLAGS=-g -fsigned-char -Wall -Wextra -pedantic -l gpib -L/usr/local/lib

tektool : tektool.c
#	gcc $(CFLAGS) tektool.c -o tektool

.PHONY : clean
clean :
	-rm -fv tektool
