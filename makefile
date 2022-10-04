CC = clang
CFLAGS = -Wall -Wextra -pedantic -g -fsanitize=address -fno-omit-frame-pointer

sopas: main.c
	$(CC) $(CFLAGS) -o $@ $<
