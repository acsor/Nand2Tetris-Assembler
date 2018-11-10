cc=gcc
flags=-Wall


.PHONY:	clear
all: test.out


test.out: test.c lexer.o utils.o
	$(cc) $(flags) -o test.out $^

scratch.out: scratch.c
	$(cc) $(flags) -o scratch.out $^

lexer.o: lexer.c lexer.h
	$(cc) $(flags) -c $^

utils.o: utils.c utils.h
	$(cc) $(flags) -c $^


clear:
	rm *.o *.out
