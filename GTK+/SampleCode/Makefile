SOURCES=$(wildcard *.c)
OUTPUT=demo

CFLAGS=
LDFLAGS=

GTK_CFLAGS=$(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS=$(shell pkg-config --libs gtk+-3.0)

.PHONY: clean

$(OUTPUT): $(SOURCES:.c=.o)
	$(CC) $(LDFLAGS) $(GTK_LIBS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f $(OUTPUT)
