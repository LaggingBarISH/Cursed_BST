#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int arrLenght=0;
int abortprogram = 0;

//Swap values of 2 integer pointers.
void swap(uint32_t *in1, uint32_t *in2){
	uint32_t temp = *in1;
	*in1 = *in2;
	*in2 = temp;
}

//Sort an uint32_t array.
void sortArray(uint32_t *array){
	int done = 1, i = 0;
	//Loop the whole array.
	for (i = 0; i < arrLenght - 1; i++){
		//Change positions of 2 integers if they are out of order.
		if(*(array + i) > *(array + i + 1)){
			swap(array + i, array + i + 1);
			//Set the flag to call itself recursively.
			done = 0;
		}
	}
	//Recursively call itself until array is no longer modified.
	if (!done) sortArray(array);
	return;
}

//Reads input from a file to an int pointer.
uint32_t* readInputToArray(FILE* input){
	int chr = 0, i = 0, j = 0, filterSpace=1;
	char allowed[] = {'0','1','2','3','4','5','6','7','8','9',' '};
	
	//Read input file, return for errors and get count of input.
	while ((chr = fgetc(input)) != EOF){
		for(i = 0; i < 11; i++){
			if (chr == allowed[i]){
				if (i == 10) {
					//Filter out multiple spaces.
					filterSpace = 1;
					break;
				}
				if(filterSpace){
					//Increase array size by one.
					arrLenght++;
					filterSpace = 0;
				}
				break;	
			} 
			if (i == 10) {
				//Input contains invalid characters. Abort program.
				abortprogram = 1;
				return NULL;
			}
		}
	}
	
	//Less than 16 integers on input. Abort.
	if (arrLenght < 16) {
		abortprogram = 1;
		return;
	}

	filterSpace = 1;
	//Initialize array.
	uint32_t *intArray = malloc(arrLenght * sizeof(uint32_t));
	memset(intArray, 0, arrLenght * sizeof(uint32_t));
	//Set file stream to start of file.
	fseek(input, 0, SEEK_SET);

	while ((chr = fgetc(input)) != EOF){
		for(i = 0; i < 11; i++){
			if (i == 10) {
				if (!filterSpace){
					filterSpace = 1;
					j++;
				}
				break;
			}
			//Parse input to int array.
			if (chr == allowed[i]){
				*(intArray + j) = *(intArray + j) * 10 + i;
				filterSpace = 0;
				break;	
			}
		}
	}
	return intArray;
}

//Structure of a tree node.
struct binary_tree_node_uint32_t{
	uint32_t value;
	struct binary_tree_node_uint32_t *leftNode;
	struct binary_tree_node_uint32_t *rightNode;
};

//Creates the tree from an int array input.
//Requires sorted input.
void createTree(struct binary_tree_node_uint32_t *tree, uint32_t *inputArray, int start, int end){
	//Find mid point and assign its value to the node.
	int mid = (end - start) / 2 + start;
	tree->value = *(inputArray + mid);
	tree->leftNode = tree->rightNode = NULL;
	//Create left child of the node. And recursively call itself with values less than mid.
	if(start != mid){
		tree->leftNode = malloc(sizeof(struct binary_tree_node_uint32_t));
		createTree(tree->leftNode, inputArray, start, mid);
	}
	//Create right child of the node. And recursively call itself with values higher than mid.
	if(end - 1 != mid){
		tree->rightNode = malloc(sizeof(struct binary_tree_node_uint32_t));
		createTree(tree->rightNode, inputArray, mid + 1, end);
	}
}

//I would love to do this part with some inline asm and BSR instruction magic. Depending on compiler optimizations it could be faster than calling a function with a loop like this.
//But i guess not for simplicity.
//Fast way to get log base 2 of an integer.
int intLog2(uint32_t number){
	int intlog=0;
	number = number >> 1;
	while (number){
		intlog++;
		number = number >> 1;
	}
	return intlog;
}

//Prints tree with depth levels. Only really used for debugging.
void printTreeValues(struct binary_tree_node_uint32_t *tree, int depth){
	printf("%i-%lu ", depth, tree->value);
	if(tree->leftNode != NULL) printTreeValues(tree->leftNode, depth + 1);
	if(tree->rightNode != NULL) printTreeValues(tree->rightNode, depth + 1);
}

//"Add leaves to the end of branches."
void addLeaves(struct binary_tree_node_uint32_t *tree, uint32_t *inputArray, int arraylenght, int *used){
	if(tree->leftNode != NULL){
		//Go left on a tree until its left child is NULL.
		addLeaves(tree->leftNode, inputArray, arraylenght, used);
	} else {
		//Create a new node on left node. With values from *inputarray.
		if (*used != arraylenght){
			tree->leftNode = malloc(sizeof(struct binary_tree_node_uint32_t));
			tree->leftNode->value = *(inputArray + *used);
			tree->leftNode->leftNode = tree->leftNode->rightNode = NULL;
			*used += 1;
		}
	}
	if(tree->rightNode != NULL){
		//Go right on a tree until its right child is NULL.
		addLeaves(tree->rightNode, inputArray, arraylenght, used);
	} else {
		//Create a new node on right node. With values from *inputarray.
		if (*used != arraylenght){
			tree->rightNode = malloc(sizeof(struct binary_tree_node_uint32_t));
			tree->rightNode->value = *(inputArray + *used);
			tree->rightNode->leftNode = tree->rightNode->rightNode = NULL;
			*used += 1;
		}
	}
}

