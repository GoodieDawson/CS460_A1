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

#define MAX_KEYS (1024)

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
    struct btNode *childNds[MAX_KEYS+1]
};

bpTree bptCreate(void) {
    bpTree b;
    b = malloc(sizeof(*b));
    assert(b);
    
    b->isLeaf = 1;
    b->keyCount = 0;
    
    return b;
}

void bptDestroy(bpTree b) {
    int i;
    if(!b->isLeaf) {
        for(i = 0; i < b->numKeys + 1; i++) {
            btDestroy(b->childNds[i]);
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

    if(i < b->numKeys && b->keys[i] == key) {
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
static bTree btInsertInternal(bTree b, int key, int *median) {
    int pos;
    int mid;
    bTree b2;
    
    pos = searchKey(b->numKeys, b->keys, key);
    
    if(pos < b->numKeys && b->keys[pos] == key) {
        return 0;
    }
    
    if(b->isLeaf) {
        memmove(&b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->numKeys - pos));
        b->keys[pos] = key;
        b->numKeys++;
        
    } else {
        b2 = btInsertInternal(b->kids[pos], key, &mid);
    
        if(b2) {

            memmove(&b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->numKeys - pos));
            memmove(&b->kids[pos+2], &b->kids[pos+1], sizeof(*(b->keys)) * (b->numKeys - pos));
            
            b->keys[pos] = mid;
            b->kids[pos+1] = b2;
            b->numKeys++;
            
        }
    }
    
    if(b->numKeys >= MAX_KEYS) {
        mid = b->numKeys/2;
        
        *median = b->keys[mid];
        
        b2 = malloc(sizeof(*b2));
        
        b2->numKeys = b->numKeys - mid - 1;
        b2->isLeaf = b->isLeaf;
        
        memmove(b2->keys, &b->keys[mid+1], sizeof(*(b->keys)) * b2->numKeys);
        if(!b->isLeaf) {
            memmove(b2->kids, &b->kids[mid+1], sizeof(*(b->kids)) * (b2->numKeys + 1));
        }
        
        b->numKeys = mid;
        
        return b2;

    } else {
        return 0;
    }
}

void btInsert(bTree b, int key) {
    bTree b1;
    bTree b2;
    int median;
    
    b2 = btInsertInternal(b, key, &median);
    
    if(b2) {
        
        b1 = malloc(sizeof(*b1));
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