#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include "mysh.h"

#define BUFSIZE 512
#define ERROR "An error has occurred\n"
#define DELIMITERS " "

// for reading/parsing the input
char buffer[BUFSIZE];
char tmp[BUFSIZE];
char * tokens[BUFSIZE] = { 0 };
int arg_count = 0;

// for the shell's I/O
int out_fd;
FILE * inFile;

// mode flags
int redirect_mode = 0, batch_mode = 0, python_mode = 0, background_mode = 0;

/**
 * This is the main shell loop that 
 * launches other functions from it
 */
void mysh_loop (int argc, char ** argv) {
	char *input, **args;
	int status = 1;

	if (argc == 2) {
		inFile = fopen (argv[1], "r");
		if (inFile == NULL) print_error();
		batch_mode = 1;	
	} else {
		inFile = stdin;
	}

	do {
		if (!batch_mode) write(STDOUT_FILENO, "mysh> ", sizeof("mysh> "));
		if (strlen(input = mysh_read()) == 0) continue;
		check_python(input);
		background(input);	
		if ((args = mysh_parse(input)) == NULL) continue;
		status = mysh_run(args);
	} while (status);
}

/**
 * This gets the user's input form stdin and 
 * returns it in the buffer
 */
char * mysh_read (void) {
	int c, length, i = 0, j = 0;

	fflush(inFile);
	memset (buffer, 0, sizeof(buffer));
	while (1) {
		if (i >= BUFSIZE) {
			print_error(); // input size check
			exit(EXIT_FAILURE);		
		}
		c = fgetc(inFile);
		if (c == EOF || c == '\n') {
			buffer[i] = '\0';
			break;
		} else {
			buffer[i] = c;
			i++;
		}
	}
	if (batch_mode) write (STDOUT_FILENO, buffer, sizeof(buffer));
	
	// this looks at the possible 'command>output' scenario and forces spaces into it so that
	// it goes to 'command > output'
	memset(tmp, 0, sizeof(tmp));
	for (i = 0, j = 0; i < strlen(buffer); i++, j++) {
                if ((i < strlen(buffer)-2) && isalpha(buffer[i]) && buffer[i+1] == '>' && isalpha(buffer[i+2])) {
                                tmp[j] = buffer[i]; tmp[j+1] = ' '; tmp[j+2] = buffer[i+1]; tmp[j+3] = ' '; tmp[j+4] = buffer[i+2];
                                j += 4;
				i += 2;
                } else {
                        tmp[j] = buffer[i];
       		}
	}	

	return tmp;
}

/**
 * Uses the strtok() function to split the user input
 * into corresponding individual commands to be run.
 */
char ** mysh_parse (char *input) {
	int i = 0;
	char *token;

	token = strtok(input, DELIMITERS);
	if (token == NULL) return NULL;
	while (token != NULL) {
		tokens[i] = token;
		i++;	
		token = strtok(NULL, DELIMITERS);
	}
	arg_count = i;
	tokens[arg_count] = NULL; // null terminate array
	return tokens;
}

/**
 * This is where different arguments get forked off
 * into running processes. Returns 1 on success and
 * 0 on error.
 */
int mysh_run (char **args) {
	pid_t cpid, wpid;	// get current process PID
	int i, status, stdout_fd;

	if (redirect_out(args)) {
		for (i = 0; args[i] != NULL; i++);
		if ((out_fd = open (args[i+1], O_CREAT|O_TRUNC|O_WRONLY, 0666)) < 0) print_error();
		redirect_mode = 1;
		stdout_fd = dup(STDOUT_FILENO); // preserve stdout file # and replace with the 
		dup2(out_fd, STDOUT_FILENO);	// redirect stout to our output file
	}

	if (strcmp(args[0], "cd") == 0) return mysh_cd(args);
	else if (strcmp(args[0], "exit") == 0) return mysh_exit();
	else if (strcmp(args[0], "wait") == 0) {
		while ((wpid = wait(&status)) > 0);
		return 1;
	}

	cpid = fork();
	if (cpid == 0) {	// child process
		execvp(args[0], args);
		print_error();
		exit(EXIT_FAILURE);
	} else if (cpid < 0) { // error condition
		print_error();
	} else { // parent process
		if (!background_mode) wpid = wait(&status);
		if (redirect_mode) {
			dup2(stdout_fd, STDOUT_FILENO); // restore stdout
			close (out_fd);
		}
	}
	return 1;
}

/**
 * Uses the built in chdir() function for the "cd" command
 *
 */
int mysh_cd (char ** args) {
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
int redirect_out (char ** args) {
	for (int i = 0; args[i] != NULL; i++) {
		if (args[i][0] == '>') {
			for (int j = i; args[j+1] != NULL; j++) {
				args[j] = NULL;
			}
			return 1;
		}
	}
	return 0;
}

/**
 * Determines if args contains '.py' and if it does returns
 * the correctly formatted python command "python file.py" 
 * and otherwise returns back the original input string
 */
void check_python (char * input) {
	int i, j, length = strlen(input);
	char * tmp = malloc (length);

	strcpy (tmp, input);
	for (i = 0; i < length-2; i++) {
		if (input[i] == '.' && input[i+1] == 'p' && input[i+2] == 'y') {
			memset (input, 0, sizeof(input));			
			// gets into a readable format 'python file.py arg1 arg2 ... '			
			strcpy (input, "python "); strcpy (input+strlen("python "), tmp);
			python_mode = 1;
		}
	}
}

/**
 * Determines if there is a '&' in the input and returns 
 * 1 if there is and returns 0 otherwise
 */
int background (char * input) {
	int length = strlen(input);
	if (input[length-1] == '&') {
		input[length-1] = '\0';
		background_mode = 1;		
		return 1;
	}
	return 0;
}

/**
 * Call this for every error condition
 *
 */
void print_error (void) {
	write (STDERR_FILENO, ERROR, strlen(ERROR));
}
