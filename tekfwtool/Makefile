
CFLAGS=-g -fsigned-char -l gpib -L/usr/local/lib

tekfwtool : target-procs.h tekfwtool.c tekfwtool.h
#	gcc $(CFLAGS) tekfwtool.c -o tekfwtool

.PHONY : clean
clean :
	-rm -fv tekfwtool
