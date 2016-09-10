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
	int n1[MAX_NUM_LENGTH], n2[MAX_NUM_LENGTH], temp[MAX_NUM_LENGTH], product[MAX_NUM_LENGTH];
	int i, j, t, l1, l2, carry;

	for (i = 0; num1[i] != '\0'; i++) {
		n1[i] = num1[i] - '0';
	}	
	l1 = i;

	for (i = 0; num2[i] != '\0'; i++) {
		n2[i] = num2[i] - '0';
	}
	l2 = i;

	int max = (l1 > l2) ? l1 : l2;
	int min = (l1 > l2) ? l2 : l1;
	int offset = max - min;

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
				temp[i];
			}
		}
		memcpy(n1, temp, sizeof(temp));
	}

	for (i = l2-1; i >= 0; i--) {
		carry = 0;
		for (j = l1-1; j >= 0; j--) {
			t = (n1[j] * n2[i]) + product[i+j] + carry;
			carry = t/10;
			product[i+j] = t % 10;
		}
	}

	for (i = 0; i < 2*max; i++) {
		printf("%d", product[i]);
	} printf("\n");
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

