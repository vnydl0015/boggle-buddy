/*
	Implementation for module which contains  
		Problem 2-related data structures and 
		functions.
	
	Sample solution implemented by Grady Fitzpatrick
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"
#include "prefixTree.h"

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64
#define DEFAULT_STR ""
#define NUM_1 1
#define NUM_2 2
#define NUM_3 3

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

/* New solutions **************************************************************/
/* 
	Sets up a solution for the given problem 
*/
struct solution *newSolution(struct problem *problem){
	struct solution *s = (struct solution *) malloc(sizeof(struct solution));
	assert(s);
	s->foundWordCount = 0;
	s->words = (char **) malloc(sizeof(char *) * (problem->wordCount * 2));
	assert(s->words);
	s->foundLetterCount = 0;
	s->followLetters = (char *) malloc(sizeof(char) * MAX_CHAR);
	assert(s->followLetters);
	return s;
}

/* Part A ********************************************************************/
/*
	Solves the given problem according to part A's definitions
	and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p){
	struct solution *s = newSolution(p);
	int i;

	/* create and add words from dictionary into prefix tree */
	struct prefixTree *t = newPrefixTree();
	addDictionaryToTree(t, p);

	bool *visited = (bool *)malloc(sizeof(bool) * p->dimension * p->dimension);
	assert(visited);

	/* perform depth-first search by looping through all words in prefix tree */
	for (i = 0; i < MAX_CHAR; i++) {
		memset(visited, false, sizeof(bool) * p->dimension * p->dimension);
		if (t->child[i]) {

			/* Loops through all words in board and compare first tree char */
			for (int j = 0; j < p->dimension * p->dimension; j++) {
				char *currChar = getStr(&(p->boardFlat[j]));
				if (strcmp(currChar, t->child[i]->word) == 0) {
				  	treeExploreA(t->child[i], s, &p->boardFlat[j], p, visited);
				}
				free(currChar);
			}
		}
	 }

	/* Get all the solutions using another DFS on marked words */
	 for (i = 0; i < MAX_CHAR; i++) { 
		if (t->child[i]) { 
			exploreSolutions(t->child[i], s, p);
		}
	 }

	 freeTree(t);
	 free(visited);
	 return s;
}

/*
	Explores both prefix tree and letters in board simultaneously using
    depth-first search approach and queue data structure.
*/
void treeExploreA(struct prefixTree *t, struct solution *s, char *charInBoard, 
	struct problem *p, bool *visited) {

	/* mark matched word */
	if (t->isEnd && !visited[charInBoard - p->boardFlat]) {
		t->markedWord = true;
	}
	visited[charInBoard - p->boardFlat] = true;

	/* terminate at leaf node */
	if (t->numChild == 0) {
		if (t->markedWord) {
			visited[charInBoard - p->boardFlat] = false;
		}
		return;
	}

	/* compares all children letters in tree against neighbours in board */
    for (int i = 0; i < MAX_CHAR; i++) {
         if (t->child[i]) {
			/* stores all the MATCHED neighbours of current letter*/
			struct queue *q = createQueue();
		 	getNeighbour(charInBoard, i, p, q);
			while (q->head) {
				char *neighbourChar = dequeue(q);
				if (!visited[neighbourChar - p->boardFlat]) {
					treeExploreA(t->child[i], s, neighbourChar, p, visited);	
				} 
			}
			freeQueue(q);
		} 
    }
	visited[charInBoard - p->boardFlat] = false;
}

/* Part B ********************************************************************/
/*
	Solves the given problem according to all part B's definitions
	and places the solution output into a returned solution value.
*/
struct solution *solveProblemB(struct problem *p){
	struct solution *s = newSolution(p);
	int i;
	int partialIndex = 0;
	struct prefixTree *partialStringEnd = NULL;

	/* create and add words from dictionary into prefix tree */
	struct prefixTree *t = newPrefixTree();
	addDictionaryToTree(t, p);

	/* perform depth-first search by looping through all words in prefix tree */
	for (i = 0; i < MAX_CHAR; i++) {
		if (t->child[i]) {

			/* check if current letter in tree matches with partial string */
			char *partialStr = getStr(&(p->partialString[0]));
			if (strcmp(t->child[i]->word, partialStr) != 0) { 
				free(partialStr);
				continue; 
			}
			free(partialStr);
			partialIndex++;
		
			/* Loops through all words in board and compare with tree char */
			for (int j = 0; j < p->dimension * p->dimension; j++) {
				char *currChar = getStr(&(p->boardFlat[j]));
				if (strcmp(currChar, t->child[i]->word) == 0) {
				  	treeExploreB(t->child[i], s, &p->boardFlat[j], partialIndex, 
                                 p, &partialStringEnd);
				}
				free(currChar);
			}
		}
	 }

