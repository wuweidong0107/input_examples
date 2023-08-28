CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc

all: input-read input-status

%: %.c
	$(CC) $^ -o $@ 

clean:
	rm -f input-read input-status