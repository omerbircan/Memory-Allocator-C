/**
 * @file avl_malloc.c
 * @author Omer Said Bircan (omer.said.bircan@gmail.com)
 * @version 0.1
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "avl_memory.h"

Node *free_list = NULL;   /* start of the free list */
static Block *heap_start = NULL; /*head of allocated memory from sbrk */

/* creates node */
Node *newNode(int size, Block *mem_block)
{
    Node *_node = malloc(sizeof(Node));
    _node->size = size;
    _node->mem_block = mem_block;
    _node->left = NULL;
    _node->right = NULL;
    _node->height = 1;
    return (_node);
}

int height(Node *tmp)
{
    if (tmp == NULL)
        return 0;
    return tmp->height;
}

int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

Node *rightRotate(Node *x)
{
    Node *y = x->left;
    Node *z = y->right;

    /** Rotation **/
    y->right = x;
    x->left = z;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    /** Return new root **/
    return y;
}

Node *leftRotate(Node *x)
{
    Node *y = x->right;
    Node *z = y->left;

    /** Rotation **/
    y->left = x;
    x->right = z;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    /** Return new root **/
    return y;
}

int getBalance(Node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

/* inserts node and updates balanced tree */
Node *insertNode(Node *root, int size, Block *b)
{
    /* found insert node */
    if (root == NULL)
        return newNode(size, b);

    if (size < root->size)
        root->left = insertNode(root->left, size, b);
    else /* bigger or equal size */
        root->right = insertNode(root->right, size, b);

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);
    /* left left subtree */
    if (balance > 1 && size < root->left->size)
        return rightRotate(root);

    /* right right subtree */
    if (balance < -1 && size > root->right->size)
        return leftRotate(root);

    /* left right subtree */
    if (balance > 1 && size > root->left->size)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    /* right left subtree */
    if (balance < -1 && size < root->right->size)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

Node *minNode(Node *node)
{
    if (node->left)
        return minNode(node->left);

    return node;
}

Node *deleteNode(Node *root, int size)
{
    if (root == NULL)
        return root;

    /* searching node in tree */
    if (size < root->size)
        root->left = deleteNode(root->left, size);

    else if (size > root->size)
        root->right = deleteNode(root->right, size);

    /* found node */
    else
    {
        /* max 1 child node */
        if ((root->left == NULL) || (root->right == NULL))
        {
            Node *temp = root->left ? root->left : root->right;

            /* No child */
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            /* 1 child node */
            else
                *root = *temp;
            free(temp);
        }
        /* 2 child node */
        else
        {
            Node *temp = minNode(root->right);
            root->size = temp->size;
            root->right = deleteNode(root->right, temp->size);
        }
    }

    /* 1 node tree */
    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left),
                           height(root->right));

    int balance = getBalance(root);

    /* left left subtree */
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    /* left right subtree */
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    /* right right subtree */
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    /* right left subtree */
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}
/*-------------------------------------------------------------*/

/* finds nearest size in AVL tree */
Node *bestFit(Node *root, int size)
{
    
    Node *currentNode = malloc(sizeof(Node));
    currentNode = root;
    Node *bestNode = NULL;

    if (currentNode->left == NULL && currentNode->right == NULL)
    {
        if(currentNode->size >= size){
            return currentNode;
        }
    }

    while(currentNode->left != NULL || currentNode->right != NULL)
    {

        if (currentNode->size >= size)
        {
            if (currentNode->size == size)
                return currentNode;
            bestNode = currentNode;
            currentNode = currentNode->left;
        }
        else /* current < size */
            currentNode = currentNode->right;
    }
    return bestNode;
}

int needSpace = 1;
size_t SIZE = 1024;

void *mymalloc(size_t size)
{
    if (needSpace)
    {

        needSpace = 0;
        heap_start = sbrk(SIZE + sizeof(Block));
        heap_start->info.size = SIZE;
        heap_start->info.isfree = 1;
        free_list = malloc(sizeof(Node));
        free_list->size = SIZE;
        free_list->mem_block = heap_start;
        free_list->right = NULL;
        free_list->left = NULL;
    }

    if (size % 16 != 0)
        size = size + 16 - (size % 16); // round to 16

    Node *tmp = bestFit(free_list, size);
    if (tmp == NULL) //size > mem size
    {
        if (size > SIZE)
        {
            perror("error: size > 1024");
            abort();
        }
        needSpace = 1;
        return mymalloc(size);
    }

    return split(tmp, size)->data;
}

/* splits block */
Block *split(Node *b, size_t size)
{
    Block *b1 = b->mem_block;
    b1->info.size = size;
    b1->info.isfree = 0;

    Block *b2 = (Block *)((char *)b + sizeof(Block) + b1->info.size);
    b2->info.isfree = 1;
    b2->info.size = b->size - sizeof(Block) - size;

    b1->next = b2;
    b2->prev = b1;

    free_list = deleteNode(free_list, b->size);
    free_list = insertNode(free_list, b2->info.size, b2);

    return b1;
}

void *myfree(void *p)
{
    Node *b = malloc(sizeof(Node));
    Block *b1 = (Block *)((char *)p - sizeof(Block));
    b1->info.isfree = 1;
    Block *prev = b1->prev;

    /* left coalescing */
    if (prev->info.isfree)
    {
        deleteNode(free_list, prev->info.size);
        prev->info.size += sizeof(Block) + b1->info.size;
        b1 = prev;
    }

    Block *next = b1->next;
    if (next->info.isfree)
    {
        deleteNode(free_list, next->info.size);
        b1->info.size += sizeof(Block) + next->info.size;
    }
    insertNode(free_list, b1->info.size, b1);
}
int main()
{
    int *a = mymalloc(64);
    printf("%d", &a);
    int *b = mymalloc(64);
    int *c = mymalloc(64);
    int *d = mymalloc(64);
}
