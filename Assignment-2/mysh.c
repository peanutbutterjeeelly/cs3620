#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 512
#define ERROR "An error has occurred\n"
#define DELIMITERS " "

void mysh_loop (void);
char * mysh_read(void);
char ** mysh_parse(char *);
int mysh_run(char **);
void print_error(void);

const char ** functions = {"cd", "exit"};

int main (int argc, char *argv[]) {
		
	mysh_loop ();

	return EXIT_SUCCESS;
}

void mysh_loop (void) {
	char *input, **args;
	int status = 1;

	do {
		printf("mysh> ");
		input = mysh_read();
		args = mysh_parse(input);
		for (int i = 0; args[i] != NULL; i++) {
			printf("%s\n", args[i]);
		}
		status = mysh_run(args);
		printf("Status: %d\n", status);
	} while (status);
}

char * mysh_read (void) {
	int c, i = 0;
	char * buffer = malloc(sizeof(char)*BUFSIZE);
	
	if (buffer == NULL) {
		print_error();
	}

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

char ** mysh_parse (char *input) {
	int i = 0;
	char *token;
	char **tokens = malloc(BUFSIZE*sizeof(char*));

	if (tokens == NULL) {
		print_error();
	}

	token = strtok(input, DELIMITERS);
	while (token != NULL) {
		tokens[i] = token;
		i++;	
		token = strtok(NULL, DELIMITERS);
	}
	tokens[i] = NULL;
	return tokens;
}

/**
 * This is where different arguments get forked off
 * into running processes. Returns 1 on success and
 * 0 on error.
 */
int mysh_run (char **args) {
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0) {	// child process
		if (execvp(args[0], args) < 0) {
			print_error();
		}
	} else if (pid < 0) {
		print_error();
	} else {
		pid = wait(&status);
	}
	return 1;
}

void print_error (void) {
	write (STDERR_FILENO, ERROR, strlen(ERROR));
	exit(EXIT_FAILURE);
}
