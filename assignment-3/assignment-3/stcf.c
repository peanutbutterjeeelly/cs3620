/**
 * Assignment 3
 * Bryan Ehlers
 * UIowa Operating Systems
 * stcf.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
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
	int start_time;
	int finish_time;
	int time_remaining;
	int execution_time;
	int response_time;
	int turnaround_time;
	bool completed;
} ;

char * buf; // general purpose reading buffer
char * tokens[BUFSIZE];

int main (int argc, char ** argv) {
	char * fileName, ** job_info;
	double average_response_time = 0, average_turnaround_time = 0;
	int length, num_jobs, arrival_index = 0, execution_index = 0, current_time = 0, done = 0, first_run = 1;
	struct proc * procs, temp, next;

	if (argc != 2) {
		fprintf(stderr, "error: Missing arguments---USAGE ./stcf job_configuration_file\n");
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
	for (int i = 0; i < num_jobs; i++) {
		procs[i].time_remaining = procs[i].execution_time;
		procs[i].job_number = i;
		procs[i].completed = false;
		procs[i].start_time = 0;
	}
	// sort on execution time for jobs arriving at the same time, run shorter job first
	for (int i = 1; i < num_jobs; i++) {		
		if (procs[i].arrival_time == procs[i-1].arrival_time && procs[i].execution_time < procs[i-1].execution_time) {
			temp = procs[i];
			procs[i] = procs[i-1];
			procs[i-1] = temp;
		}
	}

	#ifdef DEBUG
	for (int i = 0; i < num_jobs; i++) printf("procs[%d].arrival_time = %d, procs[%d].execution_time = %d, procs[%d].time_remaining = %d\n", i, procs[i].arrival_time, i, procs[i].execution_time, i, procs[i].time_remaining);
	#endif

	int total_execution_time = 0;
	for (int i = 0; i < num_jobs; i++)
		total_execution_time += procs[i].execution_time;

	int min_index = num_jobs-1, remain = 0;
	for (int time = 0; time < total_execution_time /*remain != num_jobs*/; time++) {
		min_index = num_jobs-1;
		for (int i = 0; i < num_jobs; i++) {
			if (procs[i].arrival_time <= time && procs[i].time_remaining > 0)
				min_index = i;
		}
		printf("%d\n", min_index);
		procs[min_index].time_remaining--;
		printf("time: %d\n", procs[min_index].time_remaining);

		if (procs[min_index].start_time == 0) 
			procs[min_index].start_time = time;

		if (procs[min_index].time_remaining == 0) {
			printf("here");
			remain++;
			procs[min_index].finish_time = time;
			int endTime = time+1;
			//average_response_time += endTime-procs[min_index].execution_time-procs[min_index].arrival_time;
			average_turnaround_time += endTime-procs[min_index].arrival_time;
		}
	}

	for (int i = 0; i < num_jobs; i++) 
		average_response_time += procs[i].start_time-procs[i].arrival_time;

	#ifdef DEBUG
	for (int i = 0; i < num_jobs; i++) printf("procs[%d].start_time = %d, procs[%d].arrival_time = %d, procs[%d].finish_time = %d\n", i, procs[i].start_time, i, procs[i].arrival_time, i, procs[i].finish_time);
	#endif

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
