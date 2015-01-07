TARGET = bin/matrixmult
LIBS = -lm -lpthread
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean


default: clean $(TARGET) run
all: default

run: $(TARGET)
	bin/matrixmult 0 1 2


OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ -O3

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@ -O3

clean:
	-rm -f *.o
	-rm -f $(TARGET)