
ifeq ($(OS),Windows_NT)
	OS_NAME := windows_nt
else
	OS_NAME := $(shell uname -s)
	ifeq ($(OS_NAME), Linux)
# Linux with linux-gpib
		CFLAGS += -g -fsigned-char -Wall -Wextra -pedantic -l gpib -L/usr/local/lib
	endif
	ifeq ($(OS_NAME), Darwin)
# Darwin with NI 488.2 driver
		CFLAGS += -g -fsigned-char /Library/Frameworks/NI4882.framework/Resources/ni4882.o -framework CoreFoundation -I/Library/Frameworks/NI4882.framework/Headers
	endif

	ifeq ($(OS_NAME), Darwin)
# TEST!
		CFLAGS = -g -fsigned-char -framework CoreFoundation -I/Users/ragge/src/ni4882.h-dummy /Users/ragge/src/ni4882.h-dummy/gpib-dummy.o
	endif

endif



getcaldata : getcaldata.c
#	gcc $(CFLAGS) getcaldata.c -o getcaldata

.PHONY : clean
clean :
	-rm -fv getcaldata
	-rm -rfv getcaldata.dSYM
