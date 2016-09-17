#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mysh.h"

#define BUFSIZE 512
#define ERROR "An error has occurred\n"
#define DELIMITERS " "

/**
 * This is the main shell loop that 
 * launches other functions from it
 */
void mysh_loop (void) {
	char *input, **args;
	int status = 1;

	do {
		write(STDOUT_FILENO, "mysh> ", sizeof("mysh> "));
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
		c = getchar();
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
	int status, stdout_fd;

	if (redirect_out(args)) {
		stdout_fd = dup(STDOUT_FILENO); // preserve stdout file # and replace with the 
		dup2(out_fd, STDOUT_FILENO);	// redirection file descriptor
	}

	if (strcmp(args[0], "cd") == 0) return mysh_cd(args);
	else if (strcmp(args[0], "exit") == 0) return mysh_exit();	

	pid = fork();
	if (pid == 0) {	// child process
		if (execvp(args[0], args) < 0) print_error();
	} else if (pid < 0) { // error condition
		print_error();
	} else { // parent process
		pid = wait(&status);
		dup2(stdout_fd, STDOUT_FILENO); // restore stdout file #
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
 * Determines if args contains '>' and if it does opens the 
 * following file and returns 1 otherwise returns 0
 */
int redirect_out (char **args) {
	for (int i = 0; args[i] != NULL; i++) {
		if (args[i][0] == '>') {
			if ((out_fd = open (args[i+1], O_CREAT|O_TRUNC|O_RDWR)) < 0) print_error();
			for (int j = i; args[j+1] != NULL; j++) {
				args[j] = NULL;
			}
			return 1;
		}
	}
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
