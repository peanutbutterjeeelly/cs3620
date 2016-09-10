/**
 * Name: problem2.c
 * Author: Bryan Ehlers
 * Operating Systems - UIowa
 * Assignment 1, Problem 2
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NUM_LENGTH 10000

void add_and_print(char* num1, char* num2) {
	int n1[MAX_NUM_LENGTH], n2[MAX_NUM_LENGTH], temp[MAX_NUM_LENGTH], sum[MAX_NUM_LENGTH];	
	int i, l1, l2, carry;

	// convert these character arrays to integers
	for (i = 0; num1[i] != '\0'; i++) {
		n1[i] = num1[i] - '0';
	}

	// and get length of each number
	l1 = i;

	// diddo
	for (i = 0; num2[i] != '\0'; i++) {
		n2[i] = num2[i] - '0';
	}
	l2 = i;

	// get some useful information
	int max = (l1 > l2) ? l1 : l2;		
	int min = (l1 > l2) ? l2 : l1;
	int offset = max - min;

	// align the numbers if they are different lengths
	if (l1 > l2) {
		for (i = 0; i < min; i++) {
			temp[i+offset] = n2[i];
			if (i < offset) {
				temp[i] = 0;
			}
		}
		memcpy(n2, temp, sizeof(temp));
	} else if (l2 > l1) {
		for (i = 0; i < min; i++) {
			temp[i+offset] = n1[i];
			if (i < offset) {
				temp[i] = 0;
			}
		}
		memcpy(n1, temp, sizeof(temp));
	}

	// addition algorithm
	carry = 0;
	for (i = max-1; i >= 0; i--) {
		sum[i] = (n1[i] + n2[i] + carry) % 10;
		if ((n1[i] + n2[i] + carry) >= 10) {
			carry = 1;
		} else {
			carry = 0;
		}
	}
	sum[i] = carry;

	// print result
	for (i = 0; i < max; i++) {
		printf("%d", sum[i]);
	} printf("\n");
}

int main (int argc , char * argv []) {
	
	FILE *file = fopen("problem2.input","r");
	if (file == NULL) {
		printf("Input file not found.\n");
		return 1;
	}
	char num1[MAX_NUM_LENGTH];
	char num2[MAX_NUM_LENGTH];
	while(fscanf(file, "%s %s", num1, num2) == 2) {
		add_and_print(num1, num2);
	}
	fclose(file);	
	return 0 ;
}
