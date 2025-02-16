#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

// structs ---------------------
struct Node{
	char element;
	int freq;
	struct Node * left, * right;
};

struct AVLNode{
	char element;
	int freq;
	int height;
	struct AVLNode * left, * right;
};

struct MinHeap{
	int size;
	int capacity;
	struct Node ** array;
};


/*
	Utility functions -- Huffman
*/

struct AVLNode * newNode(char element, int freq){
	struct AVLNode * node = (struct AVLNode*)malloc(sizeof(struct AVLNode));
	node->element = element;
	node->freq = freq;
	node->height = 1;
	node->left = node->right = NULL;
	return node;
}

int max(int a, int b){
	return (a > b) ? a : b;
}

int height(struct AVLNode * node){
	if(node == NULL){
		return 0;
	}
	return node->height;
}

int getBalance(struct AVLNode * node){
	if(node == NULL){
		return 0;
	}
	return height(node->left) - height(node->right);
}

struct AVLNode * rotateLeft(struct AVLNode * node){
	struct AVLNode * right = node->right;
	struct AVLNode * rightChild = right->left;
	
	right->left = node;
	node->right = rightChild;
	
	node->height = 1 + max(height(node->left), height(node->right));
	right->height = 1 + max(height(right->left), height(right->right));
	
	return right;
}

struct AVLNode * rotateRight(struct AVLNode * node){
	struct AVLNode * left = node->left;
	struct AVLNode * leftChild = left->right;
	
	left->right = node;
	node->left = leftChild;
	
	node->height = 1 + max(height(node->left), height(node->right));
	left->height = 1 + max(height(left->left), height(left->right));
	
	return left;
}

struct AVLNode * insert(struct AVLNode * node, int element, int freq){
	if(node == NULL){
		return newNode(element, freq);
	}
	
	if(freq < node->freq){
		node->left = insert(node->left, element, freq);
	}
	else if(freq > node->freq){
		node->right = insert(node->right, element, freq);
	}
	else{
		// don't allow duplicates
		return node;
	}
	
	// recalculate Heights
	node->height = 1 + max(height(node->left), height(node->right));
	
	// calculate balance factor of current node
	int balance = getBalance(node);
	
	// rotate node based on 4 cases
	// note: we check child is left or right oriented using the
	// entered node's value because this check only occurs in the parent
	// iteration of the iteration in which we add the node
	
	// the left left or the ++ case
	if(balance > 1 && (freq < node->left->freq)){
		return rotateRight(node);
	}
	
	// the right right or the -- case
	else if(balance < -1 && (freq > node->right->freq)){
		return rotateLeft(node);
	}
	
	// the left right or the +- case
	else if(balance > 1 && (freq > node->left->freq)){
		node->left = rotateLeft(node->left);
		return rotateRight(node);
	}
	
	// the right left or the -+ case
	else if(balance < -1 && (freq < node->right->freq)){
		node->right = rotateRight(node->right);
		return rotateLeft(node);
	}
	
	return node;
}


/*
	Utility functions -- Huffman
*/


void swapNodePointers(struct Node ** node1, struct Node ** node2){
	struct Node * temp = *node1;
	*node1 = *node2;
	*node2 = temp;
}

void MinHeapify(struct MinHeap * minHeap, int index){
	int smallest = index;
	int left = (index * 2) + 1;
	int right = (index * 2) + 2;
	
	if(left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq){
		smallest = left;
	}
	
	if(right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq){
		smallest = right;
	}
	
	if(smallest != index){
		swapNodePointers(&minHeap->array[smallest], &minHeap->array[index]);
		MinHeapify(minHeap, smallest);
	}
}

struct Node * extractMin(struct MinHeap * minHeap){
	
	// get the root node (which is always the minimum element in a min heap)
	struct Node * min = minHeap->array[0];
	
	// replace root node with last element of minHeapArr (last element of the last layer)
	minHeap->array[0] = minHeap->array[minHeap->size - 1];
	minHeap->size--;
	
	// re-run the minHeapify operation on the root
	// to keep the next min element ready
	MinHeapify(minHeap, 0);
	
	return min;
}

void insertMinHeap(struct Node * top, struct MinHeap * minHeap){
	
	// increase minHeap size by 1
	minHeap->size++;
	
	// get position of last node with a child
	int lastNodeWithLeaf = (minHeap->size - 2) / 2;
	
	// if top node freq less than last node with child node
	if(top->freq < minHeap->array[lastNodeWithLeaf]->freq){
		
		// move node with leaf in the end and ad top node in its place
		minHeap->array[minHeap->size - 1] = minHeap->array[lastNodeWithLeaf];
		minHeap->array[lastNodeWithLeaf] = top;
	}
	else{
		// add top node in end
		minHeap->array[minHeap->size - 1] = top;
	}
}


/*
	Operational functions
*/


struct Node * constructHuffmanTree(char elements[], int freq[], int size) {
	
