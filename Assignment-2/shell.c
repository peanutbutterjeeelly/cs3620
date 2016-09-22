/**
 * X- simple commands (ls, pwd, wc, etc ...) work
 * - need to work on getting the commands like cd(works), exit(works), etc to work. They are built
 * into the language so need to be called differently, (chdir() for cd)
 * - redirection
 * - reading commands from the script file instead of user input
 * - running the python program
 */

#include <stdlib.h>
#include <fcntl.h>
#include "mysh.h"

int main (int argc, char *argv[]) {

	mysh_loop (argc, argv);

	return EXIT_SUCCESS;
}
