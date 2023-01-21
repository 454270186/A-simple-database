xiaofeiDB : main.o repl.o compiler.o
	gcc -o xiaofeiDB main.o repl.o compiler.o


main.o : main.c repl.h compiler.h
	gcc -c main.c

repl.o : repl.c repl.h
	gcc -c repl.c

compiler.o : compiler.c compiler.h
	gcc -c compiler.c

clean : 
	rm xiaofeiDB main.o repl.o compiler.o