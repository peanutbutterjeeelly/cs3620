/**
 * Name: sort.c
 * Author: Bryan Ehlers
 * Operating Systems - UIowa
 * Assignment 1, Problem 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "sort.h"

#define BUFSIZE 1024
#define KEYSIZE 4
#define VALUESIZE 96

struct parameters {
	rec_t * recs;
	size_t length;
	int depth;
};

int compare (const void * a, const void * b);
void *sort_thread (void *pv);
void merge (rec_t * start, rec_t * mid, rec_t * end);
void sort_mt (rec_t * start, size_t length, int depth);
void sort (rec_t * recs, size_t length);

void usage (char * prog);
off_t fsize (const char * fileName);

int main (int argc, char * argv[]) {
	clock_t begin = clock();

	// assume 4 byte key + 96 byte value
	assert(sizeof(rec_t) == 100);

	// input parameters
	char * inFile, * outFile;
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "i:o:")) != -1) {
		switch (c) {
		case 'i':
			inFile = strdup(optarg);
			break;
		case 'o':
			outFile = strdup(optarg);
			break;
		default:
			usage(argv[0]);
		}
	}

	rec_t buf;
	int fd, ret;

	// get some helpful information
	int fileSize = fsize(inFile);
	int numkeys = fileSize/(KEYSIZE+VALUESIZE);	

	// allocate memory for array to hold records in
	rec_t * recs = malloc(numkeys * sizeof(rec_t));
	if (recs == NULL) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	
	// open up the file, check errors
	if ((fd = open(inFile, O_RDONLY)) < 0) {
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
		recs[i].key = buf.key;
//		printf("Key: %u\nValue: ", recs[i].key);
		for (int j = 0; j < NUMRECS; j++) {
			recs[i].record[j] = buf.record[j];
			//printf("%d ", recs[i].record[j]);
		} //printf("\n");
	}
	close(fd);

	// helpers for sorting
	rec_t temp;
	
	printf("Sorting...\n");
	//qsort (recs, numkeys, sizeof(rec_t), compare);	
	sort (recs, numkeys);

	for (int i = 0; i < numkeys; i++) {
		printf("Key: %u\nValue: ", recs[i].key);
		for (int j = 0; j < NUMRECS; j++) {
			printf("%d ", recs[i].record[j]);
		} printf("\n");
	}

	if ((fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU)) < 0) {
		perror("open outFile");
		exit(EXIT_FAILURE);
	}

	int wrret;
	for (int i = 0; i < numkeys; i++) {
		if ((wrret = write(fd, &recs, sizeof(rec_t))) < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}		


/*	// this part is trash I think will take a long time with more records, redundant
		temp.key = recs[i].key;
		for (int j = 0; j < NUMRECS; j++) {
			temp.record[j] = recs[i].record[j];
		}
		
		wrret = write(fd, &temp, sizeof(rec_t));
		if (wrret != sizeof(rec_t)) {
			perror("write");
			exit(EXIT_FAILURE);
		}*/
	}
	close(fd);

	// free the memory
	free(recs);	
	clock_t end = clock();
	printf("Time: %f\n", (double)(end-begin)/CLOCKS_PER_SEC);

	return 0;
}

// used by qsort()
int compare (const void * a, const void * b) {
        rec_t *recA = (rec_t *)a;
        rec_t *recB = (rec_t *)b;

        return (recA->key - recB->key);
}

void merge (rec_t * start, rec_t * mid, rec_t * end) {
	rec_t * res = malloc((end-start)*sizeof(*res));
	rec_t * lhs = start, *rhs = mid, *dst = res;
	
	while (lhs != mid && rhs != end) {
		*dst++ = (lhs->key <= rhs->key) ? *lhs++ : *rhs++;
	}
	
	while (lhs != mid) {
		*dst++ = *lhs++;
	}

	while (rhs != end) {
		*dst++ = *rhs++;
	}

	memcpy(start, res, (end - start)*sizeof(*res));
	free(res);
}

void sort_mt (rec_t * start, size_t length, int depth) {
	if (length < 2) {
		return;
	}

	if (depth <= 0 || length < 256) {
		qsort (start, length, sizeof(rec_t), compare);
		return;
	}

	struct parameters params = { start, length/2, depth/2 };
	pthread_t thread;

	pthread_create(&thread, NULL, sort_thread, &params);

	sort_mt (start + length/2, length - length/2, depth/2);

	pthread_join(thread, NULL);
	merge(start, start + length/2, start + length);
}

void *sort_thread (void *pv) {
	struct parameters *params = pv;
	sort_mt (params->recs, params->length, params->depth);
	return pv;
}

void sort (rec_t * recs, size_t length) {
	sort_mt (recs, length, 4);
}

void usage (char * prog) {
	fprintf(stderr, "usage: %s <-i input file> <-o output file>\n", prog);
	exit(1);
}

// returns the size of a file
off_t fsize (const char * filename) {
	struct stat st;

	if (stat(filename, &st) == 0) {
		return st.st_size;
	}
	
	fprintf(stderr, "Cannot determine size of %s: %s\n", filename, strerror(errno));

	return -1;
}
