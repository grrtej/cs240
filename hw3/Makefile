CFLAGS = -std=c99 -Wpedantic -Wall -Wextra

# Build: make
# Run: ./msh
all: msh

# Build (Debug): make clean debug
# Run GDB: gdb ./msh
# Run Valgrind: valgrind --leak-check=full ./msh
debug: CFLAGS += -O0 -g
debug: msh

msh: msh.o line.o tok.o

msh.o: msh.c line.h tok.h
line.o: line.c line.h
tok.o: tok.c tok.h

clean:
	$(RM) msh msh.o line.o tok.o

.PHONY: all debug clean
