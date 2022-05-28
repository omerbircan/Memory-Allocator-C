
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
    unsigned int size;
    unsigned int isfree;
} Info;

struct Block
{
    Info info;
    struct Block *next; /* next mem */
    struct Block *prev; /* previous mem */
    char data[0];       /*start of the allocated memory*/
};
typedef struct Block Block;

// AVL tree node
struct Node
{
    int size;           /* mem size */
    struct Node *left;  /* left subtree */
    struct Node *right; /* right subtree */
    int height;         /* height of node */
    Block *mem_block;   /* allocated memory pointer */
};
typedef struct Node Node;

/*Block: |8byte(info)|8byte next|8byte previous|0-byte(data)|*/


void *mymalloc(size_t size);
void *myfree(void *p);
Block *split(Node *b, size_t size);
Node *deleteNode(Node *root, int size);
Node *insertNode(Node *root, int size, Block *b);
Node *bestFit(Node *root, int size);