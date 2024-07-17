/*    
     Header for module which contains Problem 2-related 
        data structures and functions.
*/
#include <stdio.h>
#include "prefixTree.h"

struct problem;
struct solution;
struct node;

/* node data structure with next pointer and point to character array */
struct node {
    struct node *next;
    char *item;
};

/* queue data structure with head and tail pointers */
struct queue {
    struct node *head;
    struct node *tail;
};

/* 
    Reads the given dictionary file into a set of words
        and the given board file into a 1-D and 2-D representation.
*/
struct problem *readProblemA(FILE *dictFile, FILE *boardFile);

/* 
    Reads the given dictionary file into a set of words,
        the given board file into a 1-D and 2-D representation and
        the given partial string file as a partial string.
*/
struct problem *readProblemB(FILE *dictFile, FILE *boardFile, 
    FILE *partialStringFile);

/*
    Same as Problem A, but part is set for Part D.
*/
struct problem *readProblemD(FILE *dictFile, FILE *boardFile);

/*
    Solves the given problem according to all parts' definitions
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p);

struct solution *solveProblemB(struct problem *p);

struct solution *solveProblemD(struct problem *p);

/*
    Outputs the given solution to the given file.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName);

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem);

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem);

/*
    Gets all neighbours of current piece in board. Check if current letter in 
    tree is part of neighbours. If so, enqueue the neighbour. For all parts A-D.
    Time complexity of O(1).
*/
void getNeighbour(char *charInBoard, int treeIndex, struct problem *p, 
                  struct queue *q);

/*
    Explores both prefix tree and letters in board simultaneously using
    depth-first search approach and queue data structure.
*/
void treeExploreA(struct prefixTree *t, struct solution *s, char *charInBoard, 
    struct problem *p, bool *visited);

void treeExploreB(struct prefixTree *t, struct solution *s, char *charInBoard, 
    int partialIndex, struct problem *p, struct prefixTree **partialStringEnd);

void treeExploreD(struct prefixTree *t, struct solution *s, char *charInBoard,
    struct problem *p, bool *visited);

/*
    Add all words into prefix tree.
*/
void addDictionaryToTree(struct prefixTree *t, struct problem *p);

/*
    Inserts found word into solution (parts A-D) via DFS on prefix tree.
*/
void exploreSolutions(struct prefixTree*t, struct solution*s, struct problem*p);

/*
    Create new queue.
*/
struct queue *createQueue();

/* 
    Removes and return the item in the front-most node.
*/
char *dequeue(struct queue *queue);

/*
    Inserts new node into queue.
*/
void enqueue(struct queue *queue, char *word);

/*
    Frees queue.
*/
void freeQueue(struct queue *queue);