	/* Get all the solutions using another DFS on marked words */
	 for (i = 0; i < MAX_CHAR; i++) { 
		if (t->child[i]) { 
			exploreSolutions(t->child[i], s, p);
		}
	 }

	 freeTree(t);
	 return s;
}

/*
	Explores both prefix tree and letters in board simultaneously using
    depth-first search approach and queue data structure.
*/
void treeExploreB(struct prefixTree *t, struct solution *s, char *charInBoard, 
	int partialIndex, struct problem *p, struct prefixTree **partialStringEnd) {

	/* pointing to subsequent letter of partial string */
	if (partialIndex == p->partialSize + 1) {
		*partialStringEnd = t;
	} 
	
	/* mark matched word */
	if (t->isEnd) {
		if (*partialStringEnd) {
			(*partialStringEnd)->markedWord = true;
		} else {
			t->isEndB = true;
		}
	}

	/* terminate at leaf node */
	if (t->numChild == 0) {
		return;
	}

	/* compares all children letters in tree against neighbours in board */
    for (int i = 0; i < MAX_CHAR; i++) {
        if (t->child[i]) {
			if ((partialIndex < p->partialSize && (char) i == 
               p->partialString[partialIndex]) || partialIndex>=p->partialSize){
                /* stores all the MATCHED neighbours of current letter */
				struct queue *q = createQueue();
				getNeighbour(charInBoard, i, p, q);
				while (q->head) {
					char *neighbourChar = dequeue(q);				
					treeExploreB(t->child[i], s, neighbourChar, 
									partialIndex + 1, p, partialStringEnd);	
				}
				freeQueue(q);
			} 
		}
    }
	/* reset the pointer for every new word */
	if (t == *partialStringEnd) {
		*partialStringEnd = NULL;
	}
}

/* Part D ********************************************************************/
/*
	Solves the given problem according to all part D's definitions
	and places the solution output into a returned solution value.
*/
struct solution *solveProblemD(struct problem *p){
	struct solution *s = newSolution(p);
	int i;

	/* create and add words from dictionary into prefix tree */
	struct prefixTree *t = newPrefixTree();
	addDictionaryToTree(t, p);

	/* perform depth-first search by looping through all words in prefix tree */
	for (i = 0; i < MAX_CHAR; i++) {
		bool visited[MAX_CHAR] = {false};

		if (t->child[i]) {
			/* Loops through all words in board and compare with tree char */
			for (int j = 0; j < p->dimension * p->dimension; j++) {
				char *currChar = getStr(&(p->boardFlat[j]));
				if (strcmp(currChar, t->child[i]->word) == 0) {
					visited[i] = true;
				  	treeExploreD(t->child[i], s, &p->boardFlat[j], 
                                p, visited);
				}
				free(currChar);
			}
		}
	 }

	/* Get all the solutions using another DFS on marked words */
	 for (i = 0; i < MAX_CHAR; i++) { 
		if (t->child[i]) { 
			exploreSolutions(t->child[i], s, p);
		}
	 }

	 freeTree(t);
	 return s;
}

/*
	Explores both prefix tree and letters in board simultaneously using
    depth-first search approach and queue data structure.
*/
void treeExploreD(struct prefixTree *t, struct solution *s, char *charInBoard,
	struct problem *p, bool *visited) {

	/* mark matched word */
	if (t->isEnd) {
		t->markedWord = true;
		int len = strlen(t->word);
		/* mark word as visited */
		for (int p = 0; p < len; p++) {
			visited[(int) t->word[p]] = true;
		}
	}

	/* terminate at leaf node */
	if (t->numChild == 0) {
		return;
	}

	/* compares all children letters of current letter in tree against 
        neighbours in board */
    for (int i = 0; i < MAX_CHAR; i++) {
         if (t->child[i] && !visited[i]) {
            /* search through all neighbours of current board character */
			struct queue *q = createQueue();
		 	getNeighbour(charInBoard, i, p, q);
			char *neighbourChar = NULL;
			while (q->head) {
				neighbourChar = dequeue(q);
				if (!visited[(int) neighbourChar[0]]) {
					 treeExploreD(t->child[i], s, neighbourChar, p, visited);
				} 
			}
            /* Unvisited char for every new word */
			if (neighbourChar) {
				visited[(int) neighbourChar[0]] = false; 
			}
			freeQueue(q);
		} 
    }
}

