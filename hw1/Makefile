CFLAGS = -std=c99 -Wpedantic -Wall -Wextra

bin = hw1

all: $(bin)

debug: CFLAGS += -O0 -g
debug: $(bin)

clean:
	$(RM) $(bin)

.PHONY: all debug clean
