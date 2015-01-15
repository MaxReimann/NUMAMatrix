TARGET = simpleMMM
LIBS = -lm -lpthread -lrt -lnuma
CC = gcc-4.8
CFLAGS = -g -Wall --std=gnu11 -msse2 

.PHONY: default all clean


default: clean $(TARGET) run
all: default

run: $(TARGET)
	@echo "" >> output.log
	@date >> output.log
	@echo "" >> output.log
	@./simpleMMM | tee -a output.log

	@echo "" >> output.log
	@echo "" >> output.log


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