/* Helper functions ***********************************************************/
/* 
	Store all neighbours of current board piece (that matches with current 
    letter in tree) in queue. Time complexity of O(1).
*/
void getNeighbour(char *charInBoard, int treeIndex, struct problem *p, struct
                  queue *q) {

	/* Converting flat board into 3D grid by getting row and column indexes */
	int step = p->dimension;
	int row = (charInBoard - p->boardFlat) / p->dimension;
	int col = (charInBoard - p->boardFlat) - row * step;
    char treeChar = (char) treeIndex;

	/* check above neighbour */
	if (row != 0 && (treeChar == p->boardFlat[(row - 1) * step + col])) {
		enqueue(q, &(p->boardFlat[(row - 1) * step + col]));
	} 
	/* check below neighbour */
	if (row != (p->dimension - 1) && 
		(treeChar == p->boardFlat[(row + 1) * step + col])) {
		enqueue(q, &(p->boardFlat[(row + 1) * step + col]));
	} 
	/* check left neighbour */
	if (col != 0 && (treeChar == p->boardFlat[row * step + (col - 1)])) {
		enqueue(q, &(p->boardFlat[row * step + (col - 1)]));
	} 
	/* check right neighbour */
	if (col != (p->dimension - 1) && 
		(treeChar == p->boardFlat[row * step + (col + 1)])) {
		enqueue(q, &(p->boardFlat[row * step + (col + 1)]));
	}  
	/* check upper right neighbour */
	if (row != 0 && col != (p->dimension - 1) && 
        (treeChar == p->boardFlat[(row - 1) * step + (col + 1)])) {
		enqueue(q, &(p->boardFlat[(row - 1) * step + (col + 1)]));
	}  
	/* check upper left neighbour */
	if (row != 0 && col != 0 && 
        (treeChar == p->boardFlat[(row - 1) * step + (col - 1)])) {
		enqueue(q, &(p->boardFlat[(row - 1) * step + (col - 1)]));
	}  
	/* check lower left neighbour */
	if (row != (p->dimension - 1) && col != 0 && 
        (treeChar == p->boardFlat[(row + 1) * step + (col - 1)])) {
		enqueue(q, &(p->boardFlat[(row + 1) * step + (col - 1)]));
	} 
	/* check lower right neighbour */
	if (row != (p->dimension - 1) && col != (p->dimension - 1) && 
        (treeChar== p->boardFlat[(row + 1) * step + (col + 1)])) {
		enqueue(q, &(p->boardFlat[(row + 1) * step + (col + 1)]));
	}
}

/*
    Inserts the found word into solution data structure (for all parts A-D) via 
    DFS on prefix tree.
*/
void exploreSolutions(struct prefixTree *t,struct solution *s,struct problem*p){
	
	/* Insert solution into solution struct */
	/* For parts A and D */
	if ((p->part == PART_A || p->part == PART_D) && t->markedWord) {
		char *foundWord = (char *) malloc(strlen(t->word) + 1);
		assert(foundWord);
		strcpy(foundWord, t->word);
		s->words[(s->foundWordCount)++] = foundWord;

	/* For part B */
	} else if (p->part == PART_B) {
		if (t->isEndB) {
			s->followLetters[(s->foundLetterCount)++] = NULL_CHAR; 
		} else if (t->markedWord) {
			if (isalpha(t->word[strlen(t->word)-1])) {
		 		s->followLetters[(s->foundLetterCount)++]=
                    t->word[strlen(t->word)-1]; 
			} else {
				s->followLetters[(s->foundLetterCount)++] = NULL_CHAR; 
			}
		}
	}

	/* Traverse through the child tree */
	for (int i = 0; i < MAX_CHAR; i++) { 
		if (t->child[i]) {
			exploreSolutions(t->child[i], s, p);
		}
	}
}

/*
    Add all words into prefix tree.
*/
void addDictionaryToTree(struct prefixTree *t, struct problem *p) {
	int i;
	/* Add all words into tree */
	strcpy((t->word = malloc(NUM_2)), DEFAULT_STR);
	for (i = 0; i < p->wordCount; i++) {
		t = addWordToTree(t, p->words[i]);
	}
	/* convert all pieces in board to lower case */
	for (i = 0; i < p->dimension * p->dimension; i++) {
		p->boardFlat[i] = tolower(p->boardFlat[i]);
	}
}

