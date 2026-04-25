SDK_BASE = /home/stelios/pbsdk/SDK-B288/usr
TARGET = arm-obreey-linux-gnueabi
SYSROOT = $(SDK_BASE)/$(TARGET)/sysroot
CC = $(SDK_BASE)/bin/$(TARGET)-clang

# -march=armv7-a is safe, but let's remove neon just in case it's causing the crash
# KEEP THESE SAFE FLAGS!
CFLAGS = -I./include -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/freetype2 -Wall -O2 -march=armv7-a -mfloat-abi=softfp
LDFLAGS = -L$(SYSROOT)/usr/local/lib -L$(SYSROOT)/usr/lib -linkview -lm

SOURCES = src/main.c
OBJECTS = $(SOURCES:.c=.o)
APP_NAME = LocalSendPB.app

all: $(APP_NAME)

$(APP_NAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(APP_NAME) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(APP_NAME)