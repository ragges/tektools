
CFLAGS=-g -fsigned-char -Wall -Wextra -pedantic -l gpib -L/usr/local/lib

tekfwtool : target-procs.h tekfwtool.c tekfwtool.h
	$(CC) $(CFLAGS) tekfwtool.c -o tekfwtool

.PHONY : clean
clean :
	-rm -fv tekfwtool
