cc=gcc


.PHONY:	clear


test.out: test.c
	$(cc) -Wall -o test.out test.c

clear:
	rm *.o *.out
