
# identify platform
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
endif


# get version and build time strings
GIT_VERSION := "$(shell git describe --abbrev=7 --dirty --always --tags)"
ifneq ($(GIT_VERSION), "")
	CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif
BUILD_TIME := "$(shell date -u "+%Y-%m-%d %H:%M:%S")"
ifneq ($(BUILD_TIME), "")
	CFLAGS += -DBUILD_TIME=\"$(BUILD_TIME)\"
endif


CFLAGS += -DPROG_NAME=\"$@\"


tekfwtool : target-procs.h tekfwtool.c tekfwtool.h
	$(CC) $(CFLAGS) tekfwtool.c -o tekfwtool

.PHONY : clean
clean :
	-rm -fv tekfwtool
	-rm -rfv tekfwtool.dSYM
