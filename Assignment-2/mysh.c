/**
 * X- simple commands (ls, pwd, wc, etc ...) work
 * - need to work on getting the commands like cd(works), exit(works), etc to work. They are built
 * into the language so need to be called differently, (chdir() for cd)
 * - redirection
 * - reading commands from the script file instead of user input
 * - running the python program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 512
#define ERROR "An error has occurred\n"
#define DELIMITERS " "

void mysh_loop (void);
char * mysh_read(void);
char ** mysh_parse(char *);
int mysh_run(char **);
int mysh_cd(char **);
int mysh_exit(void);
void print_error(void);

FILE * inFile, * outFile;

int main (int argc, char *argv[]) {

	// check for batch file
	if (argc == 2) {
		inFile = fopen (argv[1], "r");
		if (inFile == NULL) print_error();
	} else inFile = stdin;
	outFile = stdout;

	mysh_loop ();

	return EXIT_SUCCESS;
}

/**
 * This is the main shell loop that 
 * launches other functions from it
 */
void mysh_loop (void) {
	char *input, **args;
	int status = 1;

	do {
		printf("mysh> ");
		input = mysh_read();
		args = mysh_parse(input);
		status = mysh_run(args);
	} while (status);
}

/**
 * This gets the user's input form stdin and 
 * returns it in the buffer
 */
char * mysh_read (void) {
	int c, i = 0;
	char * buffer = malloc(sizeof(char)*BUFSIZE);
	// memory alloc error
	if (buffer == NULL) {
		print_error();
	}
	
	// read input into the buffer
	while (1) {
		c = getc(inFile);
		if (c == EOF || c == '\n') {
			buffer[i] = '\0';
			return buffer;
		} else {
			buffer[i] = c;
		}
		i++;
	}
}

/**
 * Uses the strtok() function to split the user input
 * into corresponding individual commands to be run.
 */
char ** mysh_parse (char *input) {
	int i = 0;
	char *token;
	char **tokens = malloc(BUFSIZE*sizeof(char*));
	// memory alloc error
	if (tokens == NULL) {
		print_error();
	}

	token = strtok(input, DELIMITERS);
	while (token != NULL) {
		tokens[i] = token;
		i++;	
		token = strtok(NULL, DELIMITERS);
	}
	tokens[i] = NULL; // null terminate array
	return tokens;
}

/**
 * This is where different arguments get forked off
 * into running processes. Returns 1 on success and
 * 0 on error.
 */
int mysh_run (char **args) {
	pid_t pid;
	int status, out;

	if (strcmp(args[0], "cd") == 0) return mysh_cd(args);
	else if (strcmp(args[0], "exit") == 0) return mysh_exit();

	pid = fork();
	if (pid == 0) {	// child process
		if (execvp(args[0], args) < 0) print_error();
	} else if (pid < 0) { // error condition
		print_error();
	} else { // parent process
		pid = wait(&status);
	}
	return 1;
}

/**
 * Uses the built in chdir() function for the "cd" command
 *
 */
int mysh_cd (char **args) {
	if (args[1] == NULL) { // go to home directory
		if (chdir (getenv("HOME")) != 0) print_error();
	} else {
		if (chdir (args[1]) != 0) print_error();
	}
	return 1;
}


/**
 * returns 0 to break from the main loop
 *
 */
int mysh_exit (void) {
	return 0;
}

/**
 * Call this for every error condition
 *
 */
void print_error (void) {
	write (STDERR_FILENO, ERROR, strlen(ERROR));
	exit(EXIT_FAILURE);
}
