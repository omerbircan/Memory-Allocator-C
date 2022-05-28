/*
* author <omer bircan>
* omer.bircan@hotmail.com.tr
*/
#include "mymalloc.h"

int is_first_call = 1;

void *myalloc(size_t size, Strategy strategy)
{
    if (is_first_call)
    {
        heap_start = sbrk(1024 + 2 * sizeof(Block)); //heap_start+heap_end+1024byte
        heap_start->info.size = 1024;
        heap_start->info.isfree = 1;
        heap_btag = (void *)heap_start + sizeof(Block) + heap_start->info.size;
        heap_btag->info.size = 1024;
        heap_btag->info.isfree = 1;
        free_list = heap_start;
        free_list->next = NULL;
        heap_btag->next = NULL;
        is_first_call = 0;
        /*available free space is 1024*/
        printf("heap start:%p-heap end:%p\n", heap_start, heap_btag);
    }

    if (size % 16 != 0)
        size = size + 16 - (size % 16); // round to 16

    Block *tmp = NULL
    switch (strategy)
    {
    case bestFit:
        tmp = split(best_fit(size), size);
        break;
    case worstFit:
        tmp = split(worst_fit(size), size);
        break;
    case firstFit:
        tmp = split(first_fit(size), size);
        break;
    default:
        tmp = split(best_fit(size), size);
    }
    
    Block *block = heap_start;
    Block *last_free = NULL;
    Block *tmp2 = NULL;
    
    while (block != NULL)
        if (block->info.isFree){
            tmp2 = last_free;
            last_free = block;
            
            if (last_free == NULL){
                free_list = last_free;
            }else {
                last_free->next = tmp2;
            }
        }
        block = (Block *)(block->next);
}

Block *split(Block *b, size_t size)
{
    Block *b1 = b;
    b1->info.size = size;
    b1->info.isfree = 0;
    Info *btag = (Info *)((char *)b1 + sizeof(Block) + b1->info.size);
    btag->isfree = 0;
    btag->size = b1->info.size;

    Block *b2 = (Block *)((char *)b + sizeof(Block) + b1->info.size + sizeof(Info));
    b2->info.isfree = 1;
    b2->info.size = b->info.size - size - sizeof(Block) - sizeof(Info);

    Info *btag2 = (Info *)((char *)b2 + sizeof(Block) + b2->info.size);
    btag2->isfree = 1;
    btag2->size = b2->info.size;

    b2->next = b->next;
    b1->next = b2;

    return b1;
}

Block* first_fit(unsigned int size)
{
    Block *block = heap_start;
    while (block != NULL && block->info.size < size && block->info.isfree)
        block = (Block *)(block->next);

    // if block is null, there is no enough space in heap
    if (block == NULL)
        return NULL;
}

Block* worst_fit(unsigned int size){
    Block *block = heap_start;
    Block *max_block = block;
    
    while(block != NULL){
        if(max_block->info.size < block->info.size && block->info.isfree)
            max_block = block;

        block = (Block*)(block->next);
    }
    
    if(max_block->info.size >= size)
        return max_block;
    
    return NULL;
}

Block* best_fit(unsigned int size){
    Block *block = heap_start;
    Block *best_block = block;
    
    while(block != NULL){
        if (block->info.size < size && block->info.isfree){
            if ((block->info.size - size) < (best_block->info.size - size)){
                best_block = block;
            }
        }
        block = (Block*)(block->next);
    }

    if (best_block != NULL)
        return best_block;

    return NULL;
}

void printHeap(){
    Block* block = free_list;
    println("Blocks:");
    while (block != NULL){   
        printf("Free:\t%d\nSize:\t%p\n", block->info.size, block->info.isfree);
        block = (Block*)(block->next);
    }
    println('---------');
}
