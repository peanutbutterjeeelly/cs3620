/**
 * Name: problem3.c
 * Author: Bryan Ehlers
 * Operating Systems - UIowa
 * Assignment 1, Problem 3
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NUM_LENGTH 10000

void multiply_and_print(char* num1, char* num2) {
	int l1 = strlen(num1);
	int l2 = strlen(num2);

	int * product = malloc ((l1+l2)*sizeof(int));

	// zero out array just to be safe
	memset(product, 0, sizeof(int)*(l1+l2));

	// naive multiplication algorithm
	for (int i = l1-1; i >= 0; i--) {
		for (int j = l2-1; j >= 0; j--) {
			product[i+j+1] += ((num2[j] - '0')*(num1[i] - '0'));
		}
	}
	for (int i = (l1+l2-1); i >= 0; i--) {
		if (product[i] >= 10) {
			product[i-1] += (product[i]/10);
			product[i] %= 10;	
		}
	}

	for (int i = 0; i < (l1+l2); i++) {
		printf("%d", product[i]);
	} printf("\n");
	free(product);
}

int main (int argc , char * argv []) {
	FILE *file = fopen("problem3.input","r");
	if (file == NULL) {
		printf("Input file not found.\n");
		return 1;
	}
	char num1[MAX_NUM_LENGTH];
	char num2[MAX_NUM_LENGTH];
	while(fscanf(file, "%s %s", num1, num2) == 2) {
		multiply_and_print(num1, num2);
	}
	fclose(file);	
	return 0 ;
}

