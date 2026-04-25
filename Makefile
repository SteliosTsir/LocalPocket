SDK_BASE = /home/stelios/pbsdk/SDK-B288/usr
TARGET = arm-obreey-linux-gnueabi
SYSROOT = $(SDK_BASE)/$(TARGET)/sysroot

CC = $(SDK_BASE)/bin/$(TARGET)-clang

# We must keep the freetype2 path so inkview.h can open
CFLAGS = -I./include -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/freetype2 -Wall -O2 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LDFLAGS = -L$(SYSROOT)/usr/local/lib -L$(SYSROOT)/usr/lib -linkview

SOURCES = src/main.c
OBJECTS = $(SOURCES:.c=.o)
APP_NAME = LocalSendPB.app

all: $(APP_NAME)

$(APP_NAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(APP_NAME)