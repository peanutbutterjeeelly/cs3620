/**
 * Bryan Ehlers
 * UIowa Operating Systems
 * Assignment 5
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SQUARE(x)((x)*(x))
#define DELIMITERS " \n\t"
//#define DEBUG

typedef struct {
	int index;
	int size;
	int threadNum;
} arg;

char buf[32768];
char * tokens[32768];
char * dataFileName, * queryFileName, * outputFileName;
int numThreads;
double ** data;
double ** query;
pthread_t * tid;
int stateSpaceSize;
int numEntries_data;
int * nearest;
double average_time = 0;

void * runner (void *);
char ** parse (char *);
void error (char *);

int main (int argc, char ** argv) {
	for (int iteration = 0; iteration < 3; iteration++) {
	clock_t begin = clock();

	FILE * dataFile, * queryFile, * outputFile;	
	int numEntries_query;
	char ** dataString;

	if (argc != 5)
		error("argc");

	dataFileName = argv[1];
	queryFileName = argv[2];
	outputFileName = argv[3];
	numThreads = atoi(argv[4]);

	// allocate array to keep threads
	tid = malloc (numThreads*sizeof(pthread_t));
	if (tid == NULL)
		error("malloc tid");

	// open dataset file
	dataFile = fopen (dataFileName, "r");
	if (dataFile == NULL)
		error("fopen()");

	// get state space size and number of entries
	if (fgets (buf, sizeof(buf), dataFile) == NULL)
		error("fgets()");
	dataString = parse (buf);
	numEntries_data = atoi(dataString[0]);
	stateSpaceSize = atoi(dataString[1]);

	// alocate dataset array
	data = malloc(numEntries_data*sizeof(double *));
	if (data == NULL)
		error("malloc data");
	for (int i = 0; i < numEntries_data; i++) {
		data[i] = malloc(stateSpaceSize*sizeof(double));
		if (data[i] == NULL) error("malloc data[]");	
	}

	// read dataset file
	for (int j = 0; j < numEntries_data; j++) {
		if (fgets(buf, sizeof(buf), dataFile) == NULL)
			break;
		dataString = parse(buf);
		for (int i = 0; i < stateSpaceSize; i++)
			data[j][i] = atof(dataString[i]);
	} fclose(dataFile);

	// open query file
	queryFile = fopen (queryFileName, "r");
	if (queryFile == NULL)
		error("fopen queryFile");
	
	// get state space size and number of entries
	if (fgets (buf, sizeof(buf), queryFile) == NULL)
		error("fgets queryFile");
	dataString = parse(buf);
	assert (atoi(dataString[1])==stateSpaceSize);	// dataset and query set should have same state space dimension
	numEntries_query = atoi(dataString[0]);

	// allocate query array
	query = malloc (numEntries_query*sizeof(double *));
	if (query == NULL)
		error("malloc query");
	for (int i = 0; i < numEntries_query; i++) {
		query[i] = malloc (stateSpaceSize*sizeof(double));
		if (query[i] == NULL) error("malloc query[]");
	}
	// read query file
	for (int j = 0; j < numEntries_query; j++) {
		if (fgets(buf, sizeof(buf), queryFile) == NULL)
			break;
		dataString = parse(buf);
		for (int i = 0; i < stateSpaceSize; i++)
			query[j][i] = atof(dataString[i]);
	} fclose(queryFile);

	nearest = malloc(numEntries_query*sizeof(int));

	// launch the threads
	for (int i = 0; i < numThreads; i++) {
		arg * args = malloc(sizeof( *args));
		args->size = numEntries_query/numThreads;
		args->index = args->size * i;
		args->threadNum = i;
		if (i == numThreads-1 && numEntries_query % numThreads != 0)
			(args->size)++;
		#ifdef DEBUG		
		printf("numEntries: %d, numThreads: %d, i: %d\n", numEntries_data, numThreads, i);
		printf("Thread %d ==> start index %d, size %d\n", i, args->index, args->size);
		#endif
		pthread_create(&tid[i], NULL, runner, args);
	}

	for (int i = 0; i < numThreads; i++)
		pthread_join(tid[i], NULL);
	
	#ifdef DEBUG	
	for (int i = 0; i < numEntries_query; i++)
		printf("%d\n", nearest[i]);
	#endif

	// write to output file
	outputFile = fopen (outputFileName, "w");
	if (outputFile == NULL)
		error("fopen outputFile");

	for (int i = 0; i < numEntries_query; i++)
		fprintf(outputFile, "%d\n", nearest[i]);

	fclose(outputFile);

	// clean up
	free(data); free(query); free(tid);
	clock_t end = clock();
	average_time += ((double)(end-begin)/CLOCKS_PER_SEC);
	}
	printf("Average time spent: %f\n", average_time/3);
	return 0;
}

// thread worker function
void * runner (void * param) {
	arg * args = param;
	double nearestNeighbor;
	int nearestIndex = 0;
	double result;
	int index = 0;
	double computation[100];

	for (int queryRow = args->index; queryRow < args->index+args->size; queryRow++) {
		nearestNeighbor = LONG_MAX;
		for (int dataRow = 0; dataRow < numEntries_data; dataRow++) {		
			result = 0;
			for (int col = 0; col < stateSpaceSize; col++)
				result += SQUARE(query[queryRow][col]-data[dataRow][col]);
			result = sqrt(result);
			if (result < nearestNeighbor) {
				nearestNeighbor = result;
				nearestIndex = dataRow;
			}
		}
		nearest[queryRow] = nearestIndex+1;  // zero based offset to one based offset
	}
	free(args);
}

// uses tokenizer to parse input string
char ** parse (char * input) {
	int i = 0;
	char * token;

	token = strtok(input, DELIMITERS);
	if (token == NULL)
		return NULL;
	while (token != NULL) {
		tokens[i++] = token;
		token = strtok(NULL, DELIMITERS);
	}
	tokens[i] = NULL;
	return tokens;
}

// error function, exits
void error (char * err) {
	fprintf(stderr, "Error %s: exiting", err);
	exit(1);
}
