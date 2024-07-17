/*    
     Implementation for module which contains Prefix Trie 
        data structures and functions.
*/
#include "prefixTree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PART_D 2
#define DOUBLE 2

/* Frees tree recursively */
static void recursiveFreeTree(struct prefixTree *t);

/*******************************************************/
/*
    Creates new tree.
*/
struct prefixTree *newPrefixTree(){
    /* Structure of the prefix tree includes a dummy node. */
    struct prefixTree *retTree = (struct prefixTree *) 
        malloc(sizeof(struct prefixTree));
    assert(retTree);
    retTree->child = (struct prefixTree **) malloc(sizeof(struct prefixTree *) 
                                                   * MAX_CHAR);
    assert(retTree->child);
    memset(retTree->child, 0, sizeof(struct prefixTree *) * MAX_CHAR);
    
    /* Initialise the tree */
    retTree->word = NULL; 
    retTree->numChild = 0;
    retTree->isEnd = false;
    retTree->markedWord = false;
    retTree->isEndB = false;
    return retTree;
}

/*
    Adds word to prefix tree.
*/
struct prefixTree *addWordToTree(struct prefixTree *pt, char *word){
    struct prefixTree *currLevel = pt;
    int i = 0;

    while (word[i]) {
        char c = word[i];
        /* insert new character */
        if (!currLevel->child[(int) c]) {
            currLevel->child[(int) c] = newPrefixTree();
            currLevel->child[(int) c]->word = combineStr(currLevel->word, &c);
            (currLevel->numChild)++;
        } 
        currLevel = currLevel->child[(int) c];
        i++;
    }
    /* when it is last letter in the word */
    currLevel->isEnd = true;
    return pt;
}

/* 
    Convert single character to a string.
*/
char *getStr(char *c) {
    char *str = (char *) malloc(DOUBLE * (sizeof(char)));
    assert(str);
    str[0] = *c;
    str[1] = NULL_CHAR;
    return str;
}

/* 
    Combine two strings to a new memory space.
*/
char *combineStr(char *p1, char *p2) {
    p2 = getStr(p2);
    char *str = (char*)malloc((strlen(p1) + DOUBLE) * sizeof(char));
    strcpy(str, p1);
    strcat(str, p2);
    free(p2);
    return str;
}

/* 
    Frees tree by calling a recursive function. 
*/
void freeTree(struct prefixTree *t) {
    recursiveFreeTree(t);
}

/* 
    Frees tree recursively 
*/
static void recursiveFreeTree(struct prefixTree *t) {
    for (int i = 0; i < MAX_CHAR; i++) {
        if (t->child[i]) {
            recursiveFreeTree(t->child[i]);
            t->child[i] = NULL;
        }
    }
    free(t->word);
    free(t->child);
    free(t);
}

/*******************************************************/
