/**
 * Assignment 3
 * Bryan Ehlers
 * UIowa Operating Systems
 * fcfs.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1024
#define DELIMITERS " \n"
#define DEBUG 0

struct proc {
	int arrival_time;
	int execution_time;
	int job_number;
} ;

char * buf; // general purpose reading buffer, make sure to clear before use
char * tokens[BUFSIZE];	// for parsing the job file

void readInfoFile (const char *);
char ** parse (char *);

int main (int argc, char ** argv) {
	char * fileName, ** job_info;
	double average_response_time = 0, average_turnaround_time = 0;
	int length, num_jobs, j = 0, k = 0;
	int * execution_time, * arrival_time, * wait_time, * turnaround_time;

	if (argc != 2) {
		fprintf(stderr, "usage: ./prog <job file>\n");
		exit(1);
	} fileName = argv[1];

	readInfoFile (fileName);
	job_info = parse (buf);
	
	// gets length of job_info
	for (int i = 0; job_info[i] != NULL; i++) length = i;
	length++;
	num_jobs = (length-1)/2;
	
	#ifdef DEBUG
	printf("length = %d\n", length);
	printf("num_jobs = %d\n", num_jobs);
	#endif

	execution_time = malloc (num_jobs*sizeof(int));
	arrival_time = malloc (num_jobs*sizeof(int));
	wait_time = malloc (num_jobs*sizeof(int));
	turnaround_time = malloc (num_jobs*sizeof(int));

	for (int i = 1; i < length; i++) {
		#ifdef DEBUG
		printf("job_info[%d]: %s\n", i, job_info[i]);
		#endif
		if (i % 2 != 0) arrival_time[j++] = atoi(job_info[i]);
		else execution_time[k++] = atoi(job_info[i]);
	}
	#ifdef DEBUG
	for (int i = 0; i < num_jobs; i++) {
		printf("arrival_time[%d]: %d\n", i, arrival_time[i]);
		printf("execution_time[%d]: %d\n", i, execution_time[i]);
	}
	#endif

	for (int i = 0; i < num_jobs; i++) {
		wait_time[i] = 0;
		for (int j = arrival_time[i]; j < i; j++) 
			wait_time[i] += execution_time[j];
	}
        #ifdef DEBUG
        for (int i = 0; i < num_jobs; i++) printf("wait_time[%d]: %d\n", i, wait_time[i]);
        #endif

	for (int i = 0; i < num_jobs; i++) {
		turnaround_time[i] = execution_time[i] + wait_time[i];
		average_response_time += wait_time[i];
		average_turnaround_time += turnaround_time[i];
	}
	average_turnaround_time /= num_jobs;
	average_response_time /= num_jobs;
	printf("%.5f\n%.5f\n", average_turnaround_time, average_response_time);

	free(execution_time);
	free(arrival_time);
	free(wait_time);
	free(turnaround_time);
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

char ** parse (char * input) {
	int i = 0;
	char * token;	

	token = strtok(input, DELIMITERS);
	if (token == NULL) return NULL;
	while (token != NULL) {
		tokens[i++] = token;
		token = strtok(NULL, DELIMITERS);
	}
	tokens[i] = NULL;
	return tokens;
}
