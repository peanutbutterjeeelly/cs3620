#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include "sort.h"

#define KEYSIZE 4
#define VALUESIZE 96
#define SIZE 100
#define NUMTHREADS 3

void *sorter (void *params);
void *merger (void *params);

rec_t * recs[100] = malloc(100*sizeof(recs));

typedef struct {
	int start;
	int end;
} params;

off_t fsize (const char * filename) {
        struct stat st;

        if (stat(filename, &st) == 0) {
                return st.st_size;
        }

        fprintf(stderr, "Cannot determine size of %s: %s\n", filename, strerror(errno));

        return -1;
}

int main (int argc, char *argv[]) {
        rec_t buf;
        int fd, ret;

        // get some helpful information
        int fileSize = fsize("output.bin");
        int numkeys = fileSize/(KEYSIZE+VALUESIZE);

        // allocate memory for array to hold records in
        //rec_t **recs = malloc(numkeys * sizeof(rec_t));
        if (recs == NULL) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
        }
        for (int i = 0; i < numkeys; i++) {
                recs[i] = malloc(sizeof(rec_t));
                if (recs[i] == NULL) {
                        perror("malloc failed");
                        exit(EXIT_FAILURE);
                }
        }

        // open up the file, check errors
        if ((fd = open("output.bin", O_RDONLY)) < 0) {
                perror("open inFile");
                exit(EXIT_FAILURE);
        }

        // read the binary records
        printf("Reading...\n");
        for (int i = 0; i < numkeys; i++) {
                ret = read(fd, &buf, sizeof(buf));
                if (ret == 0) {
                        perror("read inFile");
                        exit(EXIT_FAILURE);
                }
                recs[i]->key = buf.key;
                printf("Key: %u\nValue: ", recs[i]->key);
                for (int j = 0; j < NUMRECS; j++) {
                        recs[i]->record[j] = buf.record[j];
                        printf("%d ", recs[i]->record[j]);
                } printf("\n");
        }

	int i;
	pthread_t workers[NUMTHREADS];

	params *data = (params *)malloc(sizeof(params));
	data->start = 0;
	data->end = (SIZE/2) - 1;
	pthread_create(&workers[0], 0, sorter, data);

	data = (params *)malloc(sizeof(params));
	data->start = 0;
	data->end = SIZE/2;
	pthread_create(&workers[2], 0, merger, data);
	pthread_join(workers[2], NULL);

	return 0;
}

void *sorter (void *input) {
	params *p = (params *)input;
	
	int begin = p->start;
	int end = p->end + 1;
	
	printf("Array received: ");
	for (int i = 0; i < end; i++) {
		printf("%d \n", recs[i]->key);
	} printf("\n");
	
	rec_t * temp = malloc(sizeof(*rec_t));
	for (int i = begin; i < end; i++) {
		for (int j = begin; j < (end - 1); j++) {
			if (recs[j]->key > recs[j+1]->key) {
				temp->key = recs[j]->key;
				recs[j]->key = recs[j+1]->key;
				recs[j+1]->key = temp->key;
				for (int k = 0; k < end; k++) {
					temp->record[k] = recs[j]->record[k];
					recs[j]->record[k] = recs[j+1]->record[k];
					recs[j+1]->record[k] = temp->record[k];
				}
			}
		}
	}

	printf("Sorted array: ");
	for (int i = begin; i < end; i++) {
		printf("%d ", recs[i]->key);
	}
	pthread_exit(NULL);
}

void *merger (void *input) {
	params* p = (params *)input;
	
	int begin = p->start;
	int end = p->end + 1;

	rec_t * temp;
	for (int i = begin; i < end; i++) {
		for (int j = begin; j < (end-1); j++) {
			if (recs[j]->key > recs[j+1]->key) {
				temp->key = recs[j]->key;
				recs[j]->key = recs[j+1]->key;
				recs[j+1]->key = temp->key;
				for (int k = 0; k < end; k++) {
					temp->record[k] = recs[j]->record[k];
					recs[j]->record[k] = recs[j+1]->record[k];
					recs[j+1]->record[k] = temp->record[k];
				}
			}
		}
	}

	printf("Final result: ");
	for (int i = 0; i < end; i++) {
		printf("%d ", recs[i]->key);
	} printf("\n");

	pthread_exit(NULL);

}
