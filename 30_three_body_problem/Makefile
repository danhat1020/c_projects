CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11
LIBS = -lraylib -lm -lrandom
OUT = main

all:
	$(CC) $(CFLAGS) main.c -o $(OUT) $(LIBS)

clean:
	@rm -f $(OUT)
