CC ?= cc
CFLAGS ?= -Wvla -Wall -std=c99 
SDL ?= /Library/Frameworks

LDFLAGS = -F$(SDL) -framework sdl3 -Wl,-rpath,$(SDL)

TARGET = main
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)