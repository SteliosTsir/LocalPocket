SDK_BASE = $${HOME}/pbsdk/SDK-B288/usr
TARGET = arm-obreey-linux-gnueabi
SYSROOT = $(SDK_BASE)/$(TARGET)/sysroot
CC = $(SDK_BASE)/bin/$(TARGET)-clang

# Add the new src files here!
SOURCES = src/main.c src/network.c src/ui.c
OBJECTS = $(SOURCES:.c=.o)
APP_NAME = LocalPocket.app

CFLAGS = -I./include -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/freetype2 -Wall -O2 -march=armv7-a -mfloat-abi=softfp
LDFLAGS = -L$(SYSROOT)/usr/local/lib -L$(SYSROOT)/usr/lib -linkview -lm

all: $(APP_NAME)

$(APP_NAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(APP_NAME) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(APP_NAME)