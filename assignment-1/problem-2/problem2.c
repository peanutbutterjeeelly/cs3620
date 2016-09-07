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
	int n1[MAX_NUM_LENGTH], n2[MAX_NUM_LENGTH], sum[MAX_NUM_LENGTH];	
	int i, l1, l2, carry;

	// convert these character arrays to integers
	for (i = 0; num1[i] != '\0'; i++) {
		n1[i] = num1[i] - '0';
	}

	// and get length of each number
	l1 = strlen(num2);

	// diddo
	for (i = 0; num2[i] != '\0'; i++) {
		n2[i] = num2[i] - '0';
	}
	l2 = strlen(num2);

	// get some useful information
	int max = (l1 > l2) ? l1 : l2;		
	int min = (l1 > l2) ? l2 : l1;
	int offset = max - min;

	// align the two number's digits
	if (l1 == max) {
		for (i = 0; i < min; i++) {
			n2[i+offset] = n2[i];
		}
	} else {
		for (i = 0; i < min; i++) {
			n1[i+offset] = n1[i];
		}
	}

	// addition algorithm
	carry = 0;
	for (i = max-1; i >= 0; i--) {
		sum[i] = (n1[i] + n2[i] + carry) % 10;
		printf("sum[%d]: %d", i, sum[i]);
		if ((n1[i] + n2[i] + carry) >= 10) {
			carry = 1;
		} else {
			carry = 0;
		}
		printf("Iter %d: sum = %d, carry = %d\n", i, sum[i], carry);
	}
	sum[i] = carry;

	// print result
	printf("Sum: ");
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

