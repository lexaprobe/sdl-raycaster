CC = gcc
CFLAGS = -Wvla -Wall -std=c99 
LDFLAGS = -F/Library/Frameworks -framework sdl3 -Wl,-rpath,/Library/Frameworks

TARGET = main
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)