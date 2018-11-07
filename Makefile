cc=gcc
flags=-Wall


.PHONY:	clear


test.out: test.c parser.o
	$(cc) $(flags) -o test.out $^

parser.o: parser.c parser.h
	$(cc) $(flags) -c $^

clear:
	rm *.o *.out
