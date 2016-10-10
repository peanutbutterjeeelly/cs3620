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
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1024

/*
	read the job info
	
*/

const char * fcfsInfo = "job.info.fcfs" ;
const char * sjfInfo  = "job.info.sjf"  ;
const char * stcfInfo = "job.info.stcf" ;

char * readInfoFile (char *);

int main (int argc, char ** argv) {
        int fd, r_ret, fileSize, numberOfJobs;
	char buffer[BUFSIZE];

        fd = open (fcfsInfo, O_RDONLY);
        if (fd < 0) {
                perror("open error");
                exit(1);
        }

        struct stat st;
        if (stat(fcfsInfo, &st) == 0)
                fileSize = st.st_size;

        r_ret = read (fd, buffer, fileSize);
        if (r_ret != fileSize) {
                perror("read error");
                exit(1);
        }	

	printf("File size: %d\n", fileSize);

	for (int j = 0; buffer[j] != '\0'; j++) {
		printf("%c", buffer[j]);
	}

	numberOfJobs = buffer[0]-'0';
	printf("Number of jobs: %d\n", numberOfJobs);
	int * jobInformation = malloc (fileSize);
		

	int i = 0;
	for (int j = 0; buffer[j] != '\0'; j++) {
		if (buffer[j] == '\n') continue;
		jobInformation[i] = buffer[j]-'0';
		i++;
	}

	for (int j = 0; j < fileSize/sizeof(int); j++) {
		printf("%d", jobInformation[j]);
	}

	free(jobInformation);

	return 0;
}
