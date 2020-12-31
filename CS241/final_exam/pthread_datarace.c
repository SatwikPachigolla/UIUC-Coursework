#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

static bool chosenOne;
static pthread_mutex_t m;

void* runner(void* arg) {
    int id = * ((int*)arg);
    bool chosen = false;
    sleep( (rand()%3) + 1);

    pthread_mutex_lock(&m);
    if(!chosenOne){
        sleep( (rand()%1) + 1);
        chosenOne = true;
        chosen = true;
    }
    pthread_mutex_unlock(&m);

    if(chosen) {
        printf("Chose thread %d\n",id);
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