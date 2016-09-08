#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

void
usage(void){
    fprintf(stderr, "Usage: rangesort -i inputfile -o outputfile -l lowvalue -h highvalue\n");
    exit(1);
}

void
fileusage(char *ifp){
    fprintf(stderr, "Error: Cannot open file %s\n",ifp);
    exit(1);
}

void
rangeusage(void){
    fprintf(stderr, "Error: Invalid range value\n");
    exit(1);
}

int compareobj(const void * a, const void * b){

    if(((rec_t *)a)->key > ((rec_t *)b)->key)
        return 1;
    else if(((rec_t *)a)->key == ((rec_t *)b)->key)
        return 0;
    else
        return -1;
}

int main(int argc, char **argv)
{
	clock_t start = clock();

    rec_t *records, *recordsort = NULL, *ptr = NULL;
    int totalRecordnum, totalRecordsortnum = 0;
    long int lowvalue = 0, highvalue = 0;
    
    char *ifp = "/no/such/input_file";
    char *ofp = "/no/such/output_file";
    int c, count = 0;
    char *ptrtoul;

    opterr = 0;
    while((c = getopt(argc, argv, "i:o:l:h:")) != -1){
        switch(c){
                case 'i':
                        ifp = strdup(optarg);
                        count++;
                        break;
                case 'o':
                        ofp = strdup(optarg);
                        count++;
                        break;
                case 'l':
                        lowvalue = strtol(optarg, &ptrtoul, 10);
			if (optarg == ptrtoul || errno == ERANGE)
				rangeusage();
                        count++;
                        break;
                case 'h':
                        highvalue = strtol(optarg, &ptrtoul, 10);
			if (optarg == ptrtoul || errno == ERANGE)
				rangeusage();
                        count++;
                        break;
                default:
                        usage();
                        break;
                }
        }

    if ((lowvalue > highvalue) ||
        (lowvalue > UINT_MAX) || (highvalue > UINT_MAX) ||
        (lowvalue < 0) || (highvalue < 0))
    {
        rangeusage();
    }

    if(count != 4)
    {
        usage();
    }


    //read the input file
    int fdi = open(ifp, O_RDONLY);
    if (fdi < 0){
        fileusage(ifp);
    }


    struct stat fileStats;

    if (fstat(fdi, &fileStats) < 0)
        return 1;

    totalRecordnum = fileStats.st_size;
    records = malloc(totalRecordnum);

    int rc = read(fdi, records, totalRecordnum);
    if (rc < 0){
        perror("rc less than zero");
        exit(1);
    }

    ptr = records;
    recordsort = malloc(totalRecordnum);
    int m = totalRecordnum/sizeof(rec_t);
    int i;
    //considering the values that are within the range
    for( i = 0; i < m; i++){
        if(ptr->key >= lowvalue && ptr->key <= highvalue){
                memcpy(((char *)recordsort) + totalRecordsortnum*sizeof(rec_t),  ((char *)ptr) , sizeof(rec_t));
		totalRecordsortnum++;
        }
	ptr = (rec_t *)(((char *)ptr) + sizeof(rec_t));

    }

    qsort((void *)recordsort, (size_t)totalRecordsortnum, sizeof(rec_t), compareobj);

    //open and create the output file
    int fdo = open(ofp, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fdo < 0){
        fileusage(ofp);
        exit(1);
    }

    write(fdo, recordsort, sizeof (rec_t)*totalRecordsortnum);
    (void) close(fdi);

    free(records);
    free(recordsort);

	clock_t end = clock();
	printf("Time: %f\n", (double)(end-start)/CLOCKS_PER_SEC);

    return 0;
}
