all: gccollect.c gccollect.h main.c
	gcc -g -Wall gccollect.c main.c -o collect