	// initialize minheap ------------------------
	struct MinHeap * minHeap = (struct MinHeap*)malloc(size * sizeof(struct MinHeap));
	minHeap->capacity = size;
	minHeap->size = 0;
	minHeap->array = (struct Node**)malloc(size * sizeof(struct Node*));
	
	
	// populate min heap ------------------------
	int i;
	for(i = 0; i < size; i++){
		// initialize new node
		struct Node * newNode = (struct Node*)malloc(sizeof(struct Node));
		newNode->element = elements[i];
		newNode->freq = freq[i];
		newNode->left = newNode->right = NULL;
		
		// add node to minHeap array
		minHeap->array[i] = newNode;
		minHeap->size++;
	}
	
	// convert entire minHeapArray to an actual minheap
	// if data inserted wasn't sorted in the first place
	
	// first we get the last node which is not a leaf
	int lastNonLeafNodeIndex = (minHeap->size - 2) / 2;
	
	// then we do minHeapify operation on every node with child nodes
	for(i = lastNonLeafNodeIndex; i >= 0; i--){
		MinHeapify(minHeap, i);
	}
	
	// build huffman tree
	struct Node *left, *right, *top;
	while(1){
		
		left = extractMin(minHeap);
		right = extractMin(minHeap);
		
		if(minHeap->size < 0) { break; }
				
		// create internal node that holds both left and right nodes
		struct Node * top = (struct Node*)malloc(sizeof(struct Node));
		top->left = left;
		top->right = right;
		top->freq = left->freq + right->freq;
		char internalElement = '~';
		top->element = internalElement;
		
		// insert top node into tree
		insertMinHeap(top, minHeap);
	}
	
	free(minHeap->array);
	free(minHeap);
	
	// finally the "right" node contains all the information
	return right;
	
}

struct AVLNode * constructAVLTree(struct Node ** root){
	// initialize pointer for traversal
	struct Node * iterator = *root;
	
	// initialize avl tree root
	struct AVLNode * AVLRoot = newNode(iterator->element, iterator->freq);
	
	// traverse Huffman tree in preorder and add nodes in avl tree accordingly
	void traverseHuffman(struct Node * node){
		if(node != NULL){
			// insert node into avl tree
			printf("Adding element with freq %d to AVL Tree\n", node->freq);
			AVLRoot = insert(AVLRoot, node->element, node->freq);
			// recursion ahead
			traverseHuffman(node->left);
			traverseHuffman(node->right);
		}
	}
	
	traverseHuffman(iterator);
	
	return AVLRoot;
}

// print all binary codes for each character
// recursively visit all nodes and calculate code for each symbol
void calculateCodesAVL(struct AVLNode * root, int code[], int codelen){
	
	// visit right node
	if(root->right != NULL){
		code[codelen] = 1;
		calculateCodesAVL(root->right, code, codelen+1);
	}
	
	// visit left node
	if(root->left != NULL){
		code[codelen] = 0;
		calculateCodesAVL(root->left, code, codelen+1);
	}
	
	// if current visited node is leaf node
//	if(root->left == NULL && root->right == NULL){
	if(root->element != '~'){
		
		// print code and node properties
		printf("element: %c, frequency: %d, code: ", root->element, root->freq);
		int j;
		for(j = 0; j < codelen; j++){
			printf("%d", code[j]);
		}
		printf("\n");
	}
}

void calculateCodesHuffman(struct Node * root, int code[], int codelen){
	
	// visit right node
	if(root->right != NULL){
		code[codelen] = 1;
		calculateCodesHuffman(root->right, code, codelen+1);
	}
	
	// visit left node
	if(root->left != NULL){
		code[codelen] = 0;
		calculateCodesHuffman(root->left, code, codelen+1);
	}
	
	// if current visited node is leaf node
//	if(root->left == NULL && root->right == NULL){
	if(root->element != '~'){
		
		// print code and node properties
		printf("element: %c, frequency: %d, code: ", root->element, root->freq);
		int j;
		for(j = 0; j < codelen; j++){
			printf("%d", code[j]);
		}
		printf("\n");
	}
}

void preOrderHuffman(struct Node * node){
	if(node != NULL){
		printf("%d ", node->freq);
		preOrderHuffman(node->left);
		preOrderHuffman(node->right);
	}
}

void preOrderAVL(struct AVLNode * node){
	if(node != NULL){
		printf("%d ", node->freq);
		preOrderAVL(node->left);
		preOrderAVL(node->right);
	}
}

void main() 
{ 
    char arr[] = { 'a', 'b', 'c', 'd', 'e', 'f' }; 
    int freq[] = { 5, 9, 12, 13, 16, 45 }; 
  
    int size = sizeof(arr) / sizeof(arr[0]);
    
    if(size == 0){
    	printf("Tree is empty");
    	exit(0);
	}
  
    struct Node * HuffmanRoot = constructHuffmanTree(arr, freq, size);
    struct AVLNode * AVLRoot = constructAVLTree(&HuffmanRoot);
    printf("\n");
    
	int temp1[100];
	int temp2[100];
	
    // call print codes function for huffman tree	
	// call print codes function for AVL tree
	
	
	printf("\nHuffman Pre-order traversal: ");
	preOrderHuffman(HuffmanRoot);
	printf("\nHuffman Symbol Table\n");
	calculateCodesHuffman(HuffmanRoot, temp1, 0);
	printf("\nAVL Pre-order traversal: ");
	preOrderAVL(AVLRoot);
	printf("\nAVL Symbol Table\n");
	calculateCodesAVL(AVLRoot, temp2, 0);
}
