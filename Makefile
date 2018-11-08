cc=gcc
flags=-Wall


.PHONY:	clear
all: test.out


test.out: test.c tokenizer.o
	$(cc) $(flags) -o test.out $^

tokenizer.o: tokenizer.c tokenizer.h
	$(cc) $(flags) -c $^

clear:
	rm *.o *.out
