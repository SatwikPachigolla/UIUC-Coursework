/**
 * malloc
 * CS 241 - Fall 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct metadata {
    //void* memory; Do we even need this
    size_t size;
    int free; //1 if free, 0 if not
    struct metadata *next;
    struct metadata *prev;
} metadata;

static metadata* tail; // = NULL
//static size_t requested;
static size_t allocated;
static size_t totalSbrk;

void collapse(metadata* block);
void split(metadata* block, size_t size);
metadata* find(size_t size);

/*
void printMemory() {
    fprintf(stderr, "-----  Making buffer much bigger on purpose and printing memory  -----\n");
    
    if(tail == NULL){
        return;
    }
    printf("Reverse traversal:\n");
    metadata* curr = tail;
    while(curr->prev) {
        curr = curr->prev;
    }
    //curr is at head
    printf("Forward traversal:\n");
    while(curr) {
        fprintf(stderr, "\nMetaData: %p\n", curr);
        fprintf(stderr, "Memory: %p = %p\n", curr->memory, curr+1);
        curr = curr->next;
    }
    fprintf(stderr, "-----  End print memory  -----\n");
}
*/
/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    size_t total = num*size;
    void* result = malloc(total);
    if(result){
        memset(result, 0, total);
    }
    return result;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    // printf("malloc");
    // printMemory();
    if (size == 0) {
        return NULL;
    }
    metadata* result = find(size);
    if(result) {
        result->free = 0;
        split(result, size);
        return result+1;
        // result->memory = result+1;
        // return result->memory;
    }
    else {
        //new add
        if(tail && tail->free) {
            void* addition = sbrk(size-tail->size);
            if(addition == (void*)-1) return NULL;
            totalSbrk += size-tail->size;
            allocated += size + sizeof(metadata);
            tail->size = size;
            tail->free = 0;
            return tail+1;
            // tail->memory = tail+1;
            // return tail->memory;
        }
        else {
            result = sbrk(size + sizeof(metadata));
            if (result == (void*)-1) return NULL;
            totalSbrk += size + sizeof(metadata);
            result->size = size;
            if(tail){
                tail->next = result;
            }
            result->prev = tail;
            result->next = NULL;
            tail = result;
            allocated += size + sizeof(metadata);
            result->free = 0;
            return result+1;
            // result->memory = result+1;
            // return result->memory;
        }
    } 
    // printMemory();
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // printf("free");
    // printMemory();
    if(!ptr){
        return;
    }
    metadata* toFree = ((metadata*) ptr) - 1;
    toFree->free = 1;
    allocated -= toFree->size + sizeof(metadata);
    collapse(toFree);
    // printMemory();
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    if(!ptr){
        return malloc(size);
    }
    if(size == 0){
        free(ptr);
        return NULL;
    }

    metadata* curr = ((metadata*)ptr) - 1;
    size_t oldSize = curr->size;
    if (oldSize < size) {
        if(curr->next && curr->next->free && curr->next->size + sizeof(metadata) + oldSize >= size) {
            size_t newSize = curr->next->size + sizeof(metadata) + oldSize; 
            curr->next = curr->next->next;
            if(curr->next) {
                curr->next->prev = curr;
            }
            else {
                tail = curr;
            }
            curr->size = newSize;
            allocated+=newSize-oldSize;
        }
        else {
            void* result = malloc(size);
            if(!result){
                return NULL;
            }
            memcpy(result, ptr, curr->size);
            free(ptr);
            return result;
        }
    }

    if(curr->size > size){
        split(curr, size);
    }
    
    return ptr;
}


void collapse(metadata* block) {
    size_t newSize = block->size;
    metadata* start = block;
    while(start->prev && start->prev->free){
        start = start->prev;
        newSize += sizeof(metadata) + start->size;
    }

    metadata* end = block;
    while(end->next && end->next->free) {
        end = end->next;
        newSize += sizeof(metadata) + end->size;
    }

    if ( start == end) {
        return;
    }

    start->next = end->next;
    if(start->next) {
        start->next->prev = start;
    }
    else {
        tail = start;
    }
    start->size = newSize;
    //start->memory = start + 1;
    start->free = 1;
}

metadata* find(size_t size) {
    if ( size + sizeof(metadata) > totalSbrk - allocated) {
        return NULL;
    }
    metadata* curr = tail;
    while(curr) {
        if (curr->free && curr->size >= size) {
            return curr;
        }
        curr = curr->prev;
    }
    return NULL;
}

void split(metadata* block, size_t size) {
    if (block->size < size + 1 + sizeof(metadata) /*|| block->size < 2*size*/) {
        return;
    }
    allocated -= block->size-size;

    metadata* new_block = (void*)(block+1) + size;
    size_t newBlockSize = block->size - sizeof(metadata) - size;
    new_block->size = newBlockSize;
    // new_block->memory = new_block + 1;
    new_block->free = 1;
    block->size = size;

    //linkedlist insert new node pretty much
    new_block->prev = block;
    new_block->next = block->next;
    if (block->next) {
      block->next->prev = new_block;
    } 
    else {
      tail = new_block;
    }
    block->next = new_block;
    collapse(new_block);
}
