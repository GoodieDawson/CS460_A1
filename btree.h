/*
 * 
 * You will need to write your B+Tree almost entirely from scratch. 
 * 
 * B+Trees are dynamically balanced tree structures that provides efficient support for insertion, deletion, equality, and range searches. 
 * The internal nodes of the tree direct the search and the leaf nodes hold the base data..
 * 
 * For a basic rundown on B+Trees, we will refer to parts of Chapter 10 of the textbook Ramikrishnan-Gehrke 
 * (all chapters and page numbers in this assignment prompt refer to the 3rd edition of the textbook).
 *
 * Read Chapter 10 which is on Tree Indexing in general. In particular, focus on Chapter 10.3 on B+Tree.
 */

#ifndef BTREE_H
#define BTREE_H

#include "data_types.h"
#include "query.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define MAX_KEYS (1024)

typedef struct Node *bpTree;

bpTree btCreate(void);

void bptDestroy(bpTree t);

int bptSearch(bpTree t, int key);

void bptInsert(bpTree t, int key);

/* 
Designing your C Structs for B+Tree nodes (Chapter 10.3.1)
How will you represent a B+Tree as a C Struct (or series of C structs that work together)? There are many valid ways to do this part of your design, and we leave it open to you to try and tune this as you progress through the project.
How will you account for a B+Tree node being an internal node or a leaf node? Will you have a single node type that can conditionally be either of the two types, or will you have two distinct struct types?
How many children does each internal node have? This is called the fanout of the B+Tree.
What is the maximum size for a leaf node? How about for an internal node?
What is the minimum threshold of content for a node, before it has to be part of a rebalancing?
*/

// TODO: here you will need to define a B+Tree node(s) struct(s)
struct Node { 
    int isLeaf;
    int keyCount;
    int keys[MAX_KEYS];
    struct Node *childNds[MAX_KEYS+1];
};

bpTree bptCreate(void) {
    bpTree b;
    b = (struct Node *)malloc(sizeof(*b));
    assert(b);
    
    b->isLeaf = 1;
    b->keyCount = 0;
    
    return b;
}

void bptDestroy(bpTree b) {
    int i;
    if(!b->isLeaf) {
        for(i = 0; i < b->keyCount + 1; i++) {
            bptDestroy(b->childNds[i]);
        }
    }
    free(b);
}

/* The following are methods that can be invoked on B+Tree node(s).
 * Hint: You may want to review different design patterns for passing structs into C functions.
 */

/* FIND (Chapter 10.4)
This is an equality search for an entry whose key matches the target key exactly.
How many nodes need to be accessed during an equality search for a key, within the B+Tree? 
*/

// TODO: here you will need to define FIND/SEARCH related method(s) of finding key-values in your B+Tree.

static int searchNode(int n, const int *keys, int key) {
    int lo;
    int hi;
    int mid;
    
    lo = -1;
    hi = n;
    
    while(lo + 1 < hi) {
        mid = (lo+hi)/2;
        if(keys[mid] == key) {
            return mid;
        } else if(keys[mid] < key) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return hi;
}

int bptSearch(bpTree b, int key) {
    int i;

    if(b->keyCount == 0) {
        return 0;
    }
    
    i = searchNode(b->keyCount, b->keys, key);

    if(i < b->keyCount && b->keys[i] == key) {
        return 1;
    } else {
        return(!b->isLeaf && bptSearch(b->childNds[i], key));
    }
}


/* INSERT (Chapter 10.5)
How does inserting an entry into the tree differ from finding an entry in the tree?
When you insert a key-value pair into the tree, what happens if there is no space in the leaf node? What is the overflow handling algorithm?
For Splitting B+Tree Nodes (Chapter 10.8.3)
*/

// TODO: here you will need to define INSERT related method(s) of adding key-values in your B+Tree.
static bpTree bptSubInsert(bpTree b, int key, int *median) {
    int pos;
    int mid;
    bpTree b2;
    
    pos = searchNode(b->keyCount, b->keys, key);
    
    if(pos < b->keyCount && b->keys[pos] == key) {
        return 0;
    }
    
    if(b->isLeaf) {
        memmove(&b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->keyCount - pos));
        b->keys[pos] = key;
        b->keyCount++;
        
    } else {
        b2 = bptSubInsert(b->childNds[pos], key, &mid);
    
        if(b2) {

            memmove(&b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->keyCount - pos));
            memmove(&b->childNds[pos+2], &b->childNds[pos+1], sizeof(*(b->keys)) * (b->keyCount - pos));
            
            b->keys[pos] = mid;
            b->childNds[pos+1] = b2;
            b->keyCount++;
            
        }
    }
    
    if(b->keyCount >= MAX_KEYS) {
        mid = b->keyCount/2;
        
        *median = b->keys[mid];
        
        b2 = (struct Node *)malloc(sizeof(*b2));
        
        b2->keyCount = b->keyCount - mid - 1;
        b2->isLeaf = b->isLeaf;
        
        memmove(b2->keys, &b->keys[mid+1], sizeof(*(b->keys)) * b2->keyCount);
        if(!b->isLeaf) {
            memmove(b2->childNds, &b->childNds[mid+1], sizeof(*(b->childNds)) * (b2->keyCount + 1));
        }
        
        b->keyCount = mid;
        
        return b2;

    } else {
        return 0;
    }
}

void bptInsert(bpTree b, int key) {
    bpTree b1;
    bpTree b2;
    int median;
    
    b2 = bptSubInsert(b, key, &median);
    
    if(b2) {
        
        b1 = (struct Node *)malloc(sizeof(*b1));
        assert(b1);
        
        memmove(b1, b, sizeof(*b));
         
        b->keyCount = 1;
        b->isLeaf = 0;
        b->keys[0] = median;
        b->childNds[0] = b1;
        b->childNds[1] = b2;
    }
}

/* BULK LOAD (Chapter 10.8.2)
Bulk Load is a special operation to build a B+Tree from scratch, from the bottom up, when beginning with an already known dataset.
Why might you use Bulk Load instead of a series of inserts for populating a B+Tree? Compare the cost of a Bulk Load of N data entries versus that of an insertion of N data entries? What are the tradeoffs?
*/

// TODO: here you will need to define BULK LOAD related method(s) of initially adding all at once some key-values to your B+Tree.
// BULK LOAD only can happen at the start of a workload


/*RANGE (GRADUATE CREDIT)
Scans are range searches for entries whose keys fall between a low key and high key.
Consider how many nodes need to be accessed during a range search for keys, within the B+Tree?
Can you describe two different methods to return the qualifying keys for a range search? 
(Hint: how does the algorithm of a range search compare to an equality search? What are their similarities, what is different?)
Can you describe a generic cost expression for Scan, measured in number of random accesses, with respect to the depth of the tree?
*/

// TODO GRADUATE: here you will need to define RANGE for finding qualifying keys and values that fall in a key range.



#endif

/*
Implementation was inspired and adapted from the following sources
Aspnes, James. “BTrees.” BTrees, 17 June 2014, www.cs.yale.edu/homes/aspnes/pinewiki/BTrees.html.
*/