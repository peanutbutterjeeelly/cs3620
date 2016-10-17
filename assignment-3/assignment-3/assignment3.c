/**
 * Assignment 3
 * Bryan Ehlers
 * UIowa Operating Systems
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1024

// for parsing the file use strtok() with delimits of " "

char * buf; // general purpose reading buffer, make sure to clear before use

void readInfoFile (const char *);

int main (int argc, char ** argv) {
        int fd, r_ret, fileSize, numberOfJobs;
	numberOfJobs = 3;
	int k = 0, j = 0;

        int * arrival = malloc (numberOfJobs*sizeof(int));
        int * execution = malloc (numberOfJobs*sizeof(int));

	if (argc != 2) {
		fprintf(stderr, "usage: ./prog <job file>\n");
		exit(1);
	}
	
	int arr[28];

	readInfoFile (argv[1]);
	for (int i = 0; i < 28; i++) {
		if (isdigit(buf[i])) {
			arr[j++] = buf[i]-'0';
		}
	}
	
	for (int i = 0; i < j; i++) {
		printf("%d ", arr[i]);
	}

	printf("Number of jobs: %d\n", numberOfJobs);
	printf("Arrival time of job 1: %d\n", arrival[0]);
	printf("Execution time of job 1: %d\n", execution[0]);
	printf("Arrival time of job 2: %d\n", arrival[1]);
	printf("Execution time of job 2: %d\n", execution[1]);
	printf("Arrival time of job 3: %d\n", arrival[2]);
	printf("Execution time of job 3: %d\n", execution[2]);
	free(buf);
	
	return 0;
}

void readInfoFile (const char * file_name) {
	int fd, r_ret, fileSize, i = 0;
	
	if ((fd = open (file_name, O_RDONLY)) < 0) {
		fprintf(stderr, "Error: cannot open file %s: %s\n", file_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct stat st;
	if (stat(file_name, &st) == 0)
		fileSize = st.st_size;

	buf = malloc (fileSize);

	if ((r_ret = read (fd, buf, fileSize)) != fileSize) {
		fprintf(stderr, "Error: cannot read file %s: %s\n", file_name, strerror(errno));
		exit(EXIT_FAILURE);	
	}
	close (fd);
}
