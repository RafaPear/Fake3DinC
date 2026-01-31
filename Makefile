FILES := main.c geometry.c projection.c 
OUT := app

CFLAGS  := $(shell pkg-config --cflags raylib) -Wall -Wextra
LDFLAGS := $(shell pkg-config --libs raylib)

all:
	gcc $(FILES) $(CFLAGS) $(LDFLAGS) -o $(OUT)

run: all
	./app
