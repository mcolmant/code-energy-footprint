CC=cc
CFLAGS=-c -Wall -O0
LIBPFM_LDFLAGS=-L/usr/local/lib -lpfm
LDFLAGS=-lm $(LIBPFM_LDFLAGS)
SOURCES=example.c variables.c code-energy-footprint.c fd_recv.c fd_send.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=example

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

