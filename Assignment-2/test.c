#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char ** argv) {
	FILE * batch = fopen (argv[1], "r");
	char buffer[512];
	while (fgets(buffer, sizeof(buffer), batch)) {
		write (1, buffer, strlen(buffer));
	}
}
