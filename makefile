a.out: compileAll
	gcc -ansi -Wall -pedantic *.o 
	rm -f *.o

compileAll: *.h *.c
	gcc -ansi -Wall -pedantic -c *.c
