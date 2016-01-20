
CFLAGS		= -Wall -Werror -Wextra

.PHONY: all
all: casio-get screen-to-xpm

screen-to-xpm: screen-to-xpm.o packet.o
	$(CC) -o $@ $^ $(LDFLAGS)

casio-get: casio-get.o error.o port.o packet.o
	$(CC) -o $@ $^ $(LDFLAGS) -lserialport


%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean
clean:
	rm -fv casio-get screen-to-xpm *.o
