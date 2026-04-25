# Point this to where you cloned the SDK
SDK_PATH = $(HOME)/LocalPocket
CC = $(SDK_PATH)/bin/arm-obreey-linux-gnueabi-gcc

# Tell the compiler where to find headers and libraries
CFLAGS = -I./include -I$(SDK_PATH)/include -Wall -O2
LDFLAGS = -L$(SDK_PATH)/lib -linkview -pthread

# Your source files
SOURCES = src/main.c src/mongoose.c src/cJSON.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = LocalSendPB.app

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)