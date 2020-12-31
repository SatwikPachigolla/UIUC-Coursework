#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* runner(void* arg) {
    printf("Runner Thread: Started Execution\n");
    sleep(1);
    printf("Runner Thread: Finished Execution\n");
    return NULL;
}

int main(int argc, char *argv[]) {

    pthread_t tid;
    printf("Main Thread: Creating another thread...\n");
    pthread_create(&tid, NULL, runner, NULL);
    printf("Main Thread: Created thread\n");
    printf("Main Thread: Waiting for runner thread to finish...\n");
    pthread_join(tid, NULL);
    printf("Main Thread: Joined runner thread\n");
    return 0;
}