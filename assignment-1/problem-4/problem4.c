/**
 * Name: problem.c
 * Author: Bryan Ehlers
 * Operating Systems - UIowa
 * Assignment 1, Problem 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORD_LENGTH 65

struct Node {
	char  data[MAX_WORD_LENGTH];
	struct Node* left;
	struct Node* right;
};

// to keep track of where the root is
struct Node * root = NULL;

/** 
 * helper function to allocate memory for each new node, set up 
 * the pointers, and set the data field.
 */
struct Node * buildNode (char data[]) {
	struct Node * node = malloc(sizeof(struct Node));
	strcpy(node->data, data);
	node->left = NULL;
	node->right = NULL;
	return node;
}

/**
 * helper function to insert a node based off of current node's position.
 * To just insert a node to the tree, insert(root, data).
 */
struct Node * insert (struct Node * currentNode, char data[]) {
	if (currentNode == NULL) {
		return buildNode(data);
	} else {
		if (strcmp(data, currentNode->data) < 0) {
			currentNode->left = insert(currentNode->left, data);
		} else {
			currentNode->right = insert(currentNode->right, data);
		}
		return currentNode;
	}
}

struct Node* buildTreeFromFile() {
	FILE *file = fopen("problem4.input1","r");
	if (file == NULL) {
		printf("Input file not found.\n");
		return NULL;
	}
	char word[MAX_WORD_LENGTH] ; 
	while(fscanf(file, "%s", word) == 1) {
		if (root == NULL) {
			root = buildNode(word);
		} else {
			insert(root, word);
		}
	}
	fclose(file);
	return root; // return tree root node here.
}

void print_preorder(struct Node* tree) {
	// recursive base-case
	if (tree == NULL) {
		return;
	}
	printf("%s", tree->data);
	print_preorder(tree->left);
	print_preorder(tree->right);
}

void print_inorder(struct Node* tree) {
	// recursive base-case
	if (tree == NULL) {
		return;
	}
	print_inorder(tree->left);
	printf("%s", tree->data);
	print_inorder(tree->right);
}

void print_postorder(struct Node* tree) {
	// recursive base-case
	if (tree == NULL) {
		return;
	}
	print_postorder(tree->left);
	print_postorder(tree->right);
	printf("%s", tree->data);
}

bool found_in_tree(char *word_to_search, struct Node* tree) {
	// recursive base-case
	if (tree == NULL) {
		return false;
	} else {
		if (!strcmp(word_to_search, tree->data)) {
			return true;
		} else if (strcmp(word_to_search, tree->data) < 0){
			return found_in_tree(word_to_search, tree->left);
		} else {
			return found_in_tree(word_to_search, tree->right);
		}
	}
}

void use_tree_searching(struct Node* tree) {
	FILE *file = fopen("problem4.input2","r");
	if (file == NULL) {
		printf("Input file not found.\n");
		return;
	}
	char word_to_search[MAX_WORD_LENGTH];
	while(fscanf(file, "%s", word_to_search) == 1) {
		if (found_in_tree(word_to_search, tree)) {
			printf("Found %s\n", word_to_search);
		} else {
			printf("Not Found %s\n", word_to_search);
		}
	}
	fclose(file);
}

int main (int argc , char * argv []) {
	struct Node* tree = buildTreeFromFile();
	printf("Pre-Order:\n");
	print_preorder(tree);
	printf("\nIn-Order:\n");
	print_inorder(tree);
	printf("\nPost-Order:\n");
	print_postorder(tree);
	printf("\nSearching...\n");
	use_tree_searching(tree);
	return 0;
}
