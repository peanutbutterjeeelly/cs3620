all: shell

shell: shell.o mysh.o
	gcc shell.o mysh.o -o shell

shell.o: shell.c
	gcc -c shell.c

mysh.o: mysh.c
	gcc -c mysh.c

clean:
	rm *o shell
