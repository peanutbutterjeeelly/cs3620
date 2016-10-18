/**
 * Assignment 3
 * Bryan Ehlers
 * UIowa Operating Systems
 * sjf.c
 *
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
#define DEBUG

void readInfoFile (const char *);
char ** parse (char *);

struct proc {
	int job_number;
	int arrival_time;
	int execution_time;
	int wait_time;
	int turnaround_time;
} ;

char * buf; // general purpose reading buffer
char * tokens[BUFSIZE];

int main (int argc, char ** argv) {
	char * fileName, ** job_info;
	double average_response_time = 0, average_turnaround_time = 0;
	int length, num_jobs, arrival_index = 0, execution_index = 0, current_time = 0;
	struct proc * procs, temp, next;

	if (argc != 2) {
		fprintf(stderr, "error: Missing arguments---USAGE ./sjf job_configuration_file\n");
		exit(EXIT_FAILURE);
	} fileName = argv[1];

	readInfoFile (fileName);
	job_info = parse (buf);
	
	#ifdef DEBUG
	for (int i = 0; job_info[i] != NULL; i++) printf("job_info[%d]: %s\n", i, job_info[i]);
	#endif

	for (int i = 0; job_info[i] != NULL; i++) length = i;
	length++;
	num_jobs = (length-1)/2;

	procs = malloc (num_jobs*sizeof(struct proc));
	for (int i = 1; i < length; i++) {
		if (i % 2 != 0) procs[arrival_index++].arrival_time = atoi(job_info[i]);
		else procs[execution_index++].execution_time = atoi(job_info[i]);
	}
/*
	#ifdef DEBUG
	for (int i = 0; i < num_jobs; i++) {
		printf("procs[%d].arrival_time: %d, procs[%d].execution_time: %d\n", i, procs[i].arrival_time, i, procs[i].execution_time); 
	}
	#endif
*/
	// sort on execution time for jobs arriving at the same time, run shorter job first
	for (int i = 1; i < num_jobs; i++) {		
		if (procs[i].arrival_time == procs[i-1].arrival_time && procs[i].execution_time < procs[i-1].execution_time) {
			temp = procs[i];
			procs[i] = procs[i-1];
			procs[i-1] = temp;
		}
	}
	
	#ifdef DEBUG
	for (int i = 0; i < num_jobs; i++) printf("procs[%d].arrival_time = %d, procs[%d].execution_time = %d\n", i, procs[i].arrival_time, i, procs[i].execution_time);
	#endif

	// sort the procs shortest job to longest job
	// run them like you would run fcfs


	// cycle through all jobs
	// is the arrival time after that of the current_time? For those jobs that this 
	// is true, find the minimum execution time job.

	// first job to run is special case
	procs[0].wait_time = 0;
	current_time = procs[0].execution_time;
	procs[0].wait_time = 0;
	procs[0].turnaround_time = procs[0].execution_time;
	average_response_time += procs[0].wait_time;
	average_turnaround_time += procs[0].turnaround_time;	
	for (int i = 1; i < num_jobs; i++) {
		for (int j = i; j < num_jobs; j++) {
			if (procs[j].arrival_time <= current_time)
				next = procs[j];
				if (next.execution_time < procs[i].execution_time) {
					next = procs[j];
					procs[j] = procs[i];
					procs[i] = next;				
				}
		}
		
		#ifdef DEBUG
		printf("procs[%d].arrival_time: %d, procs[%d].execution_time: %d\n", i, procs[i].arrival_time, i, procs[i].execution_time);
		#endif

		procs[i].wait_time = current_time - procs[i].arrival_time;
		current_time += procs[i].execution_time;
		#ifdef DEBUG
		printf("current_time = %d\n", current_time);
		#endif
		procs[i].turnaround_time = current_time - procs[i].arrival_time;
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