/* Queue structure ************************************************************/
/*
    Create new queue.
*/
struct queue *createQueue() {
    struct queue *queue = malloc(sizeof(struct queue));
	assert(queue);
    queue->head = queue->tail = NULL;
    return queue;
}

/* 
    Removes and return the item in the front-most node.
*/
char *dequeue(struct queue *queue) {
    if (!queue->head) { 
        return NULL; 
    }
    char *word = queue->head->item;
    struct node *curr = queue->head;
    queue->head = queue->head->next;
	free(curr);
    return word;
}

/*
    Inserts new node into queue.
*/
void enqueue(struct queue *queue, char *word) {
    struct node *newNode = malloc(sizeof(struct node));
    assert(newNode);
    newNode->next = NULL;
    newNode->item = word;

	if (!queue->head) {
		queue->head = queue->tail = newNode;
		return;
	}
    queue->tail->next = newNode;   
    queue->tail = newNode; 
}

/*
    Frees queue.
*/
void freeQueue(struct queue *queue) {
    if (!queue->head) {
		free(queue);
		return;
	}
    while (!queue->head) {
        struct node *curr = queue->head;
        queue->head = queue->head->next;
        free(curr);
    }
	free(queue);
}

/* Frees data ****************************************************************/
/*
	Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
	if(solution){
		if(solution->followLetters){
			free(solution->followLetters);
		}
		if(solution->words){
			for (int i = 0; i < solution->foundWordCount; i++) {
				free(solution->words[i]); 
			}
			free(solution->words);
		}
		free(solution);
	}
}

/*
	Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
	if(problem){
		if(problem->words){
			for(int i = 0; i < problem->wordCount; i++){
				if(problem->words[i]){
					free(problem->words[i]);
				}
			}
			free(problem->words);
		}
		if(problem->board){
			free(problem->board);
		}
		if(problem->boardFlat){
			free(problem->boardFlat);
		}
		if(problem->partialString){
			free(problem->partialString);
		}
		free(problem);
	}
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/* The following blocks of codes are written by Grady Fitzpatrick for 
	COMP20007 Assignment 2 2024 Semester 1
*/
/****************************************************************************/
/****************************************************************************/
/* Reads problems ***********************************************************/
/* 
	Reads the given dict file into a list of words 
	and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *dictFile, FILE *boardFile){
	struct problem *p = (struct problem *) malloc(sizeof(struct problem));
	assert(p);

	/* Part B onwards so set as empty. */
	p->partialString = NULL;

	int wordCount = 0;
	int wordAllocated = 0;
	char *dictText = NULL;
	char **words = NULL;

	/* Read in text. */
	size_t allocated = 0;
	/* Exit if we read no characters or an error caught. */
	int success = getdelim(&dictText, &allocated, '\0', dictFile);

	if(success == -1){
		/* Encountered an error. */
		perror("Encountered error reading dictionary file");
		exit(EXIT_FAILURE);
	} else {
		/* Assume file contains at least one character. */
		assert(success > 0);
	}

	char *boardText = NULL;
	/* Reset allocated marker. */
	allocated = 0;
	success = getdelim(&boardText, &allocated, '\0', boardFile);

	if(success == -1){
		/* Encountered an error. */
		perror("Encountered error reading board file");
		exit(EXIT_FAILURE);
	} else {
		/* Assume file contains at least one character. */
		assert(success > 0);
	}

	/* Progress through string. */
	int progress = 0;
	/* Table string length. */
	int dictTextLength = strlen(dictText);

	/* Count words present. */
	while(progress < dictTextLength){
		char *word = NULL;
		int nextProgress;
		/* Read each value into the dictionary. */
		if(progress == 0){
			/* First line. */
			int wordNumberGuess;
			assert(sscanf(dictText + progress, "%d %n", &wordNumberGuess, 
                          &nextProgress) == 1);
			/* Will fail if integer missing from the start of the words. */
			assert(nextProgress > 0);
			if(wordNumberGuess > 0){
				wordAllocated = wordNumberGuess;
				words = (char **) malloc(sizeof(char *) * wordAllocated);
				assert(words);
			}
			progress += nextProgress;
			continue;
		} else {
			assert(sscanf(dictText + progress, "%m[^\n] %n", &word, 
                          &nextProgress) == 1);
			assert(nextProgress > 0);
			progress += nextProgress;
		}

		/* Check if more space is needed to store the word. */
		if(wordAllocated <= 0){
			words = (char **) malloc(sizeof(char *) * INITIALWORDSALLOCATION);
			assert(words);
			wordAllocated = INITIALWORDSALLOCATION;
		} else if(wordCount >= wordAllocated){
			words = (char **) realloc(words, sizeof(char *) * 
				wordAllocated * 2);
			assert(words);
			wordAllocated = wordAllocated * 2;
			/* Something has gone wrong if there's not sufficient word 
				space for another word. */
			assert(wordAllocated > wordCount);
		}

		words[wordCount] = word;
		wordCount++;
	}
	
	/* Done with dictText */
	if(dictText){
		free(dictText);
	}
	
	/* Now read in board */
	progress = 0;
	int dimension = 0;
	int boardTextLength = strlen(boardText);
	/* Count dimension with first line */
	while(progress < boardTextLength){
		/* Count how many non-space characters appear in line. */
		if(boardText[progress] == '\n' || boardText[progress] == '\0'){
			/* Reached end of line. */
			break;
		}
		if(isalpha(boardText[progress])){
			dimension++;
		}
		progress++;
	}

	assert(dimension > 0);

	/* Check each line has the correct dimension. */
	for(int i = 1; i < dimension; i++){
		int rowDim = 0;
		if(boardText[progress] == '\n'){
			progress++;
		}
		while(progress < boardTextLength){
			/* Count how many non-space characters appear in line. */
			if(boardText[progress] == '\n' || boardText[progress] == '\0'){
				/* Reached end of line. */
				break;
			}
			if(isalpha(boardText[progress])){
				rowDim++;
			}
			progress++;
		}
		if(rowDim != dimension){
			fprintf(stderr, 
			"Row #%d had %d letters, different to Row #1's %d letters.\n", 
                    i + 1, rowDim, dimension);
			assert(rowDim == dimension);
		}
	}

	/* Define board. */
	char *boardFlat = (char *) malloc(sizeof(char) * dimension * dimension);
	assert(boardFlat);
	
	/* Reset progress. */
	progress = 0;
	for(int i = 0; i < dimension; i++){
		for(int j = 0; j < dimension; j++){
			int nextProgress;
			assert(sscanf(boardText + progress, "%c %n", 
                          &boardFlat[i * dimension + j], &nextProgress) == 1);
			progress += nextProgress;
		}
	}

	char **board = (char **) malloc(sizeof(char **) * dimension);
	assert(board);
	for(int i = 0; i < dimension; i++){
		board[i] = &boardFlat[i * dimension];
	}

	// fprintf(stderr, "\n");

	/* The number of words in the text. */
	p->wordCount = wordCount;
	/* The list of words in the dictionary. */
	p->words = words;
	/* The dimension of the board (number of rows) */
	p->dimension = dimension;
	/* The board, represented both as a 1-D list and a 2-D list */
	p->boardFlat = boardFlat;
	p->board = board;
	/* For Part B only. */
	p->partialString = NULL;
	p->part = PART_A;
	free(boardText);
	return p;
}

