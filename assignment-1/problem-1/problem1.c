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
#include "sort.h"

/**
 * will return positive if key A is greater than key B,
 * and will return negative if key A is less than key B
 */
int compare (const void * a, const void * b) {
	rec_t *recA = (rec_t *)a;
	rec_t *recB = (rec_t *)b;

	return (recA->key - recB->key);
}

void usage (char * prog);
off_t fsize (const char * fileName);

int main (int argc, char * argv[]) {
	// assume 4 byte key + 96 byte value
	assert(sizeof(rec_t) == 100);

	// input parameters
	char * inFile, * outFile;
	
	// file I/O variables
	int fileSize, numkeys;
	int fd, r_ret, w_ret, c;

	if (argc != 2) {
		usage(argv[0]);	
	}
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

	// get some helpful information
	fileSize = fsize(inFile);
	numkeys = fileSize/sizeof(rec_t);	

	// allocate memory for array to hold records in
	rec_t * recs = malloc(numkeys * sizeof(rec_t));
	if (recs == NULL) {
		fprintf(stderr, "Error: malloc(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	// open up the file, check errors
	fd = open(inFile, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error: Cannot open file %s: %s\n", inFile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	// read the binary records
	r_ret = read (fd, recs, numkeys*sizeof(rec_t));
	if (r_ret != (numkeys*sizeof(rec_t))) {
		fprintf(stderr, "Error: Cannot read file %s: %s\n", inFile, strerror(errno));
		exit(EXIT_FAILURE);
	}	
	close(fd);

	// do the sorting...
	qsort (recs, numkeys, sizeof(rec_t), compare);	

	// write the sorted data to output file
	fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	if (fd < 0) {
		fprintf(stderr, "Error: Cannot open file %s: %s\n", outFile, strerror(errno));
		exit(EXIT_FAILURE);
	}
	w_ret = write (fd, recs, numkeys*sizeof(rec_t));
	if (w_ret != (numkeys*sizeof(rec_t))) {
		fprintf(stderr, "Error: Cannot write to file %s: %s\n", outFile, strerror(errno));
		exit(EXIT_FAILURE);
	}
	close(fd);

	// free the memory
	free(recs);	
	
	return 0;
}

void usage (char * prog) {
	fprintf(stderr, "usage: %s -i input file -o output file\n", prog);
	exit(1);
}

off_t fsize (const char * filename) {
	struct stat st;

	if (stat(filename, &st) == 0) {
		return st.st_size;
	}
	
	fprintf(stderr, "Cannot determine size of %s: %s\n", filename, strerror(errno));

	return -1;
}
