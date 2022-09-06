CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

test: test.c
	$(CC) $(CFLAGS) test.c -o test

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f client test