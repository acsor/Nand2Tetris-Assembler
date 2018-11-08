cc=gcc
flags=-Wall


.PHONY:	clear
all: test.out


test.out: test.c tokenizer.o utils.o
	$(cc) $(flags) -o test.out $^

scratch.out: scratch.c
	$(cc) $(flags) -o scratch.out $^

tokenizer.o: tokenizer.c tokenizer.h
	$(cc) $(flags) -c $^

utils.o: utils.c utils.h
	$(cc) $(flags) -c $^


clear:
	rm *.o *.out
