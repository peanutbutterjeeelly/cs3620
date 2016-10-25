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
//#define DEBUG

struct proc {
	int job_number;
	int arrival_time;
	int execution_time;
	int wait_time;
	int turnaround_time;
} ;

char * buf; // general purpose reading buffer, make sure to clear before use
char * tokens[BUFSIZE];	// for parsing the job file
int arr[BUFSIZE];

void readInfoFile (const char *);
char ** parse (char *);

int main (int argc, char ** argv) {
	char * fileName, ** job_info;
	double average_response_time = 0, average_turnaround_time = 0;
	int length, num_jobs, arrival_index = 0, execution_index = 0, current_time = 0;
	struct proc * procs;

	if (argc != 2) {
		fprintf(stderr, "error: Missing arguments---USAGE ./fcfs job_configuration_file\n");
		exit(EXIT_FAILURE);
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

	procs = malloc (num_jobs*sizeof(struct proc));

	for (int i = 1; i < length; i++) {
		#ifdef DEBUG
		printf("job_info[%d]: %s\n", i, job_info[i]);
		#endif
		if (i % 2 != 0) procs[arrival_index++].arrival_time = atoi(job_info[i]);
		else procs[execution_index++].execution_time = atoi(job_info[i]);
	}

	#ifdef DEBUG
	for (int i = 0; i < num_jobs; i++) {
		printf("arrival_time[%d]: %d\n", i, procs[i].arrival_time);
		printf("execution_time[%d]: %d\n", i, procs[i].execution_time);
	}
	#endif

	procs[0].wait_time = 0; // in fcfs, the first job doesn't wait
	current_time += procs[0].execution_time;	
	for (int i = 1; i < num_jobs; i++) {
		procs[i].wait_time = current_time - procs[i].arrival_time;	
		current_time += procs[i].execution_time;	
	}

        #ifdef DEBUG
        for (int i = 0; i < num_jobs; i++) printf("wait_time[%d]: %d\n", i, procs[i].wait_time);
        #endif

	for (int i = 0; i < num_jobs; i++) {
		procs[i].turnaround_time = procs[i].execution_time + procs[i].wait_time;
		average_response_time += procs[i].wait_time;
		average_turnaround_time += procs[i].turnaround_time;
	}
	average_turnaround_time /= num_jobs;
	average_response_time /= num_jobs;
	printf("%.5f\n%.5f\n", average_turnaround_time, average_response_time);

	free(procs);
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
