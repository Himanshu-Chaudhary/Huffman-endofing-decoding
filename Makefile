all: huffencode huffdecode

clean:
	rm huffencode huffdecode

huffencode: huffman.h huffman.c huffencode.c
	gcc -Wall -ansi -pedantic -o huffencode huffman.c huffencode.c

huffdecode: huffman.h huffman.c huffdecode.c
	gcc -Wall -ansi -pedantic -o huffdecode huffman.c huffdecode.c


