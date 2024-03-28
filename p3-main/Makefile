CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=gnu18
LOGIN = riteshwar
SUBMITPATH = ~cs537-1/handin/$(LOGIN)/P3

.PHONY: all
all: wsh

wsh: wsh.c wsh.h
	$(CC) $(CFLAGS) -o $@ $^
run: wsh
	./wsh
pack:
	tar -czvf $(LOGIN).tar.gz wsh.h wsh.c Makefile README.md
	
submit: pack
	cp $(LOGIN).tar.gz $(SUBMITPATH)

test:
	~cs537-1/tests/P3/test-cd.csh -c
	~cs537-1/tests/P3/test-exec.csh -c
	~cs537-1/tests/P3/test-job-control.csh -c
	~cs537-1/tests/P3/test-pipe.csh -c


