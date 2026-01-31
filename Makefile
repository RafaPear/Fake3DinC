FILES := main.c
OUT := app

CFLAGS  := $(shell pkg-config --cflags raylib)
LDFLAGS := $(shell pkg-config --libs raylib)

all:
	gcc $(FILES) $(CFLAGS) $(LDFLAGS) -o $(OUT)

run: all
	./app
