//Name : Nivedita Rajendran
//Description : This program adds data into the binary tree using inorder method
//			  and prints it in ascending order
//Date : 2018/10/14

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_SIZE 12 // no. of data

// structure defining a binary tree node.  Lower sorted values will go the the left, higher to the right.
typedef struct nodeData {
	char name[30];				// character string of the data being stored in the node
	struct nodeData *left;		// pointer to the next node in the tree that is less than current node OR NULL if empty
	struct nodeData *right;		// pointer to the next node in the tree that is greater than current node OR NULL if empty
}binNodeData;

// structure defining a stack
typedef struct stack_s {
	binNodeData *nodeData;  // pointer of binary tree node
	struct stack_s *next;   // pointer to the next stack element
}stack;

binNodeData *head = NULL;	// head of the binary tree
// data to be stored in tree.  In this case there is just a character string but this could have contained more

// replace inputData used for test put it in here and is created from Prog8130AssignDataCreator.exe 
const char *inputData[DATA_SIZE] = {
	{ "goarq" },
	{ "swvuq" },
	{ "phaft" },
	{ "qncjw" },
	{ "ftyvq" },
	{ "rvsnq" },
	{ "ynmtj" },
	{ "yphuq" },
	{ "yqmvb" },
	{ "krgog" },
	{ "udxjd" },
	{ "bebmn" }
};

// FUNCTION      : push
//
// DESCRIPTION   :
//   This function will store the given tree node into the stack
//
// PARAMETERS    :
//   ptrHead - pointer to the top of the stack
//	 node    - it is a structure pointer to a binary tree node
//
// RETURNS       :
//   Nothing
void push(stack **ptrHead, binNodeData *node) {
	stack *newNode = NULL; // creates a new stack element

	newNode = (stack*)malloc(sizeof(stack_s)); // allocates memory to the new stack element
	if (newNode == NULL) {
		printf("ERROR!!!!!! STACK OVERFLOW!!!! \n");
	}

	newNode->nodeData = node;
	newNode->next = (*ptrHead);
	*ptrHead = newNode;
}
// FUNCTION      : pop
//
// DESCRIPTION   :
//   This function will pop the data at the top of the stack
//
// PARAMETERS    :
//   Nothing
//
// RETURNS       :
//   tNode - the data that is popped out of the stack
binNodeData *pop(stack **ptrHead) {
	binNodeData *tNode;
	stack *top; 

	if (*ptrHead == NULL) {
		printf("STACK UNDERFLOW \n");
		getchar();
		exit(0);
	}
	else {
		top = *ptrHead;
		tNode = top->nodeData;
		*ptrHead = top->next;
		free(top);
		return tNode;
	}

}
// FUNCTION      : addToBinaryTree
//
// DESCRIPTION   :
//   This function will store the data in newNode into a binary tree according to alphabetical order
//
// PARAMETERS    :
//   newString - the string that is to be stored in the binary tree in alphabetica order
//
// RETURNS       :
//   Nothing
void addToBinaryTree(const char *newString) {
	// add code to put items into binary tree in alphabetical order here
	// consider using strcmp library function to decide if a name is greater or less than
	binNodeData *newNode = NULL;
	newNode = (binNodeData *)malloc(sizeof(binNodeData));
	strcpy_s(newNode->name, newString);
	newNode->left = NULL;
	newNode->right = NULL;

	if (head == NULL) {
		head = newNode;
	}
	else {
		nodeData * prev = NULL;
		nodeData * curr = head;
		while (curr != NULL) {
			prev = curr;
			
			if (strcmp(curr->name, newNode->name) < 0) {
				curr = curr->right;
			}
			else {
				curr = curr->left;
			}
		}
		if (strcmp(prev->name, newNode->name) < 0) {
			prev->right = newNode;
		}
		else {
			prev->left = newNode;
		}

	}
}
 //FUNCTION      : printBinaryTree

 //DESCRIPTION   :
 //  This function will print the entire binary tree out.  You can choose to print
 //  it using recursion but more marks will be awarded if you do it non recursively.

 //PARAMETERS    :
 //  None

 //RETURNS       :
 //  Nothing
void printBinaryTree(void) {
	// add code to print data in order starting at the "head" pointer
	binNodeData *current = head;
	stack *s = NULL; // declares the stack
	int flag = 1;
	while (flag) {
		if (current != NULL) {
			push(&s, current);
			current = current->left;
		}
		else {
			if (s != NULL) {
				current = pop(&s);
				printf("%s\n", current->name);
				current = current->right;
			}
			else {
				flag = 0;
			}
		}
	}
}

int main() {
	int i;
	// displaying the input data
	printf("Data added to the Binary Tree:\n");
	for (i = 0; i < DATA_SIZE; i++)
		printf("%s \n", inputData[i]);

	// put all the data into the binary tree
	for (i = 0; i < DATA_SIZE; i++)
		addToBinaryTree(inputData[i]);

	// print the entire tree.  It should come out in alphabetical order because of the way it was stored in the tree
	printf("\n\nData after adding to binary tree, traversed inorder: \n");
	printBinaryTree();

	return 1;
}
