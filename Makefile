cc=gcc
flags=-Wall


.PHONY:	clear
all: assembler test.out scratch.out


assembler: assembler.c lexer.o utils.o
	$(cc) $(flags) -o assembler $^

test.out: test.c lexer.o utils.o
	$(cc) $(flags) -o test.out $^

lexer.o: lexer.c lexer.h
	$(cc) $(flags) -c $^

utils.o: utils.c utils.h
	$(cc) $(flags) -c $^


clear:
	rm assembler *.o *.out *.gch
