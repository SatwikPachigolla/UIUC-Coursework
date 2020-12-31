#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct node {
    struct node* next;
    int val;
} node;

void printList();

static node* head;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void insertHead(int val) {
    node* newNode = malloc(sizeof(node));
    newNode->val = val;
    pthread_mutex_lock(&m);
    newNode->next = head;
    head = newNode;
    pthread_mutex_unlock(&m);
}

int removeHead() {
    node* oldHead = NULL;

    pthread_mutex_lock(&m);
    if (head) {
        oldHead = head;
        head = head->next;
    }
    pthread_mutex_unlock(&m);

    if(oldHead) {
        int result = oldHead->val;
        free(oldHead);
        return result;
    }
    else {
        return -1;
    }
}

void* runner(void* arg) {
    int id = * ((int*)arg);

    for(int i = 0; i < 3; i++) {
        pthread_mutex_lock(&m);
        printf("Thread %d: before modification %d ", id, i);
        printList();
        pthread_mutex_unlock(&m);
        int bin = rand() % 3;
        if(bin) {
            int newVal = rand() % 10;
            printf("Thread %d: inserting %d at head...\n", id, newVal);
            insertHead(newVal);
        }
        else {
            printf("Thread %d: attempting to remove head...\n", id);
            int removed = removeHead();
            printf("Thread %d: removed %d\n", id, removed);
            
        }
        pthread_mutex_lock(&m);
        printf("Thread %d: after modification %d ", id, i);
        printList();
        pthread_mutex_unlock(&m);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    srand( time(NULL) );
    pthread_t tid1, tid2;
    int i1 = 1, i2 = 2; 
    pthread_create(&tid1, NULL, runner, &i1);
    pthread_create(&tid2, NULL, runner, &i2);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}

void printList(){
    node* curr = head;
    printf("Printing List: ");
    while(curr) {
        printf("%d ", curr->val);
        curr = curr->next;
    }
    printf("\n");
}
