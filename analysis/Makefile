CC=cc
CFLAGS=-c -Wall -O0
LDFLAGS=
SOURCES=trace.c
OBJECTS=$(SOURCES:.c=.o)

all: $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)