struct problem *readProblemB(FILE *dictFile, FILE *boardFile, 
	FILE *partialStringFile){
	/* Fill in Part A sections. */
	struct problem *p = readProblemA(dictFile, boardFile);

	char *partialString = NULL;

	/* Part B has a string that is partially given - we assume it follows 
		word conventions for the %s specifier. */
	assert(fscanf(partialStringFile, "%ms", &partialString) == 1);
	
	p->part = PART_B;
	p->partialString = partialString;
	p->partialSize = strlen(partialString);
	return p;
}

struct problem *readProblemD(FILE *dictFile, FILE *boardFile){
	/* Interpretation of inputs is same as Part A. */
	struct problem *p = readProblemA(dictFile, boardFile);
	p->part = PART_D;
	return p;
}

/* Outputs problems ***********************************************************/
/*
	Outputs the given solution to the given file. If colourMode is 1, the
	sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
	FILE *outfileName){
	assert(solution);
	switch(problem->part){
		case PART_A:
		case PART_D:
			assert(solution->foundWordCount == 0 || solution->words);
			for(int i = 0; i < solution->foundWordCount; i++){
				fprintf(outfileName, "%s\n", solution->words[i]);
			}
			break;
		case PART_B:
			assert(solution->foundLetterCount == 0 || solution->followLetters);
			for(int i = 0; i < solution->foundLetterCount; i++){
				if(isalpha(solution->followLetters[i])){
					fprintf(outfileName, "%c\n", solution->followLetters[i]);
				} else {
					fprintf(outfileName, " \n");
				}
			}
			break;
	}
}

/* Algorithms are fun!!! ******************************************************/