all: gccollect.c gccollect.h main.c
	gcc gccollect.c main.c -o collect