//Saves the number of nodes on each level of tree on an array.
void printTree(struct binary_tree_node_uint32_t *tree, uint32_t *depth){
	*depth += 1;
	if(tree->leftNode != NULL) printTree(tree->leftNode, depth+1);
	if(tree->rightNode != NULL) printTree(tree->rightNode, depth+1);
}

//Search for a key in the tree.
int treeSearch(struct binary_tree_node_uint32_t *tree, uint32_t value, int depth, int element){
	if(tree->value == value){
		//Print Depth and node location with ordinals.
		switch (element){
			case 1:
				printf("At Depth level %i, %ist element\n", depth, element);
				break;
			case 2:
				printf("At Depth level %i, %ind element\n", depth, element);
				break;
			case 3:
				printf("At Depth level %i, %ird element\n", depth, element);
				break;
			default:
				printf("At Depth level %i, %ith element\n", depth, element);
				break;
		}
		return 1;
	}
	//Recursively go into child nodes.
	if(tree->leftNode != NULL){
		if(treeSearch(tree->leftNode, value, depth + 1, element * 2 - 1)) return 1;
	}
	if(tree->rightNode != NULL){
		if(treeSearch(tree->rightNode, value, depth + 1, element * 2)) return 1;
	}
	//Return 0 if key is not found.
	return 0;
}

int main(int argc, char const *argv[])
{
	int i = 0, j = 0, temp = 0, remainder = 0, leafcount = 0;
	FILE* fstream;
	struct binary_tree_node_uint32_t *mainTree = (struct binary_tree_node_uint32_t*) malloc(sizeof(struct binary_tree_node_uint32_t));
	//Open file handle.
	fstream = fopen("input.txt", "r");
	if (fstream == NULL) {
		printf("Input file missing. Aborting.");
		return 1;
	}
	//Read from file to array.
	uint32_t* inputArray = readInputToArray(fstream);
	fclose(fstream);
	if (abortprogram){
		printf("Input file is incorrect. Aborting.");
		return 1;
	}
	sortArray(inputArray);

	//Has 0 in input
	if (*inputArray == 0){
		printf("Input file is incorrect. Aborting.");
		return 1;
	}

	//Duplicate items on input
	for (i = 0; i < arrLenght - 1; i++) {
		if(*(inputArray + i) == *(inputArray + i + 1)){
			printf("Input file is incorrect. Aborting.");
			return 1;
		}
	}

	//Calculate the nodes needed to ensure 3log4 n.
	remainder = (arrLenght + 1) % (1 << intLog2(arrLenght));
	leafcount = 3 * (intLog2(arrLenght) / 2) - intLog2(arrLenght) - 1;
	if (remainder <= leafcount){
		remainder += 2 << (intLog2(arrLenght) - 1);
		leafcount++;	
	} 
	remainder -= leafcount;
	uint32_t *leftOvers = malloc(remainder * sizeof(uint32_t)), *fullTree = malloc((arrLenght - remainder - leafcount) * sizeof(uint32_t));

	//Fill the corrosponding arrays for the main tree and the last non full depth level.
	for (i = leafcount; i < remainder + leafcount; i++){
		*(leftOvers + i - leafcount) = *(inputArray + 2 * i - leafcount);
		*(fullTree + i - leafcount) = *(inputArray + 2 * i + 1 - leafcount);
	}
	for(j = i - leafcount; j < arrLenght - i; j++) *(fullTree + j) = *(inputArray + j + i);
	
	//Creates the tree from fullTree array.
	createTree(mainTree, fullTree, 0, arrLenght - remainder - leafcount);
	
	//Adds the last level of nodes.
	addLeaves(mainTree, leftOvers, remainder, &temp);

	//Add nodes required to get 3log4 n.
	for(i = leafcount - 1; i >= 0; i--){
		temp = 0;
		addLeaves(mainTree, inputArray + i, 1, &temp);
	}

	//Print tree information.
	int *depth = malloc(3 * (intLog2(arrLenght) / 2) * sizeof(uint32_t));
	memset(depth, 0, 3 * (intLog2(arrLenght) / 2) * sizeof(uint32_t));
	printTree(mainTree, depth);
	printf("Depth level of BST is %i\n", 3 * (intLog2(arrLenght) / 2));
	for(i = 0; i < 3 * (intLog2(arrLenght) / 2); i++){
		printf("Depth level %i -> %i\n", i, *(depth + i));
	}

	free(inputArray);
	uint32_t searchValue = 0;
	while(1){
		printf("Key value to be searched (Enter 0 to exit) : ");
		int result = scanf("\n%lu", &searchValue);
		if(result){
			if(searchValue) {if(!treeSearch(mainTree, searchValue, 0, 1)) printf("%lu is not found in BST\n", searchValue);}
			else {
				printf("Exit\n");
				return 0;
			}
		} else printf("Invalid input.\n");
		//Clear stdin to avoid loop on incorrect / partially correct input.
		scanf("%*[^\n]%*c");
	}
	
	return 0;
	
}

