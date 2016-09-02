#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>


#define HI_NIBBLE(x) (((x) >> 4 ) & 0x0F)
#define LO_NIBBLE(x) ((x) & 0x0F)

#define RECSIZE 100
#define KEYSIZE 4
#define VALSIZE 96

int main (int argc, char * argv[]) {
	unsigned char key[(2*KEYSIZE)];
	unsigned char keyFinal[KEYSIZE];

	int Yeah[KEYSIZE];

	// read from binary file
	FILE * ptr = fopen("output.bin", "rb");	// r --> read, b --> binary
	if (ptr == NULL) {
		perror("File open");
		exit(1);
	}
	
	// get size of file in bytes, and number of records in file
	fseek(ptr, 0L, SEEK_END);
	int fileSize = ftell(ptr);
	int numRecs = fileSize/RECSIZE;

	// will read into this
	unsigned char buffer[fileSize];
	int buf[fileSize];	
	rewind(ptr);

	// read the file
	while (!feof(ptr)) {
		fread(buf, sizeof(buf), 1, ptr);
	}
	fclose(ptr);	

	printf("Key: ");
	for (int i = 0; i < KEYSIZE; i++) {
		Yeah[i] = buf[i];
		printf("%d ", Yeah[i]);
	} printf("\n");
	
	int result = strtol(key, NULL, 16);
	printf("%d", result);
/*
	int j = 0;
	printf("Key: ");
	for (int i = 2; i < KEYSIZE; i++) {
		key[j] = HI_NIBBLE(buffer[i]);
		key[j+1] = LO_NIBBLE(buffer[i]);
		printf("%x %x ", key[j], key[j+1]);
		j += 2;
	} key[2*KEYSIZE] = 0; printf("\n");
*/	
/*
the problem is that each key[] index actually stores one byte so "a3" for example. To use strol()
need to convert so that each key[] is "a", "3", ... and so on
*/
	// "echo" whatever was read
	unsigned char buffer2[fileSize];
	FILE * sortedFile = fopen("sorted.bin", "wb");
	if (sortedFile == NULL) {
		perror("File open");
		exit(1);
	}
	fwrite(&buffer, sizeof(buffer), 1, sortedFile);
	fclose(sortedFile);

	// read the "echo"	
	sortedFile = fopen("sorted.bin", "rb");
	if (sortedFile == NULL) {
		perror("File open");
		exit(1);
	}
	fread(buffer2, sizeof(buffer2), 1, sortedFile);
	for (int i = 0; i < sizeof(buffer2); i++) {
		printf("%x ", buffer2[i]);
	}
	printf("\n");
	fclose(sortedFile);

	return 0;
}
