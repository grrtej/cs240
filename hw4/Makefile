CFLAGS = -std=c99 -Wpedantic -Wall -Wextra

bin = ksim

# Build: make
# Run: ./ksim
all: $(bin)

# Build (Debug): make clean debug
# Debug: gdb ./ksim
debug: CFLAGS += -O0 -g
debug: $(bin)

# Dependencies
ksim: ksim.o process.o queue.o line.o tok.o

ksim.o: ksim.c process.h queue.h line.h tok.h
process.o: process.c process.h
queue.o: queue.c queue.h process.h
line.o: line.c line.h
tok.o: tok.c tok.h

clean:
	$(RM) $(bin) ksim.o process.o queue.o line.o tok.o

.PHONY: all debug clean
