CFLAGS = -std=c99 -Wpedantic -Wall -Wextra

bin = msh

all: $(bin)

debug: CFLAGS += -O0 -g
debug: $(bin)

msh: msh.o line.o tok.o

msh.o: msh.c line.h tok.h
line.o: line.c line.h
tok.o: tok.c tok.h

clean:
	$(RM) $(bin) msh.o line.o tok.o

.PHONY: all debug clean
