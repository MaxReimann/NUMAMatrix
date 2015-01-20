TARGET = bin/matrixmult
MKLROOT = /opt/intel/Compiler/11.1/072/mkl/
MKLLIB = /opt/intel/Compiler/11.1/072/mkl/lib/em64t
LIBS = -lm -lpthread -lrt -lnuma -fopenmp -Wl,--start-group $(MKLLIB)/libmkl_intel_ilp64.a $(MKLLIB)/libmkl_core.a $(MKLLIB)/libmkl_gnu_thread.a -Wl,--end-group 
CC = gcc-4.8
CFLAGS = -g -Wall -std=gnu99  -fopenmp -DMKL_ILP64  -m64 -I$(MKLROOT)/include

.PHONY: default all clean


default: clean $(TARGET) run
all: default

run: $(TARGET)
	@echo "" >> output.log
	@date >> output.log
	@echo "" >> output.log

	@bin/matrixmult 1 2 | tee -a output.log

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