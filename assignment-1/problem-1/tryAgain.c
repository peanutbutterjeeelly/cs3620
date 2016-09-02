#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sort.h"

#define BUFSIZE 1024
#define KEYSIZE 4
#define VALUESIZE 96

int main (int argc, char * argv[]) {
	char * inFile = "output.bin";
	rec_t buf;
	int fd = open (inFile, O_RDONLY);
	if (fd < 0) {
		perror("open");	
		exit(EXIT_FAILURE);
	}	

	int ret = read (fd, &buf, sizeof(buf));
	if (ret == 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	rec_t recs = buf;
	printf("Key: %u\nValue: %u\n", recs.key, recs.record[0]);

	close(fd);

	return 0;
}
