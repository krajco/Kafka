CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic
CFILES= packet.h flow_list.h flow_list.c packet_parser.h packet_parser.c main.c

producer:
	$(CC) $(CFLAGS) $(CFILES) -lm -o producer

clean:
	rm producer

zip:
	zip  xkrajc17_xgrofc00 *.c *.h *py Makefile README.md docker-compose.yml
