/*    
     Header for module which contains Prefix Trie 
        data structures and functions.
*/
#include <stdbool.h>

#ifndef PREFIXTREESTRUCT
#define PREFIXTREESTRUCT

#define MAX_CHAR 256
#define NULL_CHAR '\0'
#define DEFAULT_STR ""

struct prefixTree;

/* Data structure for prefix tree */
struct prefixTree {
    /* Partial string */
    char *word;
    /* Number of its children letters */
    int numChild;
    /* Whether it is last letter in word */
    bool isEnd;
    /* Whether it is last letter in word for part B */
    bool isEndB;
     /* Stores all its children */
    struct prefixTree **child;
    /* Whether word in dictionary */
    bool markedWord;
};

#endif

/*
    Creates new tree.
*/
struct prefixTree *newPrefixTree();

/*
    Adds words into tree.
*/
struct prefixTree *addWordToTree(struct prefixTree *pt, char *word);

/*
    Combines 2 strings.
*/
char *combineStr(char *p1, char *p2);

/* 
    Frees tree.
*/
void freeTree(struct prefixTree *t);

/*
    Converts character into string.
*/
char *getStr(char *c);
