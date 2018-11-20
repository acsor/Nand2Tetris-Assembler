# MIT License
# 
# Copyright (c) 2018 Oscar
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
cc=gcc
flags=-Wall


.PHONY:	clear
all: assembler test.out scratch.out


assembler: assembler.c lexer.o utils.o
	$(cc) $(flags) -o assembler $^

test.out: test.c lexer.o utils.o memcache.o
	$(cc) $(flags) -o test.out $^

# TO-DO `*.h' files should not be included in compilation.
lexer.o: lexer.c lexer.h
	$(cc) $(flags) -c $^

utils.o: utils.c utils.h
	$(cc) $(flags) -c $^

memcache.o: memcache.c memcache.h
	$(cc) $(flags) -c $^


clear:
	rm -f assembler *.o *.out *.gch
