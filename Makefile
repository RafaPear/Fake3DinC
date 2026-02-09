FILES := main.c geometry.c projection.c 
OUT := app

CFLAGS  := $(shell pkg-config --cflags raylib) -Wall -Wextra -O3 -ffast-math
LDFLAGS := $(shell pkg-config --libs raylib)

all:
	gcc $(FILES) $(CFLAGS) $(LDFLAGS) -o $(OUT)

run: all
	./app

clean:
	rm $(OUT)
